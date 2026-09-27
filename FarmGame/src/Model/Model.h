#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <tiny_gltf_v3.h>

#include "../Graphics/Mesh/Mesh.h"
#include "../Graphics/Texture/Texture.h"
#include "../Animation/AnimationClip.h"
#include "../Assets/AssetManager.h"

namespace Engine {

	class EntityManager; // forward decl for Spawn()

	struct ModelNode {
		std::string Name;
		glm::vec3 Translation{ 0.0f };
		glm::vec3 Rotation{ 0.0f };
		glm::vec3 Scale{ 1.0f };
		std::vector<int> MeshIndices;
		std::vector<int> Children;
	};

	class Model {
	private:

		struct RawPrimitiveData {
			std::vector<glm::vec3> Positions;
			std::vector<glm::vec3> Normals;
			std::vector<glm::vec2> UVs;
			std::vector<uint32_t> Indices;
		};

		std::vector<std::shared_ptr<Mesh>> m_Meshes;
		std::vector<RawPrimitiveData> m_RawMeshData;
		std::unordered_map<std::string, size_t> m_MeshLookup;
		std::unordered_map<std::string, size_t> m_NodeLookup;
		mutable std::unordered_map<std::string, std::shared_ptr<Mesh>> m_MergedMeshCache;
		std::vector<std::vector<int>> m_MeshPrimitiveMap;
		std::vector<int> m_MeshTextureIndex;
		std::vector<glm::vec4> m_MeshBaseColor;
		std::vector<std::shared_ptr<Texture>> m_Textures;
		std::vector<ModelNode> m_Nodes;
		std::vector<int> m_RootNodes;
		std::vector<AnimationClip> m_Animations;

		void LoadMaterials(const tg3_model& model, const std::string& baseDir);
		void LoadMeshes(const tg3_model& model);
		void LoadNodes(const tg3_model& model);
		void LoadAnimations(const tg3_model& model);

		void SpawnNodeRecursive(EntityManager* entities, int nodeIndex,
			const glm::mat4& parentWorld, const std::string& shaderName) const;

		void GatherMeshesRecursive(int nodeIndex, const glm::mat4& parentTransform,
			std::vector<std::pair<int, glm::mat4>>& out) const;
		bool GatherMeshesByName(const std::string& name,
			std::vector<std::pair<int, glm::mat4>>& out) const;
	public:
		explicit Model(const std::string& filePath);

		void Spawn(EntityManager* entities, const std::string& shaderName = "BasicShader") const; // useful for animations, every mesh have its own entity
		std::shared_ptr<Mesh> GetMesh(const std::string& name) const; // can work with group with single mesh or more, and just one mesh
		std::shared_ptr<Texture> GetMeshTexture(const std::string& name) const;

		const ModelNode* FindNode(const std::string& name) const;

		const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const { return m_Meshes; };
		const std::vector<std::shared_ptr<Texture>>& GetTextures() const { return m_Textures; }
		const std::vector<int>& GetMeshTextureIndex() const { return m_MeshTextureIndex; }
		const std::vector<glm::vec4>& GetMeshBaseColor() const { return m_MeshBaseColor; }
		const std::vector<ModelNode>& GetNodes() const { return m_Nodes; }
		const std::vector<int>& GetRootNodes() const { return m_RootNodes; }
		const std::vector<AnimationClip>& GetAnimations() const { return m_Animations; }
		const AnimationClip* FindAnimation(const std::string& name) const;
	};
}