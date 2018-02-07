#include "C_M1_Spacewar_Server.h"

C_M1_Spacewar_Server::C_M1_Spacewar_Server():

	//Create main socket and bind the SERVER_PORT to it
	socket(SDLNet_UDP_Open(SERVER_PORT)),
	clientNum{0}

{
	mainTimer.RestartTimer();
	firstTime_UpdateRemainingTime = true;
	registerChannel = 1;
}

void C_M1_Spacewar_Server::Reset()
{
	clientNum = 0;
	registerChannel = 1;
	mainTimer.RestartTimer();
	firstTime_UpdateRemainingTime = true;
	clientEndpoints.clear();
	clientEndpoints.shrink_to_fit();

	playerData.clear();
	playerData.shrink_to_fit();
	bulletData.clear();
	bulletData.shrink_to_fit();
	asteroidPositions.clear();
	asteroidPositions.shrink_to_fit();
	gameInfo.clear();
	gameInfo.shrink_to_fit();

	SDLNet_UDP_Unbind(socket, 1);
	SDLNet_UDP_Unbind(socket, 2);

	gameEnded = false;

}

unsigned short C_M1_Spacewar_Server::RegisterClients()
{
	UDPpacket registerPacket = *SDLNet_AllocPacket(1);

	m1::Recv_UDP_Packet(socket, registerPacket);

	m1::SafeLog("Received packet from: ", m1::to_string(SDLNet_ResolveIP(&registerPacket.address)), ":", registerPacket.address.port,"\n" );

	static bool newEndpoint;
	newEndpoint = true;

	for (int i = 0; i < clientEndpoints.size(); ++i)
	{
		std::vector<m1::client_endpoint>::iterator endpointIT = clientEndpoints.begin() + i;
		if (registerPacket.address.host == endpointIT->host && registerPacket.address.port == endpointIT->port)
		{
			newEndpoint = false;
		}

	}
	if (newEndpoint == true)
	{
		++clientNum;
		//Create new endpoint with the received ip/port
		clientEndpoints.push_back(registerPacket.address);
		//Create a new playerData to store the player position etc.
		playerData.push_back(m1::multiplayerData_Player());
		bulletData.push_back(m1::multiplayerData_Bullet());
		gameInfo.push_back(m1::multiplayerData_GameInfo());
		//Bind socket channel to endpoint first client = channel 1 second client = channel 2 ...
		SDLNet_UDP_Bind(socket, registerChannel, &registerPacket.address);
		m1::SafeLog("Bound IP: ", registerPacket.address.host, " PORT ", registerPacket.address.port, " To Channel ", registerChannel, "\n");
		++registerChannel;
	}

	return clientNum;
}

void C_M1_Spacewar_Server::SendStartTime()
{
	static UDPpacket p = *SDLNet_AllocPacket(sizeof(Uint32));
	p.len = sizeof(Uint32);

	static m1::C_Timer timer;
	timer.RestartTimer();
	
	for (int i = 0; i < clientEndpoints.size(); ++i)
	{	
		Uint32 startTime = STARTTIME + timer.GetTimeSinceStart();
		memcpy(p.data, &startTime, p.len);
		SDLNet_UDP_Send(socket, 1+i, &p);
	}


}



void C_M1_Spacewar_Server::Start()
{
	thread_multiplayer_send = boost::thread( [this] {this->MultiplayerThread_Send(); } );
	thread_multiplayer_receive = boost::thread( [this] { this->MultiplayerThread_Receive(); } );
}

void C_M1_Spacewar_Server::Stop()
{
	thread_multiplayer_send.join();
	thread_multiplayer_receive.join();
}

void C_M1_Spacewar_Server::DebugInformationThread()
{

	static Uint32 lastTime = SDL_GetTicks();

	if (SDL_GetTicks() - lastTime >= 1000)
	{
		//m1::SafeLog("Player1 X: ", playerData[0].xPos, " Player1 Y: ", playerData[0].yPos, "\n");
		//m1::SafeLog("Player2 X: ", playerData[1].xPos, " Player2 Y: ", playerData[1].yPos, "\n");
		//m1::SafeLog("Socket channel 1 IP: ", SDLNet_UDP_GetPeerAddress(socket, 1)->host, " PORT: ", SDLNet_UDP_GetPeerAddress(socket, 1)->port, "\n");
		//m1::SafeLog("Socket channel 2 IP: ", SDLNet_UDP_GetPeerAddress(socket, 2)->host, " PORT: ", SDLNet_UDP_GetPeerAddress(socket, 2)->port, "\n");
		lastTime = SDL_GetTicks();
	}

	

}

