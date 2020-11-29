This project was made using a mix of NASM and C++ and with the intent of being a simulation of a fixed
environment (it would not be changing) that is populated with creatures that will interact with each 
other and the environment around them. At the current time, this project is partially a map editor and
partially a simulator for the creatures. It will spawn in several creatures of different kinds (rabbit, fox, 
and bear) at random available coordinates. These creatures will slowly grow hungry and then look for a 
berry bush in the case of a rabbit or a prey in the case of the fox or the bear. Given several conditions 
(proximity to another of the same kind, different gender, appropriate season, etc.) these creatures will 
increase in population. After the death of a creature it will be removed after several seconds. There is no 
set goal for this simulation other than to be something of a simple sand box within which to watch the 
creatures interact. The simulation lacks an interface and as such, controls are described below. There is 
no UI, inventory, or crafting as per time constraints and/or lack of direction in which to proceed with it.


How to use it.
	As a map editor:
		Tile editor: Enabled when pressing the “m” key and outputs a mixed grid of map tiles, solidity, 
		and overlay tile data is to the console. This is outputted once again when disabling the map 
		editor so that the new map information can be copied and pasted into a text file as a new map.

		The scroll wheel will allow for changing the tile selected on the cursor given that the “z” key is 
		held (this is to prevent selection changing when handling something else when unintended). Left 
		clicking will place tiles and only for the tile solidity editor can the right click be used to remove 
		solidity.

	Overlay tile editor: Enabled with the “o” key,  will allow the placing of additional “top” tiles on 
		the map, and is used much like the map editor except for scrolling requiring the “x” key to be 
		held.

	Solid tile editor (impassable tile editor): When enabled using the “I” key, will add an additional 
		special tile over all of the solid tiles on the map for easy viewing and handling.

		Brush size: Can be changed by holding the “b” key and using the scroll wheel.

	Other “abilities”:
		Speed increase: When holding the “v” key, the scroll wheel can be used to adjust the speed of 
		the character.

		Population cap: Can be adjusted by holding the “p” key and using the scroll wheel.

		Teleporting: Can be used by holding the “t” key and left clicking on the map with the cursor.
	Moving:
		To navigate using the path finding code, hold the “n” key and click on the map. The players 
		character will then navigate to that position if it is reachable.

		To move without path finding, used the “wasd” keys of the arrow keys.

		The space bar is used to attack in the direction the character is facing.

