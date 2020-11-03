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
#include <algorithm>
#include <math.h>

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
	bool OverlayEditor = false;
	bool Draw_impassables = false;
	int Selected_tile = 0;
	int editorY_adjustment = 0;
	float Player_speed = 4.0f;
	bool Cheats_allowed = true;
	float Time = 0;
	int season = 4; // 1 = spring, 2 = summer, 3 = fall, 4 = winter
	float YearLength = 60.0f; // Seconds
	vector<cDynamic*> m_nvecDynamics_que; // Ques up the new creatures to be placed in the world
	int Dynamic_Cap = 80; // Set a limit to the number of creatures
	int BrushSize = 0;

	// Remove
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
		m_pPlayer->_Mass = 8.0f; // Pay no attention the lightweight Player!
		m_pPlayer->_DynamicRadius = 0.6f;
		m_pPlayer->_nHealthMax = 400;
		m_pPlayer->_nHealth = 400;

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
							int idxB = m_pCurrentMap->GetIndex(x, y);
							cout << idxB << " ";
							int idxO = m_pCurrentMap->GetOverlayIndex(x, y);
							cout << idxO << " ";
							int sol = m_pCurrentMap->GetSolid(x, y);
							cout << sol << " ";
						}
						cout << endl;
					}
				}
				if (GetKey(O).bPressed)
				{
					if (OverlayEditor)
					{
						OverlayEditor = false;
						cout << "OverlayEditor Map editing disabled" << endl;
					}
					else
					{
						OverlayEditor = true;
						cout << "OverlayEditor Map editing enabled" << endl;
					}
				}
				// Nathan: Added impassable tile editing
				if (GetKey(I).bPressed)
				{
						if (Draw_impassables)
						{
							Draw_impassables = false;
							cout << "Impassable tile editing disabled" << endl;
						}
						else
						{
							Draw_impassables = true;
							cout << "Impassable tile editing enabled" << endl;
						}
				}
				// Nathan: Added brush sizes
				if (GetKey(B).bHeld) // Second mouse button
				{
					if (int(GetMouseWheel()) != 0)
					{
						BrushSize += int(GetMouseWheel()) / 120;
						cout << "BrushSize = " << BrushSize << endl;
					}
					if (BrushSize < 0) BrushSize = 0;
					if (BrushSize > 10) BrushSize = 10;
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
			if (object != m_nvecDynamics[0])
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
			float fBorder = 0.1f; // Nathan: Made adjustments to the tile collision code
			if (object->_velx + object->_collision_velx < 0)
			{
				if (m_pCurrentMap->GetSolid(fNewObjectPosX + fBorder, object->_posy + fBorder + 0.0f) || m_pCurrentMap->GetSolid(fNewObjectPosX + fBorder, object->_posy + (1.0f - fBorder) + 0.0f))
				{
					fNewObjectPosX = (int)fNewObjectPosX + 1.0f - fBorder + 0.001f;
					object->_velx = 0;
					object->_collision_velx = 0;
				}
			}
			else if(object->_velx + object->_collision_velx > 0)
			{
				if (m_pCurrentMap->GetSolid(fNewObjectPosX + (1.0f - fBorder), object->_posy + fBorder + 0.0f) || m_pCurrentMap->GetSolid(fNewObjectPosX + (1.0f - fBorder), object->_posy + (1.0f - fBorder) + 0.0f))
				{
					fNewObjectPosX = (int)fNewObjectPosX + fBorder - 0.001f;
					object->_velx = 0;
					object->_collision_velx = 0;
				}
			}
			if (object->_vely + object->_collision_vely < 0)
			{
				if (m_pCurrentMap->GetSolid(fNewObjectPosX + fBorder + 0.0f, fNewObjectPosY + fBorder) || m_pCurrentMap->GetSolid(fNewObjectPosX + (1.0f - fBorder), fNewObjectPosY + fBorder))
				{
					fNewObjectPosY = (int)fNewObjectPosY + 1.0f - fBorder + 0.001f;
					object->_vely = 0;
					object->_collision_vely = 0;
				}
			}
			else if (object->_vely + object->_collision_vely > 0)
			{
				if (m_pCurrentMap->GetSolid(fNewObjectPosX + fBorder + 0.0f, fNewObjectPosY + (1.0f - fBorder)) || m_pCurrentMap->GetSolid(fNewObjectPosX + (1.0f - fBorder), fNewObjectPosY + (1.0f - fBorder)))
				{
					fNewObjectPosY = (int)fNewObjectPosY + fBorder - 0.001f;
					object->_vely = 0;
					object->_collision_vely = 0;
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
									if (((cDynamic_Creature*)dyn)->_sName == "Fox")
									{
										cDynamic* g1 = new cDynamic_Creature_Fox();
										Que(g1, m_nvecDynamics_que, dyn, object);
									}
									if (((cDynamic_Creature*)dyn)->_sName == "Bear")
									{
										cDynamic* g1 = new cDynamic_Creature_Bear();
										Que(g1, m_nvecDynamics_que, dyn, object);
									}
								}
			};

			//Object V Object collisions
			for (auto& dyn : m_nvecDynamics)
			{
				if (dyn != object && !dyn->_bRedundant && !object->_bRedundant && ((cDynamic_Creature*)dyn)->_nHealth > 0)
				{
					//If the object is solid then the creatures must push against each other
					if (dyn->_bSolidVsDyn && object->_bSolidVsDyn)
					{
						// Nathan: Changed the square bounding boxes for dynamic interaction into circular bounds
						float diffx = (object->_posx - dyn->_posx);
						float diffy = (object->_posy - dyn->_posy);
						if (diffy < 0.0001f && diffy > -0.0001f)
							diffy = 0.001f;
						if (diffx < 0.0001f && diffx > -0.0001f)
							diffx = 0.001f;

						// Get the distance between the centers of the objects
						float Distance = sqrt(diffx * diffx + diffy * diffy);
						//cout << "We check distance = " << Distance << endl;

						// Amount of overlap between cricular bounds
						float Overlap = (((cDynamic_Creature*)object)->_DynamicRadius + ((cDynamic_Creature*)dyn)->_DynamicRadius - Distance);
						float difference = 0.0f;

						// Check if circles overlap
						if (Overlap > 0.0f)
						{
							// Nathan: We need some interaction as I forgot to add it back in.
							interaction(dyn, object, m_nvecDynamics_que);

							// Count the number of objects colliding with
							object->_number_collisions++;

							// Finding Y component
							float Yval = sin(atan(diffx / diffy));
							// Finding X component
							float Xval = cos(atan(diffx / diffy));

							// Set the velocity in the x and y directions
							float YComponent = (Overlap) * (Yval) * (((cDynamic_Creature*)dyn)->_Mass);
							if ((object->_posx - dyn->_posx) < 0.0f)
							{
								((cDynamic_Creature*)object)->_collision_vely += -YComponent * fElapsedTime;
								if (((cDynamic_Creature*)object)->_collision_vely < -YComponent)
									((cDynamic_Creature*)object)->_collision_vely = -YComponent;
							}
							else if ((object->_posx - dyn->_posx) > 0.0f)
							{
								((cDynamic_Creature*)object)->_collision_vely += YComponent * fElapsedTime;
								if (((cDynamic_Creature*)object)->_collision_vely > YComponent)
									((cDynamic_Creature*)object)->_collision_vely = YComponent;
							}


							float XComponent = (Overlap) * (Xval) * (((cDynamic_Creature*)dyn)->_Mass);
							if ((object->_posx - dyn->_posx) < 0.0f)
							{
								((cDynamic_Creature*)object)->_collision_velx += -XComponent * fElapsedTime;
								if (((cDynamic_Creature*)object)->_collision_velx < -XComponent)
									((cDynamic_Creature*)object)->_collision_velx = -XComponent;

							}
							else if ((object->_posx - dyn->_posx) > 0.0f)
							{
								((cDynamic_Creature*)object)->_collision_velx += XComponent * fElapsedTime;
								if (((cDynamic_Creature*)object)->_collision_velx > XComponent)
									((cDynamic_Creature*)object)->_collision_velx = XComponent;
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
			// Nathan: Added an additional set of velocities to handle collision
			if (object->_number_collisions == 0)
			{
				object->_collision_velx = 0;
				object->_collision_vely = 0;
			}
			object->_number_collisions = 0; // Reset


			object->_posx = fDynamicObjectPosX + (object->_collision_velx + object->_velx) * fElapsedTime; // ~32:12
			object->_posy = fDynamicObjectPosY + (object->_collision_vely + object->_vely) * fElapsedTime;

			// Nathan: Check for hunting
			if (((cDynamic_Creature*)object)->_bHunt && ((cDynamic_Creature*)object)->_bNoTarget)
			{
				float fDistance = 200.0f;
				for (auto dyn : m_nvecDynamics)
				{
					if (((cDynamic_Creature*)dyn)->_FoodChain <= ((cDynamic_Creature*)object)->_FoodChain
						&& ((cDynamic_Creature*)dyn)->_sName != ((cDynamic_Creature*)object)->_sName)
					{
						// Check if _Target is nearby
						float fTargetX = dyn->_posx - ((cDynamic_Creature*)object)->_posx;
						float fTargetY = dyn->_posy - ((cDynamic_Creature*)object)->_posy;
						float NewfDistance = sqrtf(fTargetX * fTargetX + fTargetY * fTargetY);
						if (NewfDistance < fDistance)
						{
							fDistance = NewfDistance;
							//((cDynamic_Creature*)object)->_bTargetSelected = true;
							((cDynamic_Creature*)object)->_bNoTarget = false;
							((cDynamic_Creature*)object)->_Target = dyn;
						}
					}
						

				}
			}


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
				DrawPartialDecal({ x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY }, m_pCurrentMap->pDecalB, { float(sx) * nTileWidth, float(sy) * nTileHeight }, { 16, 16 });
				int idxO = m_pCurrentMap->GetOverlayIndex(x + fOffsetX, y + fOffsetY);
				int sxo = idxO % 10;
				int syo = idxO / 10;
				if (idxO != 1)
					DrawPartialDecal({ x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY }, m_pCurrentMap->pDecalO, { float(sxo) * nTileWidth, float(syo) * nTileHeight }, { 16, 16 });
			}
		}

		// Combined Map editor and impassable editor into one with brushes
		if (Draw_impassables || Editor || OverlayEditor)
		{
			// Size of square changed (brush size)
			int xstart = int((fOffsetX * nTileHeight + GetMouseX()) / 16) - BrushSize;
			int xfinish = (int((fOffsetX * nTileHeight + GetMouseX()) / 16) + 1 + BrushSize);
			int ystart = int((fOffsetY * nTileHeight + GetMouseY()) / 16) - BrushSize;
			int yfinish = (int((fOffsetY * nTileHeight + GetMouseY()) / 16) + 1 + BrushSize);

			// Nathan: Draw impassable icon on the solid tiles
			if (Draw_impassables)
			{
				for (int x = -1; x < nVisibleTilesX + 1; x++)
					for (int y = -1; y < nVisibleTilesY + 1; y++)
						if (m_pCurrentMap->GetSolid(x + fOffsetX, y + fOffsetY))
							DrawDecal({ x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY }, DecalMap::get().GetDecal("Impassable"));

				if (GetMouse(0).bHeld || GetMouse(1).bHeld) // If left or right mouse click + hold - > continues to place until not held
				{
					bool selected = false;
					if (GetMouse(0).bHeld)
						selected = true;// Set passable
					else
						selected = false; // Set impassable

					for (int x = xstart; x < xfinish; x++)
						for (int y = ystart; y < yfinish; y++)
							m_pCurrentMap->ModifySolid(x, y, selected);
				}
			}

			// Nathan: Added map editing
			if (Editor)
			{
				if (GetKey(Z).bHeld)
					if (int(GetMouseWheel()) != 0)
					{
						Selected_tile -= int(GetMouseWheel()) / 120;
						cout << Selected_tile << endl;
					}
				if (Selected_tile < 0) Selected_tile = 0; // Hardcoding 100 sprite limitation
				if (Selected_tile > 99) Selected_tile = 99;

				int sx = Selected_tile % 10;
				int sy = Selected_tile / 10;
				DrawPartialDecal({ float(GetMouseX()), float(GetMouseY()) }, m_pCurrentMap->pDecalB, { float(sx) * nTileWidth, float(sy) * nTileHeight }, { 16, 16 }, { 0.8f, 0.8f });

				if (GetMouse(0).bHeld) // If left mouse click + hold - > continues to place until not held
					for (int x = xstart; x < xfinish; x++)
						for (int y = ystart; y < yfinish; y++)
							m_pCurrentMap->ModifyIndex(x, y, Selected_tile);
			}

			// Nathan: Added Overlay map editing
			if (OverlayEditor)
			{
				if (GetKey(X).bHeld)
					if (int(GetMouseWheel()) != 0)
					{
						Selected_tile -= int(GetMouseWheel()) / 120;
						cout << Selected_tile << endl;
					}
				if (Selected_tile < 0) Selected_tile = 0; // Hardcoding 100 sprite limitation
				if (Selected_tile > 99) Selected_tile = 99;

				int sxo = Selected_tile % 10;
				int syo = Selected_tile / 10;
				DrawPartialDecal({ float(GetMouseX()), float(GetMouseY()) }, m_pCurrentMap->pDecalO, { float(sxo) * nTileWidth, float(syo) * nTileHeight }, { 16, 16 }, { 0.8f, 0.8f });

				if (GetMouse(0).bHeld) // If left mouse click + hold - > continues to place until not held
					for (int x = xstart; x < xfinish; x++)
						for (int y = ystart; y < yfinish; y++)
							m_pCurrentMap->ModifyOverlayIndex(x, y, Selected_tile);
			}
		}
		// Nathan: added the mouse click teleport
		if (Teleport)
		{
			m_pPlayer->_posx = int(fOffsetX + (GetMouseX() / 16));
			m_pPlayer->_posy = int(fOffsetY + (GetMouseY() / 16));
			Teleport = false;
		}


		// Draw Object
		for (auto& object : m_nvecDynamics)
			object->DrawSelf(this, fOffsetX, fOffsetY);

		m_pPlayer->DrawSelf(this, fOffsetX, fOffsetY); // Draw the player on top

		if (UpdateLocalMap(fElapsedTime))
			cout << "A Dynamic Creature has been erased..." << endl; // <- Nathan: added this for console

		return true;
	}

	bool UpdateLocalMap(float fElapsedTime) // Update map
	{
		int VecSize = m_nvecDynamics.size(); // <- Nathan: added this for console

		for (auto& object : m_nvecDynamics) // Loop to give information about creatures being deleted
			if (object->_bRedundant) 
			{
				cout << endl;
				cout << object->_sName << " is being removed." << endl;
				cout << "Health: " << ((cDynamic_Creature*)object)->_nHealth << endl;
				cout << "Fullness: " << ((cDynamic_Creature*)object)->_fullness << endl;
				cout << "Meatleft: " << ((cDynamic_Creature*)object)->_Meatleft << endl;
				cout << "At: " << ((cDynamic_Creature*)object)->_posx << "," << ((cDynamic_Creature*)object)->_posy << endl;
				cout << "DurB4Delete: " << ((cDynamic_Creature*)object)->_DurationBeforeDeletion << endl;
				if (!((cDynamic_Creature*)object)->_bNoTarget)
				{
					cout << "Target name: " << ((cDynamic_Creature*)object)->_Target->_sName << endl;
					cout << "Target Health: " << ((cDynamic_Creature*)((cDynamic_Creature*)object)->_Target)->_nHealth << endl;
					cout << "Target DurB4Delete: " << ((cDynamic_Creature*)((cDynamic_Creature*)object)->_Target)->_DurationBeforeDeletion << endl;
				}
				cout << endl;
			}


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