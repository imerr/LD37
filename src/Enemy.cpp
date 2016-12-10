//
// Created by iMer on 10.12.2016.
//

#include <iostream>
#include "Enemy.hpp"
#include "Room.hpp"

Enemy::Enemy(engine::Scene* scene) : SpriteNode(scene), m_speed(50), m_health(10), m_currentPoint(0) {
	std::cout << "Made enemy" << std::endl;
}

Enemy::~Enemy() {

}

bool Enemy::initialize(Json::Value& root) {
	if (!engine::SpriteNode::initialize(root)) {
		return false;
	}
	m_speed = root.get("speed", 50).asFloat();
	m_health = root.get("health", 10).asFloat();
	return true;
}

void Enemy::OnUpdate(sf::Time interval) {

}

void Enemy::OnInitializeDone() {
	auto room = static_cast<Room*>(m_scene);
	auto p = room->GetPath();
	SetPosition(p[0].pos.x, p[0].pos.y);
	std::cout << "Init done" << std::endl;

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
	auto tween = MakeTween<sf::Vector2f>(true,
										 GetPosition(),
										 p.pos,
										 p.distSelf / m_speed,
										 [this](const sf::Vector2f& v) {
											 SetPosition(v.x, v.y);
										 }
	);
	auto handler = new MoveDone<sf::Vector2f>(this);
	tween->OnDone.AddHandler(handler);
}
