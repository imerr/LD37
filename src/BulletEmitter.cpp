//
// Created by iMer on 11.12.2016.
//

#include <Engine/Factory.hpp>
#include "BulletEmitter.hpp"
#include "misc.hpp"
#include "Damager.hpp"
#include "Tower.hpp"

BulletEmitter::BulletEmitter(engine::Scene* scene) : Node(scene), m_fireRate(1), m_nextFire(0), m_velocity(0) {

}

bool BulletEmitter::initialize(Json::Value& root) {
	if (!engine::Node::initialize(root)) {
		return false;
	}
	m_bulletConfig = root["bulletConfig"];
	m_fireRate = root.get("rate", 1.0f).asFloat();
	m_velocity = root.get("velocity", 1.0f).asFloat();
	return true;
}

void BulletEmitter::SetActive(bool active) {
	if (m_active != active) {
		m_nextFire = 0;
	}
	if (active) {
		Fire();
	}
	engine::Node::SetActive(active);
}

void BulletEmitter::OnUpdate(sf::Time interval) {
	m_nextFire -= interval.asSeconds();
	if (m_nextFire < 0) {
		Fire();
	}
}

uint8_t BulletEmitter::GetType() const {
	return NT_BULLETEMITTER;
}

void BulletEmitter::Fire() {
	auto next = engine::Factory::CreateChild(m_bulletConfig, GetScene());
	if (next->GetType() == NT_DAMAGER) {
		static_cast<Damager*>(next)->SetTower(static_cast<Tower*>(GetParent()));
	}
	m_nextFire = m_fireRate;
	float a = (GetParent()->GetRotation() - 90) / 180 * engine::fPI;
	next->SetPosition(GetParent()->GetPosition());
	next->GetBody()->SetLinearVelocity(b2Vec2(cosf(a) * m_velocity, sinf(a) * m_velocity));
}
