#include "Room.hpp"
#include <Engine/util/json.hpp>
#include <Engine/Factory.hpp>
#include <Engine/util/Random.hpp>
#include <Engine/Button.hpp>
#include <Engine/ResourceManager.hpp>

// http://stackoverflow.com/a/37609217
std::string ToRoman(uint32_t a) {
	std::string M[] = {"", "M", "MM", "MMM"};
	std::string C[] = {"", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"};
	std::string X[] = {"", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"};
	std::string I[] = {"", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"};
	return M[a / 1000] + C[(a % 1000) / 100] + X[(a % 100) / 10] + I[(a % 10)];
}

uint32_t UpgradePrice(uint16_t level) {
	return static_cast<uint32_t>(std::min(15 * powf(static_cast<float>(level), 1.9), 999999.0f));
}

Room::Room(engine::Game* game) :
		Scene(game), m_enemyContainer(nullptr), m_nextEnemy(0), m_bloodContainer(nullptr),
		m_credits(80), m_creditText(nullptr), m_tower(nullptr), m_towerContainer(nullptr), m_towerInfo(nullptr),
		m_selectedTower(nullptr), m_selectedTowerDeleteHandler(nullptr), m_towerUpgrade(nullptr),
		m_clickUpgradeDamage(nullptr), m_clickUpgradeSpeed(nullptr), m_cookies(20) {
	m_buySound = engine::ResourceManager::instance()->MakeSound("assets/sounds/buy.ogg");
	m_errorSound = engine::ResourceManager::instance()->MakeSound("assets/sounds/error.ogg");
}

Room::~Room() {
	for (auto& h : m_buttonHandlers) {
		delete h;
	}
	m_buttonHandlers.clear();
	if (m_selectedTowerDeleteHandler) {
		delete m_selectedTowerDeleteHandler;
	}
	delete m_clickUpgradeDamage;
	delete m_clickUpgradeSpeed;
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
	m_towerUpgrade = panel->GetChildByID("towerUpgrade");
	auto upgrade = [this](bool damage){
		if (!m_selectedTower) {
			return;
		}
		uint32_t cost = UpgradePrice(m_selectedTower->GetUpgrade(damage));
		if (cost > m_credits) {
			m_errorSound->play();
			return;
		}
		m_buySound->play();
		RemoveCredits(cost);
		m_selectedTower->AddUpgrade(damage);
	};
	auto upgradeAttack = static_cast<engine::Button*>(m_towerUpgrade->GetChildByID("button_attack"));
	m_clickUpgradeDamage = upgradeAttack->OnClick.MakeHandler([upgrade](engine::Button*, sf::Vector2f){
		upgrade(true);
	});
	auto upgradeSpeed = static_cast<engine::Button*>(m_towerUpgrade->GetChildByID("button_speed"));
	m_clickUpgradeSpeed = upgradeSpeed->OnClick.MakeHandler([upgrade](engine::Button*, sf::Vector2f){
		upgrade(false);
	});
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
	w.delay *= 0.95f;
	for (auto& spawn : w.spawns) {
		spawn.first = static_cast<size_t>(spawn.first * 1.25f);
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
		m_tower->Delete();
		m_tower = nullptr;
		return;
	}

	if (m_towers.find(name) == m_towers.end()) {
		return;
	}
	auto t = m_towers[name];
	if (t.price > m_credits) {
		m_errorSound->play();
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
	m_towerUpgrade->SetActive(false);
	static_cast<engine::Text*>(m_towerInfo->GetChildByID("name"))->SetText(t.name);
	for (size_t i = 0; i < 10; i++) {
		static_cast<engine::Text*>(m_towerInfo->GetChildByID("desc_" + std::to_string(i)))
				->SetText(t.desc.size() - 1 >= i ? t.desc[i] : "");
	}
	m_buySound->play();
}

void Room::RemoveCredits(uint32_t amount) {
	if (m_credits > amount) {
		m_credits -= amount;
	} else {
		m_credits = 0;
	}
	AddCredits(0);
}


void Room::SetSelectedTower(Tower* tower) {
	if (m_selectedTower) {
		m_selectedTower->OnDelete.RemoveHandler(m_selectedTowerDeleteHandler);
		delete m_selectedTowerDeleteHandler;
	}
	m_selectedTower = tower;
	m_selectedTower->OnDelete.MakeHandler([this](const engine::Node*) {
		m_selectedTower = nullptr;
		delete m_selectedTowerDeleteHandler;
		m_selectedTowerDeleteHandler = nullptr;
	});
	delete m_selectedTowerDeleteHandler;
	m_towerInfo->SetActive(false);
	m_towerUpgrade->SetActive(true);
	static_cast<engine::Text*>(m_towerUpgrade->GetChildByID("name"))->SetText(m_towers[tower->GetName()].name);
	auto btn = m_towerUpgrade->GetChildByID("button_attack");
	static_cast<engine::Text*>(btn->GetChildByID("level"))->SetText("lv. " + ToRoman(tower->GetUpgrade(true)));
	static_cast<engine::Text*>(btn->GetChildByID("price"))
			->SetText(std::to_string(UpgradePrice(tower->GetUpgrade(true))));
	btn = m_towerUpgrade->GetChildByID("button_speed");
	static_cast<engine::Text*>(btn->GetChildByID("level"))->SetText("lv. " + ToRoman(tower->GetUpgrade(false)));
	static_cast<engine::Text*>(btn->GetChildByID("price"))
			->SetText(std::to_string(UpgradePrice(tower->GetUpgrade(false))));
}

void Room::EnemyEnd() {
	m_cookies--;
	if (m_cookies == 0) {
		m_ui->GetChildByID("game_over")->SetActive(true);
	}
}

