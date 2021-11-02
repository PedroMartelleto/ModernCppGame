# C++ Game

## TODO

* Think about how to send the data

* Simple producer/consumer network event handler
	- Example: Counting events. In this usage scenario an event handler will 'give' a semaphore each time an event occurs (incrementing the semaphore count value), and a handler task will 'take' a semaphore each time it processes an event (decrementing the semaphore count value). The count value is therefore the difference between the number of events that have occurred and the number that have been processed. In this case it is desirable for the count value to be zero when the semaphore is created.

* Retry client connection when it fails

* Basic game UI

* Better jump gravity & proper height

* Projectiles

* Life

* Menu

* More maps

* Other game modes and Enemies