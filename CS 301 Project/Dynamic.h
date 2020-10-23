/*Dynamic.h
Nathan Damon
10/15/2020
This is the cpp file for the dynamic class
NOTE: This code was made following along with javidx9's Role Playing game part #2
Linked video: https://www.youtube.com/watch?v=AWY_ITpldRk
I have made some changes to this code to better fit my project and limitations
*/

//#pragma once
#ifndef DYNAMIC_H
#define DYNAMIC_H

#include "olcPixelGameEngine.h"
using std::string;
using olc::PixelGameEngine;

class cDynamic
{
public:
	cDynamic(string n);
	~cDynamic();

public:
	float _posx, _posy;  // Posistion x,y
	float _velx, _vely;  // Velocity x,y
	bool _bSolidVsMap;
	bool _bSolidVsDyn;
	bool _bFriendly;
	bool _bRedundant;
	bool _bIsAttackable;  // Do I need this? //  probably not for this project ////////////////////////////////////////////////////////////////////////////////
	string _sName;

	float _Mass; // Nathan: added this for some movement code that I think will turn out interesting

public:
	virtual void DrawSelf(PixelGameEngine* gfx, float offsetx, float offsety) {}
	virtual void Update(float fElapsedTime, string season, cDynamic* player = nullptr) {}
};

//##################################################################################################

class cDynamic_Creature : public cDynamic
{
public:
	cDynamic_Creature(string n, olc::Decal* sprite);

protected:
	olc::Decal* m_pDecal;
	float m_fTimer;
	int m_nGraphicCounter = 0;
	enum { SOUTH = 0, WEST = 1, NORTH = 2, EAST = 3 } m_nFacingDirection;
	enum { STANDING, WALKING, CELEBRATING, DEAD } m_nGraphicState;

public:
	int _nHealth;
	int _nHealthMax;

	float _DurationBeforeDeletion; // Nathan: Added the below variables
	bool _bMingle;
	bool _bHasMingled;
	int _GrowthStage;
	float _age;
	int _fullness; // % amount
	char _cGender;

public:
	void DrawSelf(PixelGameEngine* gfx, float offsetx, float offsety) override;
	virtual void Update(float fElapsedTime, string season, cDynamic* player = nullptr) override;
	virtual void Behaviour(float fElapsedTime, string season, cDynamic* player = nullptr);
	int GetFacingDirection();

protected:
	float m_fstateTick;

};

//##################################################################################################

class cDynamic_Creature_Rabbit : public cDynamic_Creature
{
public:
	cDynamic_Creature_Rabbit();
	virtual void Behaviour(float fElapsedTime, string season, cDynamic* player = nullptr) override;

};

#endif /* DYNAMIC */