#include "Model.h"
#include "../Graphics/Texture/TextureStorage.h"
#include "../ECS/EntityManager.h"
#include "../ECS/Components/TransformComponent.h"
#include "../ECS/Components/MeshComponent.h"
#include "../ECS/Components/MaterialComponent.h"
#include "../Graphics/Shader/ShaderStorage.h"

#include <fstream>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <utility>
#include <stb_image.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <gtc/matrix_transform.hpp>
#include <gtc/matrix_inverse.hpp>
#include <gtx/matrix_decompose.hpp>
#include <gtx/quaternion.hpp>
#include <gtc/type_ptr.hpp>

namespace Engine {

	std::vector<unsigned char> DecodeBase64(const std::string& in) {
		static int lookup[256];
		static bool initialized = false;
		if (!initialized) {
			std::fill(std::begin(lookup), std::end(lookup), -1);
			const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
			for (int i = 0; i < 64; i++) lookup[(unsigned char)chars[i]] = i;
			initialized = true;
		}

		std::vector<unsigned char> out;
		int val = 0, bits = -8;
		for (unsigned char c : in) {
			if (c == '=') break;
			if (lookup[c] == -1) continue;
			val = (val << 6) + lookup[c];
			bits += 6;
			if (bits >= 0) {
				out.push_back((unsigned char)((val >> bits) & 0xFF));
				bits -= 8;
			}
		}
		return out;
	}

	std::vector<unsigned char> DecodeDataUri(const std::string& uri) {
		size_t idx = uri.find(',');
		if (idx == std::string::npos) return {};
		return DecodeBase64(uri.substr(idx + 1));
	}

	std::string UrlDecode(const std::string& in) {
		std::string out;

		for (size_t i = 0; i < in.size(); i++) {
			if (in[i] == '%' && i + 2 < in.size()) {
				out += (char)std::stoi(in.substr(i + 1, 2), nullptr, 16);
				i += 2;
			}
			else if (in[i] == '+') {
				out += ' ';
			}
			else {
				out += in[i];
			}
		}
		return out;
	}

	std::string ToStdString(tg3_str string) {
		return string.len > 0 ? std::string(string.data, string.len) : std::string();
	}

	std::vector<unsigned char> ReadFileBytes(const std::string& path) {
		std::ifstream file(path, std::ios::binary | std::ios::ate);

		if (!file.is_open()) return {};

		std::streamsize ss = file.tellg();

		file.seekg(0, std::ios::beg);

		std::vector<unsigned char> buffer((size_t)ss);

		if (!file.read(reinterpret_cast<char*>(buffer.data()), ss)) return {};

		return buffer;
	}

	int FindAttribute(const tg3_primitive& primitive, const char* name) {
		for (uint32_t i = 0; i < primitive.attributes_count; i++)
			if (tg3_str_equals_cstr(primitive.attributes[i].key, name))
				return primitive.attributes[i].value;
		return -1;
	}

	template<typename T>
	std::vector<T> ReadAccessor(const tg3_model& model, int accessorIndex) {
		const tg3_accessor& accessor = model.accessors[accessorIndex];
		const tg3_buffer_view& view = model.buffer_views[accessor.buffer_view];
		const tg3_buffer& buffer = model.buffers[view.buffer];

		const unsigned char* base = buffer.data.data + view.byte_offset + accessor.byte_offset;

		int32_t stride = tg3_accessor_byte_stride(&accessor, &view);
		if (stride <= 0) stride = (int32_t)sizeof(T);

		std::vector<T> out(accessor.count);
		for (uint64_t i = 0; i < accessor.count; i++)
			std::memcpy(&out[i], base + i * stride, sizeof(T));
		return out;
	}

	std::vector<uint32_t> ReadIndices(const tg3_model& model, int accessorIndex) {
		const tg3_accessor& accessor = model.accessors[accessorIndex];
		std::vector<uint32_t> out;

		switch (accessor.component_type) {
		case TG3_COMPONENT_TYPE_UNSIGNED_BYTE: {
			auto v = ReadAccessor<uint8_t>(model, accessorIndex);
			out.assign(v.begin(), v.end());
			break;
		}
		case TG3_COMPONENT_TYPE_UNSIGNED_SHORT: {
			auto v = ReadAccessor<uint16_t>(model, accessorIndex);
			out.assign(v.begin(), v.end());
			break;
		}
		default:
			out = ReadAccessor<uint32_t>(model, accessorIndex);
			break;
		}
		return out;
	}

