#include "Rendering/Resources/Loader/ModelLoader.h"
#include "Maths/Utils.h"

#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/postprocess.h>

#ifdef NSHIPPING
#include "Editor/Widget/WidgetEditor.h"
#include "EngineCore/Service/ServiceLocator.h"
#include "Editor/Utils/Utils.h"
#endif

using namespace Rendering::Resources::Loaders;

bool ModelLoader::importMesh(const char* pFilePath, std::vector<VK::VKMesh*>& pMesh, std::map<std::string, Rendering::Data::BoneInfo>& pBoneInfoMap, Data::BoundingBox& pBox, int& pBoneCount)
{
    Assimp::Importer importer;
    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    //importer.SetPropertyFloat("PP_GSN_MAX_SMOOTHING_ANGLE", 90);
    const aiScene* scene = importer.ReadFile(pFilePath, aiProcess_CalcTangentSpace | aiProcess_FixInfacingNormals |
                                                        aiProcess_ConvertToLeftHanded | aiProcess_JoinIdenticalVertices | 
                                                        aiProcess_Triangulate | aiPrimitiveType_LINE | aiPrimitiveType_POINT | aiProcess_GenSmoothNormals | aiProcess_GenBoundingBoxes);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        if (scene != nullptr)
            if (scene->HasAnimations())
                return false;

        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return false;
    }

    pMesh.reserve(scene->mNumMeshes);

    processNode(scene->mRootNode, scene, pMesh, pBoneInfoMap, pBoneCount);
    
    //Process BoundingBox
    pBox = convertBoundingBox(scene->mMeshes[0]->mAABB);
    pMesh[0]->mBoundingBox = pBox;
    for (int i = 1; i < scene->mNumMeshes; i++)
    {
        Data::BoundingBox box = convertBoundingBox(scene->mMeshes[i]->mAABB);
        pMesh[i]->mBoundingBox = box;
        pMesh[i]->mBoundingBox.mSize = box.mMax - box.mMin;
        pMesh[i]->mBoundingBox.computeCorner();

        pBox.mMin = Maths::FVector3::vectorMin(pBox.mMin, box.mMin);
        pBox.mMax = Maths::FVector3::vectorMax(pBox.mMax, box.mMax);
    }
    pBox.mSize = pBox.mMax - pBox.mMin;


#ifdef NSHIPPING
    /*if (pMesh.size() > 1)
    {
        std::string path = Editor::Utils::qStringToStdString(service(Editor::Widget::WidgetEditor).mLocation.mFolder) + "/";

        for (auto& mesh : pMesh)
        {
            std::string finalPath = path + mesh.mName + ".fbx";

            if (!QFile::exists(finalPath.c_str()))
                exportMesh(finalPath.c_str(), mesh);
        }
    }*/
#endif

    return true;
}

