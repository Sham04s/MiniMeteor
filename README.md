-----------------------------------

<a href="url"><img src="resources/ui/icon.png" align="left"  height="40" width="40" ></a>
# MiniMeteor
![MiniMeteor](screenshots/bullets.png)

### Description

MiniMeteor is a minimalist space shooter game inspired by the classic Asteroids. It’s an endless game with difficulty increasing according to the current score , where you need to destroy asteroids and enemies to score points. 

Originally made for the [Raylib Slo-Jam](https://itch.io/jam/raylib-slo-jam)

### Features

 - The game has powerups, like adding an extra bullet, increasing bullet speed, shields to protect the player, etc.
 - The player has a limited boost that can be used with 'shift'
 - There are two modes for the player: the normal mode; where the movement is locked to the player's front, and the directional ship mode; where the player can freely move around screen with 'wasd'
 - The game has 3 types of enemies: shooter: who periodically shoots at the player, stalker: who constantly follows the player, and pulser: who shoots bullets in the form of a circle.

### Links

 - itch.io Release: [MiniMeteor](https://sham04.itch.io/minimeteor)

### How to build
 You will need:
 - Raylib game library. Get it from the [oficial repo](https://github.com/raysan5/raylib/).
 - A c++ compiler and makefile tools.
 - Emscripten SDK. (Optional if building for web)

 And either set an environmental variable with the raylib's path or type it alongside make command
 ```console
    make RAYLIB_PATH=<path/to/raylib>
 ```

You can also type
```console
   make help
```
to get a more detailed information


### Credits
Game and UI visual assets were provided by Kenney and modified by me. Checkout Kenney's amazing website! [kenney.nl](https://kenney.nl)


*Copyright (c) 2023-2024 Sham04s*
