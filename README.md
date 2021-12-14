# C++ Game

## TODO

* Remove mob function
* Remove dead mobs from map

* Get player target
* Test pathfinding (single-thread + draw on-screen)

* Random walk mobs w/ different colors

* Render "arrow count"

----

* Proper BG, parallax, camera

* Create events for:
	* Add playerIndex to spawn player event
	* Spawning enemies
	* Spawning projectiles
	* Adding joints?

---


* Long & short jumps
	* Jump cooldown + Use forces + "increase gravity mid-air"

* Small input delay for removing shoot direction
* Handle looping of non-fixed rotation projectiles

* Separate AnimationComponent and TextureRegionComponent

---

* Audio

* Avoid implicit sync: https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming - See "buffer update"

* Do we need joints or can we use fixtures?

* Networking: Input prediction + sync inputs and snapshots + toggle
	* std::to_string is not a great solution
	* Retry client connection when it fails

----

* More maps

* Other game modes