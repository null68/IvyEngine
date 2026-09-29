#pragma once

#include "../Mesh/MeshData.h"

namespace Engine {
	class Primitives {
	public:
		static std::shared_ptr<MeshData> Cube();
		static std::shared_ptr<MeshData> Plane();
		static std::shared_ptr<MeshData> Quad();
	};
}