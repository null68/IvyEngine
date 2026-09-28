#pragma once

#include "../Mesh/MeshData.h"

namespace Engine {
	class Primitives {
		static MeshData Cube();
		static MeshData Plane();
		static MeshData Quad();
	};
}