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
	size_t m_attackFrame;
	float m_attackCooldown;
	float m_currentAttackCooldown;
	sf::Color m_circleColor;
	b2AABB m_debugAABB;
	engine::Node* m_damager;
	std::string m_name;
public:
	Tower(engine::Scene* scene);
	virtual ~Tower();
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
	bool IsPlacing() {
		return m_placementMode;
	}
	std::string GetName() const {
		return m_name;
	}
	void SetName(const std::string& name) {
		m_name = name;
	}

};


#endif
