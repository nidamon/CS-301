/*Dynamic.cpp
Nathan Damon
10/15/2020
This is the cpp file for the dynamic class
NOTE: This code was made following along with javidx9's Role Playing game part #2
Linked video: https://www.youtube.com/watch?v=AWY_ITpldRk
I have made some changes to this code to better fit my project and limitations
*/

#include "Dynamic.h"
#include "DecalMap.h"
#include "PathFinding.h"

void MovementCheck(float& x, float& y, float& oldx, float& oldy, string name); // checks for sudden teleporting

float MoveToTarget(const int Target_posx, const int Target_posy, bool& bNoTarget,
	const float posx, const float posy, float& velx, float& vely,
	const float fViewRange, const float& fReach, const float& fSpeed,
	vector<int>& vPath, bool& bHave_Path); // Moves the creature towards its target

void ShouldHunt(bool& bHunt, const float& fullness, bool& bNoTarget); // Determines whether or not the creature should hunt

void Hunger(int& fullness, const int& HungerDegredation, int& Health); // Updates the fullness and how much health is lost for low fulness

void MovementCorrection(const float movementspeed, float& velx, float& vely, 
	const float startposx, const float startposy, 
	const float finishposx, const float finishposy); // Returns a fixed movement speed for any direction

extern "C" char CharOverlayGridGet(int, int); // Reads from the char overlay grid and returns tile value

cDynamic::cDynamic(string n)
{
	_sName = n;
	_posx = 0.0f;
	_posy = 0.0f;
	_velx = 0.0f;
	_vely = 0.0f;
	_bSolidVsMap = true;
	_bSolidVsDyn = true;
	_bFriendly = true;
	_bRedundant = false;

	_DynamicRadius = 0.5f;
	_collision_velx = 0.0f;
	_collision_vely = 0.0f;
	_number_collisions = 0;
	_oldposx = 0.0f;
	_oldposy = 0.0f;
}

cDynamic::~cDynamic()
{

}

//##################################################################################################

cDynamic_Creature::cDynamic_Creature(string name, olc::Decal* sprite) : cDynamic(name)
{
	m_pDecal = sprite;
	_nHealth = 10;
	_nHealthMax = 10;
	m_nFacingDirection = SOUTH;
	m_nGraphicState = STANDING;
	m_nGraphicCounter = 0;
	m_fTimer = 0.0f;
	m_fstateTick = 0.5f;
	_bRedundant = false;
	
	// Nathan: Added the below variables
	_DurationBeforeDeletion = 9.0f; 
	_bMingle = false;
	_bHasMingled = false;
	_GrowthStage = 0;
	_age = 0.0f;
	_fullness = 100; // % amount
	_cGender = 'M';
	_DynamicRadius = 0.5f;
	_fViewRange = 3.0f;
	_fTimeToGrow = 0.2f;

	_bHunt = false;
	_bNoTarget = true;
	_bTargetSelected = false;
	_fCooldown = 0.0f;
	_AttackDmg = 5;
	_fReach = 1.0f;
	_Meatleft = 100;
	_EatAmount = 20;
	_fEatCooldown = 0.0f;
	_FoodChain = 0;
	_Target = nullptr;

	_fSpeed = 1.0f;
	_bHave_Path = false;
	_vPath;
	_posx1 = 5;
	_posy1 = 5;
	_posx2 = 5;
	_posy2 = 5;
	_fPathTimeCounter = 0.25f;
	_fdestinationX = 5;
	_fdestinationY = 5;

	_HungerDegredation = 3;
	_fRegenrationTimeForOneHP = 1.0f;
	_fRegenTimer = 0.0f;
}

void cDynamic_Creature::Update(float fElapsedTime, int season, cDynamic* target)
{
	m_fTimer += fElapsedTime;
	if (m_fTimer >= 0.2f)
	{
		m_fTimer -= 0.2f;
		m_nGraphicCounter++;
		m_nGraphicCounter %= 2; // swaps between 0 and 1
	}

	if (fabs(_velx) > 0 || fabs(_vely) > 0)
		m_nGraphicState = WALKING;
	else
		m_nGraphicState = STANDING;

	if (_nHealth <= 0)
		m_nGraphicState = DEAD;

	if (_velx < -0.1f) m_nFacingDirection = WEST;
	if (_velx > 0.1f) m_nFacingDirection = EAST;
	if (_vely < -0.1f) m_nFacingDirection = NORTH;
	if (_vely > 0.1f) m_nFacingDirection = SOUTH;

	if (_nHealth <= 0)
		_bSolidVsDyn = false;
	_fRegenTimer += fElapsedTime;
	if (_nHealth > 0) // Check if dead
		if (_fullness > 70) // Need some food to heal
			if (_nHealth < _nHealthMax) // Stop regenerating at full health
				if (_fRegenTimer > _fRegenrationTimeForOneHP)
				{
					_nHealth += 1;
					_fRegenTimer = 0.0f;
				}

	Behaviour(fElapsedTime, season, target);////////////////////////////////////////////////////
}

