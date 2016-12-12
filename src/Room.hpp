#ifndef LD37_ROOM_HPP
#define LD37_ROOM_HPP


#include <Engine/Scene.hpp>
#include <Engine/Text.hpp>
#include <unordered_map>
#include "Tower.hpp"

class Room : public engine::Scene {
public:
	struct PathPoint {
		sf::Vector2f pos;
		float dist;
		float distSelf;
		float angle;
	};
	struct Wave {
		float delay;
		float wait;
		std::vector<std::pair<size_t, std::string>> spawns;
	};
	struct TowerInfo {
		std::string file;
		std::string name;
		std::vector<std::string> desc;
		uint32_t price;
	};
protected:
	std::vector<PathPoint> m_path;
	engine::Node* m_enemyContainer;
	std::vector<Wave> m_waves;
	float m_nextEnemy;
	engine::Node* m_bloodContainer;
	uint32_t m_credits;
	engine::Text* m_creditText;
	Tower* m_tower;
	std::unordered_map<std::string, TowerInfo> m_towers;
	engine::Node* m_towerContainer;
	std::vector<engine::BaseEventHandler*> m_buttonHandlers;
	engine::Node* m_towerInfo;
	Tower* m_selectedTower;
	engine::BaseEventHandler* m_selectedTowerDeleteHandler;
	engine::Node* m_towerUpgrade;
	engine::BaseEventHandler* m_clickUpgradeDamage;
	engine::BaseEventHandler* m_clickUpgradeSpeed;
	sf::Sound* m_buySound;
	sf::Sound* m_errorSound;
	int32_t m_cookies;
public:
	Room(engine::Game* game);
	virtual ~Room();
protected:
	virtual void OnUpdate(sf::Time interval);

public:
	virtual void OnInitializeDone();

	virtual bool initialize(Json::Value& root);

	const std::vector<PathPoint>& GetPath() const {
		return m_path;
	};

	void DuplicateLastWave();

	void AddBlood(sf::Vector2f position);

	void AddCredits(uint32_t amount);
	void RemoveCredits(uint32_t amount);

	void BuyTower(std::string name);

	Tower* GetSelectedTower(){
		return m_selectedTower;
	}
	void SetSelectedTower(Tower* tower);


	void EnemyEnd();
};


#endif
