/*DecalMap.cpp
Nathan Damon
10/9/2020
The cpp file for DecalMap.h for use with Project 301
NOTE: This code was made following along with javidx9's Role Playing game part #1
Linked video: https://www.youtube.com/watch?v=xXXt3htgDok
I have made some changes to this code to better fit my project and limitations
*/


#include "DecalMap.h"
#include "Project_Maps.h"

DecalMap::DecalMap()
{
}

DecalMap::~DecalMap()
{
}

void DecalMap::LoadDecals()
{
	auto load = [&](string sName, string sFilename)
	{
		olc::Decal* d = new olc::Decal(new olc::Sprite(sFilename));
		m_mapDecals[sName] = d;
	};

	// Map
	load("Plains", "./Sprites/Environment/PlainsTileSet.png");

	// Creatures
	load("TestSpriteSheet", "./Sprites/Creatures/TestSpriteSheet.png");
	load("Rabbit", "./Sprites/Creatures/Rabbit.png");

	// Environment
	load("Tree1", "./Sprites/Environment/Tree1.png");

	// User Interface
		// Unimplemented
}