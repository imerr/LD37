#ifndef LD37_TOWER_HPP
#define LD37_TOWER_HPP


#include <Engine/SpriteNode.hpp>
#include <Engine/Text.hpp>
#include <SFML/Audio.hpp>

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
	engine::BaseEventHandler* m_placeHandler;
	float m_attackStart;
	float m_attackDuration;
	float m_attackCooldown;
	float m_currentAttackCooldown;
	sf::Color m_circleColor;
	engine::Node* m_damager;
	std::string m_name;
	engine::BaseEventHandler* m_selectHandler;
	engine::Tween<uint8_t> m_selectedFlashTween;
	uint16_t m_upgradeDamage;
	uint16_t m_upgradeSpeed;
	uint32_t m_hitTargets;
	bool m_predict;
	sf::Sound* m_sound;
	std::string m_soundFile;
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
	uint16_t GetUpgrade(bool damage = true) {
		if (damage) {
			return m_upgradeDamage;
		}
		return m_upgradeSpeed;
	}
	void AddUpgrade(bool damage = true);

	uint32_t GetHitTargets() {
		return m_hitTargets;
	}



};


#endif
