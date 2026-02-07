#pragma once
#include <vector>
#include "Component/Mesh.h"
#include "Component/Transform.h"
#include "Entity/Entity.h"

class World {
public:
	Entity CreateEntity() {
		Entity e = m_next++;
		transforms.emplace_back();
		meshes.emplace_back();
		return e;
	}
	Transform& GetTransform(Entity e) { return transforms[e]; }
	Mesh& GetMesh(Entity e) { return meshes[e]; }

	std::vector<Transform> transforms;
	std::vector<Mesh> meshes;
private:
	Entity m_next = 0;
};