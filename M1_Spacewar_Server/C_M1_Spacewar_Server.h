#pragma once
#include "Global.h"

class C_M1_Spacewar_Server
{

public:

	C_M1_Spacewar_Server();
	~C_M1_Spacewar_Server() = default;

	//Wait for the clients to register
	unsigned short RegisterClients();

	//Send the game start time to the clients
	void SendStartTime();

	void Reset();
	void Start();
	void Stop();

	void DebugInformationThread();

	void GenerateAsteroidPositions();
	void DeleteInactiveAsteroids();

	void MoveAsteroids(uint32_t deltaTime);

	void AsteroidsBulletsCollisionHandling();
	void AsteroidsPlayerCollisionHandling();

	void RaiseScores();
	void UpdateRemainingTime();
	bool CheckWhoWon();

private:

	UDPsocket socket;
	short registerChannel;

	boost::thread thread_multiplayer_send;
	boost::thread thread_multiplayer_receive;

	std::vector<m1::client_endpoint> clientEndpoints;
	unsigned short clientNum;

	std::vector<m1::multiplayerData_Player> playerData;
	std::vector<m1::multiplayerData_Bullet> bulletData;
	std::vector<m1::double_Position> asteroidPositions;
	std::vector<m1::multiplayerData_GameInfo> gameInfo;

	m1::C_Timer mainTimer;
	m1::C_Random randomGenerator;

	bool firstTime_UpdateRemainingTime;

	//FUNCTIONS
	void ReceiveAndHandleClientPackets();

	void MultiplayerThread_Send();
	void MultiplayerThread_Receive();



};

//FUNCTIONS
inline void C_M1_Spacewar_Server::ReceiveAndHandleClientPackets()
{
	//m1::SafeLog("Trying to receive a package\n");
	static UDPpacket packet = *SDLNet_AllocPacket(500);
	packet.len = 500;

	static uint8_t ID = 0;
	static short endpointIndex;

	while (gameEnded == false)
	{
		short rc = SDLNet_UDP_Recv(socket, &packet);

		if (rc == 1)
		{
			//Identify the client who sent the package
			for (int i = 0; i < CLIENT_NUM; i++)
			{
				static std::vector<m1::client_endpoint>::iterator iT;
				iT = clientEndpoints.begin() + i;

				if (packet.address.host == iT->host && packet.address.port == iT->port)
				{
					endpointIndex = i;
				}

			}
			//m1::SafeLog("Send 2\n");
			m1::SafeMemcpy(&ID, packet.data, sizeof(uint8_t));

			switch (ID)
			{

			case 0:
				//m1::SafeLog("Send 3\n");
				mutex_playerData[endpointIndex].lock();
				m1::SafeMemcpy(&playerData.at(endpointIndex), packet.data, sizeof(m1::multiplayerData_Player));
				mutex_playerData[endpointIndex].unlock();
				break;
			case 1:
				//m1::SafeLog("Send 4\n");
				mutex_bulletData[endpointIndex].lock();
				m1::SafeMemcpy(&bulletData.at(endpointIndex), packet.data, sizeof(m1::multiplayerData_Bullet));
				mutex_bulletData[endpointIndex].unlock();
				break;
			}
		}

		else if (rc == 0)
		{
		}

		else if (rc == -1)
		{
			throw SDLNet_GetError();
		}

	}

}

inline void C_M1_Spacewar_Server::GenerateAsteroidPositions()
{
	static uint32_t lastTime = mainTimer.GetTimeSinceStart();
	//m1::SafeLog("Time since start: ", mainTimer.GetTimeSinceStart(),"\n");
	if (mainTimer.GetTimeSinceStart() - lastTime >= ASTEROID_SPAWN_LATENCY)
	{
		//m1::SafeLog("Created Asteroid\n");
		mutex_asteroids.lock();
		asteroidPositions.push_back( m1::double_Position( randomGenerator.randomNumber(-ASTEROID_SIZE, WINDOW_SIZE - ASTEROID_SIZE / 2 ) , -ASTEROID_SIZE*5 ) );
		mutex_asteroids.unlock();

		lastTime = mainTimer.GetTimeSinceStart();
	}

}

inline void C_M1_Spacewar_Server::DeleteInactiveAsteroids()
{
	static std::vector<m1::double_Position>::iterator iT;

	mutex_asteroids.lock();
	for (int i = 0; i < asteroidPositions.size(); i++)
	{
		iT = asteroidPositions.begin() + i;
		if (asteroidPositions.at(i).y > WINDOW_SIZE)
		{
			asteroidPositions.erase(iT);
			asteroidPositions.shrink_to_fit();
		}
	}
	mutex_asteroids.unlock();

}

inline void C_M1_Spacewar_Server::MoveAsteroids(uint32_t deltaTime)
{
	mutex_asteroids.lock();
	for (int i = 0; i < asteroidPositions.size(); i++)
	{
		asteroidPositions.at(i).y += deltaTime * ASTEROID_SPEED;
	}
	mutex_asteroids.unlock();
}

