#pragma once
#include "Rendering/Buffers/IIndiceBuffer.h"
#include "Rendering/Data/VKTypes.h"

namespace Rendering::Buffers::VK
{
	class VKIndiceBuffer : public IIndiceBuffer
	{
		public:
			Rendering::Data::AllocatedBuffer mIndicesBuffer;

			VKIndiceBuffer(const void* pData, size_t pSize);
			~VKIndiceBuffer();

			void bind(void* pCommandBuffer) override;
			void unbind() override;
	};
}