/*DecalMap.h
Nathan Damon
10/9/2020
The header holds a map of paths to sprites/decals for use with Project 301
NOTE: This code was made following along with javidx9's Role Playing game part #1
Linked video: https://www.youtube.com/watch?v=xXXt3htgDok
I have made some changes to this code to better fit my project and limitations
*/

#ifndef DECALMAP_H
#define DECALMAP_H

#include "olcPixelGameEngine.h"
using namespace olc;
using std::string;
using std::map;
using std::vector;


class DecalMap
{
public:
	static DecalMap& get()
	{
		static DecalMap me;
		return me;
	}

	DecalMap(DecalMap const&) = delete;
	void operator=(DecalMap const&) = delete;

	olc::Decal* GetDecal(string name)
	{
		return m_mapDecals[name]; // State specifies item in vector
	}

	void LoadDecals();

private:
	DecalMap();
	~DecalMap();

	map<string, olc::Decal*> m_mapDecals;
};

#endif /* DECALMAP */

