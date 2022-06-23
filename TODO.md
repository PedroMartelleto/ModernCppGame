# C++ Game

## TODO

* Sounds

----

* Proper BG, parallax, camera

* Create events for:
	* Add playerIndex to spawn player event
	* Spawning enemies
	* Spawning projectiles
	* Adding joints?
	* Remove mobs

---

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
