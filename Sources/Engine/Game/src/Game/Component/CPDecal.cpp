#include "Game/Component/CPDecal.h"

#include <Game/Utils/ComponentType.h>
#include <EngineCore/Service/ServiceLocator.h>
#ifdef NSHIPPING
#include "Editor/Widget/WidgetEditor.h"
#include "Rendering/LineDrawer.h"
#else
#include "EngineCore/Thread/ThreadPool.h"
#endif
#include "EngineCore/Core/EngineApp.h"
#include "Game/SceneSys/SceneManager.h"
#include "EngineCore/ResourceManagement/ResourceManager.h"
#include <filesystem>
#include "Tools/Utils/PathParser.h"

using namespace Game::Component;

#define MAX_VERTICE 10000

CPDecal::CPDecal(EngineCore::EventSystem::Event<Maths::FMatrix4>& pOnValueChanged)
{
	mUniformBuffer = new Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataDecal>(VK_SHADER_STAGE_VERTEX_BIT);

	vertices.reserve(MAX_VERTICE);
	vertices.resize(0);
	mVertexBuffer = new Rendering::Buffers::VK::VKDynamicVertexBuffer(vertices.data(), vertices.capacity() * sizeof(Rendering::Geometry::Vertex));

	mTexture = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Texture>("Resources/Engine/Textures/default.png", "Resources/Engine/Textures/default.png");

#ifdef NSHIPPING
	pOnValueChanged.subscribe([this](Maths::FMatrix4& a) {
		if (previousPos == a)
			return;

		previousPos = a;
		updateDrawer(); 
		
	});

	mCubeDrawer = new Rendering::LineDrawer(*service(Editor::Widget::WidgetSceneApp).renderer);
	mUniformBufferEditor = new Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataDecal>(VK_SHADER_STAGE_VERTEX_BIT);
	updateDrawer();
#endif
}

CPDecal::CPDecal(const CPDecal& pOther) :
	mPath(pOther.mPath), mName(pOther.mName), mSize(pOther.mSize)
{
	isInit = false;
	isCalculating = false;

	mUniformBuffer = new Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataDecal>(VK_SHADER_STAGE_VERTEX_BIT);

	vertices.reserve(MAX_VERTICE);
	vertices.resize(0);
	mVertexBuffer = new Rendering::Buffers::VK::VKDynamicVertexBuffer(vertices.data(), vertices.capacity() * sizeof(Rendering::Geometry::Vertex));

	mTexture = pOther.mTexture;

#ifdef NSHIPPING
	mCubeDrawer = new Rendering::LineDrawer(*service(Editor::Widget::WidgetSceneApp).renderer);
	mUniformBufferEditor = new Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformDataDecal>(VK_SHADER_STAGE_VERTEX_BIT);
	//updateDrawer();
#endif

	//Maths::FMatrix4 previousPos;
}

CPDecal::~CPDecal()
{
	delete mUniformBuffer;

#ifdef NSHIPPING
	delete mCubeDrawer;
	mCubeDrawer = nullptr;
	delete mUniformBufferEditor;
#endif
	if (mVertexBuffer != nullptr)
		delete mVertexBuffer;
}

AComponent* CPDecal::clone()
{
	return new CPDecal(*this);
}

void CPDecal::setDecal(const std::string& pName, const char* pModel)
{
	mTexture = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Texture>(pModel, pModel);
	mName = pName;
	mPath = pModel;

	if (mTexture == nullptr)
	{
		mName = "Unknown";
		mPath = "";
		return;
	}
}

void CPDecal::setDecalWithPath(const char* pModel)
{
	if (pModel == nullptr)
	{
		mTexture = nullptr;
		mName = "";
		mPath = "";
		return;
	}

	mTexture = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Texture>(pModel, pModel);
	mPath = pModel;

	if (mTexture == nullptr)
		mPath = "";
}

void CPDecal::setDecalWithPathLua(const char* pPath)
{
	std::string currentPath = service(Game::SceneSys::SceneManager).mProjectPath + "/";
	mTexture = service(EngineCore::ResourcesManagement::ResourceManager).create<Rendering::Resources::Texture>(currentPath + pPath, currentPath + pPath);
	mPath = currentPath + pPath;

	if (mTexture == nullptr)
		mPath = "";
}

struct Plane {
	Maths::FVector3 normal;
	float d;
};

struct Triangle {
	Maths::FVector3 a, b, c;
	Maths::FVector2 uv_a, uv_b, uv_c;
};

