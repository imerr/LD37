#include <Engine/util/json.hpp>
#include <Engine/Game.hpp>
#include <Engine/util/misc.hpp>
#include "Tower.hpp"
#include "misc.hpp"
#include "Enemy.hpp"

Tower::Tower(engine::Scene* scene) : SpriteNode(scene), m_speed(0), m_damage(0), m_speedMultiplier(1),
									 m_damageMultiplier(1), m_hitbox(0, 0, 1, 1), m_placeCollision(0, 0, 1, 1),
									 m_attacking(false), m_placementMode(true), m_range(10), m_attackStart(0),
									 m_attackDuration(0), m_attackCooldown(1), m_currentAttackCooldown(0),
									 m_damager(nullptr) {
	m_clickHandler = m_scene->GetGame()->OnMouseClick.MakeHandler(
			[this](const sf::Event::MouseButtonEvent& event, bool down) -> bool {
				return m_active && m_placementMode && down && CanPlace();
			},
			[this](const sf::Event::MouseButtonEvent& event, bool down) -> bool {
				Place();
				return true;
			}, this);

}

Tower::~Tower() {
    if (m_clickHandler) {
    	m_scene->GetGame()->OnMouseClick.RemoveHandler(m_clickHandler);
    	delete m_clickHandler;
    	m_clickHandler = nullptr;
    }
}

bool Tower::initialize(Json::Value& root) {
	if (!engine::SpriteNode::initialize(root)) {
		return false;
	}
	m_speed = root.get("speed", 1.0f).asFloat();
	m_damage = root.get("damage", 1.0f).asFloat();
	m_range = root.get("range", 10.0f).asFloat();
	m_attackStart = root.get("attackStart", 1).asFloat();
	m_attackDuration = root.get("attackDuration", 1).asFloat();
	m_hitbox = engine::rectFromJson<float>(root["hitbox"]);
	m_placeCollision = engine::rectFromJson<float>(root["placeCollision"]);
	return true;
}

void Tower::OnInitializeDone() {
	auto attack = m_animations.find("attack");
	if (attack == m_animations.end()) {
		std::cerr << "Tower (" << GetFilename() << ") does not have attack animation" << std::endl;
		return;
	}
	AddSpeedMultiplier(0);
	m_damager = GetChildByID("damager");
	if (!m_damager) {
		std::cerr << "Tower doesn't have damager" << std::endl;
	}

}

void Tower::OnUpdate(sf::Time interval) {
	if (m_placementMode) {
		if (CanPlace()) {
			SetColor(sf::Color::White);
			m_circleColor = sf::Color(0, 0, 0, 30);
		} else {
			SetColor(sf::Color(255, 38, 40));
			m_circleColor = sf::Color(120, 0, 0, 30);
		}
		SetPosition(m_scene->GetGame()->GetMousePosition());
		return;
	}
	if (m_clickHandler) {
		m_scene->GetGame()->OnMouseClick.RemoveHandler(m_clickHandler);
		delete m_clickHandler;
		m_clickHandler = nullptr;
	}
	m_currentAttackCooldown -= interval.asSeconds();
	if (m_currentAttackCooldown < 0) {
		Attack(false);
		if (FindTarget()) {
			PlayAnimation("attack", "default");
			m_currentAttackCooldown = m_attackCooldown;
		}
	} else if (GetAnimationName() == "attack" || (m_damager && m_damager->IsActive())) {
		if (m_attackCooldown - m_currentAttackCooldown > m_attackStart + m_attackDuration) {
			Attack(false);
		} else if (m_attackCooldown - m_currentAttackCooldown > m_attackStart) {
			Attack(true);
		}
	}
}

void Tower::OnDraw(sf::RenderTarget& target, sf::RenderStates states, float delta) {
	if (m_placementMode) {
		sf::CircleShape circleShape;
		circleShape.setRadius(m_range);
		circleShape.setFillColor(m_circleColor);
		circleShape.setOutlineColor(sf::Color(0, 0, 0, 200));
		circleShape.setOutlineThickness(2);
		circleShape.setPosition(-m_range + getOrigin().x, -m_range + getOrigin().y);
		target.draw(circleShape, states);
	}
	engine::SpriteNode::OnDraw(target, states, delta);
}

