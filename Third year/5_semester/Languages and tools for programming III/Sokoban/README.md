# sokoban-5
## Game elements
- '#' - wall
- ' ' - ground
- '.' - storage
- '$' - box
- '@' - player
## Controls
| Key | Action |
| ----| ------ |
| space | Start game / Start next level |
| n | Skip level |
| u | Undo move |
| r | Reset game |
| q | Quit game |
| w / Up key | Move up |
| a / Left key | Move left |
| s / Down key | Move down |
| d / Right key | Move right |
## Game description
To win you must move all movable boxes to the storages. Game contains 5 levels.
## Setup
Firstly you need to install `ghc`, then to launch game:
```
ghc sokoban.hs
./sokoban
```