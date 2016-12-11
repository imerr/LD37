//
// Created by iMer on 10.12.2016.
//

#include <iostream>
#include <Engine/util/Random.hpp>
#include <Engine/Factory.hpp>
#include "Enemy.hpp"
#include "Room.hpp"
#include "misc.hpp"
namespace {
	uint64_t EnemyId = 0;
}
Enemy::Enemy(engine::Scene* scene) : SpriteNode(scene), m_speed(50), m_health(10), m_currentPoint(0), m_credits(0) {
	engine::RandomFloat<float> r(-20, 20);
	m_offset = r();
	m_id = EnemyId++;
}

Enemy::~Enemy() {

}

bool Enemy::initialize(Json::Value& root) {
	if (!engine::SpriteNode::initialize(root)) {
		return false;
	}
	m_speed = root.get("speed", 50).asFloat();
	m_health = root.get("health", 10).asFloat();
	m_credits = root.get("credits", 10).asUInt();
	return true;
}

void Enemy::OnUpdate(sf::Time interval) {

}

void Enemy::OnInitializeDone() {
	auto room = static_cast<Room*>(m_scene);
	auto p = room->GetPath();
	SetPosition(p[0].pos.x, p[0].pos.y + m_offset);
	NextPoint();
}

void Enemy::NextPoint() {
	auto room = static_cast<Room*>(m_scene);
	m_currentPoint++;
	if (m_currentPoint == room->GetPath().size()) {
		// TODO: subtract health
		//room->EnemyEnd
		Delete();
		return;
	}
	auto p = room->GetPath()[m_currentPoint];
	sf::Vector2f offset(m_offset, m_offset);
	auto tween = MakeTween<sf::Vector2f>(true,
										 GetPosition(),
										 p.pos + offset,
										 p.distSelf / m_speed,
										 [this](const sf::Vector2f& v) {
											 m_positionIncrement = v - GetPosition();
											 SetPosition(v.x, v.y);
										 }
	);
	auto handler = new MoveDone<sf::Vector2f>(this);
	tween->OnDone.AddHandler(handler);
	float rot = GetRotation();
	// we want -180 to 180, otherwise the object is gonna do weird rotation
	if (rot > 180) {
		rot -= 360;
	}
	MakeTween<float>(true,
					 rot,
					 p.angle,
					 0.5,
					 [this](float angle) {
						 SetRotation(angle);
					 }
	);
}

uint8_t Enemy::GetType() const {
	return NT_ENEMY;
}

float Enemy::GetDistance() {
	auto room = static_cast<Room*>(m_scene);
	auto p = room->GetPath()[m_currentPoint - 1]; // m_currentPoint is always at least 1

	return p.dist + engine::distance(GetPosition(), p.pos);
}

void Enemy::Damage(float damage) {
	m_health -= damage;
	if (m_health <= 0) {
		auto room = static_cast<Room*>(m_scene);
		room->AddBlood(GetPosition());
		room->AddCredits(m_credits);
		Delete();
	}
}
