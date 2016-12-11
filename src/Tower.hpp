#ifndef LD37_TOWER_HPP
#define LD37_TOWER_HPP


#include <Engine/SpriteNode.hpp>
#include <Engine/Text.hpp>

class Tower: public engine::SpriteNode {
protected:
	float m_speed;
	float m_damage;
	float m_speedMultiplier;
	float m_damageMultiplier;
	sf::FloatRect m_hitbox;
	sf::FloatRect m_placeCollision;
	bool m_attacking;
	bool m_placementMode;
	float m_range;
	engine::BaseEventHandler* m_clickHandler;
	int m_blockingContacts;
	size_t m_attackFrame;
	float m_attackCooldown;
	float m_currentAttackCooldown;
	sf::Color m_circleColor;
	b2AABB m_debugAABB;
	engine::Node* m_damager;
public:
	Tower(engine::Scene* scene);
	~Tower();
protected:
	virtual void OnDraw(sf::RenderTarget& target, sf::RenderStates states, float delta);

	virtual void OnUpdate(sf::Time interval);

public:
	virtual void OnInitializeDone();
	virtual bool initialize(Json::Value& root);
	virtual uint8_t GetType() const;
	void Place();
	void AddSpeedMultiplier(float add);
	bool CanPlace();
	bool FindTarget();

	void Attack(bool active);

	float GetDamage();
};


#endif