	Model::Model(const std::string& filePath) {
		tg3_model model;

		if (!AssetManager::LoadGLTFModel(filePath, model)) {
			std::cerr << "Failed to load model: " << filePath << std::endl;
			return;
		}

		std::string baseDir;

		size_t idx = filePath.find_last_of("/\\");
		if (idx != std::string::npos) baseDir = filePath.substr(0, idx);

		LoadMaterials(model, baseDir);
		LoadMeshes(model);
		LoadNodes(model);
		LoadAnimations(model);

		tg3_model_free(&model);
	}

	void Model::LoadMaterials(const tg3_model& model, const std::string& baseDir) {
		m_Textures.resize(model.images_count);

		for (uint32_t i = 0; i < model.images_count; i++) {
			const tg3_image& image = model.images[i];

			std::vector<unsigned char> encoded;

			if (image.buffer_view >= 0) {
				const tg3_buffer_view& view = model.buffer_views[image.buffer_view];

				const tg3_buffer& buffer = model.buffers[view.buffer];

				const unsigned char* src = buffer.data.data + view.byte_offset;

				encoded.assign(src, src + view.byte_length);
			}
			else if (image.uri.len > 0) {
				std::string uri = ToStdString(image.uri);

				if (tg3_is_data_uri(image.uri.data, image.uri.len)) {
					encoded = DecodeDataUri(uri);
				}
				else {
					encoded = ReadFileBytes(baseDir + "/" + UrlDecode(uri));
				}
			}

			if (encoded.empty())
				continue;

			int width;
			int height;
			int channels;

			unsigned char* decoded = stbi_load_from_memory(encoded.data(), static_cast<int>(encoded.size()), &width, &height, &channels, 0);

			if (!decoded) {
				std::cerr << "Failed to decode image " << i << std::endl;
				continue;
			}

			std::string textureName = "ModelTexture_" + std::to_string(i);

			m_Textures[i] =
				TextureStorage::GetInstance().Load(textureName, decoded, width, height, channels);

			stbi_image_free(decoded);
		}
	}

	void Model::LoadMeshes(const tg3_model& model) {
		m_MeshPrimitiveMap.resize(model.meshes_count);

		for (uint32_t i = 0; i < model.meshes_count; i++) {
			const tg3_mesh& mesh = model.meshes[i];
			const std::string meshName = ToStdString(mesh.name);

			for (uint32_t j = 0; j < mesh.primitives_count; j++) {
				const tg3_primitive& primitive = mesh.primitives[j];

				if (primitive.mode != -1 && primitive.mode != TG3_MODE_TRIANGLES)
					continue;

				int accessorPosition = FindAttribute(primitive, "POSITION");
				if (accessorPosition < 0)
					continue;

				auto positions = ReadAccessor<glm::vec3>(model, accessorPosition);

				std::vector<glm::vec3> normals(positions.size(), glm::vec3(0.0f));
				std::vector<glm::vec2> uvs(positions.size(), glm::vec2(0.0f));

				int accessorNormal = FindAttribute(primitive, "NORMAL");
				if (accessorNormal >= 0)
					normals = ReadAccessor<glm::vec3>(model, accessorNormal);

				int accessorUv = FindAttribute(primitive, "TEXCOORD_0");
				if (accessorUv >= 0)
					uvs = ReadAccessor<glm::vec2>(model, accessorUv);

				MeshData data;
				data.vertices.reserve(positions.size());

				for (size_t v = 0; v < positions.size(); v++) {
					data.vertices.emplace_back(positions[v], normals[v], uvs[v]);
				}

				if (primitive.indices >= 0) {
					data.indices = ReadIndices(model, primitive.indices);
				}
				else {
					data.indices.resize(positions.size());
					std::iota(data.indices.begin(), data.indices.end(), 0);
				}

				int textureIndex = -1;
				glm::vec4 baseColor(1.0f);

				if (primitive.material >= 0) {
					const tg3_material& material = model.materials[primitive.material];
					const auto& pbr = material.pbr_metallic_roughness;

					baseColor = glm::vec4(
						(float)pbr.base_color_factor[0], (float)pbr.base_color_factor[1],
						(float)pbr.base_color_factor[2], (float)pbr.base_color_factor[3]);

					int texRef = pbr.base_color_texture.index;
					if (texRef >= 0) textureIndex = model.textures[texRef].source;
				}

				int meshIndex = static_cast<int>(m_Meshes.size());

				m_Meshes.push_back(std::make_shared<Mesh>(data));
				m_MeshTextureIndex.push_back(textureIndex);
				m_MeshBaseColor.push_back(baseColor);
				m_MeshPrimitiveMap[i].push_back(meshIndex);

				RawPrimitiveData raw;
				raw.Positions = std::move(positions);
				raw.Normals = std::move(normals);
				raw.UVs = std::move(uvs);
				raw.Indices = data.indices;
				m_RawMeshData.push_back(std::move(raw));

				if (!meshName.empty() && m_MeshLookup.find(meshName) == m_MeshLookup.end())
					m_MeshLookup[meshName] = meshIndex;
			}
		}
	}

