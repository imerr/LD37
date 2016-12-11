#include "Room.hpp"
#include <Engine/util/json.hpp>
#include <Engine/Factory.hpp>
#include <Engine/util/Random.hpp>
#include <Engine/Button.hpp>

Room::Room(engine::Game* game) :
		Scene(game), m_enemyContainer(nullptr), m_nextEnemy(0), m_bloodContainer(nullptr),
		m_credits(200), m_creditText(nullptr), m_tower(nullptr), m_towerContainer(nullptr), m_towerInfo(nullptr) {
}

Room::~Room() {
	for (auto& h : m_buttonHandlers) {
		delete h;
	}
	m_buttonHandlers.clear();
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
	m_waves.reserve(root["towers"].size());
	for (auto& tower : root["towers"]) {
		TowerInfo t;
		t.file = tower.get("file", "nofile").asString();
		t.name = tower.get("name", "No Name").asString();
		for (auto& desc : tower["desc"]) {
			t.desc.push_back(desc.asString());
		}
		t.price = tower.get("price", 100).asUInt();
		m_towers.insert(std::make_pair(t.file, t));
	}
	return true;
}

void Room::OnInitializeDone() {
	m_enemyContainer = GetChildByID("enemyContainer");
	m_bloodContainer = GetChildByID("bloodContainer");
	m_towerContainer = GetChildByID("towerContainer");
	auto panel = m_ui->GetChildByID("panel");
	m_creditText = static_cast<engine::Text*>(panel->GetChildByID("credits"));
	for (const auto& info : m_towers) {
		std::string name = info.first;
		auto btn = static_cast<engine::Button*>(panel->GetChildByID(name));
		m_buttonHandlers.push_back(btn->OnClick.MakeHandler([this, name](engine::Button*, sf::Vector2f) {
			BuyTower(name);
		}));
		static_cast<engine::Text*>(btn->GetChildByID("price"))->SetText(std::to_string(info.second.price));
	}
	// Update credit display
	AddCredits(0);
	m_towerInfo = panel->GetChildByID("towerInfo");
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
	if (!m_tower || !m_tower->IsPlacing()) {
		m_towerInfo->SetActive(false);
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

void Room::BuyTower(std::string name) {
	if (m_tower && m_tower->IsPlacing()) {
		AddCredits(m_towers[m_tower->GetName()].price);
		m_tower->Delete(); // TODO: this crashes for some reason, figure out WHY
		//m_tower->SetActive(false);
		m_tower = nullptr;
		return;
	}

	if (m_towers.find(name) == m_towers.end()) {
		return;
	}
	auto t = m_towers[name];
	if (t.price > m_credits) {
		// TODO: add error sound
		return;
	}
	m_tower = static_cast<Tower*>(
			engine::Factory::CreateChildFromFile(
					"assets/scripts/" + t.file + ".json",
					m_towerContainer
			)
	);
	if (m_tower) {
		m_tower->SetName(name);
	}
	RemoveCredits(m_towers[m_tower->GetName()].price);
	m_towerInfo->SetActive(true);
	static_cast<engine::Text*>(m_towerInfo->GetChildByID("name"))->SetText(t.name);
	for (size_t i = 0; i < 10; i++) {
		static_cast<engine::Text*>(m_towerInfo->GetChildByID("desc_" + std::to_string(i)))
				->SetText(t.desc.size() - 1 >= i ? t.desc[i] : "");
	}
}

void Room::RemoveCredits(uint32_t amount) {
	if (m_credits > amount) {
		m_credits -= amount;
	} else {
		m_credits = 0;
	}
	AddCredits(0);
}