void ModelLoader::exportMesh(const char* pFilePath, VK::VKMesh& aMesh)
{
    aiScene scene;

    scene.mRootNode = new aiNode();
         
    scene.mMaterials = new aiMaterial * [1];
    scene.mMaterials[0] = new aiMaterial();
    scene.mNumMaterials = 1;

    scene.mMeshes = new aiMesh * [1];
    scene.mNumMeshes = 1;   
    scene.mMeshes[0] = new aiMesh();
    scene.mMeshes[0]->mMaterialIndex = 0;

    scene.mRootNode->mMeshes = new unsigned int[1];
    scene.mRootNode->mMeshes[0] = 0;
    scene.mRootNode->mNumMeshes = 1;


    auto pMesh = scene.mMeshes[0];

    const auto& vVertices = aMesh.mVertices;

    pMesh->mVertices = new aiVector3D[vVertices.size()];
    pMesh->mNumVertices = vVertices.size();

    pMesh->mTextureCoords[0] = new aiVector3D[vVertices.size()];
    pMesh->mNumUVComponents[0] = vVertices.size();

    pMesh->mNormals = new aiVector3D[vVertices.size()];

    for (auto itr = vVertices.begin(); itr != vVertices.end(); ++itr) {

        const auto& v = itr->mPosition;
        const auto& n = itr->mNormals;
        const auto& t = itr->mTexCoords;

        pMesh->mVertices[itr - vVertices.begin()] = aiVector3D(v.x, v.y, v.z);
        pMesh->mTextureCoords[0][itr - vVertices.begin()] = aiVector3D(t.x, t.y, 0);
        pMesh->mNormals[itr - vVertices.begin()] = aiVector3D(n.x, n.y, n.z);
    }


    const auto vTriangles = aMesh.mIndices;

    if (vTriangles.size() % 3 == 0)
    {
        pMesh->mFaces = new aiFace[vTriangles.size() / 3];
        pMesh->mNumFaces = vTriangles.size() / 3;

        int faceIdx = 0;
        for (int i = 0; i < vTriangles.size(); i += 3, faceIdx++)
        {

            aiFace& face = pMesh->mFaces[faceIdx];

            face.mIndices = new unsigned int[3];
            face.mNumIndices = 3;

            face.mIndices[0] = vTriangles[i];
            face.mIndices[1] = vTriangles[i + 1];
            face.mIndices[2] = vTriangles[i + 2];
        }
    }
    else if (vTriangles.size() % 4 == 0)
    {
        pMesh->mFaces = new aiFace[vTriangles.size() / 4];
        pMesh->mNumFaces = vTriangles.size() / 4;

        int faceIdx = 0;
        for (int i = 0; i < vTriangles.size(); i += 4, faceIdx++)
        {

            aiFace& face = pMesh->mFaces[faceIdx];

            face.mIndices = new unsigned int[4];
            face.mNumIndices = 4;

            face.mIndices[0] = vTriangles[i];
            face.mIndices[1] = vTriangles[i + 1];
            face.mIndices[2] = vTriangles[i + 2];
            face.mIndices[3] = vTriangles[i + 3];
        }
    }

    Assimp::Exporter exporter;
    aiReturn res = exporter.Export(&scene, "fbx", pFilePath);

    if (res != AI_SUCCESS)
        std::cout << "ERROR::ASSIMP:: " << exporter.GetErrorString() << std::endl;
    
    delete[] pMesh->mFaces;
    pMesh->mFaces = nullptr;

    delete[] pMesh->mNormals;
    pMesh->mNormals = nullptr;

    delete[] pMesh->mTextureCoords[0];
    pMesh->mTextureCoords[0] = nullptr;

    delete[] pMesh->mVertices;
    pMesh->mVertices = nullptr;

    delete[] scene.mRootNode->mMeshes;
    scene.mRootNode->mMeshes = nullptr;

    delete[] scene.mMeshes;
    scene.mMeshes = nullptr;

    delete[] scene.mMaterials;
    scene.mMaterials = nullptr;

    delete scene.mRootNode;
    scene.mRootNode = nullptr;
}

void ModelLoader::processNode(const aiNode* pNode, const aiScene* pScene, std::vector<VK::VKMesh*>& pMesh, std::map<std::string, Rendering::Data::BoneInfo>& pBoneInfoMap, int& pBoneCount)
{
    for (unsigned int i = 0; i < pNode->mNumMeshes; i++)
    {
        aiMesh* mesh = pScene->mMeshes[pNode->mMeshes[i]];
        processMesh(mesh, pScene, pMesh, pBoneInfoMap, pBoneCount);
    }

    for (unsigned int i = 0; i < pNode->mNumChildren; i++)
        processNode(pNode->mChildren[i], pScene, pMesh, pBoneInfoMap, pBoneCount);
}