void cDynamic_Creature::DrawSelf(PixelGameEngine* gfx, float offsetx, float offsety)
{
	float nSheetOffsetX = 0; // Offset in the sprite sheet in the case that the there are other sprites before this one on it
	float nSheetOffsetY = 0;

	switch (m_nGraphicState) // Picks the partial sprite according to how the creature is behaiving
	{
	case STANDING:
		nSheetOffsetX = m_nFacingDirection * 16;
		break;
	case WALKING:
		nSheetOffsetX = m_nFacingDirection * 16;
		nSheetOffsetY = m_nGraphicCounter * 16;
		break;
	case DEAD: //5th item 2nd row of sprite sheet
		nSheetOffsetX = 4 * 16;
		nSheetOffsetY = 1 * 16;
		break;
	}

	float growthscale = 0.6f; // Nathan: Added stages of growth
	if (_GrowthStage == 1)
		growthscale = 0.7f;
	if (_GrowthStage == 2)
		growthscale = 0.85f;
	if (_GrowthStage == 3)
		growthscale = 1.0f;

	gfx->DrawPartialDecal( // Sprite
		{ (_posx - offsetx) * 16.0f, (_posy - offsety) * 16.0f }, 
		m_pDecal, { nSheetOffsetX, nSheetOffsetY }, { 16, 16 }, 
		{ growthscale, growthscale });

	if (_nHealth > 0)
	{
		gfx->DrawPartialDecal( // Health bar
			{ (_posx - offsetx) * 16.0f + 3.0f, (_posy - offsety - 0.2f) * 16.0f },
			DecalMap::get().GetDecal("Status"), { 10.0f - 10.0f * ((float)_nHealth / (float)_nHealthMax), 0 }, { 10, 1 },
			{ growthscale, growthscale });

		gfx->DrawPartialDecal( // Fullness bar
			{ (_posx - offsetx) * 16.0f + 3.0f, (_posy - offsety - 0.1f) * 16.0f },
			DecalMap::get().GetDecal("Status"), { 10.0f - (float)(_fullness / 10), 1 }, { 10, 1 },
			{ growthscale, growthscale });
	}
	else
	{
		int MaxMeatleft = 10;
		if (_sName == "Rabbit")
			MaxMeatleft = 70;
		if (_sName == "Fox")
			MaxMeatleft = 100;
		if (_sName == "Bear")
			MaxMeatleft = 200;
		gfx->DrawPartialDecal( // Meatleft bar
			{ (_posx - offsetx) * 16.0f + 3.0f, (_posy - offsety - 0.1f) * 16.0f },
			DecalMap::get().GetDecal("Status"), { 10.0f - (float)(_Meatleft * 10 / MaxMeatleft), 2 }, { 10, 1 },
			{ growthscale, growthscale });
	}
}

void cDynamic_Creature::Behaviour(float fElapsedTime, int season, cDynamic* target)
{
	// No default behaviour
}

int cDynamic_Creature::GetFacingDirection()
{
	return m_nFacingDirection;
}

