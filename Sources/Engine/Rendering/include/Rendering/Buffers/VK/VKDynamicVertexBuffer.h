#pragma once
#include "Rendering/Buffers/IVertexBuffer.h"
#include "Rendering/Data/VKTypes.h"

namespace Rendering::Buffers::VK
{
	class VKDynamicVertexBuffer : public IVertexBuffer
	{
		public:
			Rendering::Data::AllocatedBuffer mVertexBuffer;

			VKDynamicVertexBuffer(const void* pData, size_t pSize);
			~VKDynamicVertexBuffer();

			void bind(void* pCommandBuffer) override;
			void unbind() override;
	};
}