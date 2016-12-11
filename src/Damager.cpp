//
// Created by iMer on 11.12.2016.
//

#include <Engine/Scene.hpp>
#include "Damager.hpp"
#include "Tower.hpp"
#include "misc.hpp"
#include "Enemy.hpp"

Damager::Damager(engine::Scene* scene) : SpriteNode(scene), m_tower(nullptr), m_towerDeleteHandler(nullptr), m_hits(0) {
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
				if (m_enemies.find(enemy->GetId()) != m_enemies.end() || m_hits >= GetTower()->GetHitTargets()) {
					return;
				}
				m_hits++;
				enemy->Damage(tower->GetDamage());
				m_enemies.insert(enemy->GetId());
				if (m_hits >= GetTower()->GetHitTargets()) {
					if (m_despawnTime != std::numeric_limits<float>::infinity()) {
						Delete();
					} else {
						SetRender(false);
					}
				}
			}
		}
	});
}

Damager::~Damager() {
	m_scene->OnContactPreSolve.RemoveHandler(m_contactHandler);
	if (m_towerDeleteHandler) {
		delete m_towerDeleteHandler;
	}
}

void Damager::OnUpdate(sf::Time interval) {
	if (m_despawnTime != std::numeric_limits<float>::infinity()) {
		auto p = GetPosition();
		if (p.x < -100 || p.y < -100 || p.x > m_scene->GetSize().x + 100 || p.y > m_scene->GetSize().y + 100) {
			Delete();
			return;
		}
	}
	m_body->SetAwake(true);
}

uint8_t Damager::GetType() const {
	return NT_DAMAGER;
}

Tower* Damager::GetTower() {
	if (m_tower) {
		return m_tower;
	}
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
		m_hits = 0;
		m_body->SetSleepingAllowed(true);
	} else {
		SetRender(true);
		m_body->SetSleepingAllowed(false);
	}
	engine::Node::SetActive(active);
}

void Damager::SetTower(Tower* tower) {
	if (m_towerDeleteHandler) {
		delete m_towerDeleteHandler;
	}
	m_tower = tower;
	m_towerDeleteHandler = m_tower->OnDelete.MakeHandler([this](const engine::Node* node){
		m_tower = nullptr;
		m_towerDeleteHandler = nullptr;
	});
}
