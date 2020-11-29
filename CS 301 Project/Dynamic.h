/*Dynamic.h
Nathan Damon
10/15/2020
This is the cpp file for the dynamic class
NOTE: This code was made following along with javidx9's Role Playing game part #2
Linked video: https://www.youtube.com/watch?v=AWY_ITpldRk
I have made some changes to this code to better fit my project and limitations
*/


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
	string _sName;

	float _Mass; // Nathan: added this for some movement code that I think will turn out interesting
	float _DynamicRadius; // Gives the radius for dynamic collision repulsion
	float _collision_velx, _collision_vely;  // Collision Velocity x,y
	int _number_collisions; // Keep track of how many objects colliding with
	float _oldposx; // old position
	float _oldposy;

public:
	virtual void DrawSelf(PixelGameEngine* gfx, float offsetx, float offsety) {}
	virtual void Update(float fElapsedTime, int season, cDynamic* target = nullptr) {}
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

	// Nathan: Added the below variables
	float _DurationBeforeDeletion; 
	bool _bMingle;
	bool _bHasMingled;
	int _GrowthStage;
	float _age;
	int _fullness; // % amount
	char _cGender;
	float _DynamicRadius; // Gives the radius for dynamic collision repulsion
	float _fViewRange; // How far away the creature can see
	float _fTimeToGrow; // How fast the creatures grow up

	bool _bHunt; // Is creature hunting
	bool _bNoTarget;
	bool _bTargetSelected;
	float _fCooldown; // Cooldown for attacking
	int _AttackDmg;
	float _fReach;
	int _Meatleft;
	int _EatAmount;
	float _fEatCooldown;
	int _FoodChain;
	cDynamic* _Target;

	float _fSpeed;
	bool _bHave_Path;
	std::vector<int> _vPath;
	float _posx1; // The four pos here are for calculating the next point to move to
	float _posy1;
	float _posx2;
	float _posy2;
	float _fPathTimeCounter;
	float _fdestinationX;
	float _fdestinationY;

	float _HungerDegredation;
	float _fRegenrationTimeForOneHP;
	float _fRegenTimer;

public:
	void DrawSelf(PixelGameEngine* gfx, float offsetx, float offsety) override;
	virtual void Update(float fElapsedTime, int season, cDynamic* target = nullptr) override;
	virtual void Behaviour(float fElapsedTime, int season, cDynamic* target = nullptr);
	int GetFacingDirection();
	float MoveTo(const float Target_posx, const float Target_posy); // Nathan: Path finds and moves in a direction

protected:
	float m_fstateTick;

};

//##################################################################################################

class cDynamic_Creature_Rabbit : public cDynamic_Creature
{
public:
	cDynamic_Creature_Rabbit();
	int _BerryposX;
	int _BerryposY;
	virtual void Behaviour(float fElapsedTime, int season, cDynamic* target = nullptr) override;

};

//##################################################################################################

class cDynamic_Creature_Fox : public cDynamic_Creature
{
public:
	cDynamic_Creature_Fox();
	virtual void Behaviour(float fElapsedTime, int season, cDynamic* target = nullptr) override;

};

//##################################################################################################

class cDynamic_Creature_Bear : public cDynamic_Creature
{
public:
	cDynamic_Creature_Bear();
	virtual void Behaviour(float fElapsedTime, int season, cDynamic* target = nullptr) override;

};
#endif /* DYNAMIC */