// Nathan: Added Moveto for path finding
float cDynamic_Creature::MoveTo(float Target_posx, float Target_posy)
{
	float distance = (Target_posx - _posx) * (Target_posx - _posx) + (Target_posy - _posy) * (Target_posy - _posy);
	if (distance <= 0.125f)
	{
		_bHave_Path = false;
		return distance;
	}

	if ((int)Target_posx < 1) // Correct the x and y for using the integral image
		Target_posx = 1.0;
	if ((int)Target_posy < 1)
		Target_posy = 1.0;
	if ((int)Target_posx > Width - 1) // Correct the x and y for attempt to path find out side of map
		Target_posx = Width - 1;
	if ((int)Target_posy > Width - 1) // The width is the same as the hieght
		Target_posy = Width - 1;

	if (!_bHave_Path)
	{
		PathFinding Path((int)_posx, (int)_posy, (int)Target_posx, (int)Target_posy);
		_vPath = Path.Path_Get(); // Creates and returns a path to follow
		if (_vPath[0] == -1)
		{
			_vPath.clear();
			return 0.0f; // Stops the path finding
		}
		_bHave_Path = true;

		_posx1 = (float)(_vPath.back() % Width);
		_posy1 = (float)(_vPath.back() / Width);

		if (_vPath.size() > 1)
		{
			_posx2 = (float)(_vPath[_vPath.size() - 2] % Width); // These get the x and y of the second to last item
			_posy2 = (float)(_vPath[_vPath.size() - 2] / Width);
		}

	}
	else
	{
		if (_vPath.size() > 1)
		{
			// If close enough to the next point set new pos1 and pos2
			if ((_posx1 - _posx2) * (_posx1 - _posx2) + (_posy1 - _posy2) * (_posy1 - _posy2) + 0.01f >
				(_posx2 - _posx)* (_posx2 - _posx) + (_posy2 - _posy) * (_posy2 - _posy))
			{
				_vPath.pop_back(); // Remove the last item as we are at its location
				_posx1 = _posx2;
				_posy1 = _posy2;
				if (_vPath.size() > 1) // Recheck for more than 1 item in path
				{
					_posx2 = (float)(_vPath[_vPath.size() - 2] % Width); // These get the x and y of the second to last item
					_posy2 = (float)(_vPath[_vPath.size() - 2] / Width);
				}
			}
			MovementCorrection(_fSpeed, _velx, _vely, _posx, _posy, _posx1, _posy1);
		}
		else
		{
			MovementCorrection(_fSpeed, _velx, _vely, _posx, _posy, _posx1, _posy1);
			if ((_posx1 - _posx) * (_posx1 - _posx) + (_posy1 - _posy) * (_posy1 - _posy) < 0.125f) // 0.25 being 0.5 units away from target pos
			{
				if (_vPath.size() == 1)
					_vPath.pop_back(); // Remove the final item as we are at its location
				_bHave_Path = false;
			}
		}
	}
	return distance;
}

//##################################################################################################

cDynamic_Creature_Rabbit::cDynamic_Creature_Rabbit() : cDynamic_Creature("Rabbit", DecalMap::get().GetDecal("Rabbit"))
{
	_bFriendly = false;
	_nHealth = 20;
	_nHealthMax = 20;
	m_fstateTick = 1.0f;
	_bRedundant = false;
	_bMingle = false;
	_bHasMingled = false;
	_Mass = 1.5f; // baby rabbit weight
	_DynamicRadius = 0.25f; // Rabbits are small right?
	_fViewRange = 4.0f;
	_fTimeToGrow = 1.5f;

	_bHunt = false;
	_bNoTarget = true;
	_bTargetSelected = false;
	_fReach = 1.0f;
	_Meatleft = 70;
	_EatAmount = 8;
	_fEatCooldown = 0.0f;
	_FoodChain = 1;
	_fSpeed = 0.6f;

	_BerryposX = 5;
	_BerryposY = 5;
}

