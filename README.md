# Metal Maniac 🎮💥

[🔫 Gameplay Video](https://youtube.com/shorts/h3-jmia6l1M?si=Th_7YPBnEvFB8ox4)

## Table of Contents
- [Description](#description)
- [Features](#features)
- [Installation](#installation)
- [Controls](#controls)
- [Game Mechanics](#game-mechanics)
- [Assets](#assets)
- [Development](#development)


## Description
Metal Maniac is a side-scrolling shooter game built with Raylib where players control a soldier battling through enemy forces. Features include:
- Smooth character movement and shooting mechanics
- Intelligent enemy AI with pursuit behavior
- Progressive difficulty system
- Jet enemies with bombing mechanics
- Health and enemy counter systems

## Features
### Core Gameplay
- Run, jump, and shoot mechanics
- Directional aiming (left/right)
- Animated sprite sheets
- 5-life health system

### Enemy Systems
- Smart enemy pursuit AI
- Enemy shooting mechanics
- Jet bombers with projectile drops
- Increasing difficulty each level

### Audio/Visual
- Parallax scrolling background
- Particle effects
- Custom sound effects
- Background music tracks

## Installation
### Clone the repository:
```bash
git clone https://github.com/abdullahforecasts/Metal-Maniac.git
```

### Install dependencies:
- Raylib (v4.0+ recommended)
- C++17 compatible compiler

### Compile and run:
```bash
cd Metal-Maniac
g++ main.cpp -o game -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./game
```

## Controls
| Key      | Action                |
|----------|----------------------|
| ← →  | Move left/right       |
| Space    | Jump                 |
| F        | Fire weapon          |
| ESC      | Quit game            |
| R        | Restart after death  |
| N        | Advance to next level|

## Game Mechanics
### Progression System:
- Defeat all enemies (counter shown top-right) to win
- Each level adds +5 enemies
- Jets appear after the first level

### Combat:
- Enemies respawn when hit
- Bullet collision detection
- Health bar visual feedback

## Assets
The game includes:
- 36+ sprite sheets
- 8+ sound effects
- 2 background music tracks
- Multiple background layers

All assets properly credited in [CREDITS.md](CREDITS.md).

## Development
### Built with:
- C++17
- Raylib 4.0
- SDL for audio mixing

### Project Structure:
```
Metal-Maniac/
├── main.cpp            # Main game loop
├── assets/             # All game resources
│   ├── characters/     # Player/enemy sprites
│   ├── sfx/            # Sound effects
│   └── music/          # Background tracks
└── CREDITS.md         # Asset attributions
```

