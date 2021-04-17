#include "draw.h"
#include "object.h"
#include "network.h"
#include "game.h"

//images in header format
#include "ship.h"
#include "fuelbar.h"
#include "font.h"
#include "boost.h"
#include "smoke.h"

//global variables
cDraw Draw;
cGame Game;
cClient Client;
cMainShip Ship1;

float fBoostRotation;
//for the glRotatef function
float fRotationArr[4];

int iBoostStartID;
int iBoostEndID;

float fThrustSmoke[SMOKE_PARTICLES];

//some variables should only be init'd when loading the first time
bool gameFirstTimeUse = true;
int iMenuOptions = 0;

GLuint uiT[MAX_TEXTURES];

//everything to be initialised when entering the main menu state
static void menuInit()
{
	iIPTextLength = 0;
}

//everything to be initialised when entering the game state
static void gameInit()
{
	if (gameFirstTimeUse)
	{
		Ship1.Create();

		//load the game
		Game.SaveOrLoad(false);

		//load statically held textures
		uiT[0] = Draw.TextureInit(ship_bmp, SHIP_SIZE, SHIP_SIZE);
		uiT[1] = Draw.TextureInit(fuelbar_bmp, FUELBAR_SIZE_X, FUELBAR_SIZE_Y);
		uiT[3] = Draw.TextureInit(boost_bmp, BOOST_SIZE, BOOST_SIZE);
		uiT[4] = Draw.TextureInit(smoke_bmp, SMOKE_SIZE, SMOKE_SIZE);

		for (int i = 0; i < SMOKE_PARTICLES; i++)
		{
			fThrustSmoke[i] = (SMOKE_SPRITE_SIZE * 0.4f) * i;
		}

		gameFirstTimeUse = false;
	}

	else
	{
		//save the highscore that the player has
		Game.SaveOrLoad(true);
	}

	Game.Init();

	Ship1.Reset();

	fBoostRotation = 0.0f;
	fRotationArr[0] = 0.0f;
	fRotationArr[1] = 0.0f;
	fRotationArr[2] = 0.0f;
	fRotationArr[3] = 1.0f;
	
	iBoostStartID = 0;
	iBoostEndID = 0;
}

