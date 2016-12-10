#ifndef GAMEWRAPS_ROOM_HPP
#define GAMEWRAPS_ROOM_HPP


#include <Engine/Scene.hpp>

class Room : public engine::Scene {
public:
	struct PathPoint {
		sf::Vector2f pos;
		float dist;
	};
protected:
	std::vector<PathPoint> m_path;
public:
	Room(engine::Game* game);

	~Room();
	virtual bool initialize(Json::Value& root);
	const std::vector<PathPoint>& GetPath() const {
		return m_path;
	};
};


#endif //GAMEWRAPS_ROOM_HPP