void Tower::Place() {
	if (!CanPlace()) {
		return;
	}
	m_placementMode = false;
}

void Tower::AddSpeedMultiplier(float add) {
	auto attack = m_animations.find("attack");
	m_attackCooldown = m_speed;
	if (attack == m_animations.end()) {
		m_speedMultiplier += add;
		return;
	}
	attack->second->SetSpeed(attack->second->GetSpeed() * (m_speedMultiplier + add) / m_speedMultiplier);
	m_attackCooldown += attack->second->GetSpeed() * attack->second->GetFrames().size();
	m_speedMultiplier += add;
}

bool Tower::FindTarget() {
	float distance = 0;
	Enemy* enemy = nullptr;
	auto cb = engine::MakeAABBQueryCallback([this, &enemy, &distance](b2Fixture* fixture) {
		engine::Node* node = static_cast<engine::Node*>(fixture->GetBody()->GetUserData());
		if (node->GetType() == NT_ENEMY) {
			float distToTower = engine::distance(node->GetPosition(), GetPosition());
			if (distToTower > m_range) {
				return true;
			}
			Enemy* e = static_cast<Enemy*>(node);
			float enemyDist = e->GetDistance();
			if (enemyDist > distance) {
				enemy = e;
				distance = enemyDist;
			}
		}
		return true;
	});
	b2AABB aabb;
	auto pos = GetPosition();
	aabb.lowerBound.x = (pos.x - m_range) / m_scene->GetPixelMeterRatio();
	aabb.lowerBound.y = (pos.y - m_range) / m_scene->GetPixelMeterRatio();
	aabb.upperBound.x = (pos.x + m_range) / m_scene->GetPixelMeterRatio();
	aabb.upperBound.y = (pos.y + m_range) / m_scene->GetPixelMeterRatio();

	m_scene->GetWorld()->QueryAABB(&cb, aabb);
	if (enemy) {
		float rot = GetRotation();
		// we want -180 to 180, otherwise the object is gonna do weird rotation
		if (rot > 180) {
			rot -= 360;
		}
		float toRot = engine::b2Angle(GetPosition(), enemy->GetPosition()) * 180 / engine::fPI + 90;
		if (toRot > 180) {
			toRot -= 360;
		}
		MakeTween<float>(true,
						 rot,
						 toRot,
						 0.3f / m_speedMultiplier,
						 [this](float angle) {
							 SetRotation(angle);
						 }
		);
		return true;
	}
	return false;
}

bool Tower::CanPlace() {
	if (GetPosition().x > 1024 || GetPosition().x < 0 ||
		GetPosition().y > 576 || GetPosition().y < 0) {
		return false;
	}
	bool found = false;
	auto cb = engine::MakeAABBQueryCallback([this, &found](b2Fixture* fixture) {
		engine::Node* ud = static_cast<engine::Node*>(fixture->GetBody()->GetUserData());
		if (ud != this && ud->GetType() != NT_DAMAGER) {
			found = true;
			return false;
		}
		return true;
	});
	b2AABB aabb;
	auto pos = GetPosition();
	aabb.lowerBound.x = (pos.x - m_placeCollision.left - m_placeCollision.width / 2) / m_scene->GetPixelMeterRatio();
	aabb.lowerBound.y = (pos.y - m_placeCollision.top - m_placeCollision.height / 2) / m_scene->GetPixelMeterRatio();
	aabb.upperBound.x = (pos.x - m_placeCollision.left + m_placeCollision.width / 2) / m_scene->GetPixelMeterRatio();
	aabb.upperBound.y = (pos.y - m_placeCollision.top + m_placeCollision.height / 2) / m_scene->GetPixelMeterRatio();

	m_scene->GetWorld()->QueryAABB(&cb, aabb);
	return !found;
}

uint8_t Tower::GetType() const {
	return NT_TOWER;
}

void Tower::Attack(bool active) {
	if (m_damager && m_damager->IsActive() != active) {
		m_damager->SetActive(active);
	}
}

float Tower::GetDamage() {
	return m_damage * m_damageMultiplier;
}


