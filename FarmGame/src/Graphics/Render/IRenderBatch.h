#pragma once

#include <glm.hpp>
#include "RenderQueue.h"

namespace Engine {
	class IRenderBatch {
	public:
		virtual ~IRenderBatch() = default;

		virtual void Begin() = 0;

		virtual void End() = 0;

		virtual void Flush(const glm::mat4& view, const glm::mat4& projection) = 0;

		virtual bool HasContent() const = 0;

		virtual bool IsFull() const = 0;

		virtual RenderQueue GetQueue() const = 0;

		virtual int GetSortOrder() const { return 0; }
	};
}
