# Age of Champions
Tribute game to Age of Empires II developed by DemonRealm.   
Age of Champions aims to preserve the core gameplay of the Age of Empires franchise while introducing new features such as champions similar to the ones from Warcraft III.

##Installation
Drop the game folder inside the .zip somewhere in your computer and execute the .exe

## How to play (Beta)

To win you have to find the enemy city and destroy the enemy town center, don't forget to defend your town or the enemies will raid towards your city and destroy it. If the enemy manages to destroy your town center you lose the game.


### Main Menu
* Click on the single player to open the load menu at the right, click on Default to load a default game or in load game to open a previously saved game
* Click on the book to open the game wiki in your browser
* Click on the arrow to quit the game

### Game controls
* Mouse Left Click to select a unit, building or resource
* Mouse Right Click to Make the unit do actions (Move/Attack/Recolect)
* Mouse Double Click to select all units of the same type
* Mouse Drag to select unit groups (doesn't work on enemies groups)
* When selecting both units or buildings a panel will appear on the left bottom corner, passing the mouse over the diferent images will show what they do and the resources needed, clicking on them will do those actions.
* Click on the book icon when the warrior is selected to open his skill tree
* Click on the [+] buttons on the skill tree to gain that skills (only one per tier)
* Click on the skill icons to activate them
* Arrows or drag the mouse to the corners to move the camera 
* Click on the scroll at the upper-right part of the screen to open the in-game menu
* Champions selections shortcuts are placed in the upper right corner of the minimap
* Holding Ctrl while building will allow build more buildings of the same type without the need to clik on the building icon again
* Ctrl+unit selection allows to select more units without deselecting other ones
* [A]: Activate the first skill of the selected champion
* [S]: Activate the second skill of the selected champion
* [D]: Activate the third skill of the selected champion
* [F]: Toggle Fullscreen/Windowed mode
* [P]: Pause the game
* [+]: Turn up the volume
* [-]: Turn down the volume
* 1: Build house
* 2: Build town center
* 3: Build stable
* 4: Build lumber camp
* 5: Build mining camp
* 6: Build blacksmith
* 7: Build university
* 8: Build monastery
* 9: Build archery range
* 0: Build barracks

Note: All spawns require the player to wait some time before the actual unit spawns, this is not shown in the current UI.
Note 2: Num shorcuts can only be used when a villager is selected


#### Debug controls
(only available after activating debug mode (F1))   
These controls are not meant to be used during normal playthroughs, the use of this keys i meant for testing purposes only.

* [F1]:Activate/Deactivate debug mode     
* [F2]: Activate map logic Debug
* [F3]: Generate Villager 
* [Z]: Generete Tree
* [X]: Generete Berry Bush
* [C]: Generate Stone quarry
* [V]: Generate Gold ore
* [W]: Generate Warrior
* [E]: Generate Wizard
* [R]: Generate Hunter
* [G]: Generate God unit
* [B]: Add 100 of each resource


## Media 
**Facebook:** https://www.facebook.com/DemonRealm.P2    
**Twitter:** https://twitter.com/Demon_Realm    
**Github page:** https://github.com/DeamonRealm/Age_of_Empires_II


## Credits

#### Libraries
This software was made using [SDL](https://www.libsdl.org/) libraries.   

This software is based on [pugixml](http://pugixml.org) library.
pugixml is Copyright (C) 2006-2015 Arseny Kapoulkine. 

Virtual file system was made using [Physfs](https://icculus.org/physfs/) library. 

#### Tools
[Deleaker](http://www.deleaker.com/) is used to control all memory leaks.

[Broflier](https://github.com/bombomby/brofiler/wiki) is used to manage app timming.

## Changelog

### v0.9 (Beta)
* New loading screen
* Added: Button to the wiki with tutorials on how ot play (tutorials page is in progress)
* Fixed: #56
* Fixed: #62
* Fixed: #70
* Fixed: #71
* Fixed: #72
* Fixed: #73


### v0.8.3
* Added: God unit for debug mode (only for testing purposes)
* Improved: Warrior Slash animation
* Fixed: #63
* Fixed: #64
* Fixed: #66
* Fixed: #67
* Fixed: #68
* Fixed: #69

### v0.8.2
* Added: Intro video
* Added: Volume controls
* Added: You can order the villager to go save resources
* Added: Production queue (need polish)
* Improved: Mage revive skill

### v0.8.1 (Alpha)
* Added: Shortcut buttons to the 3 champions 
* Added: Shortcut keys to the champion skills
* Added: Bug report button in the main menu
* Added: Loading screen dot animation
* Added: Remaining particles for champions skills
* Added: Attack alert
* Added: Move indicator
* Improved: AI raids
* Improved: Movement system
* Fixed: #51 
* Fixed: #52 
* Fixed: #53 
* Fixed: #55 
* Fixed: #59 
* Fixed: #60 
* Fixed: #61 

### v0.8 (Pre-Alpha)
* Added: Default game map has more resources
* Added: Skill indicators for champions skills
* Added: Multiple units can collect resources
* Added: Champion skills icons
* Added: Hunter skills sounds
* Added: Pause system
* Added: Champions lvl up system
* Improved: Mulitple units attacks
* Improved: AI troop generation
* Improved: AI building 
* Improved: Animation timing
* Fixed: #34 
* Fixed: #36 
* Fixed: #37 
* Fixed: #38 
* Fixed: #45 
* Fixed: #47 
* Fixed: #46 
* Fixed: #50 

### v0.7
* Added: Art for the right branch hunter skills
* Added: Art for enemy buildings 
* Added: Input redefinition through config.xml file
* Updated: Wizard active skills sound
* Updated: Building-Unit collision
* AI: Now recollects diferent resources
* AI: Manages his own resources to build, create units and upgrade
* AI: Always sends 10 troops to attack when it has enough units
* Fixed: #35 


### v0.6
* Added: Save/Load functionality
* Added: New champions Wizard and Hunter
* Added: Fog of war
* Added: Troops upgrades
* Added: Some more buildings (Stable, blacksmith, house, archery)
* Added: Ctrl+click when building let's you build more buildings without going to the villager panel
* Added: Enemy structures (For now they use the same sprite as the player ones)
* Improved: Building constructions now works with two mouse click, like the original AoE
* Improved: Recollecting trees now first chops the tree and then starts recollecting
* Improved: Camera movement
* Revamped: Enemy AI behaviour
* Fixed #30 
* Fixed #32 
* Fixed #33 
* Fixed #34 

### v0.5

* Added: Warrior skills (defense/attack buff + stun + slash)
* Added: Win/lose condition
* Added: End of game panel
* Added: AI attacks buildings
* Added: Buildings death (destruction)
* Added: In game pause menu
* Improved: Autoattack scann
* Changed: Spawn properties (both enemies & player)
* Changed: Debug controls can only be accesed when in debug mode
* Fixed issue #21 
* Fixed issue #22 
* Fixed issue #24 
* Fixed issue #25 
* Fixed issue #26 
* Fixed issue #27 
* Fixed issue #28 
* Fixed issue #29 

### v0.4
* Removed: Arbalest Spawn with space
* Added: Basic Enemy AI (Spawn 10 units at the top of the map and they move to the bottom)
* Added: Units scann for enemies in their vision and attack them (only arbalest & enemy militia), units only scann when they are on idle
* Added: Warrior attack buff (logic and animation)
* Added: Warrior UI
* Added: Load predefined scene
* Added: Barracks building defined (lacks functionality)
* Added: Sound (Background, Die, Attack, Select, Spawn)
* Changed: Resource recolection logic (should avoid crashes)
* Fixed: When a selected resource depletes game no longer crashes
* Fixed: When a resource depletes the logic map no longer blocks the way



#### v0.3
* Removed: Movement keys
* Removed: Animation type and direction controls
* Removed: HPA pathfinding method (should be replaced with JPS in future updates)
* Added: Mouse controls the unit movement
* Added: Predefined movement (doesn't have any indicators, only works in groups(more than 1 unit))
* Added: Multiple troops movement
* Added: Attack action (right now it can attack any unit be allies, foes or even himself)
* Added: Unit creation (Town center can spawn arbalest units)
* Added: Recollection ability (can only happen one time for villager, doesn't leave the good in the town center)
* Added: Map logic (units shouldn't pass trough obstacles)
* Changed: Almost all controls (see "How to play" for more info)
* Integration of the Quadtree optimitzation with units search and map draw (should improve the framerate)
* Improved troops actions   
Note: Animtions are not yet impleneted pls don't report them as a bug, we now that they don't work.


#### v0.2.1
* Changed: movenet keys
* Added: Control to change animation type
* Better control over the direction of the animations

#### v0.2
* Added: Buildings shadows
* Added: Console Units Factory
* Added: Main Menu & HUB sprite
* Added: Map Tiles debug mode

#### v0.1
* Added: A controllable villager moving around.
* Added: Correct render order that simulates the third dimension in builds
* Added: Tiled map 
* Added: A basic console
* Added: Simple UI test for scroll selection