void ModelLoader::processMesh(const aiMesh* pMesh, const aiScene* pScene, std::vector<VK::VKMesh*>& pVKMesh, std::map<std::string, Rendering::Data::BoneInfo>& pBoneInfoMap, int& pBoneCount)
{
    std::vector<Rendering::Geometry::Vertex> vertices;
    std::vector<uint32_t> indices;

    //vertices
    for (unsigned int i = 0; i < pMesh->mNumVertices; i++)
    {
        Rendering::Geometry::Vertex vertex;
        
        // position
        {
            Maths::FVector3 vector(pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z);
            vertex.mPosition = vector;
        }

        // normals
        if (pMesh->HasNormals())
        {
            Maths::FVector3 vector(pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z);
            vertex.mNormals = vector;
        }

        // texture coordinates
        if (pMesh->mTextureCoords[0])
        {
            Maths::FVector2 vector(pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y);
            vertex.mTexCoords = vector;
        }

        if (pMesh->HasTangentsAndBitangents())
        {
            // tangent
            {
                Maths::FVector3 vector(pMesh->mTangents[i].x, pMesh->mTangents[i].y, pMesh->mTangents[i].z);
                vertex.mTangent = vector;
            }

            // bitangent
            {
                Maths::FVector3 vector(pMesh->mBitangents[i].x, pMesh->mBitangents[i].y, pMesh->mBitangents[i].z);
                vertex.mBitangent = vector;
            }
        }

        vertices.push_back(vertex);
    }

    // faces
    for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
    {
        aiFace face = pMesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    extractBoneWeightForVertices(vertices, pBoneInfoMap, pBoneCount, pMesh, pScene);

    pVKMesh.push_back(new VK::VKMesh(vertices, indices, pMesh->mName.C_Str()));
}

void ModelLoader::extractBoneWeightForVertices(std::vector<Rendering::Geometry::Vertex>& pVertices, std::map<std::string, Rendering::Data::BoneInfo>& pBoneInfoMap, int& pBoneCount, const aiMesh* pMesh, const aiScene* pScene)
{
    for (int boneIndex = 0; boneIndex < pMesh->mNumBones; ++boneIndex)
    {
        int boneID = -1;
        std::string boneName = pMesh->mBones[boneIndex]->mName.C_Str();
        if (pBoneInfoMap.find(boneName) == pBoneInfoMap.end())
        {
            Rendering::Data::BoneInfo newBoneInfo;
            newBoneInfo.mId = pBoneCount;
            newBoneInfo.mOffset = convertMatrix(pMesh->mBones[boneIndex]->mOffsetMatrix);
            pBoneInfoMap[boneName] = newBoneInfo;
            boneID = pBoneCount;
            pBoneCount++;
        }
        else
            boneID = pBoneInfoMap[boneName].mId;

        if (boneID == -1)
            return;

        aiVertexWeight* weights = pMesh->mBones[boneIndex]->mWeights;
        int numWeights = pMesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;

            if (vertexId <= pVertices.size())
                setVertexBoneData(pVertices[vertexId], boneID, weight);
        }
    }
}

void ModelLoader::setVertexBoneData(Rendering::Geometry::Vertex& pVertex, int pBoneId, float pWeight)
{
    for (unsigned int i = 0; i < MAX_BONE_INFLUENCE; i++)
        if (pVertex.mBoneIDs[i] < 0)
        {
            pVertex.mWeights[i] = pWeight;
            pVertex.mBoneIDs[i] = pBoneId;
            break;
        }
}

Maths::FMatrix4 ModelLoader::convertMatrix(const aiMatrix4x4& pFrom)
{
    Maths::FMatrix4 to;
    to(0, 0) = pFrom.a1; to(1, 0) = pFrom.a2; to(2, 0) = pFrom.a3; to(3, 0) = pFrom.a4;
    to(0, 1) = pFrom.b1; to(1, 1) = pFrom.b2; to(2, 1) = pFrom.b3; to(3, 1) = pFrom.b4;
    to(0, 2) = pFrom.c1; to(1, 2) = pFrom.c2; to(2, 2) = pFrom.c3; to(3, 2) = pFrom.c4;
    to(0, 3) = pFrom.d1; to(1, 3) = pFrom.d2; to(2, 3) = pFrom.d3; to(3, 3) = pFrom.d4;
    return to;
}

Rendering::Data::BoundingBox ModelLoader::convertBoundingBox(const aiAABB& pFrom)
{
    return { {pFrom.mMin.x, pFrom.mMin.y, pFrom.mMin.z}, {pFrom.mMax.x, pFrom.mMax.y, pFrom.mMax.z} };
}