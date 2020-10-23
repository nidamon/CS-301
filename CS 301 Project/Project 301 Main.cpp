/*Project 301
aka a simulated habitat
Nathan Damon
10/3/2020
This program will simulate a small habit with different animals.
Huge Shout out to YouTuber javidx9 for his Console Game Engine that he has made available on github
and his RPG game tutorial from which I built upon for this project.
*/

//Main is at the bottom of this file



#define OLC_PGE_APPLICATION
#include "Project_Maps.h"
#include "DecalMap.h"
#include "Dynamic.h"
#include "olcPixelGameEngine.h"
using namespace olc;
#include <iostream>
using std::cout;
using std::cin;
using std::endl;
#include <string>
using std::string;

//Assembly Functions Ahead!!!

extern "C" int set_int_variable1(int value);
extern "C" int set_int_variable2(int value);
extern "C" int set_int_variable3(int value);
extern "C" int set_int_variable4(int value);
extern "C" float set_float_variable1(int value);
extern "C" float set_float_variable2(int value);
extern "C" float set_float_variable3(int value);
extern "C" float set_float_variable4(int value);
extern "C" int set_all_function_use_variables_to_0(void);


// Unimplemented
extern "C" int wants_to_eat(void); // Takes float_variable1(fullness) and float_variable2(maxfullness)


// Implemented
extern "C" int SeasonOfYear(float); // Takes a float of time and returns 1-4 based on season
extern "C" int YearlengthSet(float);// Takes a float and sets the year's length to that value

// Creates a small habitat with the ambition to be more
class Topdown_Game : public olc::PixelGameEngine
{
public:
	Topdown_Game()
	{
		// Name you application
		sAppName = "Habitat Simulation";
	}

private:
	cMap* m_pCurrentMap = nullptr;

	cDynamic_Creature* m_pPlayer = nullptr;

	vector<cDynamic*> m_nvecDynamics; // Contains the dynamic objects

	float fCameraPosX = 0.0f;
	float fCameraPosY = 0.0f;

