module;
#include <vector>
export module TileGridComponent;
import Globals;

static constexpr int MAX_ITEMS_PER_TILE = 6;

export enum TileType {
	GRASS
};

export struct Tile {
	TileType type = TileType::GRASS;
	bool isWalkable = true;
	Entity occupant = static_cast<Entity>(-1);
	Entity items[MAX_ITEMS_PER_TILE];
	int item_count;
};


export struct TileGrid {
	int width, height;
	int tile_data_id;
};