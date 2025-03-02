module;
#include <assimp/Importer.hpp>  
#include <assimp/scene.h>      
#include <assimp/postprocess.h>
export module ModelManager;
import std;
import Model;


export class ModelManager
{
public:
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager&) = delete;

	static ModelManager& Get()
	{
		static ModelManager instance;
		return  instance;
	}

	std::shared_ptr<Model> GetModel(const std::string& path)
	{
		if (models.find(path) != models.end())
		{
			return models[path];
		}
		else {
			std::shared_ptr<Model> model = LoadModelFromFile(path);
			if (model)
			{
				models[path] = model;
			}
			else {
				std::cerr << "[ModelManager::Get] ERROR -> Failed to load model from path " << path << std::endl;
			}
			return  model;
		}

	}

	void Clean()
	{
		for (auto& [path, model] : models)
		{
			model->Destroy();
		}
		models.clear();
	}

private:
	ModelManager() = default;
	std::unordered_map<std::string, std::shared_ptr<Model>> models;

	std::shared_ptr<Model> LoadModelFromFile(const std::string& path)
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(
			path,
			aiProcess_Triangulate |           
			aiProcess_GenSmoothNormals |      
			aiProcess_FlipUVs |               
			aiProcess_JoinIdenticalVertices   
		);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cerr << "[ModelManager::LoadModelFromFile] ASSIMP ERROR ->  " << importer.GetErrorString() << std::endl;
			return nullptr;
		}

		if (scene->mNumMeshes == 0) {
			std::cerr << "[ModelManager::LoadModelFromFile] ASSIMP ERROR -> No meshes found in model: " << path << std::endl;
			return nullptr;
		}

		aiMesh* mesh = scene->mMeshes[0];

		std::vector<float> vertices;
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			vertices.push_back(mesh->mVertices[i].x);
			vertices.push_back(mesh->mVertices[i].y);
			vertices.push_back(mesh->mVertices[i].z);

			if (mesh->HasTextureCoords(0))
			{
				vertices.push_back(mesh->mTextureCoords[0][i].x);
				vertices.push_back(mesh->mTextureCoords[0][i].y);
			}
			else {
				vertices.push_back(0.0f);
				vertices.push_back(0.0f);
			}
		}



		std::vector<unsigned int> indices;
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; ++j) {
				indices.push_back(face.mIndices[j]);
			}
		}

		glm::vec3 diffuseColor(1.0f); // Default white color
		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			aiColor3D aiDiffuseColor(0.0f, 0.0f, 0.0f);
			material->Get(AI_MATKEY_COLOR_DIFFUSE, aiDiffuseColor);
			diffuseColor = glm::vec3(aiDiffuseColor.r, aiDiffuseColor.g, aiDiffuseColor.b);
		}

		std::cout << "Diffuse Color: (" << diffuseColor.r << ", " << diffuseColor.g << ", " << diffuseColor.b << ")\n";
		return std::make_shared<Model>(Model::Create(vertices, indices, diffuseColor));
	}
};