int main(int argc, char** argv)
{
	bool bErr = Draw.Init();
	if (bErr == false)
	{
		return -1;
	}

	//start the game out in the menu state
	int state = STATE_MENU;

	//load the font regardless of initial state
	uiT[2] = Draw.TextureInit(font_bmp, FONT_SIZE, FONT_SIZE);

	if (state == STATE_MENU)
	{
		menuInit();
	}

	else if (state == STATE_GAME)
	{
		gameInit();
	}

	//shorthand vars to store the ships pos
	float fShipX(0.0f), fShipY(0.0f);

	//text for printing out different information
	char cFontAltitudeText[ALTITUDE_TEXT_SIZE];
	char cFontHSText[HIGH_SCORE_TEXT_SIZE];
	char cGameTimerText[GAME_TIMER_TEXT_SIZE];
	sprintf(&cGameTimerText[0], "TIME: -00:00:0%i", -Game.GetTime().mSeconds);

	while (!bQuit)
	{
		Game.GetFrameTime().mNowTime = glfwGetTime();
		Game.GetFrameTime().mDeltaTime = (Game.GetFrameTime().mNowTime - 
			Game.GetFrameTime().mLastTime);

		//updates

		if ((Game.GetFrameTime().mNowTime - Game.GetFrameTime().mLastTime) >= TARGET_FPS)
		{
			if (state == STATE_MENU)
			{
				//quit if the escape button is pressed or the window close event is triggered
				bQuit = glfwWindowShouldClose(pWindow) | (cLastKey == 0);

				//press enter and proceed to the game
				if (cLastKey == 1)
				{
					if (iMenuOptions == 0)
					{
						//init the game assets
						gameInit();
						state = STATE_GAME;
					}

					else if (iMenuOptions == 1)
					{
						bClientConnectedToHost = true;
						//create a client
						if (!bClientInit)
						{
							bClientConnectedToHost = Client.Init();
							if (iClientState == CLIENT_STATE_INIT_SUCCESS)
							{
								pthread_mutex_init(&mutex1, NULL);
								bClientInit = true;
							}
						}

						if (bClientConnectedToHost)
						{
							//if the client cannot connect and we've picked to do multiplayer,
							//reset out state back to menu
							bClientConnectedToHost = Client.Connect(cIPAddrText, 7654, 5000);
							Client.IPFree();

							//get a set id from the server
							if (iClientState == CLIENT_STATE_CONNECTING)
							{
								//tell the server everything is good on client end and we're connecting
								//this is set to false on client end if we disconnect
								myData.bConnected = true;
								Client.GetNewID();
								if (iClientState == CLIENT_STATE_NEW_ID)
								{
									//init the game assets
									gameInit();
									state = STATE_GAME;
								}

								else
								{
									bClientConnectedToHost = false;
								}
							}
						}				
					}
				}

				//press up (multiplayer -> singleplayer)
				else if (cLastKey == 9)
				{
					iMenuOptions -= 1;
					if (iMenuOptions < 0)
					{
						iMenuOptions = 0;
					}
				}

				//press down (singleplayer -> multiplayer)
				else if (cLastKey == 8)
				{
					iMenuOptions += 1;
					if (iMenuOptions > 1)
					{
						iMenuOptions = 1;
					}
				}

				//only show the ip box if you have selected multiplayer
				else if (iMenuOptions == 1 && ((cLastKey >= 32 && cLastKey <= 122) || cLastKey == 3))
				{
					//backspace
					if (cLastKey == 3)
					{
						iIPTextLength -= 1;
						cIPAddrText[iIPTextLength] = 0;
						if (iIPTextLength < 0)
						{
							iIPTextLength = 0;
						}
					}

					else
					{
						cIPAddrText[iIPTextLength++] = cLastKey;
						if (iIPTextLength > IP_MAX_TEXT)
						{
							iIPTextLength = IP_MAX_TEXT;
						}
					}
				}
			}

			else if (state == STATE_GAME)
			{
				fShipX = std::get<0>(Ship1.GetXY());
				fShipY = std::get<1>(Ship1.GetXY());

				if (cLastKey == 0)
				{
					state = STATE_MENU;

					//disconnect from the server, kill the receiving thread
					if (bClientConnectedToHost)
					{
						//send a packet that tells the server we're done
						myData.bConnected = false;
						//make main thread wait till the networking thread is done
						pthread_join(thread1, NULL);
					}
				}

				if (bClientConnectedToHost)
				{ 
					if (iClientState == CLIENT_STATE_NEW_ID)
					{
						//update the ships data
						Ship1.SetXY(globalData.data[iServerID].fPos[0], 
							globalData.data[iServerID].fPos[1]);

						//setup the thread to run the send/rec from server
						int iRet;
						if ((iRet = pthread_create(&thread1, NULL, cClient_Update, NULL)))
						{
							printf("Thread creation failed!\n");
							//kill the connection and go back to the menu
						}

						//update to the client being in the online game
						iClientState = CLIENT_STATE_IN_GAME;
					}

					else if (iClientState == CLIENT_STATE_SERVER_FULL)
					{
						//kill the connection and go back to the menu
						bClientConnectedToHost = false;
						//revert back to the menu if the max capacity is limited
						state = STATE_MENU;
					}

					else if (iClientState == CLIENT_STATE_IN_GAME)
					{
						//update the client positions to send off to the server
						fClientPos[0] = fShipX;
						fClientPos[1] = fShipY;
					}
				}

				if (Game.GetTime().mSeconds >= 0)
				{
					Ship1.Update(&bKeys[0], Game.GetFrameTime().mDeltaTime);
				}
				
				//get the boost sprite to render in the current viewport
				Game.BoostGetStartEndID(&iBoostStartID, &iBoostEndID);

				//collision checks against the existing boost sprites
				for (int i = iBoostStartID; i < iBoostEndID; i++)
				{
					if (Ship1.CollisionUpdate(cBoostXOffsets[i] * BOOST_EXTEND_X, -BOOST_Y_INC * i, BOOST_SIZE))
					{
						//if we hit a boost sprite, then propell the ship in the direction of facing
						Ship1.BoostTrajectoryUpdate(fBoostRotation);
					}
				}

				Game.BackgroundUpdate(Ship1);
				Game.CameraUpdate(Ship1);
				Game.UpdateScores(Ship1);

				sprintf(&cFontAltitudeText[0], "ALTITUDE: %i", Game.GetCurrentAltitude());
				sprintf(&cFontHSText[0], "HIGH SCORE: %i", Game.GetCurrentHighScore());

				Game.TimerUpdate(cGameTimerText);
			}

			cLastKey = -1;
			
			//rendering

			if (state == STATE_MENU)
			{
				Draw.Clear(0.0f, 0.0f, 0.0f, 0.0f);

				Draw.TextureBind(uiT[2]);
				if (iMenuOptions == 0)
				{
					Draw.Color3f(1.0f, 1.0f, 1.0f);
				}

				else
				{
					Draw.Color3f(0.3f, 0.3f, 0.3f);
				}
				
				Draw.Text(HALF_SCR_WIDTH, HALF_SCR_HEIGHT + 25.0f, 16.0f, 9.0f, "SINGLE PLAYER GAME", 18, true);
				
				if (iMenuOptions == 1)
				{
					Draw.Color3f(1.0f, 1.0f, 1.0f);
				}

				else
				{
					Draw.Color3f(0.3f, 0.3f, 0.3f);
				}

				Draw.Text(HALF_SCR_WIDTH, HALF_SCR_HEIGHT + 50.0f, 16.0f, 9.0f, "MULTIPLAYER GAME", 16, true);
				Draw.Color3f(1.0f, 1.0f, 1.0f);

				if (iMenuOptions == 1)
				{
					switch (iClientState)
					{
						case CLIENT_STATE_NULL:
						Draw.Text(HALF_SCR_WIDTH, HALF_SCR_HEIGHT + 120.0f, 16.0f, 9.0f, "SERVER ADDRESS", 14, true);
						break; 
						case CLIENT_STATE_INIT_FAILED:
						Draw.Text(HALF_SCR_WIDTH, HALF_SCR_HEIGHT + 120.0f, 16.0f, 9.0f, "CLIENT: FAILED TO INIT", 22, true);
						break; 
						case CLIENT_STATE_INIT_SUCCESS:
						Draw.Text(HALF_SCR_WIDTH, HALF_SCR_HEIGHT + 120.0f, 16.0f, 9.0f, "CLIENT: INIT SUCCESSFUL", 23, true);
						break; 
						case CLIENT_STATE_CONNECTING:
						Draw.Text(HALF_SCR_WIDTH, HALF_SCR_HEIGHT + 120.0f, 16.0f, 9.0f, "CLIENT: CONNECTING...", 21, true);
						break; 
						case CLIENT_STATE_CONNECTING_FAILED:
						Draw.Text(HALF_SCR_WIDTH, HALF_SCR_HEIGHT + 120.0f, 16.0f, 9.0f, "CLIENT: CONNECTION FAILED", 25, true);
						break; 
						case CLIENT_STATE_CONNECTING_SUCCESS:
						Draw.Text(HALF_SCR_WIDTH, HALF_SCR_HEIGHT + 120.0f, 16.0f, 9.0f, "CLIENT: CONNECTION SUCCESSFUL", 29, true);
						break; 
						case CLIENT_STATE_NEW_ID:
						Draw.Text(HALF_SCR_WIDTH, HALF_SCR_HEIGHT + 120.0f, 16.0f, 9.0f, "CLIENT: NEW ID", 14, true);
						break; 
						case CLIENT_STATE_SERVER_FULL:
						Draw.Text(HALF_SCR_WIDTH, HALF_SCR_HEIGHT + 120.0f, 16.0f, 9.0f, "SERVER IS FULL", 14, true);
						break; 
					}
					
					Draw.Text(HALF_SCR_WIDTH, HALF_SCR_HEIGHT + 85.0f, 16.0f, 9.0f, cIPAddrText, iIPTextLength, true);
					Draw.TextureUnbind();
					Draw.Box(HALF_SCR_WIDTH - (6.8f * 16.0f), HALF_SCR_HEIGHT + 80.0f, 240.0f, 27.0f, 1.25f);
				}
			}

			else if (state == STATE_GAME)
			{
				Draw.Clear(0.33f * Game.GetBackgroundFadeFactor(), 
					0.56f * Game.GetBackgroundFadeFactor(), 
					0.80f * Game.GetBackgroundFadeFactor(), Game.GetCamY());

				Draw.Stars(0.0f, &Game.GetCamY());

				Draw.TextureBind(uiT[4]);

				//local thrusters
				if (Ship1.GetThrottle())
				{
					for (int i = SMOKE_PARTICLES; i >= 0; --i)
					{
						if (!bKeys[GLFW_KEY_A])
						{
							Draw.Thrust(fShipX - 2.0f, fShipY + 25.0f, &fThrustSmoke[i], THRUST_LENGTH, SMOKE_SPRITE_SIZE, 10.0f * Game.GetFrameTime().mDeltaTime);
						}

						if (!bKeys[GLFW_KEY_D])
						{
							Draw.Thrust(fShipX + 20.0f, fShipY + 25.0f, &fThrustSmoke[i], THRUST_LENGTH, SMOKE_SPRITE_SIZE, 10.0f * Game.GetFrameTime().mDeltaTime);
						}
					}
				}
				

				//multiplayer thrusters
				if (iClientState == CLIENT_STATE_IN_GAME)
				{
					for (int j = 0; j < SERVER_MAX_CLIENTS; j++)
					{
						if (globalData.data[j].cId != iServerID && globalData.data[j].bConnected)
						{
							for (int i = SMOKE_PARTICLES; i >= 0; --i)
							{
								Draw.Thrust(globalData.data[j].fPos[0] - 2.0f, globalData.data[j].fPos[1] + 25.0f, &fThrustSmoke[i], THRUST_LENGTH, SMOKE_SPRITE_SIZE, 10.0f * Game.GetFrameTime().mDeltaTime);
								Draw.Thrust(globalData.data[j].fPos[0] + 20.0f, globalData.data[j].fPos[1] + 25.0f, &fThrustSmoke[i], THRUST_LENGTH, SMOKE_SPRITE_SIZE, 10.0f * Game.GetFrameTime().mDeltaTime);
							}
						}
					}
				}
				
				Draw.TextureBind(uiT[3]);

				//get the boost sprites spinning
				fBoostRotation += 20.0f * Game.GetFrameTime().mDeltaTime;
				if (fBoostRotation >= 360.0f)
					fBoostRotation = 0.0f;

				fRotationArr[0] = fBoostRotation;

				for (int i = iBoostStartID; i < iBoostEndID; i++)
				{
					Draw.PushMatrix();
					Draw.TransformQuad(cBoostXOffsets[i] * BOOST_EXTEND_X, -BOOST_Y_INC * i, BOOST_SIZE, BOOST_SIZE,
						fRotationArr);
					Draw.Quad();
					Draw.PopMatrix();
				}

				//reset the rotation back to the original value for rendering the rest of the objects
				fRotationArr[0] = 0.0f;

				Draw.TextureBind(uiT[0]);
				Draw.PushMatrix();
				Draw.TransformQuad(fShipX, fShipY, SHIP_SIZE, SHIP_SIZE, fRotationArr);
				Draw.Quad();
				Draw.PopMatrix();

				//if its multiplayer, then draw the other clients excluding us
				if (iClientState == CLIENT_STATE_IN_GAME)
				{
					for (int i = 0; i < SERVER_MAX_CLIENTS; i++)
					{
						if (globalData.data[i].cId != iServerID && globalData.data[i].bConnected)
						{
							Draw.PushMatrix();
							Draw.TransformQuad(globalData.data[i].fPos[0], globalData.data[i].fPos[1], 
								SHIP_SIZE, SHIP_SIZE, fRotationArr);
							Draw.Quad();
							Draw.PopMatrix();
						}
					}
				}

				Draw.TextureBind(uiT[1]);
				int iFuelCellsRemaining = (int)(Ship1.GetFuelRemaining() * 0.1f);
				for (int i = 0; i < iFuelCellsRemaining; i++)
				{
					Draw.PushMatrix();
					Draw.TransformQuad(10 + ((FUELBAR_SIZE_X + FUEL_CELL_SPRITE_SEPERATION) * i), 10 + Game.GetCamY(), FUELBAR_SIZE_X, FUELBAR_SIZE_Y,
						fRotationArr);
					Draw.Quad();
					Draw.PopMatrix();
				}

				Draw.TextureBind(uiT[2]);
				Draw.Text(10, 30 + Game.GetCamY(), 16, 9, cFontAltitudeText, strlen(cFontAltitudeText), false);
				Draw.Text(10, 50 + Game.GetCamY(), 16, 9, cFontHSText, strlen(cFontHSText), false);
				Draw.Text(10, 70 + Game.GetCamY(), 16, 9, cGameTimerText, strlen(cGameTimerText), false);
			}

			Game.ShowFPS();
			Draw.SwapBuffers();
			Game.GetFrameTime().mLastTime = Game.GetFrameTime().mNowTime;
		}
	}

	pthread_mutex_destroy(&mutex1);

	Draw.TextureFree(uiT[0]);
	Draw.TextureFree(uiT[1]);
	Draw.TextureFree(uiT[2]);
	Draw.TextureFree(uiT[4]);
	Draw.TextureFree(uiT[3]);
	Draw.Free();
	Client.Free();
	return 0;
}