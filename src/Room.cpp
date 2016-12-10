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
	// Make sure we reserve enough space so pointers don't change
	m_path.reserve(root["path"].size());
	for (size_t i = 0; i < root["path"].size(); i++) {
		PathPoint path;
		path.pos = engine::vector2FromJson<float>(root["path"][i]);
		path.distSelf = 0;
		path.angle = 0;
		if (prev) {
			float self = engine::distance(path.pos, prev->pos);
			path.distSelf = self;
			dist += self;
			path.angle = engine::b2Angle(prev->pos, path.pos) * 180 / engine::fPI;
		}
		path.dist = dist;
		m_path.push_back(path);
		prev = &m_path.back();
	}
	m_waves.reserve(root["waves"].size());
	for (auto& wave : root["waves"]) {
		
	}
	return true;
}

