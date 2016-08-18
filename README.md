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
$ ./build/simulate-keys LShift RShift
$ ./build/simulate-keys --press-release LShift RShift
$ ./build/simulate-keys --press LShift
$ ./build/simulate-keys --release LShift
$ ./build/simulate-keys CapsLock NumLock --press LShift
$ ./build/simulate-keys --press LShift --release RShift
$ ./build/simulate-keys --toggle Level3Shift
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
* Level3Shift

Author
------

[Viacheslav Lotsmanov](https://github.com/unclechu)

License
-------

[GNU/GPLv3](./LICENSE)