void cDynamic_Creature_Rabbit::Behaviour(float fElapsedTime, int season, cDynamic* target)
{
	if (_nHealth > 0) //Nathan: If the rabbit is still alive
	{

		if (_GrowthStage != 3 && _age < 100.0f) // Nathan: Added for aging
		{
			_age += (fElapsedTime / _fTimeToGrow);
			if (_GrowthStage < 1 &&_age >= 10.0f && _age < 20.0f)
			{
				_Meatleft = 40;
				_EatAmount = 4;
				_Mass = 1.5f;
				_GrowthStage = 1;
			}

			if (_GrowthStage < 2 && _age >= 20.0f && _age < 30.0f)
			{
				_Meatleft = 50;
				_EatAmount = 6;
				_Mass = 2.0f;
				_GrowthStage = 2;
				_bSolidVsDyn = true;
			}
			if (_GrowthStage < 3 && _age >= 30.0f)
			{
				_Meatleft = 70;
				_EatAmount = 8;
				_Mass = 3.0f;
				_GrowthStage = 3;
			}

		}

		if (_GrowthStage == 3 && season == 1 && !_bHasMingled)
			_bMingle = true;

		if (_GrowthStage == 3 && season != 1)
		{
			_bHasMingled = false;
		}

		m_fstateTick -= fElapsedTime; // This runs miscellaneous behaviours 
		if (m_fstateTick <= 0.0f)
		{
			Hunger(_fullness, _HungerDegredation, _nHealth);
			if (_fullness < 60) // This gets the rabbit to search for berry bushes
			{
				int TempDistance = 100;
				int NewTempDistance = 100;
				for (int y = (int)_posy - 8; y < (int)_posy + 8; y++)
				{
					if (y < 0)
						y = 0;
					if (y > Height - 1) // Height is from ASM
						break;
					for (int x = (int)_posx - 8; x < (int)_posx + 8; x++)
					{
						if (x < 0)
							x = 0;
						if (x > Width - 1) // Width is from ASM
							break;
						if ((int)CharOverlayGridGet(x, y) == 9)
						{
							NewTempDistance = (x - (int)_posx) * (x - (int)_posx) + (y - (int)_posy) * (y - (int)_posy);
							if (NewTempDistance < TempDistance) // Set new closest berry bush coordinates
							{
								TempDistance = NewTempDistance;
								_BerryposX = x;
								_BerryposY = y;
								_bNoTarget = false; // On the hunt for berry delicious things
							}
						}
					}
				}
			}
			m_fstateTick += 1.5f;
		}

		if (!_bNoTarget) // This if statement get the rabbit to move towards the berry bush
		{
			float fDistance = MoveToTarget(_BerryposX, _BerryposY, _bNoTarget, _posx, _posy, _velx, _vely, _fViewRange, _fReach, _fSpeed, _vPath, _bHave_Path);
			if (!_bHave_Path)
			{
				_fdestinationX = _BerryposX;
				_fdestinationY = _BerryposY;
				_vPath.clear();
			}

			if (fDistance < _fReach)
			{
				if (_fEatCooldown <= 0.0f) // This if for eating the berries
				{
					_fullness += _EatAmount; // Makeing the berry bushes have unlimited berries for rabbits at least
					_fEatCooldown += 0.8f;
				}
				else
					_fEatCooldown -= fElapsedTime;
			}
		}

		if (_fullness == 100) // The rabbit is no longer hungery
			_bNoTarget = true;

		_fPathTimeCounter -= fElapsedTime; // This runs the path finding code
		if (_fPathTimeCounter <= 0.0f)
		{
			if (!_bHave_Path && _bNoTarget)
			{
				_fdestinationX = _posx + (float)((rand() % 8) - 4);
				_fdestinationY = _posy + (float)((rand() % 8) - 4);
			}

			MoveTo(_fdestinationX, _fdestinationY); // Move to the set location
			MovementCheck(_posx, _posy, _oldposx, _oldposy, _sName);

			_fPathTimeCounter += 0.25f;
		}
	}
	else // Nathan: Added for death
	{
		_velx = 0;
		_vely = 0;
		_DurationBeforeDeletion -= fElapsedTime;
		if (_DurationBeforeDeletion < 0.0f)
			_bRedundant = true;
	}
}

//##################################################################################################

cDynamic_Creature_Fox::cDynamic_Creature_Fox() : cDynamic_Creature("Fox", DecalMap::get().GetDecal("Fox"))
{
	_bFriendly = false;
	_nHealth = 50;
	_nHealthMax = 50;
	m_fstateTick = 1.0f;
	_bRedundant = false;
	_bMingle = false;
	_bHasMingled = false;
	_Mass = 3.0f;
	_DynamicRadius = 0.4f;
	_fViewRange = 6.0f;
	_fTimeToGrow = 3.5f;

	_bHunt = false;
	_bNoTarget = true;
	_bTargetSelected = false;
	_fCooldown = 0.0f;
	_AttackDmg = 8;
	_fReach = 1.5f;
	_Meatleft = 100;
	_EatAmount = 20;
	_fEatCooldown = 0.0f;
	_FoodChain = 2;
	_Target = nullptr;
	_fSpeed = 0.7f;
}

