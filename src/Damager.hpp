#ifndef LD37_DAMAGER_HPP
#define LD37_DAMAGER_HPP


#include <Engine/Node.hpp>
#include <unordered_set>
#include "Enemy.hpp"

class Tower;
class Damager: public engine::Node {
protected:
	engine::BaseEventHandler* m_contactHandler;
	std::unordered_set<Enemy*> m_enemies;
public:
	Damager(engine::Scene* scene);
	~Damager();
	Tower* GetTower();
protected:
	virtual void OnUpdate(sf::Time interval);

public:
	virtual uint8_t GetType() const;

	virtual void SetActive(bool active);
};


#endif
