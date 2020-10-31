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

extern "C" char CharSet(char); // Sets a char value
extern "C" char CharGridModify(int, int); // Modifies the char grid
extern "C" char CharGridGet(int, int); // Reads from the char grid and returns tile value

extern "C" int BoolGridModify(int, int); // Modifies the bool grid
extern "C" int BoolGridGet(int, int); // Reads from the bool grid and returns collision bool


cMap::cMap()
{
	pDecal = nullptr;
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

bool cMap::Create(string fileData, olc::Decal* Decal, string name)
{
	sName = name;
	pDecal = Decal;
	ifstream data(fileData, ios::in | ios::binary);
	if (data.is_open())
	{
		data >> nWidth >> nHeight;
		if (nWidth * nHeight > 64 * 64)
			std::cout << "Dimensions error: Assembly too small." << std::endl;
		DimensionsSet(nWidth, nHeight); // Send dimensions to assembly
		m_solids = new bool[nWidth * nHeight];
		m_indices = new int[nWidth * nHeight];

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
	Create("./Maps/MapPlains1.txt", DecalMap::get().GetDecal("Plains"), "Plains");
}

bool cMap_Plains::PopulateDynamics(vector<cDynamic*>& vecDyns, std::default_random_engine& e1)
{
	// Rabbit
	for (int i = 0; i < 0; i++)
	{
		std::uniform_int_distribution<int> uniform_dist(0, 128); 
		cDynamic* g1 = new cDynamic_Creature_Rabbit();
		vecDyns.push_back(g1);
		g1->_posx = uniform_dist(e1) % 15 + 5.0f;
		g1->_posy = uniform_dist(e1) % 7 + 1.0f;
		((cDynamic_Creature*)g1)->_age = 90.0f;
		if (uniform_dist(e1) % 2 == 0)
		{
			((cDynamic_Creature*)g1)->_cGender = 'M';
			std::cout << "Male Rabbit created" << std::endl;
		}
		else
		{
			((cDynamic_Creature*)g1)->_cGender = 'F';
			std::cout << "Female Rabbit created" << std::endl;
		}
	}
	// Fox
	for (int i = 0; i < 0; i++)
	{
		std::uniform_int_distribution<int> uniform_dist(0, 128);
		cDynamic* g1 = new cDynamic_Creature_Fox();
		vecDyns.push_back(g1);
		g1->_posx = uniform_dist(e1) % 15 + 5.0f;
		g1->_posy = uniform_dist(e1) % 7 + 1.0f;
		((cDynamic_Creature*)g1)->_age = 90.0f;
		if (uniform_dist(e1) % 2 == 0)
		{
			((cDynamic_Creature*)g1)->_cGender = 'M';
			std::cout << "Male Fox created" << std::endl;
		}
		else
		{
			((cDynamic_Creature*)g1)->_cGender = 'F';
			std::cout << "Female Fox created" << std::endl;
		}

	}
	// Bear
	for (int i = 0; i < 0; i++)
	{
		std::uniform_int_distribution<int> uniform_dist(0, 128);
		cDynamic* g1 = new cDynamic_Creature_Bear();
		vecDyns.push_back(g1);
		g1->_posx = uniform_dist(e1) % 15 + 5.0f;
		g1->_posy = uniform_dist(e1) % 7 + 1.0f;
		((cDynamic_Creature*)g1)->_age = 90.0f;
		if (uniform_dist(e1) % 2 == 0)
		{
			((cDynamic_Creature*)g1)->_cGender = 'M';
			std::cout << "Male Bear created" << std::endl;
		}
		else
		{
			((cDynamic_Creature*)g1)->_cGender = 'F';
			std::cout << "Female Bear created" << std::endl;
		}

	}
	
	std::cout << std::endl;

	/*cDynamic_Creature* tree1 = new cDynamic_Creature("Tree", DecalMap::get().GetDecal("Tree1"));
	tree1->_posx = 15.0f;
	tree1->_posy = 6.0f;
	vecDyns.push_back(tree1);*/

	return true;
}

bool cMap_Plains::OnInteraction(vector<cDynamic*>& vecDynobject, cDynamic* target, NATURE nature)
{
	if (target->_sName == "Tree")
	{
		if (target->_bSolidVsDyn == true)
			target->_bSolidVsDyn = false;
		else if (target->_bSolidVsDyn == false)
			target->_bSolidVsDyn = true;
	}

	if (target->_sName == "Rabbit" || target->_sName == "Fox" || target->_sName == "Bear")
	{
		cDynamic_Creature* dyn = (cDynamic_Creature*)target;
		dyn->_nHealth = max(dyn->_nHealth - 15, 0);
	}


	return false;
}