// Helper function to calculate the dot product of two vectors
float dotProduct(const Maths::FVector3& a, const Maths::FVector3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Helper function to calculate the intersection point of a line segment and a plane
Maths::FVector3 intersect(const Maths::FVector3& a, const Maths::FVector3& b, const Plane& plane) {
    Maths::FVector3 intersection;
    Maths::FVector3 direction = b - a;
    float t = (plane.d - dotProduct(plane.normal, a)) / dotProduct(plane.normal, direction);
    intersection.x = a.x + t * direction.x;
    intersection.y = a.y + t * direction.y;
    intersection.z = a.z + t * direction.z;
    return intersection;
}

Maths::FVector3 calculatePlaneNormal(const Maths::FVector3& v1, const Maths::FVector3& v2, const Maths::FVector3& v3) {
	Maths::FVector3 edge1 = { v2.x - v1.x, v2.y - v1.y, v2.z - v1.z };
	Maths::FVector3 edge2 = { v3.x - v1.x, v3.y - v1.y, v3.z - v1.z };

	Maths::FVector3 normal;
	normal.x = edge1.y * edge2.z - edge1.z * edge2.y;
	normal.y = edge1.z * edge2.x - edge1.x * edge2.z;
	normal.z = edge1.x * edge2.y - edge1.y * edge2.x;

	float length = std::sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));
	if (length > 0) {
		normal.x /= length;
		normal.y /= length;
		normal.z /= length;
	}
	else {
		normal.x = 0.0f;
		normal.y = 0.0f;
		normal.z = 0.0f;
	}

	return normal;
}

