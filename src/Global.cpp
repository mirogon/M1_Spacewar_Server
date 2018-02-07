#include "Global.h"

bool gameEnded = false;

const int CLIENT_NUM = 2;
const Uint32 STARTTIME = 3000;
const unsigned int SERVER_PORT = 55555;

const short SERVER_TICKRATE = 64;
const short SERVER_TICKRATE_DELAY = 1000/SERVER_TICKRATE;

boost::mutex mutex_iostream;
boost::mutex mutex_playerData[2];
boost::mutex mutex_bulletData[2];
boost::mutex mutex_gameInfo[2];
boost::mutex mutex_asteroids;
boost::mutex mutex_socket;
boost::mutex mutex_memcpy;


const uint32_t PLAYTIME = 60000;

//Asteroid
const float ASTEROID_SPEED = 0.4f;
const int ASTEROID_SIZE = 50;
const float ASTEROID_SPAWN_LATENCY = 750;

//Client
const int WINDOW_SIZE = 720;

//Player
const int PLAYERSHIP_SIZE = 50;

//Laser
const int LASER_SIZE = 2;