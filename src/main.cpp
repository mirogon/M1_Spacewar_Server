#include "C_M1_Spacewar_Server.h"
#undef main

int main(int argc, char* argv[])
{
	try
	{

		if (SDL_Init(0) == -1)
		{
			throw SDL_GetError();
		}

		if (SDLNet_Init() == -1)
		{
			throw SDLNet_GetError();
		}


		//Open SERVER_PORT on windows firewall for this program
		std::string firewallException = "FirewallException.exe ";
		firewallException += argv[0];
		system(firewallException.c_str());

		C_M1_Spacewar_Server gameServer;

		while (true)
		{
			m1::SafeLog("Server Started\n");
			
			m1::C_Timer mainTimer;
			mainTimer.RestartTimer();

			m1::SafeLog("Waiting for clients to connect\n");

			while (gameServer.RegisterClients() != CLIENT_NUM)
			{
			}

			m1::SafeLog("2 unique clients connected!\n");

			SDL_Delay(500);

			gameServer.SendStartTime();

			m1::SafeLog("Start time sent to all clients!\n");

			gameServer.Start();

			SDL_Delay(STARTTIME);

			while (gameEnded == false)
			{
				static uint32_t lastTime = mainTimer.GetTimeSinceStart();

				gameServer.UpdateRemainingTime();
				if (gameServer.CheckWhoWon() == true)
				{
					gameEnded = true;
				}
				gameServer.GenerateAsteroidPositions();

				gameServer.MoveAsteroids(mainTimer.GetTimeSinceStart() - lastTime);
				lastTime = mainTimer.GetTimeSinceStart();

				gameServer.DeleteInactiveAsteroids();

				gameServer.AsteroidsBulletsCollisionHandling();
				gameServer.AsteroidsPlayerCollisionHandling();
				gameServer.RaiseScores();

				//gameServer.DebugInformationThread();

			}

			gameServer.Stop();
			gameServer.Reset();
			m1::SafeLog("Server Reset\n");

		}

	}


	catch (std::exception& e)
	{
		std::cerr << "Error with code: " << e.what() << std::endl;
	}
	catch (const char* e)
	{
		std::cerr << "Error with code: " << e << std::endl;
	}

	SDLNet_Quit();
	SDL_Quit();

	system("pause");
	return 0;
}