inline void C_M1_Spacewar_Server::AsteroidsBulletsCollisionHandling()
{
	static SDL_Rect laserRect{0,0,LASER_SIZE,LASER_SIZE};
	static SDL_Rect asteroidRect{ 0,0,ASTEROID_SIZE,ASTEROID_SIZE };

	mutex_bulletData[0].lock();
	mutex_asteroids.lock();
	for (int i = 0; i < bulletData[0].bulletNum; i++)
	{
		for (int o = 0; o < asteroidPositions.size(); o++)
		{
		
			laserRect.x = bulletData[0].bulletPositions[i].x;
			laserRect.y = bulletData[0].bulletPositions[i].y;
			asteroidRect.x = asteroidPositions.at(o).x;
			asteroidRect.y = asteroidPositions.at(o).y;
		
			if (m1::CollisionDetection(laserRect, asteroidRect) != m1::E_CollisionDirection::None)
			{
				asteroidPositions.erase(asteroidPositions.begin() + o);
				asteroidPositions.shrink_to_fit();
				mutex_gameInfo[0].lock();
				gameInfo[0].score += 100;
				mutex_gameInfo[0].unlock();
			}
		}
	}
	mutex_asteroids.unlock();
	mutex_bulletData[0].unlock();

	mutex_bulletData[1].lock();
	mutex_asteroids.lock();
	for (int i = 0; i < bulletData[1].bulletNum; i++)
	{
		for (int o = 0; o < asteroidPositions.size(); o++)
		{
			laserRect.x = bulletData[1].bulletPositions[i].x;
			laserRect.y = bulletData[1].bulletPositions[i].y;
			asteroidRect.x = asteroidPositions.at(o).x;
			asteroidRect.y = asteroidPositions.at(o).y;

			if (m1::CollisionDetection(laserRect, asteroidRect) != m1::E_CollisionDirection::None)
			{
				asteroidPositions.erase(asteroidPositions.begin() + o);
				asteroidPositions.shrink_to_fit();
				mutex_gameInfo[1].lock();
				gameInfo[1].score += 100;
				mutex_gameInfo[1].unlock();
			}
		}
	}
	mutex_asteroids.unlock();
	mutex_bulletData[1].unlock();

}

inline void C_M1_Spacewar_Server::AsteroidsPlayerCollisionHandling()
{
	static SDL_Rect playerRect{ 0,0,PLAYERSHIP_SIZE, PLAYERSHIP_SIZE };
	static SDL_Rect asteroidRect{ 0,0,ASTEROID_SIZE,ASTEROID_SIZE };

	mutex_asteroids.lock();
	for (int i = 0; i < asteroidPositions.size(); i++)
	{
		mutex_playerData[0].lock();
		playerRect.x = playerData[0].xPos;
		playerRect.y = playerData[0].yPos;
		mutex_playerData[0].unlock();
	
		asteroidRect.x = asteroidPositions.at(i).x;
		asteroidRect.y = asteroidPositions.at(i).y;

		if ( m1::CollisionDetection(playerRect, asteroidRect) != m1::E_CollisionDirection::None )
		{
			//m1::SafeLog("Player 1 game over \n");
			asteroidPositions.erase(asteroidPositions.begin() + i);
			asteroidPositions.shrink_to_fit();

			mutex_gameInfo[0].lock();
			gameInfo[0].score -= 250;
			mutex_gameInfo[0].unlock();
		}
	}
	mutex_asteroids.unlock();

	mutex_asteroids.lock();
	for (int i = 0; i < asteroidPositions.size(); i++)
	{
		mutex_playerData[1].lock();
		playerRect.x = playerData[1].xPos;
		playerRect.y = playerData[1].yPos;
		mutex_playerData[1].unlock();

		asteroidRect.x = asteroidPositions.at(i).x;
		asteroidRect.y = asteroidPositions.at(i).y;

		if (m1::CollisionDetection(playerRect, asteroidRect) != m1::E_CollisionDirection::None)
		{
			//m1::SafeLog("Player 2 game over \n");
			asteroidPositions.erase(asteroidPositions.begin() + i);
			asteroidPositions.shrink_to_fit();

			mutex_gameInfo[1].lock();
			gameInfo[1].score -= 250;
			mutex_gameInfo[1].unlock();
		}
	}
	mutex_asteroids.unlock();
}

inline void C_M1_Spacewar_Server::RaiseScores()
{
	static uint32_t lastTime = mainTimer.GetTimeSinceStart();

	if (mainTimer.GetTimeSinceStart() - lastTime >= 1000)
	{
		mutex_gameInfo[0].lock();
		gameInfo[0].score += 25;
		mutex_gameInfo[0].unlock();

		mutex_gameInfo[1].lock();
		gameInfo[1].score += 25;
		mutex_gameInfo[1].unlock();

		lastTime = mainTimer.GetTimeSinceStart();
	}
}

inline void C_M1_Spacewar_Server::UpdateRemainingTime()
{
	static uint32_t startTime = mainTimer.GetTimeSinceStart();

	if (firstTime_UpdateRemainingTime == true)
	{
		firstTime_UpdateRemainingTime = false;
		startTime = mainTimer.GetTimeSinceStart();

	}

	if ((startTime + PLAYTIME) - mainTimer.GetTimeSinceStart() == 0 || (startTime + PLAYTIME) - mainTimer.GetTimeSinceStart() > PLAYTIME)
	{
		gameInfo[0].remainingTime = 0;
		gameInfo[1].remainingTime = 0;
	}

	else
	{
		gameInfo[0].remainingTime = (startTime + PLAYTIME) - mainTimer.GetTimeSinceStart();
		gameInfo[1].remainingTime = (startTime + PLAYTIME) - mainTimer.GetTimeSinceStart();
	}



}

inline bool C_M1_Spacewar_Server::CheckWhoWon()
{
	if (gameInfo[0].remainingTime == 0 && gameInfo[1].remainingTime == 0)
	{
		if (gameInfo[0].score > gameInfo[1].score)
		{
			gameInfo[0].won = true;
			return true;
		}
		
		else if (gameInfo[1].score > gameInfo[0].score)
		{
			gameInfo[1].won = true;
			return true;
		}

		else if (gameInfo[0].score == gameInfo[1].score)
		{

		}



	}

	return false;

}