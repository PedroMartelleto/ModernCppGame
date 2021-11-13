# C++ Game

## TODO

* Separate AnimationComponent and TextureRegionComponent
* Render BG

* Multithreaded job system

* Enemies
	* AI Component
	* Pathfinding on a separate thread

* Create events for:
	* Spawning enemies
	* Spawning projectiles
	* Adding joints?

* Render health bars + "matches"
* Render "arrow count"

* Basic game UI

---

* Long & short jumps
	* Jump cooldown + Use forces + "increase gravity mid-air"

* Small input delay for removing shoot direction
* Handle looping of non-fixed rotation projectiles

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