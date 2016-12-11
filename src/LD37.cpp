#include <Engine/Factory.hpp>
#include "LD37.hpp"
#include "Room.hpp"

LD37::LD37() : Game(1224, 576) {
	m_scene = engine::Factory::create<Room>("assets/scripts/room.json", this);
	m_windowTitle = "One Room - Ludum Dare 37";
}

LD37::~LD37() {

}
