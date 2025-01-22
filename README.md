# Jump King Recreation 1.0

This is a project made by Team Hutao, formed by Isaac Ramírez and Xavier Chaparro, with the objective of recreating the platformer video game Jump King in SDL2.

Our team finally reached the 1.0 release of the game, including plenty of new features, as well as a functionally playable loop with an ending.
There is also a complete functional Start menu, with options to customize your experience.

In the following text, we will explain all the aditions we managed to add in this project

## ALPHA RELEASE ADDITIONS
In this first release of the game, our team has included the first 5 levels (not zones!) of the game, with a simple movement mechanic featuring a jump option, even though it isn't the original jump mechanic (we plan to add it in future releases). To complete the levels, you just need to do the same as in the original game: reach the top of the level, and it will automatically change to the next one.

We also added some debug features for testing purposes and to help the player: {Show collisions, god mode, limit FPS (30 FPS), die}.

## BETA RELEASE ADDITIONS
In this second release, we added a bunch of features to add new content not existent in the original game, which are:

- ENEMIES
  We added 2 types of Crows: The first can fly freely once it finds the player, chasing directly to the current player position in a certain range. The second one (with a ring in its beak) does not fly, chasing the player until he falls, having the same range of pathfinding.
  These enemies are able to kill the player by collisioning with him, but we added a new player mechanic to kill those enemies. If the player jumps on them, they instantly die, and making the player jump a bit.
  The enemies use a pathfinding method to find and chase the player.

- CHECKPOINT
  We added a bonfire that acts as a checkpoint for the player; it can be found in level 4 and saves the state of the player and enemies once the player goes through it. Once the bonfire is touched, the color changes to blue (previously red), and a sound effect assures that the player got the checkpoint. If an enemy kills the player, he will respawn there.

- MUSIC AND SOUND EFFECTS
  We added music through all the game and all sound effects for jumping, landing, dying, grabbing the checkpoint, when the enemy starts to chase, and for when the player kills an enemy.

- NEW DEBUG FEATURES
  With all the new additions, we complemented them with debug features to test and help the player:
  
  · Now you can Load the first and second level as you please (F1, F2) and spawn in the beginning of each level (F3).
  
  · You can Save the state of the game (F5) and load it (F6) as you please.
  
  · You can see the pathfinding of each enemy by pressing the same key for showing the collisions (F9).
  
  · God mode (F10) now makes the player invincible against enemies.
  
## 1.0 RELEASE ADDITIONS
In the 1.0 release, we added even more new content that neither exists in the original game. 
We also added menus to create a flowchart that lets you play the game and all of its aspects without the need of debug keys or closing the program.

- ITEMS
  We added 3 new items that create a more diverse and challenging experience; these are:
  
  · Invincibility crown: This item makes you invulnerable to enemies for a brief 5 seconds.

  · Extra life: Gives you an extra life.

  · Coins: Each coin item gives you 5 coins to your total count; they are collectibles.

- BOSS
  We created a final boss for you to fight against at the top of the game. After you've passed through all the levels, it's time for you to fight Hutao, who has more lives than a normal enemy and behaves more aggressively, good luck!

- NEW LEVELS
  The level count has been considerably increased, with a whopping total of 12 levels, including the boss room. This fact combined with new scenarios to fight the enemies, new music, diverse items and different checkpoints make for a great adventure.
  You can also fast travel between active bonfires by pressing the up or down arrow keys.

- GUI AND MENUS 
  We implemented the GUI for the game, with different screens and information to be selected by the player:

  · Intro screen: A brief 2-second screen showing the logo and name of the group. If you are in a hurry, you can skip it by pressing space.

  · Main menu: A recreation of the Jump King menu with different buttons for you to press. You can start a new game by pressing PLAY and continue a previous save file (if there is any) by pressing CONTINUE.
               You can also adjust different settings in CONFIG, which are full-screen activation or deactivation, and changing the volume of the SFX and music. Finally, you can see more in-depth credits when pressing CREDITS and exiting the game in exiting
  
  · Pause menu: Pressing ESC while in game lets you pause the menu to access various options. You can continue playing if you click RESUME. Check the same options available in the main menu by pressing CONFIG. 
                You can also go to the main menu with BACK TO TITLE and exit the game with EXIT.

  · In game UI: Now, while playing, you can see how many lives you've got, your coin count and in game time. When you are on top of a bonfire you are able to teleport to the other ones you have activated; this is shown thanks to a UI image.

  · Ending screen: If you manage to beat the final boss, you are awarded by a final screen; that also shows the time you have taken to beat the game and the coin count, so it gives you more motivation to beat it again with better stats. From here on, you can go back to the main menu.

  · Death screen: If you end up losing all of your lives you are faced by a death screen. The only option from here is to go back to the main menu.

- NEW DEBUG FEATURES:
  You can see the bounds of the GUI elements (F8).

# Controls:

MOVE: a (left), d (right)

JUMP: Space

FAST TRAVEL: Up or down arrows (when inside of an active bonfire)

# Debug Features:

View Colliders and paths: F9

God mode: F10 (invincible, use W A S D to move around)

Limit fps (30fps): F11

Start from level 1: F1

Start from level 2: F2

Start from the beginning of the current level: F3

Die: F4 

Save Game: F5

Load Game: F6

Show GUI bounds: F8