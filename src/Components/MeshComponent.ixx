export module MeshComponent;
import Model;
import <memory>;
import <GL/glew.h>;

export struct Mesh {
	std::shared_ptr<Model> model;
};