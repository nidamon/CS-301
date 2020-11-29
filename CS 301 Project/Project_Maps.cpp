/*Project_Maps.cpp
Nathan Damon
10/9/2020
This is the cpp file for the maps class
NOTE: This code was made following along with javidx9's Role Playing game part #1
Linked video: https://www.youtube.com/watch?v=xXXt3htgDok
I have made some changes to this code to better fit my project and limitations
*/
//#pragma once
#include "Project_Maps.h"
#include "DecalMap.h"
#include <fstream>
using std::ifstream;
using std::ios;
using std::vector;
#include <Windows.h>
#include <random>
using std::random_device;

extern "C" int DimensionsSet(int, int); // Sets the width and height

extern "C" int CharOverlayGridModify(int, int); // Modifies the char overlay grid
extern "C" char CharOverlayGridGet(int, int); // Reads from the char overlay grid and returns tile value

extern "C" char CharSet(char); // Sets a char value
extern "C" char CharGridModify(int, int); // Modifies the char grid
extern "C" char CharGridGet(int, int); // Reads from the char grid and returns tile value

extern "C" int BoolGridModify(int, int); // Modifies the bool grid
extern "C" int BoolGridGet(int, int); // Reads from the bool grid and returns collision bool

cMap::cMap()
{
	pDecalB = nullptr;
	pDecalO = nullptr;
	nWidth = 0;
	nHeight = 0;
	m_solids = nullptr;
	m_indices = nullptr;
}

cMap::~cMap()
{
	delete[] m_solids;
	delete[] m_indices;
}

int cMap::ModifyOverlayIndex(int x, int y, int Selected_tile) // Nathan: Added to edit grid that is laid on top of the world
{
	if (x >= 0 && x < nWidth && y >= 0 && y < nHeight)
	{
		CharSet(Selected_tile); // Send the tile to assembly as an 8bit value
		// Set specified x,y value to the char sent to above
		CharOverlayGridModify(x, y); // (rcx, rdx)
	}
	else
		return 0; // If there is a problem with setting the tiles
}

int cMap::ModifyIndex(int x, int y, int Selected_tile) // Nathan: Added to edit maps more easily
{
	if (x >= 0 && x < nWidth && y >= 0 && y < nHeight)
	{
		CharSet(Selected_tile); // Send the tile to assembly as an 8bit value
		// Set specified x,y value to the char sent to above
		CharGridModify(x, y); // (rcx, rdx)
	}
	else
		return 0; // If there is a problem with setting the tiles
}

int cMap::ModifySolid(int x, int y, bool One) // Nathan: Added to edit the impassable tiles more easily
{
	if (x >= 0 && x < nWidth && y >= 0 && y < nHeight)
	{
		if (One) // Set to 1
		{
			if (BoolGridGet(x, y) == 0)
				// Set specified value to its opposite
				BoolGridModify(x, y); // (rcx, rdx)
		}
		else // Set to 0
		{
			if (BoolGridGet(x, y) == 1)
				// Set specified value to its opposite
				BoolGridModify(x, y); // (rcx, rdx)
		}
	}
	else
		return 0; // If there is a problem with setting the tiles
}

int cMap::GetOverlayIndex(int x, int y) // Nathan: Added an additional grid that is laid on top of the world
{
	if (x >= 0 && x < nWidth && y >= 0 && y < nHeight)
	{
		// Get the value at the specified coordinates
		return (int)CharOverlayGridGet(x, y);
	}
	else
		return 0; // If there is a problem with accessed information
}

int cMap::GetIndex(int x, int y)
{
	if (x >= 0 && x < nWidth && y >= 0 && y < nHeight)
	{
		// Get the value at the specified coordinates
		return (int)CharGridGet(x, y);
	}
	else
		return 0; // If there is a problem with accessed information
}

bool cMap::GetSolid(int x, int y)
{
	if (x >= 0 && x < nWidth && y >= 0 && y < nHeight)
	{
		// Get the value at the specified coordinates
		return BoolGridGet(x, y); // (rcx, rdx)
	}
	else
		return 0; // If there is a problem with accessed information
}