	void Model::LoadNodes(const tg3_model& model) {
		m_Nodes.resize(model.nodes_count);

		for (uint32_t i = 0; i < model.nodes_count; i++) {
			const tg3_node& node = model.nodes[i];
			auto& out = m_Nodes[i];

			out.Name = node.name.len > 0 ? ToStdString(node.name) : ("Node" + std::to_string(i));
			out.Children.assign(node.children, node.children + node.children_count);
			if (node.mesh >= 0) out.MeshIndices = m_MeshPrimitiveMap[node.mesh];

			if (node.has_matrix) {
				float m[16];
				for (int k = 0; k < 16; k++) m[k] = (float)node.matrix[k];
				glm::mat4 matrix = glm::make_mat4(m);
				glm::vec3 skew; glm::vec4 perspective; glm::quat rotation;
				glm::decompose(matrix, out.Scale, rotation, out.Translation, skew, perspective);
				out.Rotation = glm::eulerAngles(rotation);
			}
			else {
				out.Translation = { (float)node.translation[0], (float)node.translation[1], (float)node.translation[2] };
				out.Scale = { (float)node.scale[0], (float)node.scale[1], (float)node.scale[2] };
				glm::quat q((float)node.rotation[3], (float)node.rotation[0], (float)node.rotation[1], (float)node.rotation[2]);
				out.Rotation = glm::eulerAngles(q);
			}

			if (m_NodeLookup.find(out.Name) == m_NodeLookup.end())
				m_NodeLookup[out.Name] = i;
		}

		if (model.scenes_count > 0) {
			int sceneIdx = model.default_scene >= 0 ? model.default_scene : 0;
			const tg3_scene& scene = model.scenes[sceneIdx];
			m_RootNodes.assign(scene.nodes, scene.nodes + scene.nodes_count);
		}
	}

	void Model::LoadAnimations(const tg3_model& model) {
		for (uint32_t a = 0; a < model.animations_count; a++) {
			const tg3_animation& anim = model.animations[a];
			AnimationClip clip;
			clip.Name = anim.name.len > 0 ? ToStdString(anim.name) : "Animation";
			float maxTime = 0.0f;

			for (uint32_t c = 0; c < anim.channels_count; c++) {
				const tg3_animation_channel& channel = anim.channels[c];
				if (channel.target.node < 0) continue;

				const tg3_animation_sampler& sampler = anim.samplers[channel.sampler];
				const std::string& nodeName = m_Nodes[channel.target.node].Name;
				auto times = ReadAccessor<float>(model, sampler.input);

				if (tg3_str_equals_cstr(channel.target.path, "rotation")) {
					auto values = ReadAccessor<glm::vec4>(model, sampler.output);
					for (size_t i = 0; i < times.size() && i < values.size(); i++) {
						glm::quat q(values[i].w, values[i].x, values[i].y, values[i].z);
						clip.AddKeyframe(nodeName, AnimationChannel::Rotation, times[i], glm::eulerAngles(q));
						maxTime = std::max(maxTime, times[i]);
					}
				}
				else {
					auto values = ReadAccessor<glm::vec3>(model, sampler.output);
					AnimationChannel ch = tg3_str_equals_cstr(channel.target.path, "scale")
						? AnimationChannel::Scale : AnimationChannel::Position;
					for (size_t i = 0; i < times.size() && i < values.size(); i++) {
						clip.AddKeyframe(nodeName, ch, times[i], values[i]);
						maxTime = std::max(maxTime, times[i]);
					}
				}
			}

			clip.Length = maxTime;
			m_Animations.push_back(std::move(clip));
		}
	}