	// Nathan: Added these variables
	bool Teleport = false;
	bool Editor = false;
	int Selected_tile = 0;
	int editorY_adjustment = 0;
	float Player_speed = 4.0f;
	bool Cheats_allowed = true;
	float Time = 0;
	int season = 1; // 1 = spring, 2 = summer, 3 = fall, 4 = winter
	float YearLength = 60.0f; // Seconds
	vector<cDynamic*> m_nvecDynamics_que; // Ques up the new creatures to be placed in the world
	int Dynamic_Cap = 50; // Set a limit to the number of creatures
	float Repulsion = 2.2f; // How fast the creatures will repel each other when overlapping
	float RepulsionRate = 15.0f; // How fast creatures are repulsed
	//int Zoom = 1;

protected:
	virtual bool OnUserCreate()// Called once at the start, so create things here
	{
		DecalMap::get().LoadDecals();

		random_device r;
		std::default_random_engine e1(r());

		m_pCurrentMap = new cMap_Plains();

		m_pPlayer = new cDynamic_Creature("player", DecalMap::get().GetDecal("TestSpriteSheet"));
		m_pPlayer->_posx = 5.0f;
		m_pPlayer->_posy = 5.0f;
		m_pPlayer->_GrowthStage = 3;

		m_nvecDynamics.push_back(m_pPlayer); // Put player in the vector first
		m_pCurrentMap->PopulateDynamics(m_nvecDynamics, e1);

		YearlengthSet(YearLength);

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{
		// Nathan: Added seasons for the creatures 
		season = SeasonOfYear(fElapsedTime); // Now in assembly

		m_pPlayer->_velx = 0.0f; // Set velocity to zero
		m_pPlayer->_vely = 0.0f;

		// Handle Input
		if (IsFocused())
		{
			if (Cheats_allowed)
			{
				// Nathan: Added mouse clicking
				if (GetKey(T).bHeld) // Nathan: T for Teleport
					if (GetMouse(0).bReleased) // If left mouse click
					{
						Teleport = true;
					}
				// Nathan: Added map editing
				if (GetKey(M).bPressed)
				{
					if (Editor)
					{
						Editor = false;
						cout << "Map editing disabled" << endl;
					}
					else
					{
						Editor = true;
						cout << "Map editing enabled" << endl;
					}

					for (int y = 0; y < m_pCurrentMap->nHeight; y++) // Displays the Tile data
					{
						for (int x = 0; x < m_pCurrentMap->nWidth; x++)
						{
							int idx = m_pCurrentMap->GetIndex(x, y);
							cout << idx;
							int sol = m_pCurrentMap->GetSolid(x, y);
							cout << " " << sol << " ";
						}
						cout << endl;
					}
				}
				// Nathan: Added speed hacks
				if (GetKey(V).bHeld) // Second mouse button
				{
					if (int(GetMouseWheel()) != 0)
					{
						Player_speed += float(GetMouseWheel()) / 30;
						cout << "Player_speed = " << Player_speed << endl;
					}
					if (Player_speed < 1) Player_speed = 1.0f;
					if (Player_speed > 20.0f) Player_speed = 20.f;
				}
				// Nathan: Added population limiter
				if (GetKey(P).bHeld) // Second mouse button
				{
					if (int(GetMouseWheel()) != 0)
					{
						Dynamic_Cap += int(GetMouseWheel()) / 24;
						if (Dynamic_Cap < 5) Dynamic_Cap = 5;
						if (Dynamic_Cap > 200) Dynamic_Cap = 200;
						cout << "Dynamic_Cap = " << Dynamic_Cap << endl;
					}
				}
				// Nathan: Added repulsion adjustment
				if (GetKey(R).bHeld)
				{
					if (GetMouse(0).bHeld) // First mouse button
					{
						if (int(GetMouseWheel()) != 0)
						{
							Repulsion += float(GetMouseWheel()) / 1200;
							cout << "Repulsion = " << Repulsion << endl;
						}
						if (Repulsion < 0.1f) Repulsion = 0.1f; // limiting Repulsion at 0.1
						if (Repulsion > 3.0f) Repulsion = 3.0f; // limiting Repulsion at 3.0
					}
					if (GetMouse(1).bHeld) // Second mouse button
					{
						if (int(GetMouseWheel()) != 0)
						{
							RepulsionRate += float(GetMouseWheel()) / 600;
							cout << "RepulsionRate = " << RepulsionRate << endl;
						}
						if (RepulsionRate < 0.1f) RepulsionRate = 0.1f; // limiting RepulsionRate at 0.1
						if (RepulsionRate > 30.0f) RepulsionRate = 30.0f; // limiting RepulsionRate at 30.0
					}
				}
			}
			//if (GetKey(Z).bHeld)
			//{
			//	if (int(GetMouseWheel()) != 0)
			//	{
			//		Zoom += int(GetMouseWheel()) / 120;
			//		if (Zoom < 1) Zoom = 1; // Hardcoding 100 sprite limitation
			//		if (Zoom > 4) Player_speed = 20.f;
			//		cout << "Zoom = " << Zoom << endl;
			//	}
			//}
			if (GetKey(UP).bHeld || GetKey(W).bHeld) // Nathan: Added the wasd keys
			{
				m_pPlayer->_vely = -Player_speed;
			}
			if (GetKey(DOWN).bHeld || GetKey(S).bHeld)
			{
				m_pPlayer->_vely = Player_speed;
			}
			if (GetKey(LEFT).bHeld || GetKey(A).bHeld)
			{
				m_pPlayer->_velx = -Player_speed;
			}
			if (GetKey(RIGHT).bHeld || GetKey(D).bHeld)
			{
				m_pPlayer->_velx = Player_speed;
			}
			if (GetKey(SPACE).bReleased) // Interaction requested
			{
				// Grab a point from the direction the player is facing and check for interaction
				float fTestX, fTestY;
				if (m_pPlayer->GetFacingDirection() == 0) // South
				{
					fTestX = m_pPlayer->_posx + 0.5f;
					fTestY = m_pPlayer->_posy + 1.5f;
				}
				if (m_pPlayer->GetFacingDirection() == 1) // West
				{
					fTestX = m_pPlayer->_posx - 0.5f;
					fTestY = m_pPlayer->_posy + 0.5f;
				}
				if (m_pPlayer->GetFacingDirection() == 2) // North
				{
					fTestX = m_pPlayer->_posx + 0.5f;
					fTestY = m_pPlayer->_posy - 0.5f;
				}
				if (m_pPlayer->GetFacingDirection() == 3) // North
				{
					fTestX = m_pPlayer->_posx + 1.5f;
					fTestY = m_pPlayer->_posy + 0.5f;
				}

				// Check if test point has hit a dynamic object
				for (auto dyns : m_nvecDynamics)
				{
					if (fTestX > dyns->_posx&& fTestX < (dyns->_posx + 1.0) && fTestY > dyns->_posy&& fTestY < (dyns->_posy + 1.0f))
					{
						if (dyns->_bFriendly)
						{
							m_pCurrentMap->OnInteraction(m_nvecDynamics, dyns, cMap::TALK); //////////////////////
						}
						else
						{
							// Interaction was with something not friendly - perform an attack
							m_pCurrentMap->OnInteraction(m_nvecDynamics, dyns, cMap::FIGHT); /////////////////////////
						}
					}
				}
			}
		}

		if (m_nvecDynamics.size() > Dynamic_Cap) // Nathan: added entity cap
		{
			for (int i = Dynamic_Cap; i < m_nvecDynamics.size(); i++)
				((cDynamic_Creature*)m_nvecDynamics[i])->_nHealth = 0;
		}

		for (auto& object : m_nvecDynamics)
		{
			// Check if creature is within the map bounds
			if (object->_posx < 0 || object->_posx >(m_pCurrentMap->nWidth - 1) ||
				object->_posy < 0 || object->_posy >(m_pCurrentMap->nHeight - 1))
			{
				object->_bRedundant = true;
				cout << "A " << object->_sName << " got outside the habitat!" << endl;
			}


			float fNewObjectPosX = object->_posx + object->_velx * fElapsedTime;
			float fNewObjectPosY = object->_posy + object->_vely * fElapsedTime;

			// Collision
			float fBorder = 0.1f;
			if (object->_velx <= 0)
			{
				if (m_pCurrentMap->GetSolid(fNewObjectPosX + fBorder, object->_posy + fBorder + 0.0f) || m_pCurrentMap->GetSolid(fNewObjectPosX + fBorder, object->_posy + (1.0f - fBorder)))
				{
					fNewObjectPosX = (int)fNewObjectPosX + 1;
					object->_velx = 0;
				}
			}
			else
			{
				if (m_pCurrentMap->GetSolid(fNewObjectPosX + (1.0f - fBorder), object->_posy + fBorder + 0.0f) || m_pCurrentMap->GetSolid(fNewObjectPosX + (1.0f - fBorder), object->_posy + (1.0f - fBorder)))
				{
					fNewObjectPosX = (int)fNewObjectPosX;
					object->_velx = 0;
				}
			}

			if (object->_vely <= 0)
			{
				if (m_pCurrentMap->GetSolid(fNewObjectPosX + fBorder + 0.0f, fNewObjectPosY + fBorder) || m_pCurrentMap->GetSolid(fNewObjectPosX + (1.0f - fBorder), fNewObjectPosY + 0.0f))
				{
					fNewObjectPosY = (int)fNewObjectPosY + 1;
					object->_vely = 0;
				}
			}
			else
			{
				if (m_pCurrentMap->GetSolid(fNewObjectPosX + fBorder + 0.0f, fNewObjectPosY + (1.0f - fBorder)) || m_pCurrentMap->GetSolid(fNewObjectPosX + (1.0f - fBorder), fNewObjectPosY + (1.0f - fBorder)))
				{
					fNewObjectPosY = (int)fNewObjectPosY;
					object->_vely = 0;
				}
			}

			float fDynamicObjectPosX = fNewObjectPosX;
			float fDynamicObjectPosY = fNewObjectPosY;

			// Nathan: Checks the creature for if it can spawn a new creature
			auto interaction = [](const cDynamic* dyn, const cDynamic* object, vector<cDynamic*>& m_nvecDynamics_que)
			{
				if (((cDynamic_Creature*)dyn)->_sName == ((cDynamic_Creature*)object)->_sName)
					if (((cDynamic_Creature*)dyn)->_cGender != ((cDynamic_Creature*)object)->_cGender)
						if (((cDynamic_Creature*)dyn)->_GrowthStage == 3 && ((cDynamic_Creature*)object)->_GrowthStage == 3)
							if (((cDynamic_Creature*)dyn)->_bMingle && ((cDynamic_Creature*)object)->_bMingle)
								if (!((cDynamic_Creature*)dyn)->_bHasMingled && !((cDynamic_Creature*)object)->_bHasMingled)
								{
									((cDynamic_Creature*)dyn)->_bHasMingled = true;
									((cDynamic_Creature*)dyn)->_bMingle = false;
									((cDynamic_Creature*)object)->_bHasMingled = true;
									((cDynamic_Creature*)object)->_bMingle = false;

									auto Que = [](cDynamic* g1, vector<cDynamic*>& m_nvecDynamics_que, const cDynamic* dyn, const cDynamic* object)
									{
										m_nvecDynamics_que.push_back(g1);
										int temp = rand() % 4;
										int posx = 0;
										int posy = 0;
										if (temp == 0) posx = 1;
										if (temp == 1) posx = -1;
										if (temp == 2) posy = 1;
										if (temp == 3) posy = -1;

										g1->_bSolidVsDyn = false; // temporary freedom to move around other dynamics
										g1->_posx = ((cDynamic_Creature*)dyn)->_posx + posx;
										g1->_posy = ((cDynamic_Creature*)dyn)->_posy + posy;
										if (rand() % 2 == 0)
										{
											((cDynamic_Creature*)g1)->_cGender = 'M';
											std::cout << "Male " << dyn->_sName << " created" << std::endl;
										}
										else
										{
											((cDynamic_Creature*)g1)->_cGender = 'F';
											std::cout << "Female " << dyn->_sName << " created" << std::endl;
										}
									};

									if (((cDynamic_Creature*)dyn)->_sName == "Rabbit")
									{
										cDynamic* g1 = new cDynamic_Creature_Rabbit();
										Que(g1, m_nvecDynamics_que, dyn, object);
									}
									/*if (((cDynamic_Creature*)dyn)->_sName == "Fox")
									{
										cDynamic* g1 = new cDynamic_Creature_Fox();
										Que(g1, m_nvecDynamics_que, dyn, object);
									}
									if (((cDynamic_Creature*)dyn)->_sName == "Bear")
									{
										cDynamic* g1 = new cDynamic_Creature_Bear();
										Que(g1, m_nvecDynamics_que, dyn, object);
									}*/
								}
			};

			//Object V Object collisions
			for (auto& dyn : m_nvecDynamics)
			{
				if (dyn != object)
				{
					//If the object is solid then the player must not overlap
					if (dyn->_bSolidVsDyn && object->_bSolidVsDyn)
					{
						// Check if bounding rectangles overlap
						if (fDynamicObjectPosX < (dyn->_posx + 1.0f) && (fDynamicObjectPosX + 1.0f) > dyn->_posx&&                    // 4 float cmp in asm
							object->_posy < (dyn->_posy + 1.0f) && (object->_posy + 1.0f) > dyn->_posy)
						{
							// First Check Horizontally - Check Left
							if (object->_velx <= 0)
							{
								//fDynamicObjectPosX = dyn->_posx + 1.0f;	
								dyn->_velx += (RepulsionRate * (1 - (dyn->_posx - object->_posx) * (dyn->_posx - object->_posx))) * fElapsedTime / dyn->_Mass;
								if (dyn->_velx > Repulsion)
									dyn->_velx = Repulsion;
								interaction(dyn, object, m_nvecDynamics_que);
							}
							else
							{
								//fDynamicObjectPosX = dyn->_posx - 1.0f;
								dyn->_velx -= (RepulsionRate * (1 - (dyn->_posx - object->_posx) * (dyn->_posx - object->_posx))) * fElapsedTime / dyn->_Mass;
								if (dyn->_velx < -Repulsion)
									dyn->_velx = -Repulsion;
								interaction(dyn, object, m_nvecDynamics_que);
							}
						}

						// Check if bounding rectangles overlap
						if (fDynamicObjectPosX < (dyn->_posx + 1.0f) && (fDynamicObjectPosX + 1.0f) > dyn->_posx&&
							fDynamicObjectPosY < (dyn->_posy + 1.0f) && (fDynamicObjectPosY + 1.0f) > dyn->_posy)
						{
							// First Check Vetically - Check Left
							if (object->_vely <= 0)
							{
								//fDynamicObjectPosY = dyn->_posy + 1.0f;
								dyn->_vely += (RepulsionRate * (1 - (dyn->_posy - object->_posy) * (dyn->_posy - object->_posy))) * fElapsedTime / dyn->_Mass;
								if (dyn->_vely > Repulsion)
									dyn->_vely = Repulsion;
								interaction(dyn, object, m_nvecDynamics_que);
							}
							else
							{
								//fDynamicObjectPosY = dyn->_posy - 1.0f;
								dyn->_vely -= (RepulsionRate * (1 - (dyn->_posy - object->_posy) * (dyn->_posy - object->_posy))) * fElapsedTime / dyn->_Mass;
								if (dyn->_vely < -Repulsion)
									dyn->_vely = -Repulsion;
								interaction(dyn, object, m_nvecDynamics_que);
							}

						}
					}
					else
					{
						if (object == m_nvecDynamics[0])
						{
							// Object is player and can interact with things
							if (fDynamicObjectPosX < (dyn->_posx + 1.0f) && (fDynamicObjectPosX + 1.0f) > dyn->_posx&&
								object->_posy < (dyn->_posy + 1.0f) && (object->_posy + 1.0f) > dyn->_posy)
							{
								// Then check if it is map related
								//~20:25
							}
						}
					}
				}

			}

			object->_posx = fDynamicObjectPosX; // ~32:12
			object->_posy = fDynamicObjectPosY;

			object->Update(fElapsedTime, season, m_nvecDynamics[0]);
		}

		// Nathan: Add new creatures to the vector of dynamic creatures
		if (m_nvecDynamics_que.size() > 0)
			for (auto& dyn : m_nvecDynamics_que)
			{
				m_nvecDynamics.push_back(dyn);
				m_nvecDynamics_que.pop_back();
			}

		fCameraPosX = m_pPlayer->_posx;
		fCameraPosY = m_pPlayer->_posy;

		//Draw level
		int nTileWidth = 16;
		int nTileHeight = 16;
		int nVisibleTilesX = ScreenWidth() / nTileWidth;
		int nVisibleTilesY = ScreenHeight() / nTileHeight;

		// Calculate Top-Leftmost visible tile
		float fOffsetX = fCameraPosX - (float)nVisibleTilesX / 2.0f;
		float fOffsetY = fCameraPosY - (float)nVisibleTilesY / 2.0f;

		// Clamp camera to game boundaries
		if (fOffsetX < 0) fOffsetX = 0;
		if (fOffsetY < 0) fOffsetY = 0;
		if (fOffsetX > m_pCurrentMap->nWidth - nVisibleTilesX) fOffsetX = m_pCurrentMap->nWidth - nVisibleTilesX;
		if (fOffsetY > m_pCurrentMap->nHeight - nVisibleTilesY) fOffsetY = m_pCurrentMap->nHeight - nVisibleTilesY;

		// Get offsets for smooth movement 
		float fTileOffsetX = (fOffsetX - (int)fOffsetX) * nTileWidth;
		float fTileOffsetY = (fOffsetY - (int)fOffsetY) * nTileHeight;

		// Draw visible tile map
		for (int x = -1; x < nVisibleTilesX + 1; x++)
		{
			for (int y = -1; y < nVisibleTilesY + 1; y++)
			{
				int idx = m_pCurrentMap->GetIndex(x + fOffsetX, y + fOffsetY);
				int sx = idx % 10;
				int sy = idx / 10;
				DrawPartialDecal({ x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY }, m_pCurrentMap->pDecal, { float(sx) * nTileWidth, float(sy) * nTileHeight }, { 16, 16 });
			}
		}
		// Nathan: added the mouse click teleport
		if (Teleport)
		{
			m_pPlayer->_posx = int(fOffsetX + (GetMouseX() / 16));
			m_pPlayer->_posy = int(fOffsetY + (GetMouseY() / 16));
			Teleport = false;
		}

		// Nathan: Added map editing
		if (Editor)
		{
			if (int(GetMouseWheel()) != 0)
			{
				Selected_tile += int(GetMouseWheel()) / 120;
				cout << Selected_tile << endl;
			}
			if (Selected_tile < 0) Selected_tile = 0; // Hardcoding 100 sprite limitation
			if (Selected_tile > 99) Selected_tile = 99;

			int sx = Selected_tile % 10;
			int sy = Selected_tile / 10;
			DrawPartialDecal({ float(GetMouseX()), float(GetMouseY()) }, m_pCurrentMap->pDecal, { float(sx) * nTileWidth, float(sy) * nTileHeight }, { 16, 16 });

			if (GetMouse(0).bHeld) // If left mouse click + hold - > continues to place until not held
				m_pCurrentMap->ModifyIndex(int((fOffsetX * nTileHeight + GetMouseX()) / 16), int((fOffsetY * nTileHeight + GetMouseY()) / 16), Selected_tile);
		}


		// Draw Object
		for (auto& object : m_nvecDynamics)
			object->DrawSelf(this, fOffsetX, fOffsetY);

		m_pPlayer->DrawSelf(this, fOffsetX, fOffsetY);

		if (UpdateLocalMap(fElapsedTime))
			cout << "A Lifeform has been erased..." << endl; // <- Nathan: added this for console

		return true;
	}

	bool UpdateLocalMap(float fElapsedTime) // Update map
	{
		int VecSize = m_nvecDynamics.size(); // <- Nathan: added this for console

		// Erase and delete redundant creatures
		m_nvecDynamics.erase(
			remove_if(m_nvecDynamics.begin(), m_nvecDynamics.end(),
				[](const cDynamic* d) {return ((cDynamic_Creature*)d)->_bRedundant; }), m_nvecDynamics.end());

		if (VecSize > m_nvecDynamics.size()) // <- Nathan: added this for console
			return true;
		return false;
	}
};







int main()
{
	Topdown_Game Simulation;
	if (Simulation.Construct(400, 320, 3, 3))
		Simulation.Start();
	return 0;
}