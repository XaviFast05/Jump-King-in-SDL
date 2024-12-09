# Jump King Recreation

This is a project made by Team Hutao, formed by Isaac Ramírez and Xavier Chaparro, with the objective of recreating the platformer video game Jump King in SDL2.

In this first release of the game, our team has included the first 5 levels (not zones!) of the game, with a simple movement mechanic featuring a jump option, even though it isn't the original jump mechanic (we plan to add it in future releases). To complete the levels, you just need to do the same as in the original game: reach the top of the level, and it will automatically change to the next one.

We also added some debug features for testing purposes and to help the player: {Show collisions, god mode, limit FPS (30 FPS), die}.

## NEW ADITIONS
In this second release, we added a bunch of features to add new content not existent in the original game, which are:

- ENEMIES
  We added 2 tipes of Crows: The first can fly freely once it founds the player, chasing directly to the current player position in a certain range. The second one (with a ring in it's beak) does not fly, chasing the player untill he falls, having the same range of pathfinding.
  These enemies are able to kill the player by collisioning with him, but we added a new player mechanic to kill those enemies. If the player jumps on them, they instantly die and making the player jump a bit.
  The enemies use a pathfinding method to find and chase the player.

- CHECKPOINT
  We added a bonefire that acts as a checkpoint for the player, it can be found in level 4, and saves the state of the player and enemies once the player goes through it. Once the bonefire is touched, the color changes to blue (previously red) and a sound effect assures that the player got the checkpoint. If an enemie kills the player, he will respawn there.

- MUSIC AND SOUND EFFECTS
  We added music through all the game and all sound effects for jumping, landing, dying, grabbing the checkpoint, when the enemy starts to chase and for when the player kills an enemy.

- NEW DEBUG FEATURES
  
  With all the new additions, we complemented them with debug features to test and help the player:
  
  · Now you can Load the first and second level as you please (F1, F2), and spawn in the beginning of each level (F3).
  
  · You can Save the state of the game (F5) and load it (F6) as you please.
  
  · You can see the pathfinding of each enemy pressing the same key for showing the collisions (F9).
  
  · God mode (F10) now makes the player invencible against enemies.
  
  

# Controls:

MOVE: a (left), d (right)

JUMP: Space

# Debug Features:

View Colliders and paths: F9

God mode: F10 (invecible, use W A S D to move around)

Limit fps (30fps): F11

Start from level 1: F1

Start from level 2: F2

Start from the beginning of the current level: F3

Die: F4 

Save Game: F5

Load Game: F6
