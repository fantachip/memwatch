Back in the good old days of Battlefield 2, memwatch has been used to overwrite memory of a running Battlefield two process and do cool things like removing fog and adding a sniper zoom to a tank cannon. 

Have you ever imagined what it was like to have sniper zoom on a tank and no fog? It was awesome! Using memwatch you could press a button, zoom in to an enemy base several kilometers away and shoot them with your tank cannon. It was so far away that you could not even see the explosions because they would not be rendered when they were so far away. And on Dalian Plant you could park your tank close to the water edge and repeatedly shoot the enemy planes on the carrier far away in the sea right after they spawn - over and over again. 

Here is an example of flying a helicopter with no fog. 
![](http://i.imgur.com/FzHFoWd.jpg)

But that was not all. Due to how battlefield two was designed, you could press a button with memwatch running and kick the enemy commander - and then you could make yourself the commander and press another button to switch teams. Now you could see where all the enemy soldiers are and shoot them before it was too late. 

HOW DOES IT WORK
================

MemWatch is really designed to work with any executable. You can pick one you want to tamper with. MemWatch can access another process memory and read and write stuff to it (keep in mind that this was during the time of Windows XP). This can be anything - and in the case of battlefield two it was memory locations for variables that controlled different ingame aspects of the game. 

MemWatch stores it's settings inside a configuration file, called a "profile". 

Here is a portion of the profile of a BF2 settings file: 
	process=BF2.exe
	section=variable
	type=char
	description=[game] Selected team
	location=[[0x9d903c]+0x60]+0xd8
	section=value
	val_hotkey=120
	val_value=2
	val_description=USMC
	section=value
	val_hotkey=121
	val_value=1
	val_description=MEC

What this does is it defines a floating point variable that is accessed by first reading location 0x9d903c, then adding a value 0x60 to the result and reading the memory location of [0x9d903c]+0x60. Then again add an offset of 0xd8 to that and finally read the location on the resulting address. The floating point value can be either 1 or 2, which specifies which team you are on in game. Incidently, this setting also controls which team members you see when you open the map. 

EXPLANATION
-----------
In order to understand why this is done this way, you have to think about how C structures are arranged in memory. In this case, the location 0x9d903c is a static locaiton inside the process of BF2.exe. This is probably a singleton pointer, or a static global variable that points to the main game state. So you read the address of the memory pointed to by this address. 

Now, this game state probably also contains pointers to other variables that are dynamically allocated by "new" or "malloc" the addresses of these memory locations are not static, but the locations of pointers that point to them are. This is why we add an offset of 0x60 to the original location. This gives us the offset to the pointer that points to the next data structure (player state in this case). We read that location and then add a new offset of 0xd8 in the map state to get the position of the variable that indicates current team your local client thinks you are on. Overwriting this memory location obviously makes the rest of the client code think that you are on a different team than the team you are actually on and act accordingly. 

EPILOGUE
========

The application is written in C with GTK for the gui. Tested on Windows XP with an old old version of BF2 (think 2008). 
