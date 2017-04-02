



# Age of Champions
Tribute game to Age of Empires II developed by DemonRealm.


## How to play
Right now there are only prototype releases to test the functionality of our game dute to this fact all debug controlls can be activated without the need of being on the debug mode (F1).  
Controls for v.0.3 (Vertical Slice prototype)   
v0.3 is quite similar to the AoE editor but with far less options, you can spawn difernt units, resources and bulding and test some of their functionality.   

* Mouse Left Click to select a unit, building or resource
* Mouse Right Click to Make the unit do actions (Move/Attack/Recolect)
* Mouse Double Click to select all units of the same type
* Mouse Drag to select unit groups (doesn't work on enemies groups)
* Click on the skull with the selected/s unit to kill them
* Click on the book icon when the warrior is selected to open his skill tree
* Click on the [+] buttons on the skill tree to gain that skills (only one per tier)
* Click on the skill icons to activate them (only works for the first tier and it's the same an attack boost aura)
* Click on the warrior icon in the town center panel to spawn a warrior (only one per game)

#### Debug controls
* GRAVE to activate/deactivate console (enter "help" in the console to see all the commands)
* [F1]:Activate debug mode     
* [F2]:Generate Barracks  
* [F3]: Generate Villager
* [N]: Generate Arbalest
* [M]: Generate Milita
* [Z]: Generete Tree
* [X]: Generete Berry Bush
* [C]: Generate Stone quarry
* [V]: Generate Gold ore
* [W]: Generate Warrior
* Arrows to move the camera

## Media 
**Facebook:**https://www.facebook.com/DemonRealm.P2    
**Twitter:**https://twitter.com/Demon_Realm    
**Github page:** https://github.com/DeamonRealm/Age_of_Empires_II


## Credits

#### Libraries
This software was made using [SDL](https://www.libsdl.org/) libraries.   

This software is based on [pugixml](http://pugixml.org) library.
pugixml is Copyright (C) 2006-2015 Arseny Kapoulkine. 

Virtual file system was made using [Physfs](https://icculus.org/physfs/) library. 

#### Tools
[Dr.Memory](http://www.drmemory.org/) is used to control all memory leaks.

[Broflier](https://github.com/bombomby/brofiler/wiki) is used to manage app timming.

## Changelog
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
