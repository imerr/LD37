#include "Room.hpp"
#include <Engine/util/json.hpp>
#include <Engine/Factory.hpp>
#include <Engine/util/Random.hpp>
#include <iostream>
#include <string>

Room::Room(engine::Game* game) : Scene(game), m_enemyContainer(nullptr), m_nextEnemy(0), m_credits(0),
								 m_creditText(nullptr), m_tower(nullptr) {

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
		Wave w;
		w.delay = wave.get("delay", 0.5f).asFloat();
		w.wait = wave.get("wait", 5.0f).asFloat();
		for (auto& spawn : wave["spawns"]) {
			w.spawns.push_back(std::make_pair(
					static_cast<size_t>(spawn.get(0u, 1).asUInt()),
					spawn.get(1u, "ant").asString()
			));
		}
		m_waves.push_back(w);
	}
	return true;
}

void Room::OnInitializeDone() {
	m_enemyContainer = GetChildByID("enemyContainer");
	m_bloodContainer = GetChildByID("bloodContainer");
	auto panel = m_ui->GetChildByID("panel");
	m_creditText = static_cast<engine::Text*>(panel->GetChildByID("credits"));

}

void Room::OnUpdate(sf::Time interval) {
	engine::Scene::OnUpdate(interval);
	m_nextEnemy -= interval.asSeconds();
	if (m_nextEnemy < 0) {
		// Endless
		if (m_waves.size() <= 2) {
			DuplicateLastWave();
		}
		auto& wave = m_waves.front();
		if (wave.spawns.size() == 0) {
			m_waves.erase(m_waves.begin());
			m_nextEnemy += wave.wait;
		} else {
			auto& spawn = wave.spawns.front();
			if (spawn.first == 0) {
				wave.spawns.erase(wave.spawns.begin());
			} else {
				engine::Factory::CreateChildFromFile("assets/scripts/" + spawn.second + ".json", m_enemyContainer);
				spawn.first--;
				m_nextEnemy += wave.delay;
			}
		}
	}
}

void Room::DuplicateLastWave() {
	Wave w = m_waves.back();
	//w.delay *= 0.9;
	for (auto& spawn : w.spawns) {
		//	spawn.first = static_cast<size_t>(spawn.first * 1.25f);
	}
	m_waves.push_back(w);
}

void Room::AddBlood(sf::Vector2f position) {
	auto n = engine::Factory::CreateChildFromFile("assets/scripts/blood.json", m_bloodContainer);
	n->SetPosition(position);
	engine::RandomFloat<float> r(0, 360);
	n->SetRotation(r());
}

void Room::AddCredits(uint32_t amount) {
	m_credits += amount;
	m_creditText->SetText(std::to_string(m_credits));
}
