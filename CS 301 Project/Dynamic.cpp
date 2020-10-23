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

	_DurationBeforeDeletion = 5.0f; // Nathan: Added the below variables
	_bMingle = false;
	_bHasMingled = false;
	_GrowthStage = 0;
	_age = 0.0f;
	_fullness = 70; // % amount
	_cGender = 'M';
}

void cDynamic_Creature::Update(float fElapsedTime, int season, cDynamic* player)
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

	Behaviour(fElapsedTime, season, player);////////////////////////////////////////////////////
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

	gfx->DrawPartialDecal({ (_posx - offsetx) * 16.0f, (_posy - offsety) * 16.0f }, m_pDecal, { nSheetOffsetX, nSheetOffsetY }, { 16, 16 }, { growthscale, growthscale });
}

void cDynamic_Creature::Behaviour(float fElapsedTime, int season, cDynamic* player)
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
	_nHealth = 10;
	_nHealthMax = 10;
	m_fstateTick = 1.0f;
	_bIsAttackable = true;
	_bRedundant = false;
	_bMingle = false;
	_bHasMingled = false;
	_Mass = 1.5f; // baby rabbit weight
}

void cDynamic_Creature_Rabbit::Behaviour(float fElapsedTime, int season, cDynamic* player)
{
	// Check if player is nearby
	float fTargetX = player->_posx - _posx;
	float fTargetY = player->_posy - _posy;
	float fDistance = sqrtf(fTargetX * fTargetX + fTargetY * fTargetY);


	if (_nHealth > 0) // If the rabbit is still alive
	{

		if (_age < 300.0f) // Nathan: Added for aging
		{
			_age += fElapsedTime; ///////////////////////////////////////////////###################################
			if (_age >= 30.0f && _age < 60.0f)
			{
				_Mass = 1.5f;
				_GrowthStage = 1;
			}

			if (_age >= 60.0f && _age < 90.0f)
			{
				_Mass = 2.0f;
				_GrowthStage = 2;
				_bSolidVsDyn = true;
			}
			if (_age >= 90.0f)
			{
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
			if (fDistance < 6.0f)
			{
				_velx = (fTargetX / fDistance) * 2.0f;
				_vely = (fTargetY / fDistance) * 2.0f;
			}
			else
			{
				_velx = 0;
				_vely = 0;
			}

			m_fstateTick += 0.5f;
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