	void Model::SpawnNodeRecursive(EntityManager* entities, int nodeIndex,
		const glm::mat4& parentWorld, const std::string& shaderName) const {
		const ModelNode& node = m_Nodes[nodeIndex];

		glm::mat4 local = glm::translate(glm::mat4(1.0f), node.Translation)
			* glm::mat4_cast(glm::quat(node.Rotation))
			* glm::scale(glm::mat4(1.0f), node.Scale);

		glm::mat4 world = parentWorld * local;

		for (int meshIndex : node.MeshIndices) {
			Entity* entity = entities->CreateEntity();

			glm::vec3 position, scale, skew;
			glm::vec4 perspective;
			glm::quat rotation;
			glm::decompose(world, scale, rotation, position, skew, perspective);

			auto& transform = entity->AddComponent<TransformComponent>();
			transform.Position = position;
			transform.Rotation = glm::eulerAngles(rotation);
			transform.Scale = scale;

			auto& meshComponent = entity->AddComponent<MeshComponent>();
			meshComponent.mesh = m_Meshes[meshIndex];

			auto& material = entity->AddComponent<MaterialComponent>();
			material.shader = ShaderStorage::GetInstance().Get(shaderName);

			int texIndex = m_MeshTextureIndex[meshIndex];
			material.texture = (texIndex >= 0) ? m_Textures[texIndex] : nullptr;
			material.textureSlot = 0;
		}

		for (int childIndex : node.Children) {
			SpawnNodeRecursive(entities, childIndex, world, shaderName);
		}
	}

	void Model::Spawn(EntityManager* entities, const std::string& shaderName) const {
		for (int rootIndex : m_RootNodes) {
			SpawnNodeRecursive(entities, rootIndex, glm::mat4(1.0f), shaderName);
		}
	}

	void Model::GatherMeshesRecursive(int nodeIndex, const glm::mat4& parentTransform,
		std::vector<std::pair<int, glm::mat4>>& out) const {
		const ModelNode& node = m_Nodes[nodeIndex];

		glm::mat4 local = glm::translate(glm::mat4(1.0f), node.Translation)
			* glm::mat4_cast(glm::quat(node.Rotation))
			* glm::scale(glm::mat4(1.0f), node.Scale);

		glm::mat4 world = parentTransform * local;

		for (int meshIndex : node.MeshIndices)
			out.emplace_back(meshIndex, world);

		for (int childIndex : node.Children)
			GatherMeshesRecursive(childIndex, world, out);
	}

	bool Model::GatherMeshesByName(const std::string& name,
		std::vector<std::pair<int, glm::mat4>>& out) const {
		auto nit = m_NodeLookup.find(name);
		if (nit == m_NodeLookup.end())
			return false;

		const ModelNode& node = m_Nodes[nit->second];

		for (int meshIndex : node.MeshIndices)
			out.emplace_back(meshIndex, glm::mat4(1.0f));

		for (int childIndex : node.Children)
			GatherMeshesRecursive(childIndex, glm::mat4(1.0f), out);

		return true;
	}

	std::shared_ptr<Mesh> Model::GetMesh(const std::string& name) const {
		auto mit = m_MeshLookup.find(name);
		if (mit != m_MeshLookup.end())
			return m_Meshes[mit->second];

		auto cit = m_MergedMeshCache.find(name);
		if (cit != m_MergedMeshCache.end())
			return cit->second;

		std::vector<std::pair<int, glm::mat4>> found;
		if (!GatherMeshesByName(name, found) || found.empty())
			return nullptr;

		MeshData merged;
		for (auto& [meshIndex, transform] : found) {
			const RawPrimitiveData& raw = m_RawMeshData[meshIndex];
			glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(transform));
			uint32_t baseVertex = (uint32_t)merged.vertices.size();

			for (size_t v = 0; v < raw.Positions.size(); v++) {
				glm::vec3 pos = glm::vec3(transform * glm::vec4(raw.Positions[v], 1.0f));
				glm::vec3 normal = glm::normalize(normalMatrix * raw.Normals[v]);
				merged.vertices.emplace_back(pos, normal, raw.UVs[v]);
			}
			for (uint32_t index : raw.Indices)
				merged.indices.push_back(baseVertex + index);
		}

		auto mesh = std::make_shared<Mesh>(merged);
		m_MergedMeshCache[name] = mesh;
		return mesh;
	}

	std::shared_ptr<Texture> Model::GetMeshTexture(const std::string& name) const {
		auto mit = m_MeshLookup.find(name);
		if (mit != m_MeshLookup.end()) {
			int texIndex = m_MeshTextureIndex[mit->second];
			return texIndex >= 0 ? m_Textures[texIndex] : nullptr;
		}

		std::vector<std::pair<int, glm::mat4>> found;
		if (!GatherMeshesByName(name, found) || found.empty())
			return nullptr;

		int texIndex = m_MeshTextureIndex[found[0].first];
		return texIndex >= 0 ? m_Textures[texIndex] : nullptr;
	}

	const ModelNode* Model::FindNode(const std::string& name) const {
		auto it = m_NodeLookup.find(name);

		if (it == m_NodeLookup.end())
			return nullptr;		

		return &m_Nodes[it->second];
	}

	const AnimationClip* Model::FindAnimation(const std::string& name) const {
		for (const auto& clip : m_Animations)
			if (clip.Name == name) return &clip;
		return nullptr;
	}
}