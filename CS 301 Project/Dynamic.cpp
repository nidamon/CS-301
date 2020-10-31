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
	_bIsAttackable = true;
	
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
	float nSheetOffsetX = 0;
	float nSheetOffsetY = 0;

	switch (m_nGraphicState) // 17:30 Part #2
	{
	case STANDING:
		nSheetOffsetX = m_nFacingDirection * 16;
		break;
	case WALKING:
		nSheetOffsetX = m_nFacingDirection * 16;
		nSheetOffsetY = m_nGraphicCounter * 16; // ~19:00
		break;
	case CELEBRATING: //5th item 1st row
		nSheetOffsetX = 4 * 16;
		break;
	case DEAD: //5th item 2nd row
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

//##################################################################################################


cDynamic_Creature_Rabbit::cDynamic_Creature_Rabbit() : cDynamic_Creature("Rabbit", DecalMap::get().GetDecal("Rabbit"))
{
	_bFriendly = false;
	_nHealth = 20;
	_nHealthMax = 20;
	m_fstateTick = 1.0f;
	_bIsAttackable = true;
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
}

void cDynamic_Creature_Rabbit::Behaviour(float fElapsedTime, int season, cDynamic* target)
{
	if (_nHealth > 0) // If the rabbit is still alive
	{

		if (_age < 100.0f) // Nathan: Added for aging
		{
			_age += (fElapsedTime / _fTimeToGrow);
			if (_age >= 10.0f && _age < 20.0f)
			{
				_Meatleft = 40;
				_EatAmount = 4;
				_Mass = 1.5f;
				_GrowthStage = 1;
			}

			if (_age >= 20.0f && _age < 30.0f)
			{
				_Meatleft = 50;
				_EatAmount = 6;
				_Mass = 2.0f;
				_GrowthStage = 2;
				_bSolidVsDyn = true;
			}
			if (_age >= 30.0f)
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


		m_fstateTick -= fElapsedTime;
		if (m_fstateTick <= 0.0f)
		{
			switch (rand() % 3) // Get movement on x-axis
			{
			case 0:
				_velx = 0.0f;
				break;
			case 1:
				_velx = _fSpeed;
				break;
			case 2:
				_velx = -_fSpeed;
				break;
			default:
				break;
			}
			switch (rand() % 3) // Get movement on y-axis
			{
			case 0:
				_vely = 0.0f;
				break;
			case 1:
				_vely = _fSpeed;
				break;
			case 2:
				_vely = -_fSpeed;
				break;
			default:
				break;
			}
			m_fstateTick += 1.5f;
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
	_bIsAttackable = true;
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
	if (_nHealth > 0) // If the Fox is still alive
	{
		if (_age < 100.0f) // Nathan: Added for aging
		{
			_age += (fElapsedTime / _fTimeToGrow);
			if (_age >= 10.0f && _age < 20.0f)
			{
				_AttackDmg = 3;
				_Meatleft = 30;
				_EatAmount = 10;
				_Mass = 3.0f;
				_GrowthStage = 1;
			}
			if (_age >= 20.0f && _age < 30.0f)
			{
				_AttackDmg = 5;
				_Meatleft = 60;
				_EatAmount = 15;
				_Mass = 4.0f;
				_GrowthStage = 2;
			}
			if (_age >= 30.0f)
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

		if (_fullness < 40)
			_bHunt = true;
		if (_fullness > 70)
		{
			_bHunt = false;
			if (_fullness == 100)
				_bNoTarget = true;
		}

		if (!_bNoTarget) //Path finding here
		{
			// Check if _Target is nearby
			float fTargetX = _Target->_posx - _posx;
			float fTargetY = _Target->_posy - _posy;
			float fDistance = sqrtf(fTargetX * fTargetX + fTargetY * fTargetY);
			if (fDistance < _fViewRange)
			{
				_velx = (fTargetX / fDistance) * _fSpeed;
				if (_velx > 5.0f)
					_velx = 5.0f;
				if (_velx < -5.0f)
					_velx = -5.0f;
				_vely = (fTargetY / fDistance) * _fSpeed;
				if (_vely > 5.0f)
					_vely = 5.0f;
				if (_vely < -5.0f)
					_vely = -5.0f;
			}
			else
			{
				_bNoTarget = true;
				_velx = 0;
				_vely = 0;
			}
			if (!_Target->_bRedundant)
			{
				if (fDistance < _fReach)
				{
					if (_fCooldown <= 0.0f && ((cDynamic_Creature*)_Target)->_nHealth > 0.0f)
					{
						((cDynamic_Creature*)_Target)->_nHealth -= _AttackDmg;
						_fCooldown += 2.0f;
					}
					else
						_fCooldown -= fElapsedTime;

					if (((cDynamic_Creature*)_Target)->_nHealth <= 0 && ((cDynamic_Creature*)_Target)->_Meatleft > 0 && _fEatCooldown <= 0.0f)
					{
						if ((((cDynamic_Creature*)_Target)->_Meatleft - _EatAmount) < 0)
						{
							_fullness += ((cDynamic_Creature*)_Target)->_Meatleft;
							((cDynamic_Creature*)_Target)->_Meatleft = 0;
						}
						if ((((cDynamic_Creature*)_Target)->_Meatleft - _EatAmount) > 0)
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
		m_fstateTick -= fElapsedTime;
		if (m_fstateTick <= 0.0f)
		{
			switch (rand() % 3) // Get movement on x-axis
			{
			case 0:
				_velx = 0.0f;
				break;
			case 1:
				_velx = _fSpeed;
				break;
			case 2:
				_velx = -_fSpeed;
				break;
			default:
				break;
			}
			switch (rand() % 3) // Get movement on y-axis
			{
			case 0:
				_vely = 0.0f;
				break;
			case 1:
				_vely = _fSpeed;
				break;
			case 2:
				_vely = -_fSpeed;
				break;
			default:
				break;
			}


			_fullness -= 6;
			if (_fullness < 15)
				_nHealth -= 3;
			if (_fullness < 0)
				_fullness = 0;

			m_fstateTick += ((float)(rand() % 20) * 0.1f + 1.5f);
		}
	}
	else // Nathan: Added for death
	{
		_velx = 0;
		_vely = 0;
		_DurationBeforeDeletion -= fElapsedTime;
		if (_DurationBeforeDeletion < 0.0f)
		{
			//std::cout << "Fox removed with " << _nHealth << "health at (" << _posx << "," << _posy << ").";
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
	_bIsAttackable = true;
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
	if (_nHealth > 0) // If the Bear is still alive
	{
		if (_age < 100.0f) // Nathan: Added for aging
		{
			_age += (fElapsedTime / _fTimeToGrow);
			if (_age >= 10.0f && _age < 20.0f)
			{
				_AttackDmg = 5;
				_Meatleft = 50;
				_EatAmount = 15;
				_FoodChain = 2;
				_Mass = 5.0f;
				_GrowthStage = 1;
			}
			if (_age >= 20.0f && _age < 30.0f)
			{
				_AttackDmg = 10;
				_Meatleft = 150;
				_EatAmount = 25;
				_FoodChain = 3;
				_Mass = 8.0f;
				_GrowthStage = 2;
			}
			if (_age >= 30.0f)
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

		if (_fullness < 40)
			_bHunt = true;
		if (_fullness > 60)
		{
			_bHunt = false;
			if (_fullness == 100)
				_bNoTarget = true;
		}

		if (!_bNoTarget) //Path finding here
		{
			// Check if _Target is nearby
			float fTargetX = _Target->_posx - _posx;
			float fTargetY = _Target->_posy - _posy;
			float fDistance = sqrtf(fTargetX * fTargetX + fTargetY * fTargetY);
			if (fDistance < _fViewRange)
			{
				_velx = (fTargetX / fDistance) * _fSpeed;
				if (_velx > 5.0f)
					_velx = 5.0f;
				if (_velx < -5.0f)
					_velx = -5.0f;
				_vely = (fTargetY / fDistance) * _fSpeed;
				if (_vely > 5.0f)
					_vely = 5.0f;
				if (_vely < -5.0f)
					_vely = -5.0f;

			}
			else
			{
				_bNoTarget = true;
				_velx = 0;
				_vely = 0;
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
					if (((cDynamic_Creature*)_Target)->_nHealth <= 0 && ((cDynamic_Creature*)_Target)->_Meatleft > 0 && _fEatCooldown <= 0.0f)
					{
						if ((((cDynamic_Creature*)_Target)->_Meatleft - _EatAmount) < 0) // Food left < amount eaten in one bite
						{
							_fullness += ((cDynamic_Creature*)_Target)->_Meatleft;
							((cDynamic_Creature*)_Target)->_Meatleft = 0;
						}
						if ((((cDynamic_Creature*)_Target)->_Meatleft - _EatAmount) > 0) // Food left > amount eaten in one bite
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

		m_fstateTick -= fElapsedTime;
		if (m_fstateTick <= 0.0f) // Normal updating here (movement and fullness)
		{
			switch (rand() % 3) // Get movement on x-axis
			{
			case 0:
				_velx = 0.0f;
				break;
			case 1:
				_velx = _fSpeed;
				break;
			case 2:
				_velx = -_fSpeed;
				break;
			default:
				break;
			}
			switch (rand() % 3) // Get movement on y-axis
			{
			case 0:
				_vely = 0.0f;
				break;
			case 1:
				_vely = _fSpeed;
				break;
			case 2:
				_vely = -_fSpeed;
				break;
			default:
				break;
			}


			_fullness -= _HungerDegredation;
			if (_fullness < 15)
				_nHealth -= 3;
			if (_fullness < 0)
				_fullness = 0;

			m_fstateTick += ((float)(rand() % 20) * 0.1f + 1.5f);
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