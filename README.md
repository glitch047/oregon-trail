# Oregon Trail Game (1985 Apple II Version)

A recreation of the classic Oregon Trail game for Linux systems, inspired by the 1985 Apple II version.

## Features

- Character and party management
- Resource management (money, food, ammunition, supplies)
- Weather system
- Health and illness simulation
- Hunting mini-game
- River crossing decisions
- Random events and challenges
- Trading system
- Scoring system

## Building and Running

### Prerequisites

- C++17 compatible compiler (g++ recommended)
- SDL2 libraries (SDL2, SDL2_image, SDL2_ttf, SDL2_mixer)

### Installation of Dependencies (Debian/Ubuntu/Kali)

```bash
sudo apt-get update
sudo apt-get install build-essential libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev
```

### Building the Game

```bash
# Navigate to the project directory
cd ~/Desktop/oregon-trail

# Build the game
make

# Run the game
make run
```

## Controls

- **Arrow Keys**: Navigate menus
- **Enter**: Select option
- **Space**: Action button (used in hunting, etc.)
- **Escape**: Exit/Back

## Game Structure

The game follows the journey of pioneers traveling the Oregon Trail from Independence, Missouri to Oregon's Willamette Valley in 1848.

Players must:
1. Choose a profession (affects starting money)
2. Purchase supplies
3. Navigate the trail
4. Handle random events
5. Maintain party health
6. Reach Oregon before winter

## License

This project is created for educational purposes only. The original Oregon Trail was developed by MECC.

