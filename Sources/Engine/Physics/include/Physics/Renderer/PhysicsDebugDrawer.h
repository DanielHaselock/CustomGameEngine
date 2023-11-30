#pragma once
#include <Bullet/LinearMath/btIDebugDraw.h>
#include "Rendering/Geometry/DebugVertex.h"
#include "Rendering/Data/UniformData.h"
#include "Rendering/Buffers/VK/VKUniformBuffer.h"
#include "Rendering/Data/Material.h"
#include "Rendering/Buffers/VK/VKDynamicVertexBuffer.h"


namespace Physics::Renderer
{
	class PhysicsDebugDrawer : public btIDebugDraw
	{
	public:
		static const int MAX_LINE = 5000000;

		void* mRenderer;

		std::vector<Rendering::Geometry::DebugVertex> mLineVertices;
		Rendering::Data::Material mMat;
		Rendering::Buffers::VK::VKUniformBuffer<Rendering::Data::UniformData> mUniformBuffer;
		Rendering::Buffers::VK::VKDynamicVertexBuffer* mVertexBuffer = nullptr;
		int mDebugMode = 1;

		PhysicsDebugDrawer(void* pRenderer);

		virtual ~PhysicsDebugDrawer();

		virtual void drawLine(const btVector3& pFrom, const btVector3& pTo, const btVector3& pFromColor, const btVector3& pToColor) override;

		virtual void drawLine(const btVector3& pFrom, const btVector3& pTo, const btVector3& pColor) override;

		virtual void drawSphere(const btVector3& pCenter, btScalar pRadius, const btVector3& pColor) override;

		virtual void drawTriangle(const btVector3& pPointA, const btVector3& pPointB, const btVector3& pPointC, const btVector3& pColor, btScalar pAlpha) override;

		virtual void drawContactPoint(const btVector3& pPointOnB, const btVector3& pNormalOnB, btScalar pDistance, int pLifeTime, const btVector3& pColor) override;

		virtual void reportErrorWarning(const char* pWarningString) override;

		virtual void draw3dText(const btVector3& location, const char* pTextString) override;

		virtual void setDebugMode(int pDebugMode) override;

		virtual int getDebugMode() const override;

		void flushLines() override;

		void updateViewProj(Maths::FMatrix4& pViewProj);
	};
}