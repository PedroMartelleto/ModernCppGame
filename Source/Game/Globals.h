#pragma once

namespace Game
{
	const float ONE_METER_IN_PIXELS = 16.0f;
	const float MAP_SCALE = 2.0f;
	const float PIXELS_TO_METERS = 1.0f / (ONE_METER_IN_PIXELS * MAP_SCALE);
	const float METERS_TO_PIXELS = 1.0f / PIXELS_TO_METERS;
}