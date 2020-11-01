/*Project_Maps.h
Nathan Damon
10/9/2020
This is the header file for the maps class
NOTE: This code was made following along with javidx9's Role Playing game part #1
Linked video: https://www.youtube.com/watch?v=xXXt3htgDok
I have made some changes to this code to better fit my project and limitations
*/

#ifndef PROJECT_MAPS_H
#define PROJECT_MAPS_H

#include "Dynamic.h"
#include "DecalMap.h"
#include <random>
using std::random_device;
using std::string;
using std::map;
using std::vector;

class cMap
{
public:
	enum NATURE
	{
		TALK,
		WALK,
		FIGHT
	};

public:
	cMap();
	~cMap();

public:
	int nWidth;
	int nHeight;
	string sName;
	olc::Decal* pDecalB; // Base grid decal
	olc::Decal* pDecalO; // Overlay grid decal

	int ModifyOverlayIndex(int x, int y, int Selected_tile); // Nathan: Added to edit grid that is laid on top of the world
	int ModifyIndex(int x, int y, int Selected_tile); // Nathan: Added to edit maps more easily
	int ModifySolid(int x, int y, bool One); // Nathan: Added to edit the impassable tiles more easily
	int GetOverlayIndex(int x, int y); // Nathan: Added an additional grid that is laid on top of the world
	int GetIndex(int x, int y);
	bool GetSolid(int x, int y);
	bool Create(string fileData, olc::Decal* BaseDecal, olc::Decal* OverlayDecal, string name);

	virtual bool PopulateDynamics(vector<cDynamic*>& vecDyns, std::default_random_engine& e1)
	{
		return false;
	}

	virtual bool OnInteraction(vector<cDynamic*>& vecDynobject, cDynamic* target, NATURE nature)
	{
		return false;
	}

private:
	int* m_indices = nullptr;
	bool* m_solids = nullptr;
};

//##################################################################################################

class cMap_Plains : public cMap
{
public:
	cMap_Plains();
	bool PopulateDynamics(vector<cDynamic*>& vecDyns, std::default_random_engine& e1)override;
	bool OnInteraction(vector<cDynamic*>& vecDynobject, cDynamic* target, NATURE nature) override;
};

#endif /* PROJECT_MAPS */
