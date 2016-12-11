//
// Created by iMer on 11.12.2016.
//

#include <Engine/Scene.hpp>
#include "Damager.hpp"
#include "Tower.hpp"
#include "misc.hpp"
#include "Enemy.hpp"

Damager::Damager(engine::Scene* scene) : Node(scene) {
	m_contactHandler = m_scene->OnContactPreSolve.MakeHandler([this](b2Contact* contact, const b2Manifold* manifold) {
		engine::Node* a = static_cast<engine::Node*>(contact->GetFixtureA()->GetBody()->GetUserData());
		engine::Node* b = static_cast<engine::Node*>(contact->GetFixtureB()->GetBody()->GetUserData());
		if (a == this || b == this) {
			contact->SetEnabled(false);
			engine::Node* other = b;
			if (b == this) {
				other = a;
			}
			if (other->GetType() == NT_ENEMY) {
				Tower* tower = GetTower();
				Enemy* enemy = static_cast<Enemy*>(other);
				if (m_enemies.find(enemy) != m_enemies.end()) {
					return;
				}
				enemy->Damage(tower->GetDamage());
				m_enemies.insert(enemy);
			}
		}
	});
}

Damager::~Damager() {
	m_scene->OnContactPreSolve.RemoveHandler(m_contactHandler);
}

void Damager::OnUpdate(sf::Time interval) {
	Tower* tower = GetTower();
	this->SetPosition(tower->GetPosition());
	this->SetRotation(tower->GetRotation());
}

uint8_t Damager::GetType() const {
	return NT_DAMAGER;
}

Tower* Damager::GetTower() {
	engine::Node* tower = GetParent();
	if (tower->GetType() != NT_TOWER) {
		tower = tower->GetParent();
	}
	if (tower->GetType() != NT_TOWER) {
		std::cerr << "[Damager] Could not find tower within 2 parents" << std::endl;
		return nullptr;
	}
	return static_cast<Tower*>(tower);
}

void Damager::SetActive(bool active) {
	if (!active) {
		m_enemies.clear();
	}
	engine::Node::SetActive(active);
}
