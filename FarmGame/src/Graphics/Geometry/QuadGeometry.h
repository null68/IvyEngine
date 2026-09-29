#pragma once

namespace Engine {
	// Shared unit-quad geometry: centered at the origin, corners at (+-0.5, +-0.5),
	// UVs 0..1. This is the base mesh for any GPU-instanced quad batch (particles,
	// UI elements, and anything similar added later) - one draw call per batch still
	// instances this single quad, exactly like MeshBatch instances a Mesh.
	//
	// Lazily created on first use (Get()) and lives for the process lifetime; callers
	// never own or delete it.
	class QuadGeometry {
	public:
		static QuadGeometry& Get();

		// Releases the shared VBO/IBO now. See ShaderStorage::Shutdown for why this needs
		// to be explicit rather than left to this singleton's own static destructor -
		// Renderer::Shutdown calls this.
		static void Shutdown();

		// Binds the shared vertex/index buffers and configures attribute location 0
		// (vec2 position) and location 1 (vec2 uv) on whichever VAO is currently bound.
		// Call this right after binding your own VAO, then configure your batch's
		// per-instance attributes starting at location 2.
		void ConfigureBaseAttributes() const;

		unsigned int GetIndexCount() const { return 6; }

	private:
		QuadGeometry();
		~QuadGeometry();
		QuadGeometry(const QuadGeometry&) = delete;
		QuadGeometry& operator=(const QuadGeometry&) = delete;

		void Release();

		unsigned int m_VBO = 0;
		unsigned int m_IBO = 0;
	};
}
