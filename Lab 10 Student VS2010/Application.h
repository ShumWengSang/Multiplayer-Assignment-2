#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <hge.h>
#include <hgerect.h>
#include <hgeFont.h>
#include "ship.h"
#include "missile.h"
#include <vector>
#include "RakPeerInterface.h"
#include "ChatSystem.h"
#include "SecondCustomPacket.h"
#include "DiffieHelman\DiffieHelmanKey.h"
#include "CRC4.h"

class HGE;
class hgeFont;

using namespace RakNet;

//! The default angular velocity of the ship when it is in motion
static const float DEFAULT_ANGULAR_VELOCITY = 3.0f; 
//! The default acceleration of the ship when powered
static const float DEFAULT_ACCELERATION = 100.0f;

/**
* The application class is the main body of the program. It will
* create an instance of the graphics engine and execute the
* Update/Render cycle.
*
*/

class Application
{
	HGE* hge_; //!< Instance of the internal graphics engine
	typedef std::vector<Ship*> ShipList;  //!< A list of ships
	ShipList ships_; //!< List of all the ships in the universe
	RakPeerInterface* rakpeer_;
	unsigned int timer_;
	
	// Lab 10 Task 1 : add variables for local missle
	Missile* mymissile;
	bool have_missile;
	bool keydown_enter;
	bool keydown_space;

	// Lab 10 Task 8 : add variables to handle networked missiles
	typedef std::vector<Missile*> MissileList;
	MissileList missiles_;


	bool Init();
	static bool Loop();
	void Shutdown();
	bool checkCollisions(Ship* ship);
	void ProcessWelcomePackage();
	bool SendInitialPosition();

	void CreateMissile( float x, float y, float w, int id );

	void SendCollision( Ship* ship );

	bool Receive();
	void Send();
	bool Keyboard(float timedelta);

	bool RecordedTimeN;

	bool KeyExchange;

	CRC4 RC4;
	DiffieHelmanKey Key;

public:
	Application();
	~Application() throw();

	void Start();
	bool Update();
	void Render();

	unsigned int Time;
	bool ExitGameMessage;
	std::auto_ptr<hgeFont> ExitMessage;
	std::vector<std::string> Text;

	ChatSystem * theChat;
};

#endif