float map(float value, float low1, float high1, float low2, float high2)
{
	return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

Maths::FVector2 generateUV(Maths::FVector3& vertex, const Maths::FVector3& vertexA, const Maths::FVector3& vertexB, const Maths::FVector3& vertexC) {
	Maths::FVector2 uv;

	Maths::FVector3 normal = calculatePlaneNormal(vertexA, vertexB, vertexC);

	if (normal.x == 1 || normal.x == -1)
	{
		/*if (normal.x == 1)
			vertex.x += 0.09f;
		else
			vertex.x -= 0.09f;*/

		uv.x = map(vertex.z * -1, -1, 1, 0, 1);
		uv.y = map(vertex.y * -1, -1, 1, 0, 1);
	}
	else if (normal.y == 1 || normal.y == -1)
	{
		/*if (normal.y == 1)
			vertex.y += 0.09f;
		else
			vertex.y -= 0.09f;*/

		uv.x = map(vertex.x * -1, -1, 1, 0, 1);//(vertex.x - minBox.x) / (maxBox.x - minBox.x);
		uv.y = map(vertex.z * -1, -1, 1, 0, 1);//1 - (vertex.z - minBox.z) / (maxBox.z - minBox.z);
	}
	else if (normal.z == 1 || normal.z == -1)
	{
		/*if (normal.z == 1)
			vertex.z -= 0.09f;
		else
			vertex.z += 0.09f;*/

		uv.x = map(vertex.x * -1, -1, 1, 0, 1);//(vertex.x - minBox.x) / (maxBox.x - minBox.x);
		uv.y = map(vertex.y * -1, -1, 1, 0, 1);//1 - (vertex.y - minBox.y) / (maxBox.y - minBox.y);
	}


	return uv;
}

// Clip the triangle against a single plane
std::vector<Triangle> clipAgainstPlane(const std::vector<Triangle>& polygons, const Plane& plane) {
	std::vector<Triangle> clippedPolygons;

	for (const Triangle& polygon : polygons) {
		std::vector<Maths::FVector3> clippedVertices;

		Maths::FVector3 prevPoint = polygon.c;
		float prevDist = dotProduct(prevPoint, plane.normal) - plane.d;

		for (int i = 0; i < 3; i++) {
			const Maths::FVector3& currPoint = (i == 0) ? polygon.a : ((i == 1) ? polygon.b : polygon.c);

			float currDist = dotProduct(currPoint, plane.normal) - plane.d;

			if (currDist < 0) {
				if (prevDist >= 0) {
					Maths::FVector3 intersection = intersect(prevPoint, currPoint, plane);
					clippedVertices.push_back(intersection);
				}
				clippedVertices.push_back(currPoint);
			}
			else if (prevDist < 0) {
				Maths::FVector3 intersection = intersect(prevPoint, currPoint, plane);
				clippedVertices.push_back(intersection);
			}

			prevPoint = currPoint;
			prevDist = currDist;
		}

		if (clippedVertices.size() == 3) {
			Triangle clippedTriangle;
			clippedTriangle.a = clippedVertices[0];
			clippedTriangle.b = clippedVertices[1];
			clippedTriangle.c = clippedVertices[2];
			clippedPolygons.push_back(clippedTriangle);
		}
		else if (clippedVertices.size() == 4) {
			// If the polygon is a quad, split it into two triangles
			Triangle clippedTriangle1;
			clippedTriangle1.a = clippedVertices[0];
			clippedTriangle1.b = clippedVertices[1];
			clippedTriangle1.c = clippedVertices[2];
			clippedPolygons.push_back(clippedTriangle1);

			Triangle clippedTriangle2;
			clippedTriangle2.a = clippedVertices[2];
			clippedTriangle2.b = clippedVertices[3];
			clippedTriangle2.c = clippedVertices[0];
			clippedPolygons.push_back(clippedTriangle2);
		}
		else if (clippedVertices.size() == 5) {
			// If the polygon has five vertices, split it into three triangles
			Triangle clippedTriangle1;
			clippedTriangle1.a = clippedVertices[0];
			clippedTriangle1.b = clippedVertices[1];
			clippedTriangle1.c = clippedVertices[2];
			clippedPolygons.push_back(clippedTriangle1);

			Triangle clippedTriangle2;
			clippedTriangle2.a = clippedVertices[2];
			clippedTriangle2.b = clippedVertices[3];
			clippedTriangle2.c = clippedVertices[4];
			clippedPolygons.push_back(clippedTriangle2);

			Triangle clippedTriangle3;
			clippedTriangle3.a = clippedVertices[4];
			clippedTriangle3.b = clippedVertices[0];
			clippedTriangle3.c = clippedVertices[2];
			clippedPolygons.push_back(clippedTriangle3);
		}
		else if (clippedVertices.size() == 6) {
			// If the polygon has six vertices, split it into four triangles
			Triangle clippedTriangle1;
			clippedTriangle1.a = clippedVertices[0];
			clippedTriangle1.b = clippedVertices[1];
			clippedTriangle1.c = clippedVertices[2];
			clippedPolygons.push_back(clippedTriangle1);

			Triangle clippedTriangle2;
			clippedTriangle2.a = clippedVertices[2];
			clippedTriangle2.b = clippedVertices[3];
			clippedTriangle2.c = clippedVertices[4];
			clippedPolygons.push_back(clippedTriangle2);

			Triangle clippedTriangle3;
			clippedTriangle3.a = clippedVertices[4];
			clippedTriangle3.b = clippedVertices[5];
			clippedTriangle3.c = clippedVertices[0];
			clippedPolygons.push_back(clippedTriangle3);

			Triangle clippedTriangle4;
			clippedTriangle4.a = clippedVertices[0];
			clippedTriangle4.b = clippedVertices[2];
			clippedTriangle4.c = clippedVertices[4];
			clippedPolygons.push_back(clippedTriangle4);
		}
	}

	return clippedPolygons;
}

// Clip the triangle against the 3D box
std::vector<Triangle> clipTriangle(const Triangle& triangle, const Maths::FVector3& minBox, const Maths::FVector3& maxBox) {
	std::vector<Triangle> polygon;
	polygon.push_back(triangle);

	std::vector<Triangle> clippedPolygon = polygon;

	// Define the six planes of the box
	std::vector<Plane> planes = {
		{{1, 0, 0}, maxBox.x},
		{{-1, 0, 0}, -minBox.x},
		{{0, 1, 0}, maxBox.y},
		{{0, -1, 0}, -minBox.y},
		{{0, 0, 1}, maxBox.z},
		{{0, 0, -1}, -minBox.z}
	};

	for (const Plane& plane : planes) {
		clippedPolygon = clipAgainstPlane(clippedPolygon, plane);
		if (clippedPolygon.empty()) {
			break;
		}
	}

	return clippedPolygon;
}

bool isOverlapping1D(float minX1, float maxX1, float minX2, float maxX2)
{
	return maxX1 >= minX2 && maxX2 >= minX1;
} 

bool isOverlapping2D(float minX1, float maxX1, float minY1, float maxY1, float minX2, float maxX2, float minY2, float maxY2)
{
	return isOverlapping1D(minX1, maxX1, minX2, maxX2) &&
		isOverlapping1D(minY1, maxY1, minY2, maxY2);
}

bool isOverlapping3D(const Maths::FVector3& minBox1, const Maths::FVector3& maxBox1, const Maths::FVector3& minBox, const Maths::FVector3& maxBox)
{
	return isOverlapping1D(minBox1.x, maxBox1.x, minBox.x, maxBox.x) &&
		isOverlapping1D(minBox1.y, maxBox1.y, minBox.y, maxBox.y) &&
		isOverlapping1D(minBox1.z, maxBox1.z, minBox.z, maxBox.z);
}

bool areBoundingBoxesTouching(const Maths::FVector3& minBox1, const Maths::FVector3& maxBox1, const Maths::FVector3& minBox, const Maths::FVector3& maxBox) {
	// Check for overlap in the x-axis
	if (minBox1.x > maxBox.x || maxBox1.x < minBox.x)
		return false;

	// Check for overlap in the y-axis
	if (minBox1.y > maxBox.y || maxBox1.y < minBox.y)
		return false;

	// Check for overlap in the z-axis
	if (minBox1.z > maxBox.z || maxBox1.z < minBox.z)
		return false;

	// If there is overlap in all three axes, the boxes are touching
	return true;
}

#ifdef NSHIPPING
void CPDecal::updateDrawerLine()
{
	if (!isActive)
		return;

	Game::Data::Actor* actor = (Game::Data::Actor*)mOwner;
	if (actor == nullptr)
		return;

	mCubeDrawer->reset();
	Maths::FVector3& pos = actor->getTransform()->mWorldPosition;

	Maths::FVector3 min = { pos.x - (mSize.x), pos.y - (mSize.y), pos.z - (mSize.z) };
	Maths::FVector3 max = { pos.x + (mSize.x), pos.y + (mSize.y), pos.z + (mSize.z) };

	mCubeDrawer->drawLine(min, { min.x, min.y, max.z }, { 0, 1, 0 });
	mCubeDrawer->drawLine(min, { min.x, max.y, min.z }, { 0, 1, 0 });
	mCubeDrawer->drawLine(min, { max.x, min.y, min.z }, { 0, 1, 0 });

	mCubeDrawer->drawLine(max, { max.x, max.y, min.z }, { 0, 1, 0 });
	mCubeDrawer->drawLine(max, { max.x, min.y, max.z }, { 0, 1, 0 });
	mCubeDrawer->drawLine(max, { min.x, max.y, max.z }, { 0, 1, 0 });

	mCubeDrawer->drawLine({ min.x, max.y, max.z }, { min.x, max.y, min.z }, { 0, 1, 0 });
	mCubeDrawer->drawLine({ max.x, max.y, min.z }, { min.x, max.y, min.z }, { 0, 1, 0 });

	mCubeDrawer->drawLine({ min.x, max.y, max.z }, { min.x, min.y, max.z }, { 0, 1, 0 });
	mCubeDrawer->drawLine({ max.x, max.y, min.z }, { max.x, min.y, min.z }, { 0, 1, 0 });
			   
	mCubeDrawer->drawLine({ max.x, min.y, max.z }, { min.x, min.y, max.z }, { 0, 1, 0 });
	mCubeDrawer->drawLine({ max.x, min.y, max.z }, { max.x, min.y, min.z }, { 0, 1, 0 });
}

void CPDecal::drawLine(Maths::FMatrix4& pViewProj)
{
	if (!isActive)
		return;

	if (mCubeDrawer != nullptr)
	{
		if (!isInit)
			updateDrawer();

		mCubeDrawer->updateViewProj(pViewProj);
		mCubeDrawer->flushLines();
	}
}

void CPDecal::drawEditor(VkCommandBuffer pCmd, Rendering::Data::Material& pPipeLine, Maths::FMatrix4& pViewProj)
{
	if (!isActive || isCalculating)
		return;

#ifdef NSHIPPING
	drawLine(pViewProj);
#endif

	if (vertices.size() == 0)
		return;

	Game::Data::Actor* actor = (Game::Data::Actor*)mOwner;
	if (actor == nullptr)
		return;

	mUniformBufferEditor->mData.mViewProjection = pViewProj;
	mUniformBufferEditor->mData.mModel = actor->getTransform()->mWorldMatrix;
	mUniformBufferEditor->updateData();

	pPipeLine.bindDescriptor("texSampler", mTexture->mTextureSets);
	pPipeLine.bindDescriptor("ubo", mUniformBufferEditor->mDescriptorSets);
	pPipeLine.bindPipeLine(pCmd);

	mVertexBuffer->bind(pCmd);

	vkCmdSetDepthBias(pCmd, 8, 0, -2.5f);
	vkCmdDraw(pCmd, vertices.size(), 1, 0, 0);
	vkCmdSetDepthBias(pCmd, 0, 0, 0);
}
#endif

void CPDecal::updateDrawer()
{
	if (!isActive || isCalculating)
		return;

	service(EngineCore::Thread::ThreadPool).queueJob([this] {
		isCalculating = true;

	#ifdef NSHIPPING
		updateDrawerLine();
	#endif

		vertices.resize(0);

		Game::Data::Actor* actor = (Game::Data::Actor*)mOwner;
		if (actor == nullptr)
			return;

		Maths::FVector3& pos = actor->getTransform()->mWorldPosition;
		Maths::FMatrix4 inv = actor->getTransform()->mWorldMatrix.inverse();

		std::list<Game::Data::Actor*>& actors = service(Game::SceneSys::SceneManager).mCurrentScene->mActors;
		for (auto actor : actors)
		{
			if (actor->mComponents.find(Utils::ComponentType::MeshRenderer) == actor->mComponents.end())
				continue;

			Maths::FMatrix4& modelView = actor->getTransform()->getWorldMatrix();

			for (Game::Component::AComponent* aModel : actor->mComponents[Utils::ComponentType::MeshRenderer])
			{
				Game::Component::CPModel* model = (Game::Component::CPModel*)aModel;

				if (!model->canReceiveDecal)
					continue;

				/*if (!areBoundingBoxesTouching(model->mModel->mBox, pos - (mSize), pos + (mSize)))
					continue;*/

				for (auto& mesh : model->mModel->mMeshes)
				{
					/*Maths::FVector4 min = modelView * Maths::FVector4(mesh.mBoundingBox.mMin.x, mesh.mBoundingBox.mMin.y, mesh.mBoundingBox.mMin.z, 1);
					Maths::FVector4 max = modelView * Maths::FVector4(mesh.mBoundingBox.mMax.x, mesh.mBoundingBox.mMax.y, mesh.mBoundingBox.mMax.z, 1);*/

					Maths::FVector3 min = mesh->mBoundingBox.mMin + actor->getTransform()->getWorldPosition();
					Maths::FVector3 max = mesh->mBoundingBox.mMax + actor->getTransform()->getWorldPosition();

					/*if (!isOverlapping3D(Maths::FVector3(min.x, min.y, min.z), Maths::FVector3(max.x, max.y, max.z), pos - (mSize), pos + (mSize)))
						continue;*/

					for (int i = 0; i < mesh->mIndices.size(); i += 3)
					{
						Maths::FVector3& p1 = mesh->mVertices[mesh->mIndices[i]].mPosition;
						Maths::FVector3& p2 = mesh->mVertices[mesh->mIndices[i + 1]].mPosition;
						Maths::FVector3& p3 = mesh->mVertices[mesh->mIndices[i + 2]].mPosition;

						Maths::FVector3 p1_c;
						Maths::FVector3 p2_c;
						Maths::FVector3 p3_c;

						//point 1
						Maths::FVector4 decalPos1;
						{
							Maths::FVector4 worldPos = modelView * Maths::FVector4(p1.x, p1.y, p1.z, 1);
							
							p1_c.x = worldPos.x;
							p1_c.y = worldPos.y;
							p1_c.z = worldPos.z;

							if (Maths::FVector3::length(p1_c - pos) > 10)
								continue;
						}

						//point 2
						Maths::FVector4 decalPos2;
						{
							Maths::FVector4 worldPos = modelView * Maths::FVector4(p2.x, p2.y, p2.z, 1);
							p2_c.x = worldPos.x;
							p2_c.y = worldPos.y;
							p2_c.z = worldPos.z;

							if (Maths::FVector3::length(p2_c - pos) > 10)
								continue;
						}

						//point 3
						Maths::FVector4 decalPos3;
						{
							Maths::FVector4 worldPos = modelView * Maths::FVector4(p3.x, p3.y, p3.z, 1);
							p3_c.x = worldPos.x;
							p3_c.y = worldPos.y;
							p3_c.z = worldPos.z;

							if (Maths::FVector3::length(p3_c - pos) > 10)
								continue;
						}

						// Define a triangle outside the cube
						Triangle triangle =
						{
							{p1_c.x, p1_c.y, p1_c.z},
							{p2_c.x, p2_c.y, p2_c.z},
							{p3_c.x, p3_c.y, p3_c.z},
						};

						// Clip the triangle against the 3D box
						std::vector<Triangle> clippedTriangle = clipTriangle(triangle, pos - (mSize), pos + (mSize));
						for (int i = 0; i < clippedTriangle.size(); i++)
						{
							{
								Rendering::Geometry::Vertex v;

								Maths::FVector4 posF = inv * Maths::FVector4(clippedTriangle[i].a.x, clippedTriangle[i].a.y, clippedTriangle[i].a.z, 1);
								v.mPosition = Maths::FVector3(posF.x, posF.y, posF.z);
								v.mTexCoords = generateUV(v.mPosition, clippedTriangle[i].a, clippedTriangle[i].b, clippedTriangle[i].c);
							
								vertices.push_back(v);
							}

							{
								Rendering::Geometry::Vertex v;
								Maths::FVector4 posF = inv * Maths::FVector4(clippedTriangle[i].b.x, clippedTriangle[i].b.y, clippedTriangle[i].b.z, 1);
								v.mPosition = Maths::FVector3(posF.x, posF.y, posF.z);
								v.mTexCoords = generateUV(v.mPosition, clippedTriangle[i].a, clippedTriangle[i].b, clippedTriangle[i].c);
							
								vertices.push_back(v);
							}

							{
								Rendering::Geometry::Vertex v;
								Maths::FVector4 posF = inv * Maths::FVector4(clippedTriangle[i].c.x, clippedTriangle[i].c.y, clippedTriangle[i].c.z, 1);
								v.mPosition = Maths::FVector3(posF.x, posF.y, posF.z);
								v.mTexCoords = generateUV(v.mPosition, clippedTriangle[i].a, clippedTriangle[i].b, clippedTriangle[i].c);
							
								vertices.push_back(v);
							}
						}
					}
				}
			}
		}

		Rendering::Buffers::VK::VKBuffer::mapBuffer(vertices.data(), vertices.size() * sizeof(Rendering::Geometry::Vertex), mVertexBuffer->mVertexBuffer.mAllocation);

		isInit = true;
		isCalculating = false;
	});
}

void CPDecal::draw(VkCommandBuffer pCmd, Rendering::Data::Material& pPipeLine, Maths::FMatrix4& pViewProj)
{
	if (!isActive || isCalculating)
		return;

	if (vertices.size() == 0)
		return;

	Game::Data::Actor* actor = (Game::Data::Actor*)mOwner;
	if (actor == nullptr)
		return;

	mUniformBuffer->mData.mViewProjection = pViewProj;
	mUniformBuffer->mData.mModel = actor->getTransform()->mWorldMatrix;
	mUniformBuffer->updateData();

	pPipeLine.bindDescriptor("texSampler", mTexture->mTextureSets);
	pPipeLine.bindDescriptor("ubo", mUniformBuffer->mDescriptorSets);
	pPipeLine.bindPipeLine(pCmd);

	mVertexBuffer->bind(pCmd);

	vkCmdSetDepthBias(pCmd, 8, 0, -2.5f);
	vkCmdDraw(pCmd, vertices.size(), 1, 0, 0);
	vkCmdSetDepthBias(pCmd, 0, 0, 0);
}

void CPDecal::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& pWriter)
{
	pWriter.StartObject();

	pWriter.Key("Type");
	pWriter.Int((int)Game::Utils::ComponentType::Decal);

	pWriter.Key("Active");
	pWriter.Bool(isActive);

	pWriter.Key("Name");
	pWriter.String(mName.c_str());

#ifdef NSHIPPING
	std::string currentPath = Editor::Utils::qStringToStdString(service(Editor::Widget::WidgetEditor).mLocation.mFolder);
	pWriter.Key("Path");
	pWriter.String(mPath.empty() ? "" : mPath.substr(currentPath.length()).c_str());
#endif

	pWriter.Key("Size");
	pWriter.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
		pWriter.StartArray();
		pWriter.Double(mSize.x);
		pWriter.Double(mSize.y);
		pWriter.Double(mSize.z);
	pWriter.EndArray();
	pWriter.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);

	pWriter.EndObject();
}