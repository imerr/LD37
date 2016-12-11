
#include <Engine/Factory.hpp>
#include "LD37.hpp"
#include "Enemy.hpp"
#include "Tower.hpp"
#include "Damager.hpp"

int main() {
	engine::Factory::RegisterType("enemy", engine::Factory::CreateChildNode<Enemy>);
	engine::Factory::RegisterType("tower", engine::Factory::CreateChildNode<Tower>);
	engine::Factory::RegisterType("damager", engine::Factory::CreateChildNode<Damager>);

	LD37 game;
	game.run();
}