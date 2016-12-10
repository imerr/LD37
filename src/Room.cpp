#include <Engine/util/json.hpp>
#include <iostream>
#include "Room.hpp"

Room::Room(engine::Game* game) : Scene(game) {

}

Room::~Room() {

}

bool Room::initialize(Json::Value& root) {
	if (!engine::Scene::initialize(root)) {
		return false;
	}
	float dist = 0;
	PathPoint* prev = nullptr;
	// Make sure we reserve enough space so pointers dont change
	m_path.reserve(root["path"].size());
	for (size_t i = 0; i < root["path"].size(); i++) {
		PathPoint path;
		path.pos = engine::vector2FromJson<float>(root["path"][i]);
		path.dist = dist;
		m_path.push_back(path);
		if (prev) {
			dist += engine::distance(path.pos, prev->pos);
		}
		prev = &m_path.back();
	}
	return true;
}
