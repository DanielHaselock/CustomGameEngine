#include "Physics/Renderer/PhysicsDebugDrawer.h"
#include "Rendering/Resources/VK/PipeLineBuilder.h"
#include "Rendering/Renderer/VK/VKRenderer.h"
#include "Rendering/Buffers/VK/VKVertexBuffer.h"
#include "Rendering/Renderer/VK/VKRenderer.h"

using namespace Physics::Renderer;

PhysicsDebugDrawer::PhysicsDebugDrawer(void* pRenderer)
	: mUniformBuffer(VK_SHADER_STAGE_VERTEX_BIT), mMat(Rendering::Renderer::Resources::VK::PipeLineBuilder()
		.initPipeLine("Resources/Engine/Shaders/LineVertex.vert.spv",
			"Resources/Engine/Shaders/LineFrag.frag.spv", ((Rendering::Renderer::VK::VKRenderer*)pRenderer)->mRenderPass, true, true)), mRenderer(pRenderer)
{
	mUniformBuffer.mData.mModel = Maths::FMatrix4::createTransformMatrix({}, {}, { Maths::FVector3::One });

	mMat.bindDescriptor("ubo", mUniformBuffer.mDescriptorSets);

	mLineVertices.reserve(MAX_LINE);
	mVertexBuffer = new Rendering::Buffers::VK::VKDynamicVertexBuffer(mLineVertices.data(), mLineVertices.capacity() * sizeof(Rendering::Geometry::DebugVertex));
}

PhysicsDebugDrawer::~PhysicsDebugDrawer()
{
	delete mVertexBuffer;
}

void PhysicsDebugDrawer::drawLine(const btVector3& pFrom, const btVector3& pTo, const btVector3& pFromColor, const btVector3& pToColor)
{
}

void PhysicsDebugDrawer::drawLine(const btVector3& pFrom, const btVector3& pTo, const btVector3& pColor)
{
	mLineVertices.emplace_back(Rendering::Geometry::DebugVertex(pFrom.x(), pFrom.y(), pFrom.z(), pColor.x(), pColor.y(), pColor.z()));
	mLineVertices.emplace_back(Rendering::Geometry::DebugVertex(pTo.x(), pTo.y(), pTo.z(), pColor.x(), pColor.y(), pColor.z()));
}

void PhysicsDebugDrawer::drawSphere(const btVector3& pCenter, btScalar pRadius, const btVector3& pColor)
{
}

void PhysicsDebugDrawer::drawTriangle(const btVector3& pPointA, const btVector3& pPointB, const btVector3& pPointC, const btVector3& pColor, btScalar pAlpha)
{
}

void PhysicsDebugDrawer::drawContactPoint(const btVector3& pPointOnB, const btVector3& pNormalOnB, btScalar pDistance, int pLifeTime, const btVector3& pColor)
{
}

void PhysicsDebugDrawer::reportErrorWarning(const char* pWarningString)
{
}

void PhysicsDebugDrawer::draw3dText(const btVector3& pLocation, const char* pTextString)
{
}

void PhysicsDebugDrawer::setDebugMode(int pDebugMode)
{
	mDebugMode = pDebugMode;
}

int PhysicsDebugDrawer::getDebugMode() const
{
	return mDebugMode;
}

void PhysicsDebugDrawer::flushLines()
{
	if (mLineVertices.size() == 0)
		return;

	Rendering::Buffers::VK::VKBuffer::mapBuffer(mLineVertices.data(), mLineVertices.size() * sizeof(Rendering::Geometry::DebugVertex), mVertexBuffer->mVertexBuffer.mAllocation);

	VkCommandBuffer cmd = ((Rendering::Renderer::VK::VKRenderer*)mRenderer)->getCurrentCommandBuffer();

	mMat.bindPipeLine(cmd);

	mVertexBuffer->bind(cmd);
	vkCmdDraw(cmd, mLineVertices.size(), 1, 0, 0);

	mLineVertices.resize(0);
}

void PhysicsDebugDrawer::updateViewProj(Maths::FMatrix4& pViewProj)
{
	if (mUniformBuffer.mData.mViewProjection == pViewProj)
		return;

	mUniformBuffer.mData.mViewProjection = pViewProj;
	mUniformBuffer.updateData();
}