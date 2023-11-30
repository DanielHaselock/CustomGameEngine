#pragma once

namespace Rendering::Buffers
{
	class IVertexBuffer
	{
		public:
			virtual void bind(void* pCommandBuffer) = 0;
			virtual void unbind() = 0;
	};
}