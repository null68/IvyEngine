#include <iostream>
#include <fstream>
#include <string>
#include "AssetManager.h"

namespace Engine {
	const char* AssetManager::LoadShaderFile(const char* filePath) {
		std::string shaderCode;

		std::ifstream shaderFile(filePath);
		if (shaderFile.is_open()) {
			std::string line;
			while (std::getline(shaderFile, line)) {
				shaderCode += line + "\n";
			}
			shaderFile.close();
		}
		else {
			std::cerr << "Failed to open shader file: " << filePath << std::endl;
			return nullptr;
		}
		char* shaderCodeCStr = new char[shaderCode.size() + 1];
		strcpy_s(shaderCodeCStr, shaderCode.size() + 1, shaderCode.c_str());
		return shaderCodeCStr;
	}

	bool AssetManager::LoadGLTFModel(const std::string& filePath, tg3_model& model) {
		tg3_parse_options opts;
		tg3_error_stack errors;

		tg3_parse_options_init(&opts);
		tg3_error_stack_init(&errors);

		tg3_error_code err = tg3_parse_file(&model, &errors, filePath.c_str(), (uint32_t) filePath.size(), &opts);

		if (err != TG3_OK) {
			for (uint32_t i = 0; i < errors.count; i++) {
				fprintf(stderr, "[%d] %s\n", (int)errors.entries[i].severity,
					errors.entries[i].message ? errors.entries[i].message : "(null)");
			}

			tg3_error_stack_free(&errors);
			tg3_model_free(&model);

			return false;
		}

		tg3_error_stack_free(&errors);

		return true;
	}
}