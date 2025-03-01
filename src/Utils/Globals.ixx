export module Globals;

import std;
import <cstdint>;



/*
* CONSTANTS
****************************************************************************/

export constexpr auto TARGET_FPS	    = 60.0f;
export constexpr auto TARGET_FRAME_TIME = (1.0f / TARGET_FPS);

/*
* TYPES
***************************************************************************/
export using Signature			 = std::bitset<64>;
export using Entity				 = uint32_t;
export using ComponentType		 = uint32_t;

/*
* EVENT TYPES
***************************************************************************/
export enum EventType {
	SDL_EVENT = 0,
	MOVEMENT_EVENT,
	ATTACK_EVENT,
	CAMERA_UPDATE_EVENT,
	PLAYER_MOVEMENT_EVENT
};