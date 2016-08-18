xlib-keys-combo-simulator
=========================

Simulation of pressing keys combinations.

Build
-----

```bash
$ make
```

Usage examples
--------------

```bash
$ ./build/xlib-keys-combo-simulator LShift RShift
$ ./build/xlib-keys-combo-simulator --press-release LShift RShift
$ ./build/xlib-keys-combo-simulator --press LShift
$ ./build/xlib-keys-combo-simulator --release LShift
$ ./build/xlib-keys-combo-simulator CapsLock NumLock --press LShift
$ ./build/xlib-keys-combo-simulator --press LShift --release RShift
```

List of possible keys to use
----------------------------

This list can be expanded in the future.
I made this tool for my own purposes and have added keys that I only need
at that time.

* LShift
* RShift
* NumLock
* CapsLock
* Level3

Author
------

[Viacheslav Lotsmanov](https://github.com/unclechu)

License
-------

[GNU/GPLv3](./LICENSE)