void cDynamic_Creature_Fox::Behaviour(float fElapsedTime, int season, cDynamic* target)
{
	if (_nHealth > 0) //Nathan: If the Fox is still alive
	{
		if (_GrowthStage != 3 && _age < 100.0f) // Nathan: Added for aging
		{
			_age += (fElapsedTime / _fTimeToGrow);
			if (_GrowthStage < 1 && _age >= 10.0f && _age < 20.0f)
			{
				_AttackDmg = 3;
				_Meatleft = 30;
				_EatAmount = 10;
				_Mass = 3.0f;
				_GrowthStage = 1;
			}
			if (_GrowthStage < 2 && _age >= 20.0f && _age < 30.0f)
			{
				_AttackDmg = 5;
				_Meatleft = 60;
				_EatAmount = 15;
				_Mass = 4.0f;
				_GrowthStage = 2;
			}
			if (_GrowthStage < 3 && _age >= 30.0f)
			{
				_AttackDmg = 8;
				_Meatleft = 100;
				_EatAmount = 20;
				_Mass = 5.0f;
				_GrowthStage = 3;
			}

		}

		if (_GrowthStage == 3 && season == 1 && !_bHasMingled)
			_bMingle = true;
		if (_GrowthStage == 3 && season != 1)
		{
			_bHasMingled = false;
		}

		ShouldHunt(_bHunt, _fullness, _bNoTarget);

		if (!_bNoTarget) //Path finding here
		{
			float fDistance = MoveToTarget(_Target->_posx, _Target->_posy, _bNoTarget, _posx, _posy, _velx, _vely, _fViewRange, _fReach, _fSpeed, _vPath, _bHave_Path);
			if (!_bHave_Path)
			{
				if (_fdestinationX != _Target->_posx)
					_fdestinationX = _Target->_posx;
				if (_fdestinationY != _Target->_posy)
					_fdestinationY = _Target->_posy;
				_vPath.clear();
			}

			if (!_Target->_bRedundant)
			{
				if (fDistance < _fReach)
				{
					if (_fCooldown <= 0.0f && ((cDynamic_Creature*)_Target)->_nHealth > 0.0f) // Attack
					{
						((cDynamic_Creature*)_Target)->_nHealth -= _AttackDmg;
						_fCooldown += 2.0f;
					}
					else
						_fCooldown -= fElapsedTime;

					if (((cDynamic_Creature*)_Target)->_nHealth <= 0 && ((cDynamic_Creature*)_Target)->_Meatleft > 0 && _fEatCooldown <= 0.0f) // This if for eating the prey
					{
						if ((((cDynamic_Creature*)_Target)->_Meatleft - _EatAmount) < 0)  // Food left < amount eaten in one bite
						{
							_fullness += ((cDynamic_Creature*)_Target)->_Meatleft;
							((cDynamic_Creature*)_Target)->_Meatleft = 0;
						}
						else if ((((cDynamic_Creature*)_Target)->_Meatleft - _EatAmount) > 0)  // Food left > amount eaten in one bite
						{
							((cDynamic_Creature*)_Target)->_Meatleft -= _EatAmount;
							_fullness += _EatAmount;
						}
						_fEatCooldown += 2.0f;
					}
					else
						_fEatCooldown -= fElapsedTime;
				}
			}
			if (_Target->_bRedundant)
				_bNoTarget = true;
		}

		m_fstateTick -= fElapsedTime; // This runs miscellaneous behaviours
		if (m_fstateTick <= 0.0f)
		{
			Hunger(_fullness, _HungerDegredation, _nHealth);
			m_fstateTick += (1.5f);
		}

		_fPathTimeCounter -= fElapsedTime; // This runs the path finding code
		if (_fPathTimeCounter <= 0.0f)
		{
			if (!_bHave_Path && _bNoTarget)
			{
				_fdestinationX = _posx + (float)((rand() % 8) - 4);
				_fdestinationY = _posy + (float)((rand() % 8) - 4);
			}

			if (!_bNoTarget && ((_Target->_posx - _posx) * (_Target->_posx - _posx) + (_Target->_posy - _posy) * (_Target->_posy - _posy)) < _fReach * _fReach) // If the creature isn't close enough to a target
			{
				// There is nothing to do here
			}
			else
				MoveTo(_fdestinationX, _fdestinationY); // Move to the set location
			MovementCheck(_posx, _posy, _oldposx, _oldposy, _sName);

			_fPathTimeCounter += 0.25f;
		}
	}
	else // Nathan: Added for death
	{
		_velx = 0;
		_vely = 0;
		_DurationBeforeDeletion -= fElapsedTime;
		if (_DurationBeforeDeletion < 0.0f)
		{
			_bRedundant = true;
		}

	}
}

