#ifndef LD37_ROOM_HPP
#define LD37_ROOM_HPP


#include <Engine/Scene.hpp>
#include <Engine/Text.hpp>
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
protected:
	std::vector<PathPoint> m_path;
	engine::Node* m_enemyContainer;
	std::vector<Wave> m_waves;
	float m_nextEnemy;
	engine::Node* m_bloodContainer;
	uint32_t m_credits;
	engine::Text* m_creditText;
	Tower* m_tower;
public:
	Room(engine::Game* game);

protected:
	virtual void OnUpdate(sf::Time interval);

public:
	virtual void OnInitializeDone();

	~Room();
	virtual bool initialize(Json::Value& root);
	const std::vector<PathPoint>& GetPath() const {
		return m_path;
	};

	void DuplicateLastWave();
	void AddBlood(sf::Vector2f position);
	void AddCredits(uint32_t amount);
};


#endif
