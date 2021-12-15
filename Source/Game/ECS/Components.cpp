#include "Components.h"
#include "../GameData.h"
#include "../Networking/Packet.h"

BitBuffer8 MobComponent::CreateEventBitBuffer() const
{
	BitBuffer8 buffer;
	buffer.Set(GameData::GetMobActionBit("MOVE_UP"), wantsToJump);
	buffer.Set(GameData::GetMobActionBit("MOVE_LEFT"), horizontalMoveDir < 0.0f);
	buffer.Set(GameData::GetMobActionBit("MOVE_RIGHT"), horizontalMoveDir > 0.0f);
	buffer.Set(GameData::GetMobActionBit("MOVE_DOWN"), horizontalMoveDir > 0.0f);
	buffer.Set(GameData::GetMobActionBit("SHOOT_DOWN"), readyToShoot);
	buffer.Set(GameData::GetMobActionBit("SHOOT_UP"), wantsToShoot);

	return buffer;
}

void MobComponent::Reset(ProjectileInventoryComponent* inventory)
{
	health = maxHealth;
	invencibilityTicks = 0;
	readyToShoot = false;
	wantsToJump = false;

	if (inventory != nullptr)
	{
		inventory->projectiles.clear();

		// If this mob has access to a projectile, give it its projectiles
		for (int i = 0; i < initialAmmo; ++i)
		{
			inventory->projectiles.push_back(GameData::GetProjectileData(projectile));
		}
	}
}