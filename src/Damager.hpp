#ifndef LD37_DAMAGER_HPP
#define LD37_DAMAGER_HPP


#include <Engine/Node.hpp>
#include <unordered_set>
#include "Enemy.hpp"

class Tower;
class Damager: public engine::SpriteNode {
protected:
	engine::BaseEventHandler* m_contactHandler;
	std::unordered_set<uint64_t> m_enemies;
	Tower* m_tower;
	uint32_t m_hits;
	engine::BaseEventHandler* m_towerDeleteHandler;
public:
	Damager(engine::Scene* scene);
	virtual ~Damager();
	Tower* GetTower();
	void SetTower(Tower* tower);

protected:
	virtual void OnUpdate(sf::Time interval);

public:
	virtual uint8_t GetType() const;

	virtual void SetActive(bool active);
};


#endif