//##################################################################################################

cDynamic_Creature_Bear::cDynamic_Creature_Bear() : cDynamic_Creature("Bear", DecalMap::get().GetDecal("Bear"))
{
	_bFriendly = false;
	_nHealth = 100;
	_nHealthMax = 100;
	m_fstateTick = 1.0f;
	_bRedundant = false;
	_bMingle = false;
	_bHasMingled = false;
	_Mass = 3.0f;
	_DynamicRadius = 0.4f;
	_fViewRange = 8.0f;
	_fTimeToGrow = 5.0f;

	_bHunt = false;
	_bNoTarget = true;
	_bTargetSelected = false;
	_fCooldown = 0.0f;
	_AttackDmg = 15;
	_fReach = 1.8f;
	_Meatleft = 200;
	_EatAmount = 35;
	_fEatCooldown = 0.0f;
	_FoodChain = 3;
	_Target = nullptr;
	_fSpeed = 0.5f;
}

void cDynamic_Creature_Bear::Behaviour(float fElapsedTime, int season, cDynamic* target)
{
	if (_nHealth > 0) //Nathan: If the Bear is still alive
	{
		if (_GrowthStage != 3 && _age < 100.0f) // Nathan: Added for aging
		{
			_age += (fElapsedTime / _fTimeToGrow);
			if (_GrowthStage < 1 && _age >= 10.0f && _age < 20.0f)
			{
				_AttackDmg = 5;
				_Meatleft = 50;
				_EatAmount = 15;
				_FoodChain = 2;
				_Mass = 5.0f;
				_GrowthStage = 1;
			}
			if (_GrowthStage < 2 && _age >= 20.0f && _age < 30.0f)
			{
				_AttackDmg = 10;
				_Meatleft = 150;
				_EatAmount = 25;
				_FoodChain = 3;
				_Mass = 8.0f;
				_GrowthStage = 2;
			}
			if (_GrowthStage < 3 && _age >= 30.0f)
			{
				_AttackDmg = 15;
				_Meatleft = 200;
				_EatAmount = 35;
				_Mass = 12.0f;
				_GrowthStage = 3;
			}

		}

		if (_GrowthStage == 3 && season == 1 && !_bHasMingled)
			_bMingle = true;
		if (_GrowthStage == 3 && season != 1)
		{
			_bHasMingled = false;
		}

		ShouldHunt(_bHunt, _fullness, _bNoTarget);

		if (!_bNoTarget) //Path finding here
		{
			float fDistance = MoveToTarget(_Target->_posx, _Target->_posy, _bNoTarget, _posx, _posy, _velx, _vely, _fViewRange, _fReach, _fSpeed, _vPath, _bHave_Path);
			if (!_bHave_Path)
			{
				if (_fdestinationX != _Target->_posx)
					_fdestinationX = _Target->_posx;
				if (_fdestinationY != _Target->_posy)
					_fdestinationY = _Target->_posy; 
				_vPath.clear();
			}

			if (!_Target->_bRedundant)
			{
				if (fDistance < _fReach)
				{
					if (_fCooldown <= 0.0f && ((cDynamic_Creature*)_Target)->_nHealth > 0.0f) // Attack
					{
						((cDynamic_Creature*)_Target)->_nHealth -= _AttackDmg;
						_fCooldown += 2.0f;
					}
					else
						_fCooldown -= fElapsedTime;

					// Eat
					if (((cDynamic_Creature*)_Target)->_nHealth <= 0 && ((cDynamic_Creature*)_Target)->_Meatleft > 0 && _fEatCooldown <= 0.0f)  // This if for eating the prey
					{
						if ((((cDynamic_Creature*)_Target)->_Meatleft - _EatAmount) < 0) // Food left < amount eaten in one bite
						{
							_fullness += ((cDynamic_Creature*)_Target)->_Meatleft;
							((cDynamic_Creature*)_Target)->_Meatleft = 0;
						}
						else if ((((cDynamic_Creature*)_Target)->_Meatleft - _EatAmount) > 0) // Food left > amount eaten in one bite
						{
							((cDynamic_Creature*)_Target)->_Meatleft -= _EatAmount;
							_fullness += _EatAmount;
						}
						_fEatCooldown += 2.0f; // Needs to chew their food before next bite
					}
					else
						_fEatCooldown -= fElapsedTime;
				}
			}
			if (_Target->_bRedundant)
				_bNoTarget = true;
		}

		m_fstateTick -= fElapsedTime; // This runs miscellaneous behaviours
		if (m_fstateTick <= 0.0f)
		{
			Hunger(_fullness, _HungerDegredation, _nHealth);

			m_fstateTick += (1.5f);
		}

		_fPathTimeCounter -= fElapsedTime; // This runs the path finding code
		if (_fPathTimeCounter <= 0.0f)
		{
			if (!_bHave_Path && _bNoTarget)
			{
				_fdestinationX = _posx + (float)((rand() % 8) - 4);
				_fdestinationY = _posy + (float)((rand() % 8) - 4);
			}

			if (!_bNoTarget && ((_Target->_posx - _posx) * (_Target->_posx - _posx) + (_Target->_posy - _posy) * (_Target->_posy - _posy)) < _fReach * _fReach) // If the creature isn't close enough to a target
			{
				// There is nothing to do here
			}
			else
				MoveTo(_fdestinationX, _fdestinationY); // Move to the set location
			MovementCheck(_posx, _posy, _oldposx, _oldposy, _sName);

			_fPathTimeCounter += 0.25f;
		}
	}
	else // Nathan: Added for death
	{
		_velx = 0;
		_vely = 0;
		_DurationBeforeDeletion -= fElapsedTime;
		if (_DurationBeforeDeletion < 0.0f)
		{
			_bRedundant = true;
		}

	}
}

