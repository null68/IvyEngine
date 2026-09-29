#pragma once

namespace Engine {
	class EntityManager;
	namespace RenderSystems {
		void SubmitMeshes(EntityManager& entities);
		void SubmitParticles(EntityManager& entities);
		void SubmitUI(EntityManager& entities);
	}
}
