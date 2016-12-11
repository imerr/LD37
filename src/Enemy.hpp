#ifndef LD37_ENEMY_HPP
#define LD37_ENEMY_HPP


#include <Engine/SpriteNode.hpp>

class Enemy: public engine::SpriteNode {
public:
	template <typename T>
	struct MoveDone: public engine::EventHandler<void , engine::Tween<T>*> {
		Enemy* m_node;
		MoveDone(Enemy* node) : m_node(node) {

		}

		virtual void handle(engine::Tween<T>* param) {
			m_node->NextPoint();
			param->OnDone.RemoveHandler(this);
			delete this;
		}
	};
protected:
	float m_speed;
	float m_health;
	size_t m_currentPoint;
	float m_offset;
	uint32_t m_credits;
	sf::Vector2f m_positionIncrement;
public:
	Enemy(engine::Scene* scene);
	virtual ~Enemy();

protected:
	virtual void OnUpdate(sf::Time interval);
	void NextPoint();

public:
	virtual void OnInitializeDone();

public:
	virtual bool initialize(Json::Value& root);

	virtual uint8_t GetType() const;

	float GetDistance();
	void Damage(float damage);

	sf::Vector2f GetPositionIn(float seconds) {
		return GetPosition() + (m_positionIncrement * (seconds / (1.0f/60.0f)));
	}

};


#endif //LD37_ENEMY_HPP