//##################################################################################################
//##################################################################################################

// Informs of improper jumping to new location
void MovementCheck(float& x, float& y, float& oldx, float& oldy, string name) 
{
	if ((int)oldx != 0 && (int)oldy != 0)
		if (oldx - 5.0f < x && oldx + 5.0f > x && oldy - 5.0f < y && oldy + 5.0f > y)
		{
			oldx = x;
			oldy = y;
		}
		else
		{
			std::cout << std::endl;
			std::cout << name << " jumped from:" << std::endl;
			std::cout << "X: " << oldx << " to " << x << std::endl;
			x = oldx;
			std::cout << "Y: " << oldy << " to " << y << std::endl;
			y = oldy;
		}
}

// Determines if the creature is close enough to reach the target
float MoveToTarget(const int Target_posx, const int Target_posy, bool& bNoTarget,
	const float posx, const float posy, float& velx, float& vely,
	const float fViewRange, const float& fReach, const float& fSpeed,
	vector<int>& vPath, bool& bHave_Path)
{
	// Check if _Target is nearby
	float fTargetX = Target_posx - posx;
	float fTargetY = Target_posy - posy;
	float fDistance = sqrtf(fTargetX * fTargetX + fTargetY * fTargetY);
	if (fDistance < fViewRange)
	{
		if (fDistance < fReach / 2.0f)
		{
			velx = 0;
			vely = 0;
			bHave_Path = false;
			vPath.clear();
		}
	}
	else
	{
		bNoTarget = true;
	}
	return fDistance;
}

// Determines if the creature should hunt
void ShouldHunt(bool &bHunt, const float& fullness, bool& bNoTarget)
{
	if (!bHunt)
		if (fullness < 40)
			bHunt = true;
	if (bHunt)
		if (fullness > 70)
		{
			bHunt = false;
			if (fullness == 100)
				bNoTarget = true;
		}
}

void Hunger(int& fullness, const int & HungerDegredation, int& Health)
{
	fullness -= HungerDegredation;
	if (fullness < 15)
		Health -= 3;
	if (fullness < 0)
		fullness = 0;
}

// Returns a fixed movement speed for any direction
void MovementCorrection(const float movementspeed, float& velx, float& vely, 
	const float startposx, const float startposy, 
	const float finishposx, const float finishposy)
{
	float diffx = (startposx - finishposx);
	float diffy = (startposy - finishposy);
	if (diffy < 0.0001f && diffy > -0.0001f)
		diffy = 0.001f;
	if (diffx < 0.0001f && diffx > -0.0001f)
		diffx = 0.001f;

	// Finding X component
	velx = (cos(atan(diffy / diffx)) * movementspeed);
	if (startposx - finishposx > 0.0f)
		velx *= -1.0f;
	// Finding Y component
	vely = (cos(atan(diffx / diffy)) * movementspeed);
	if (startposy - finishposy > 0.0f)
		vely *= -1.0f;
}