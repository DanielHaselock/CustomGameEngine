#pragma once
#include "Rendering/Buffers/IVertexBuffer.h"
#include "Rendering/Data/VKTypes.h"

namespace Rendering::Buffers::VK
{
	class VKVertexBuffer : public IVertexBuffer
	{
		public:
			Rendering::Data::AllocatedBuffer mVertexBuffer;

			VKVertexBuffer(const void* pData, size_t pSize);
			~VKVertexBuffer();

			void bind(void* pCommandBuffer) override;
			void unbind() override;
	};
}