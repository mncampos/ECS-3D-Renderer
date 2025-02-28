export module WorldComponents;
import std;
import Globals;

export enum TileType {
	Grass,
	Lava,
	Water,
	Wall
};

export struct Tile {
	TileType type = TileType::Grass;
	bool isWalkable = true;
	Entity occupied = static_cast<Entity>(-1);
	std::stack<Entity> contained_items;
};

