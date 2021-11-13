# C++ Game

## TODO

* Enemies
	* Spawner thread? (Add spawn positions too)
	* Add AI Component
	* Pathfinding on a separate thread

* Create events for:
	* Spawning enemies
	* Spawning projectiles
	* Adding joints

* Render health bars + "matches"
* Render BG
* Render "arrow count"

* Basic game UI

---

* Long & short jumps
	* Jump cooldown + Use forces + "increase gravity mid-air"

* Small input delay for removing shoot direction
* Handle looping of non-fixed rotation projectiles

---

* Audio

* Separate AnimationComponent and TextureRegionComponent
* Do we need joints or can we use fixtures?

* Networking: Input prediction + sync inputs and snapshots + toggle
	* std::to_string is not a great solution
	* Retry client connection when it fails

----

* More maps

* Other game modes