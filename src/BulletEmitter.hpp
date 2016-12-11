//
// Created by iMer on 11.12.2016.
//

#ifndef LD37_BULLETEMITTER_HPP
#define LD37_BULLETEMITTER_HPP


#include <Engine/Node.hpp>

class BulletEmitter : public engine::Node {
protected:
	Json::Value m_bulletConfig;
	float m_fireRate;
protected:
	float m_nextFire;
	float m_velocity;
public:
	virtual uint8_t GetType() const;

public:
	BulletEmitter(engine::Scene* scene);

protected:
	virtual void OnUpdate(sf::Time interval);

public:
	virtual void SetActive(bool active);

	virtual bool initialize(Json::Value& root);

	float GetVelocity() const {
		return m_velocity;
	}

	float GetFireRate() const {
		return m_fireRate;
	}

	void SetFireRate(float m_fireRate) {
		BulletEmitter::m_fireRate = m_fireRate;
	}

	void Fire();
};


#endif
