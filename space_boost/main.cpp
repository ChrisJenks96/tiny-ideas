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

//some variables should only be init'd when loading the first time
bool bGameFirstTimeUse = true;
int iMenuOptions = 0;

//everything to be initialised when entering the main menu state
static void menuInit()
{
	//frame time/time updates - not really game related
	Game.Init();
	Client.IPFree();
}

//everything to be initialised when entering the game state
static void gameInit(GLuint* pTextures, float& rBoostRotation, int& rBoostStartID, int& rBoostEndID, float* pRotationArr)
{
	if (bGameFirstTimeUse)
	{
		Ship1.Create();

		//load the game
		Game.SaveOrLoad(false);

		//load statically held textures
		pTextures[0] = Draw.TextureInit(ship_bmp, SHIP_SIZE, SHIP_SIZE);
		pTextures[1] = Draw.TextureInit(fuelbar_bmp, FUELBAR_SIZE_X, FUELBAR_SIZE_Y);
		pTextures[3] = Draw.TextureInit(boost_bmp, BOOST_SIZE, BOOST_SIZE);
		pTextures[4] = Draw.TextureInit(smoke_bmp, SMOKE_SIZE, SMOKE_SIZE);

		bGameFirstTimeUse = false;
	}

	else
	{
		//save the highscore that the player has
		Game.SaveOrLoad(true);
	}

	Ship1.Reset();

	rBoostRotation = 0.0f;
	memset(&pRotationArr[0], 0, sizeof(float) * 3);
	pRotationArr[3] = 1.0f;
	
	rBoostStartID = 0;
	rBoostEndID = 0;
}

