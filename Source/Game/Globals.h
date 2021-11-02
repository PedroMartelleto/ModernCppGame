#pragma once

namespace Game
{
	const float TILE_SIZE = 16.0f;
	const float MAP_SCALE = 2.0f;
	const float PIXELS_TO_METERS = 1.0f / (TILE_SIZE * MAP_SCALE);
	const float METERS_TO_PIXELS = 1.0f / PIXELS_TO_METERS;
}