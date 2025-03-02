export module AttackEvent;
import EventManager;
import <glm/glm.hpp>;
import std;
import Globals;
import TileGridComponent;


export class AttackEvent : public ECS::Event {
	public:
		AttackEvent(Entity attacker, Tile& tile) : attacker(attacker), tile(tile) {}

		EventType GetType() const override
		{
			return ATTACK_EVENT;
		}

		Entity GetAttacker() const
		{
			return attacker;
		}

		Tile& GetTile()
		{
			return tile;
		}


private:
	Entity attacker;
	Tile& tile;
};