int main(int argc, char** argv)
{
	bool bErr = Draw.Init();
	if (bErr == false)
	{
		return -1;
	}

	//start the game out in the menu state
	int iState = STATE_MENU;

	GLuint uiT[MAX_TEXTURES];
	//shorthand vars to store the ships pos
	float fShipX(0.0f), fShipY(0.0f);

	float fBoostRotation = 0.0f;
	//for the glRotatef function
	float fRotationArr[4];
	//masking out the boost sprites
	int iBoostStartID;
	int iBoostEndID;

	float fThrustSmoke[SMOKE_PARTICLES] = {
		0.0f, (SMOKE_SPRITE_SIZE * 0.4f), 
		(SMOKE_SPRITE_SIZE * 0.8f), 
		(SMOKE_SPRITE_SIZE * 1.2f)
	};

	//text for printing out different information
	char cFontAltitudeText[ALTITUDE_TEXT_SIZE];
	char cFontHSText[HIGH_SCORE_TEXT_SIZE];
	char cGameTimerText[GAME_TIMER_TEXT_SIZE];

	//load the font regardless of initial state
	uiT[2] = Draw.TextureInit(font_bmp, FONT_SIZE, FONT_SIZE);

	if (iState == STATE_MENU)
	{
		menuInit();
	}

	else if (iState == STATE_GAME)
	{
		gameInit(uiT, fBoostRotation, iBoostStartID, iBoostEndID, fRotationArr);
	}

	sprintf(&cGameTimerText[0], "TIME: -00:00:0%i", -Game.GetTime().mSeconds);

	while (!Draw.IsQuit())
	{
		Game.GetFrameTime().mNowTime = glfwGetTime();
		Game.GetFrameTime().mDeltaTime = (Game.GetFrameTime().mNowTime - 
			Game.GetFrameTime().mLastTime);

		//updates

		if (Game.GetFrameTime().mDeltaTime >= TARGET_FPS)
		{
			if (iState == STATE_MENU)
			{
				//quit if the escape button is pressed or the window close event is triggered
				Draw.SetQuit(glfwWindowShouldClose(Draw.GetWindow()) | (cLastKey == 0));

				//press enter and proceed to the game
				if (cLastKey == 1)
				{
					if (iMenuOptions == 0)
					{
						//init the game assets
						gameInit(uiT, fBoostRotation, iBoostStartID, iBoostEndID, fRotationArr);
						iState = STATE_GAME;
					}

					else if (iMenuOptions == 1)
					{
						//create a client, wont re-create if the client init flag is on
						Client.Init();
					
						if (Client.GetConnectedToHost())
						{
							//if the client cannot connect and we've picked to do multiplayer,
							//reset out state back to menu
							Client.Connect(7654, 5000);
							Client.IPFree();
							//tell the server everything is good on client end and we're connecting
							//this is set to false on client end if we disconnect
							if (Client.GetNewID())
							{
								//init the game assets
								gameInit(uiT, fBoostRotation, iBoostStartID, iBoostEndID, fRotationArr);
								iState = STATE_GAME;
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
					Client.UpdateIPAddrText(cLastKey);
				}
			}

			else if (iState == STATE_GAME)
			{
				fShipX = std::get<0>(Ship1.GetXY());
				fShipY = std::get<1>(Ship1.GetXY());
				
				//escape
				if (cLastKey == 0)
				{
					iState = STATE_MENU;
					Client.Disconnect();
				}

				if (Client.GetConnectedToHost())
				{
					if (Client.GetClientState() == CLIENT_STATE_NEW_ID)
					{
						Client.CreateUpdateThread(Ship1);
					}

					else if (Client.GetClientState() == CLIENT_STATE_SERVER_FULL)
					{
						//revert back to the menu if the max capacity is limited
						iState = STATE_MENU;
					}

					else if (Client.GetClientState() == CLIENT_STATE_IN_GAME)
					{
						//update the client positions to send off to the server
						Client.SetClientPos(fShipX, fShipY);

						//Update the direction of the ship used (for thrust visual)
						Client.SetDataShipDirection(bKeys[GLFW_KEY_W], bKeys[GLFW_KEY_A], bKeys[GLFW_KEY_D]);
					}

					//spacebar
					if (cLastKey == 32)
					{
						Client.SetClientReadyToPlay(true);
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

				if (iMenuOptions == 0 || (iMenuOptions == 1 && Client.GetServerReadyToLetUsPlay()))
				{
					Game.TimerUpdate(cGameTimerText);
				}
			}

			cLastKey = -1;
			
			//rendering

			if (iState == STATE_MENU)
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
					switch (Client.GetClientState())
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
					
					Draw.Text(HALF_SCR_WIDTH, HALF_SCR_HEIGHT + 85.0f, 16.0f, 9.0f, Client.GetIPAddrText(), Client.GetIPAddrTextLength(), true);
					Draw.TextureUnbind();
					Draw.Box(HALF_SCR_WIDTH - (6.8f * 16.0f), HALF_SCR_HEIGHT + 80.0f, 240.0f, 27.0f, 1.25f);
				}
			}

			else if (iState == STATE_GAME)
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
				DATA_PACKET_GLOBAL& globPacketData = Client.GetGlobalData();
				if (Client.GetClientState() == CLIENT_STATE_IN_GAME)
				{
					for (int j = 0; j < SERVER_MAX_CLIENTS; j++)
					{
						if (globPacketData.data[j].cId != Client.GetServerID() && globPacketData.data[j].bConnected)
						{
							//throttle
							if (globPacketData.data[j].bPressUp)
							{
								for (int i = SMOKE_PARTICLES; i >= 0; --i)
								{
									if (!globPacketData.data[j].bPressLeft)
									{
										Draw.Thrust(globPacketData.data[j].fPos[0] - 2.0f, globPacketData.data[j].fPos[1] + 25.0f, &fThrustSmoke[i], THRUST_LENGTH, SMOKE_SPRITE_SIZE, 10.0f * Game.GetFrameTime().mDeltaTime);
									}

									if (!globPacketData.data[j].bPressRight)
									{
										Draw.Thrust(globPacketData.data[j].fPos[0] + 20.0f, globPacketData.data[j].fPos[1] + 25.0f, &fThrustSmoke[i], THRUST_LENGTH, SMOKE_SPRITE_SIZE, 10.0f * Game.GetFrameTime().mDeltaTime);
									}
								}
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
				if (Client.GetClientState() == CLIENT_STATE_IN_GAME)
				{
					for (int i = 0; i < SERVER_MAX_CLIENTS; i++)
					{
						if (globPacketData.data[i].cId != Client.GetServerID() && 
							globPacketData.data[i].bConnected)
						{
							Draw.PushMatrix();
							Draw.TransformQuad(globPacketData.data[i].fPos[0], globPacketData.data[i].fPos[1], 
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

			//Game.ShowFPS();
			Draw.SwapBuffers();
			Game.GetFrameTime().mLastTime = Game.GetFrameTime().mNowTime;
		}
	}

	Draw.TextureFree(uiT[0]);
	Draw.TextureFree(uiT[1]);
	Draw.TextureFree(uiT[2]);
	Draw.TextureFree(uiT[4]);
	Draw.TextureFree(uiT[3]);
	Draw.Free();
	Client.Free();
	return 0;
}