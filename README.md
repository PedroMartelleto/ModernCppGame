# C++ Game

## TODO

* Path following
* Update current node (on a separate thread if possible)

* Test job system

* A* on a separate thread

* Remove dead mobs from map
* Win/lose screen

* Enemies
	* Level graph
	* AI Component
	* Pathfinding on a separate thread

* Proper BG, parallax, camera

* Game structure and UI
* Render "arrow count"

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