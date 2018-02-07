#pragma once
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <boost\thread.hpp>
#include <SDL.h>
#include <SDL_net.h>
#include <thread>
#include <M1Timer.h>
#include <M1Random.h>

/*
	playerIndex 0 = first Client 1 = second Client

*/
extern bool gameEnded;

extern const int CLIENT_NUM;
extern const Uint32 STARTTIME;
extern const unsigned int SERVER_PORT;
extern const short SERVER_TICKRATE;
extern const short SERVER_TICKRATE_DELAY;

extern boost::mutex mutex_iostream;
extern boost::mutex mutex_playerData[2];
extern boost::mutex mutex_bulletData[2];
extern boost::mutex mutex_gameInfo[2];
extern boost::mutex mutex_asteroids;
extern boost::mutex mutex_socket;
extern boost::mutex mutex_memcpy;

//GENERAL


extern const uint32_t PLAYTIME;
//Asteroid
extern const float ASTEROID_SPEED;
extern const int ASTEROID_SIZE;
extern const float ASTEROID_SPAWN_LATENCY;
//Client
extern const int WINDOW_SIZE;
//Laser
extern const int LASER_SIZE;

//Player
extern const int PLAYERSHIP_SIZE;
//FUNCTIONS



namespace m1 
{
	template<typename T>
	inline void SafeLog(const T& s)
	{
		mutex_iostream.lock();
		std::cout << s;
		mutex_iostream.unlock();
	}

	template<typename FIRST, typename... REST>
	inline void SafeLog(const FIRST& first, const REST&... rest)
	{
		SafeLog(first);
		SafeLog(rest...);
	}

	inline void SafeMemcpy(void* dst, const void* src, size_t size)
	{
		mutex_memcpy.lock();
		memcpy(dst, src, size);
		mutex_memcpy.unlock();
	}



	struct Sint16_Position
	{
		Sint16_Position() : x{ 0 }, y{ 0 } {}
		Sint16_Position(Sint16 x_, Sint16 y_) : x{ x_ }, y{ y_ } {}
		
		Sint16 x;
		Sint16 y;
	};

	struct double_Position
	{
		double_Position() :	x{ 0 },	y{ 0 }	{}

		double_Position(double x_, double y_):	x{x_},	y{y_} {}

		double x;
		double y;
	};

	struct multiplayerData_Player
	{
		multiplayerData_Player() :
			ID{0},
			xPos{ 0 },
			yPos{ 0 }

		{
		}
		const Uint8 ID;
		Sint16 xPos;
		Sint16 yPos;
	};

	struct multiplayerData_Bullet
	{
		multiplayerData_Bullet() :
			ID{ 1 },
			bulletNum{ 0 },
			bulletPositions{ {0,0} }
		{
			
		}
		multiplayerData_Bullet(const multiplayerData_Bullet& copy) :
			ID{ 1 }
		{
			bulletNum = copy.bulletNum;
			for (int i = 0; i < bulletNum; i++)
			{
				bulletPositions[i] = copy.bulletPositions[i];
			}
		}


		const Uint8 ID;
		uint8_t bulletNum;
		Sint16_Position bulletPositions[64];

	};

	struct multiplayerData_Asteroid
	{

		multiplayerData_Asteroid() :
			ID{ 2 },
			asteroidPositions{{0,0}}
		{

		}

		void Init(std::vector<m1::double_Position>& positions)
		{
			asteroidNum = positions.size();
			if (asteroidNum > 32)
			{
				asteroidNum = 32;
			}

			for (int i = 0; i < asteroidNum; i++)
			{
				asteroidPositions[i].x = static_cast<Sint16>(positions.at(i).x);
				asteroidPositions[i].y = static_cast<Sint16>(positions.at(i).y);
			}
		}

		const Uint8 ID;
		uint8_t asteroidNum;
		Sint16_Position asteroidPositions[32];
	};

	struct multiplayerData_GameInfo
	{
		multiplayerData_GameInfo() : ID{ 3 }, score{ 0 }, otherScore{0}, remainingTime { 60000 }, won{ false } {}
		const Uint8 ID;
		int32_t score;
		int32_t otherScore;
		uint32_t remainingTime;
		bool won;
	};

	typedef IPaddress client_endpoint;

	inline void Recv_UDP_Packet(UDPsocket& socket, UDPpacket& packet)
	{
		short rc = SDLNet_UDP_Recv(socket, &packet);
		if (rc == 1)
		{
			return;
		}

		if (rc == 0)
		{
			Recv_UDP_Packet(socket, packet);
		}

		else if (rc == -1)
		{
			throw SDLNet_GetError();
		}

	}


	enum E_CollisionDirection { Left = 0, Right = 1, Bot = 2, Top = 3, None = 4 };


	inline m1::E_CollisionDirection CollisionDetection(const SDL_Rect& pCollisionBox, const SDL_Rect& collisionBox)
	{
		float w = 0.5 * (pCollisionBox.w + collisionBox.w);
		float h = 0.5 * (pCollisionBox.h + collisionBox.h);
		float dx = (pCollisionBox.x + pCollisionBox.w / 2) - (collisionBox.x + collisionBox.w / 2);
		float dy = (pCollisionBox.y + pCollisionBox.h / 2) - (collisionBox.y + collisionBox.h / 2);

		if (abs(dx) <= w && abs(dy) <= h)
		{
			/* collision! */
			float wy = w * dy;
			float hx = h * dx;

			if (wy > hx)
			{
				if (wy > -hx)
				{
					return Bot;
				}

				else
				{
					return Left;
				}
			}

			else
			{
				if (wy > -hx)
				{
					return Right;
				}

				else
				{
					return Top;
				}

			}

		}

		else return None;

	}

	//CAST EVERYTHING TO A STRING
	template <typename T>
	inline std::string to_string(T value)
	{
		std::ostringstream os;
		os << value;
		return os.str();
	}

}
