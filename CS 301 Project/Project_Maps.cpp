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


cMap::cMap()
{
	pDecal = nullptr;
	//pDecalV = { nullptr };
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
		m_indices[y * nWidth + x] = Selected_tile;
	else
		return 0; // If there is a problem with setting the tiles
}

int cMap::GetIndex(int x, int y)
{
	if (x >= 0 && x < nWidth && y >= 0 && y < nHeight)
		return m_indices[y * nWidth + x];
	else
		return 0; // If there is a problem with accessed information
}

bool cMap::GetSolid(int x, int y)
{
	if (x >= 0 && x < nWidth && y >= 0 && y < nHeight)
		return m_solids[y * nWidth + x];
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
		m_solids = new bool[nWidth * nHeight];
		m_indices = new int[nWidth * nHeight];
		for (int i = 0; i < nWidth * nHeight; i++)
		{
			data >> m_indices[i];
			data >> m_solids[i];
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
	Create("./Maps/MapTest2.txt", DecalMap::get().GetDecal("Plains"), "Plains");
}

bool cMap_Plains::PopulateDynamics(vector<cDynamic*>& vecDyns, std::default_random_engine& e1)
{

	for (int i = 0; i < 6; i++)
	{
		std::uniform_int_distribution<int> uniform_dist(0, 128);
		cDynamic* g1 = new cDynamic_Creature_Rabbit();
		vecDyns.push_back(g1);
		g1->_posx = uniform_dist(e1) % 15 + 5.0f;
		g1->_posy = uniform_dist(e1) % 15 + 5.0f;
		((cDynamic_Creature*)g1)->_age = 90;
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
	std::cout << std::endl;

	cDynamic_Creature* tree1 = new cDynamic_Creature("Tree", DecalMap::get().GetDecal("Tree1"));
	tree1->_posx = 15.0f;
	tree1->_posy = 6.0f;
	vecDyns.push_back(tree1);

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

	if (target->_sName == "Rabbit")
	{
		cDynamic_Creature_Rabbit* dyn = (cDynamic_Creature_Rabbit*)target;
		dyn->_nHealth = max(dyn->_nHealth - 5, 0);
	}


	return false;
}