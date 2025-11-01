# PlatformGame

## 📋 Description
A challenging 2D pixel-art platformer where you must navigate through dangerous levels filled with enemies to find a hidden key and escape. 

## 🎮 How to Play

### Controls
- **Movement**: W, A, S, D keys
- **Jump & Double jump**: Space bar
- **Action**: E key
- **Run**: Shift key

### Debug Keys
- **H key** Shows / hides a help menu showing the keys
- **F9** Visualize the colliders / logic
- **F10** God Mode (fly around, cannot be killed)
- **F11** Enable/Disable FPS cap to 30

## ✨ Implemented Features

### Release 1.0
- Initial level design and layout
- Player movement and controls (WASD, Space for jump/double jump, Shift to run)
- Player character animations (idle, walk, run, jump)
- Checkpoint system
- Lives system
- Bug fixes:
  - Fixed camera following issues
  - Fixed collision detection errors
  - Fixed background
- Double jump mechanic
- Key collection system to unlock the exit
- Collision detection system
- Pixel-art graphics and animations
- Debug menu (F9, F10, F11, H)
- Bug fixes:
  - Fixed player getting stuck in walls
  - Fixed double jump not resetting properly

## 📖 Game Detailed Info

### Gameplay Mechanics

**Movement System**
- Smooth character controller with acceleration and deceleration
- Walking speed: [X] units/second
- Running speed (holding Shift): [Y] units/second
- Jump height: [Z] units
- Double jump available after first jump

**Physics**
- Gravity: Custom gravity system for precise platforming
- Collision detection using tile-based system
- Momentum preservation during jumps

**Objective System**
- Find the hidden key somewhere in the level
- Key unlocks the exit door
- Navigate through a swamp terrain to reach the end

**Lives & Checkpoints**
- Start with 3 lives
- Checkpoints save your progress
- Respawn at last checkpoint when losing a life
- Game over when all lives are lost

### Characters
**Playable Characters** Play as a:

- **Pink Monster**: A brave pink creature ready to face any challenge

### Art
- https://craftpix.net/file-licenses/

## 🛠️ Installation & Setup
1. Clone the repository
2. Run the executable

## 👥 Team Members
- Joel Martinez Arjona - Programmer
- Sofia Giner Vargas - Artist, designer

## 🔗 Links
- [Repository](link)

## 📝 License
MIT License

Copyright (c) 2024 Pedro Omedas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.