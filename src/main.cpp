
#include <Engine/Factory.hpp>
#include "LD37.hpp"
#include "Enemy.hpp"

int main() {
	engine::Factory::RegisterType("enemy", engine::Factory::CreateChildNode<Enemy>);

	LD37 game;
	game.run();
}