void C_M1_Spacewar_Server::MultiplayerThread_Send()
{
	//m1::SafeLog("Start Send\n");

	UDPpacket playerDataPacket = *SDLNet_AllocPacket(sizeof(m1::multiplayerData_Player));
	playerDataPacket.len = sizeof(m1::multiplayerData_Player);

	UDPpacket bulletDataPacket = *SDLNet_AllocPacket(sizeof(m1::multiplayerData_Bullet));
	bulletDataPacket.len = sizeof(m1::multiplayerData_Bullet);

	UDPpacket asteroidDataPacket = *SDLNet_AllocPacket(sizeof(m1::multiplayerData_Asteroid));
	asteroidDataPacket.len = sizeof(m1::multiplayerData_Asteroid);

	UDPpacket gameInfoPacket = *SDLNet_AllocPacket(sizeof(m1::multiplayerData_GameInfo));
	gameInfoPacket.len = sizeof(m1::multiplayerData_GameInfo);

	static Uint32 startTime = SDL_GetTicks();
	uint32_t sleepTime = 0;

	while (gameEnded == false)
	{
		
		startTime = SDL_GetTicks();

		//Sent Player 2 data to client 1
		m1::SafeMemcpy(playerDataPacket.data, &playerData.at(1), sizeof(m1::multiplayerData_Player));
		SDLNet_UDP_Send(socket, 1, &playerDataPacket);
		//Send Player 1 data to client 2
		m1::SafeMemcpy(playerDataPacket.data, &playerData.at(0), sizeof(m1::multiplayerData_Player));
		SDLNet_UDP_Send(socket, 2, &playerDataPacket);

		//Send Player 2 bullet data to client 1
		m1::SafeMemcpy(bulletDataPacket.data, &bulletData.at(1), sizeof(m1::multiplayerData_Bullet));
		SDLNet_UDP_Send(socket, 1, &bulletDataPacket);
		//Send Player 1 bullet data to client 2
		m1::SafeMemcpy(bulletDataPacket.data, &bulletData.at(0), sizeof(m1::multiplayerData_Bullet));
		SDLNet_UDP_Send(socket, 2, &bulletDataPacket);

		//Send Asteroid data to all clients 
		static m1::multiplayerData_Asteroid cacheAsteroidData;
			
		cacheAsteroidData.Init(asteroidPositions);
		m1::SafeMemcpy(asteroidDataPacket.data, &cacheAsteroidData, sizeof(m1::multiplayerData_Asteroid));
		SDLNet_UDP_Send(socket, 1, &asteroidDataPacket);
		SDLNet_UDP_Send(socket, 2, &asteroidDataPacket);
		
		//Send gameInfo to all clients
		gameInfo[0].otherScore = gameInfo[1].score;
		gameInfo[1].otherScore = gameInfo[0].score;

		m1::SafeMemcpy(gameInfoPacket.data, &gameInfo[0], sizeof(m1::multiplayerData_GameInfo));
		SDLNet_UDP_Send(socket, 1, &gameInfoPacket);

		m1::SafeMemcpy(gameInfoPacket.data, &gameInfo[1], sizeof(m1::multiplayerData_GameInfo));
		SDLNet_UDP_Send(socket, 2, &gameInfoPacket);

		sleepTime = SERVER_TICKRATE_DELAY - (SDL_GetTicks() - startTime);
		if (sleepTime > 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
		}

		
	}

	gameInfo[0].otherScore = gameInfo[1].score;
	gameInfo[1].otherScore = gameInfo[0].score;

	m1::SafeMemcpy(gameInfoPacket.data, &gameInfo[0], sizeof(m1::multiplayerData_GameInfo));
	SDLNet_UDP_Send(socket, 1, &gameInfoPacket);

	m1::SafeMemcpy(gameInfoPacket.data, &gameInfo[1], sizeof(m1::multiplayerData_GameInfo));
	SDLNet_UDP_Send(socket, 2, &gameInfoPacket);

}

void C_M1_Spacewar_Server::MultiplayerThread_Receive()
{	
	//Receive Player 1, Player2 data
	ReceiveAndHandleClientPackets();
}