bool cMap::Create(string fileData, olc::Decal* BaseDecal, olc::Decal* OverlayDecal, string name)
{
	sName = name;
	pDecalB = BaseDecal;
	pDecalO = OverlayDecal;
	bool HasOverlay = false;
	ifstream data(fileData, ios::in | ios::binary);
	if (data.is_open())
	{
		data >> nWidth >> nHeight >> HasOverlay;
		if (nWidth * nHeight > 64 * 64)
			std::cout << "Dimensions error: Assembly too small." << std::endl;
		DimensionsSet(nWidth, nHeight); // Send dimensions to assembly

		int TempTileValue = 0;
		bool TempSolidValue = 0;
		for (int y = 0; y < nHeight; y++)
		{
			for (int x = 0; x < nWidth; x++)
			{
				data >> TempTileValue;
				if (TempTileValue < 0 || TempTileValue > 99)
					TempTileValue = 0;

				CharSet(TempTileValue); // Send the tile to assembly as an 8bit value
				// Set specified x,y value to the char sent to above
				CharGridModify(x, y); // (rcx, rdx)

				if (HasOverlay) // Check if map has overlay or not
				{
					data >> TempTileValue;
					if (TempTileValue < 0 || TempTileValue > 99)
						TempTileValue = 0;
					CharSet(TempTileValue); // Send the tile to assembly as an 8bit value
					// Set specified x,y value to the char sent to above
					CharOverlayGridModify(x, y); // (rcx, rdx)
				}

				data >> TempSolidValue;
				if (TempSolidValue)// Set specified value to its opposite
					BoolGridModify(x, y); // (rcx, rdx)
			}
		}
		return true;
	}
	else
		return false;
}

//##################################################################################################

cMap_Plains::cMap_Plains()
{
	//		  Map Text file			Tile sets              mapname    Area name
	Create("./Maps/MapContinent.txt", DecalMap::get().GetDecal("Plains"), DecalMap::get().GetDecal("Overlay"), "Plains");
}

bool cMap_Plains::PopulateDynamics(vector<cDynamic*>& vecDyns, std::default_random_engine& e1)
{
	//Rabbit
	for (int i = 0; i < 20; i++)
	{
		std::uniform_int_distribution<int> uniform_dist(0, 128); 
		cDynamic* g1 = new cDynamic_Creature_Rabbit();
		vecDyns.push_back(g1);

		g1->_posx = uniform_dist(e1) % 64;
		g1->_posy = uniform_dist(e1) % 64;
		while (BoolGridGet(g1->_posx, g1->_posy) != 0) // Get a position that is not solid
		{
			g1->_posx = uniform_dist(e1) % 64;
			g1->_posy = uniform_dist(e1) % 64;
		}

		((cDynamic_Creature*)g1)->_age = 90.0f;
		if (uniform_dist(e1) % 2 == 0)
		{
			((cDynamic_Creature*)g1)->_cGender = 'M';
			std::cout << "Male Rabbit initialized" << std::endl;
		}
		else
		{
			((cDynamic_Creature*)g1)->_cGender = 'F';
			std::cout << "Female Rabbit initialized" << std::endl;
		}
	}
	// Fox
	for (int i = 0; i < 10; i++)
	{
		std::uniform_int_distribution<int> uniform_dist(0, 128);
		cDynamic* g1 = new cDynamic_Creature_Fox();
		vecDyns.push_back(g1);

		g1->_posx = uniform_dist(e1) % 64;
		g1->_posy = uniform_dist(e1) % 64;
		while (BoolGridGet(g1->_posx, g1->_posy) != 0) // Get a position that is not solid
		{
			g1->_posx = uniform_dist(e1) % 64;
			g1->_posy = uniform_dist(e1) % 64;
		}

		((cDynamic_Creature*)g1)->_age = 90.0f;
		if (uniform_dist(e1) % 2 == 0)
		{
			((cDynamic_Creature*)g1)->_cGender = 'M';
			std::cout << "Male Fox initialized" << std::endl;
		}
		else
		{
			((cDynamic_Creature*)g1)->_cGender = 'F';
			std::cout << "Female Fox initialized" << std::endl;
		}
	}
	// Bear
	for (int i = 0; i < 10; i++)
	{
		std::uniform_int_distribution<int> uniform_dist(0, 128);
		cDynamic* g1 = new cDynamic_Creature_Bear();
		vecDyns.push_back(g1);

		g1->_posx = uniform_dist(e1) % 64;
		g1->_posy = uniform_dist(e1) % 64;
		while (BoolGridGet(g1->_posx, g1->_posy) != 0) // Get a position that is not solid
		{
			g1->_posx = uniform_dist(e1) % 64;
			g1->_posy = uniform_dist(e1) % 64;
		}

		((cDynamic_Creature*)g1)->_age = 90.0f;
		if (uniform_dist(e1) % 2 == 0)
		{
			((cDynamic_Creature*)g1)->_cGender = 'M';
			std::cout << "Male Bear initialized" << std::endl;
		}
		else
		{
			((cDynamic_Creature*)g1)->_cGender = 'F';
			std::cout << "Female Bear initialized" << std::endl;
		}
	}
	
	std::cout << std::endl;
	return true;
}

bool cMap_Plains::OnInteraction(vector<cDynamic*>& vecDynobject, cDynamic* target, NATURE nature)
{
	if (target->_sName == "Rabbit" || target->_sName == "Fox" || target->_sName == "Bear")
	{
		cDynamic_Creature* dyn = (cDynamic_Creature*)target;
		dyn->_nHealth = max(dyn->_nHealth - 15, 0);
	}
	return false;
}