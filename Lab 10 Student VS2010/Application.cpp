#include "WindowsIncludes.h"
#include "Application.h"
#include "ship.h"
#include "Globals.h"
#include "MyMsgIDs.h"
#include "Bitstream.h"
#include "GetTime.h"
#include <hge.h>
#include <string>
#include <iostream>
#include <fstream>

// Lab 10 Task 9a : Uncomment the macro NETWORKMISSILE

#define NETWORKMISSLE


float GetAbsoluteMag( float num )
{
	if ( num < 0 )
	{
		return -num;
	}

	return num;
}

/** 
* Constuctor
*
* Creates an instance of the graphics engine and network engine
*/

Application::Application() 
:	hge_(hgeCreate(HGE_VERSION))
,	rakpeer_(RakPeerInterface::GetInstance())
,	timer_( 0 )
// Lab 10 Task 2 : add new initializations
,	mymissile(0)
,	keydown_enter( false )

{
	ExitGameMessage = false;
	RecordedTimeN = false;
	keydown_space = false;
}

/**
* Destructor
*
* Does nothing in particular apart from calling Shutdown
*/

Application::~Application() throw()
{
	ExitMessage.release();
	Shutdown();
	rakpeer_->Shutdown(100);
	RakPeerInterface::DestroyInstance(rakpeer_);
}

/**
* Initialises the graphics system
* It should also initialise the network system
*/

bool Application::Init()
{
	std::ifstream inData;	
	std::string serverip;

	inData.open("serverip.txt");

	inData >> serverip;

	srand( (unsigned int)RakNet::GetTime() );

	hge_->System_SetState(HGE_FRAMEFUNC, Application::Loop);
	hge_->System_SetState(HGE_WINDOWED, true);
	hge_->System_SetState(HGE_USESOUND, false);
	hge_->System_SetState(HGE_TITLE, "Movement");
	hge_->System_SetState(HGE_LOGFILE, "movement.log");
	hge_->System_SetState(HGE_DONTSUSPEND, true);
	

		Text.push_back("You have lost or server max players reached.");

	if(hge_->System_Initiate()) 
	{
		ships_.push_back(new Ship(rand() % 4 + 1, (float)400, (float)550));
		ships_.at(0)->SetName("My Ship");

		ExitMessage.reset(new hgeFont("font1.fnt"));
		ExitMessage->SetScale(0.5);

		theChat = new ChatSystem();

		if (RAKNET_STARTED == rakpeer_->Startup(1, &SocketDescriptor(), 1))
		{
			rakpeer_->SetOccasionalPing(true);
			if (RAKNET_STARTED == rakpeer_->Connect(serverip.c_str(), 1691, 0, 0))
				return true;
		}
	}



	return false;
}

/**
* Update cycle
*
* Checks for keypresses:
*   - Esc - Quits the game
*   - Left - Rotates ship left
*   - Right - Rotates ship right
*   - Up - Accelerates the ship
*   - Down - Deccelerates the ship
*
* Also calls Update() on all the ships in the universe
*/
bool Application::Update()
{
	if (ExitGameMessage)
	{
		static int Time = hge_->Timer_GetTime();

		if (hge_->Timer_GetTime() - Time > 10)
		{
			return true;
		}
	}
	else
	{
		float timedelta = hge_->Timer_GetDelta();
		if (Keyboard(timedelta))
			return true;

		if (ships_.front()->ShipHP <= 0)
		{
			ExitGameMessage = true;
			return false;
		}

		for (ShipList::iterator ship = ships_.begin();
			ship != ships_.end(); ship++)
		{
			(*ship)->Update(timedelta);

			//collisions
			if ((*ship) == ships_.at(0))
				checkCollisions((*ship));
		}

		// Lab 10 Task 5 : Updating the missile
		if (mymissile)
		{
			if (mymissile->Update(ships_, timedelta))
			{
				// have collision
				if (!mymissile->RecordedTime)
				{
					Time = hge_->Timer_GetTime();
					mymissile->RecordedTime = true;
				}
			}
			if (mymissile->Explode)
			{

				if ((hge_->Timer_GetTime() - Time) > 3)
				{
					mymissile->Explode = false;
					delete mymissile;
					mymissile = NULL;
				}
			}
		}


		// Lab 10 Task 13 : Update network missiles
		for (MissileList::iterator missile = missiles_.begin();
			missile != missiles_.end(); missile++)
		{
			if ((*missile)->Update(ships_, timedelta))
			{
				// have collision
				if (!(*missile)->RecordedTime)
				{
					Time = hge_->Timer_GetTime();
					(*missile)->RecordedTime = true;
				}
			}
			if ((*missile)->Explode)
			{
				if ((hge_->Timer_GetTime() - Time) > 3)
				{
					(*missile)->Explode = false;
					delete (*missile);
					(*missile) = NULL;
				}
			}
		}

		for (MissileList::iterator missile = missiles_.begin();
			missile != missiles_.end();)
		{
			if ((*missile) == NULL)
			{
				missile = missiles_.erase(missile);
			}
			else
			{
				missile++;
			}
		}

		if (Receive())
			return true;

		Send();
	}
	return false;
}


/**
* Render Cycle
*
* Clear the screen and render all the ships
*/
void Application::Render()
{

	hge_->Gfx_BeginScene();
	hge_->Gfx_Clear(0);

	if (ExitGameMessage)
	{
		float x = hge_->System_GetState(HGE_SCREENWIDTH);
		float y = hge_->System_GetState(HGE_SCREENHEIGHT);
		ExitMessage->Render(x/2, y/2, HGETEXT_CENTER,
			Text[0].c_str());
		//Text[0].c_str()
	}
	else
	{

		ShipList::iterator itr;
		for (itr = ships_.begin(); itr != ships_.end(); itr++)
		{
			(*itr)->Render();
		}

		// Lab 10 Task 6 : Render the missile
		if (mymissile)
		{
			mymissile->Render();
		}

		// Lab 10 Task 12 : Render network missiles
		MissileList::iterator itr2;
		for (itr2 = missiles_.begin(); itr2 != missiles_.end(); itr2++)
		{
			(*itr2)->Render();
		}

		theChat->Render();
	}
	hge_->Gfx_EndScene();
}


/** 
* Main game loop
*
* Processes user input events
* Supposed to process network events
* Renders the ships
*
* This is a static function that is called by the graphics
* engine every frame, hence the need to loop through the
* global namespace to find itself.
*/
bool Application::Loop()
{
	Global::application->Render();
	return Global::application->Update();
}

/**
* Shuts down the graphics and network system
*/

void Application::Shutdown()
{
	hge_->System_Shutdown();
	hge_->Release();
}

/** 
* Kick starts the everything, called from main.
*/
void Application::Start()
{
	if (Init())
	{
		hge_->System_Start();
	}
}

bool Application::SendInitialPosition()
{
	ShipPacket theShip;

	theShip.theID = ID_INITIALPOS;
	theShip.ServerX = ships_.at(0)->GetX();
	theShip.ServerY = ships_.at(0)->GetY();
	theShip.ShipType = ships_.at(0)->GetType();

	char data[sizeof(ShipPacket)];
	theShip.Serialize(data);
	/*RakNet::BitStream bs;
	unsigned char msgid = ID_INITIALPOS;
	bs.Write(msgid);
	bs.Write(ships_.at(0)->GetX());
	bs.Write(ships_.at(0)->GetY());
	bs.Write(ships_.at(0)->GetType());*/

	std::cout << "Sending pos" << ships_.at(0)->GetX() << " " << ships_.at(0)->GetY() << std::endl;

	rakpeer_->Send(data, strlen(data) + 1,HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);

	return true;
}

bool Application::checkCollisions(Ship* ship)
{
	for (std::vector<Ship*>::iterator thisship = ships_.begin();
		thisship != ships_.end(); thisship++)
	{
		if( (*thisship) == ship ) continue;	//skip if it is the same ship

		if( ship->HasCollided( (*thisship) ) )
		{
			if( (*thisship)->CanCollide( (unsigned int)RakNet::GetTime() ) &&  ship->CanCollide( (unsigned int)RakNet::GetTime() ) )
			{
				std::cout << "collide!" << std::endl;

#ifdef INTERPOLATEMOVEMENT
			if( GetAbsoluteMag( ship->GetVelocityY() ) > GetAbsoluteMag( (*thisship)->GetVelocityY() ) )
			{
				// this transfers vel to thisship
				(*thisship)->SetVelocityY( (*thisship)->GetVelocityY() + ship->GetVelocityY()/3 );
				ship->SetVelocityY( - ship->GetVelocityY() );

				(*thisship)->SetServerVelocityY( (*thisship)->GetServerVelocityY() + ship->GetServerVelocityY()/3 );
				ship->SetServerVelocityY( - ship->GetServerVelocityY() );
			}
			else
			{
				ship->SetVelocityY( ship->GetVelocityY() + (*thisship)->GetVelocityY()/3 ); 
				(*thisship)->SetVelocityY( -(*thisship)->GetVelocityY()/2 );

				ship->SetServerVelocityY( ship->GetServerVelocityY() + (*thisship)->GetServerVelocityY()/3 ); 
				(*thisship)->SetServerVelocityY( -(*thisship)->GetServerVelocityY()/2 );
			}
			
			if( GetAbsoluteMag( ship->GetVelocityX() ) > GetAbsoluteMag( (*thisship)->GetVelocityX() ) )
			{
				// this transfers vel to thisship
				(*thisship)->SetVelocityX( (*thisship)->GetVelocityX() + ship->GetVelocityX()/3 );
				ship->SetVelocityX( - ship->GetVelocityX() );

				(*thisship)->SetServerVelocityX( (*thisship)->GetServerVelocityX() + ship->GetServerVelocityX()/3 );
				ship->SetServerVelocityX( - ship->GetServerVelocityX() );
			}
			else
			{
				// ship transfers vel to asteroid
				ship->SetVelocityX( ship->GetVelocityX() + (*thisship)->GetVelocityX()/3 ); 
				(*thisship)->SetVelocityX( -(*thisship)->GetVelocityX()/2 );

				ship->SetServerVelocityX( ship->GetServerVelocityX() + (*thisship)->GetServerVelocityX()/3 ); 
				(*thisship)->SetServerVelocityX( -(*thisship)->GetServerVelocityX()/2 );
			}

				ship->SetPreviousLocation();
#else
			if( GetAbsoluteMag( ship->GetVelocityY() ) > GetAbsoluteMag( (*thisship)->GetVelocityY() ) )
			{
				// this transfers vel to thisship
				(*thisship)->SetVelocityY( (*thisship)->GetVelocityY() + ship->GetVelocityY()/3 );
				ship->SetVelocityY( - ship->GetVelocityY() );
			}
			else
			{
				ship->SetVelocityY( ship->GetVelocityY() + (*thisship)->GetVelocityY()/3 ); 
				(*thisship)->SetVelocityY( -(*thisship)->GetVelocityY()/2 );
			}
			
			if( GetAbsoluteMag( ship->GetVelocityX() ) > GetAbsoluteMag( (*thisship)->GetVelocityX() ) )
			{
				// this transfers vel to thisship
				(*thisship)->SetVelocityX( (*thisship)->GetVelocityX() + ship->GetVelocityX()/3 );
				ship->SetVelocityX( - ship->GetVelocityX() );
			}
			else
			{
				// ship transfers vel to asteroid
				ship->SetVelocityX( ship->GetVelocityX() + (*thisship)->GetVelocityX()/3 ); 
				(*thisship)->SetVelocityX( -(*thisship)->GetVelocityX()/2 );
			}


//				ship->SetVelocityY( -ship->GetVelocityY() );
//				ship->SetVelocityX( -ship->GetVelocityX() );
			
				ship->SetPreviousLocation();
#endif
				SendCollision( (*thisship) );

				return true;
			}
				
		}

	}
	
	return false;
}

void Application::SendCollision( Ship* ship )
{
	//RakNet::BitStream bs;
	//unsigned char msgid = ID_COLLIDE;
//	bs.Write( msgid );
//	bs.Write( ship->GetID() );
	//bs.Write( ship->GetX() );
	//bs.Write( ship->GetY() );
	//bs.Write( ship->GetVelocityX() );
	//bs.Write( ship->GetVelocityY() );
#ifdef INTERPOLATEMOVEMENT
	//bs.Write( ship->GetServerVelocityX() );
	//bs.Write( ship->GetServerVelocityY() );
#endif

	ShipPacket theShip;
	theShip.theID = ID_COLLIDE;
	theShip.ShipID = ship->GetID();
	theShip.ServerX = ship->GetX();
	theShip.ServerY = ship->GetY();
	theShip.VelX = ship->GetVelocityX();
	theShip.VelY = ship->GetVelocityY();
	theShip.ServerVelX = ship->GetServerVelocityX();
	theShip.ServerVelY = ship->GetServerVelocityY();

	char data[sizeof(ShipPacket)];
	theShip.Serialize(data);
	rakpeer_->Send(data, strlen(data) + 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);

}

void Application::CreateMissile(float x, float y, float w, int id)
{
#ifdef NETWORKMISSLE
	// Lab 10 Task 9b : Implement networked version of createmissile

	if( mymissile )
	{
		MissilePacket theMissile;
		theMissile.deleted = 1;
		theMissile.theID = ID_UPDATEMISSILE;
		theMissile.OwnerID = id;
		theMissile.X = x;
		theMissile.Y = y;
		theMissile.W = w;

		char data[sizeof(MissilePacket)];
		theMissile.Serialize(data);
		// send update throughnetwork to delete this missile
		
		rakpeer_->Send(data, strlen(data) + 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);

		// delete existing missile
		delete mymissile;
		mymissile = 0;		
	}
		
	// add a new missile based on the following parameter coordinates
	mymissile = new Missile("missile.png", x, y, w, id );
// send network command to add new missile

	MissilePacket theMissile;

	theMissile.theID = ID_NEWMISSILE;
	theMissile.OwnerID = id;
	theMissile.X = x;
	theMissile.Y = y;
	theMissile.W = w;
	char data[sizeof(MissilePacket)];
	theMissile.Serialize(data);

	rakpeer_->Send(data, strlen(data) + 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);

#else
// Lab 10 Task 3 : Implement local version missile creation
		if( mymissile )
	{
		// delete existing missile
		delete mymissile;
		mymissile = 0;		
	}
		
	// add a new missile based on the following parameter coordinates
	mymissile = new Missile("missile.png", x, y, w, id );

#endif
}

bool Application::Receive()
{
	if (Packet* packet = rakpeer_->Receive())
	{
		RakNet::BitStream bs(packet->data, packet->length, false);
		
		unsigned char msgid = 0;
		RakNet::Time timestamp = 0;

		bs.Read(msgid);

		if (msgid == ID_TIMESTAMP)
		{
			bs.Read(timestamp);
			bs.Read(msgid);
		}

		switch(msgid)
		{
		case ID_CONNECTION_REQUEST_ACCEPTED:
			std::cout << "Connected to Server" << std::endl;
			break;

		case ID_NO_FREE_INCOMING_CONNECTIONS:
		case ID_CONNECTION_LOST:
		case ID_DISCONNECTION_NOTIFICATION:
			std::cout << "Lost Connection to Server" << std::endl;
			rakpeer_->DeallocatePacket(packet);
			return true;

		case ID_WELCOME:
			{
				unsigned int shipcount, id;
				float x_, y_;
				int type_;
				std::string temp;
				char chartemp[5];

				bs.Read(id);
				ships_.at(0)->setID( id );	
				bs.Read(shipcount);

				for (unsigned int i = 0; i < shipcount; ++ i)
				{
					bs.Read(id);
					bs.Read(x_);
					bs.Read(y_);
					bs.Read(type_);
					std::cout << "New Ship pos" << x_ << " " << y_ << std::endl;
					Ship* ship = new Ship(type_, x_, y_ ); 
					temp = "Ship ";
					temp += _itoa(id, chartemp, 10);
					ship->SetName(temp.c_str());
					ship->setID( id );
					ships_.push_back(ship);
				}

				SendInitialPosition();
			}
			break;

		case ID_NEWSHIP:
			{
				unsigned int id;
				bs.Read(id);

				if( id == ships_.at(0)->GetID() )
				{
					// if it is me
					break;
				}
				else
				{
					float x_, y_;
					int type_;
					std::string temp;
					char chartemp[5];

					bs.Read( x_ );
					bs.Read( y_ );
					bs.Read( type_ );
					std::cout << "New Ship pos" << x_ << " " << y_ << std::endl;
					Ship* ship = new Ship(type_, x_, y_);
					temp = "Ship "; 
					temp += _itoa(id, chartemp, 10);
					ship->SetName(temp.c_str());
					ship->setID( id );
					ships_.push_back(ship);
				}

			}
			break;

		case ID_LOSTSHIP:
			{
				unsigned int shipid;
				bs.Read(shipid);
				for (ShipList::iterator itr = ships_.begin(); itr != ships_.end(); ++itr)
				{
					if ((*itr)->GetID() == shipid)
					{
						delete *itr;
						ships_.erase(itr);
						break;
					}
				}
			}
			break;

		case ID_INITIALPOS:
			break;

		case ID_MOVEMENT:
			{
				ShipPacket theShip;
				char data[sizeof(ShipPacket)];
				bs.Read(data);
				//theShip.Deserialize(data);

				float x = theShip.ServerX;  float y = theShip.ServerY; float w = theShip.ServerW;
				unsigned int shipid = theShip.ShipID;
				float VelX = theShip.ServerVelX; float VelY = theShip.ServerVelY; float AngVel = theShip.AngularVelocity;

				for (ShipList::iterator itr = ships_.begin(); itr != ships_.end(); ++itr)
				{
					if ((*itr)->GetID() == shipid)
					{
						// this portion needs to be changed for it to work
#ifdef INTERPOLATEMOVEMENT

						(*itr)->SetServerLocation( x, y, w ); 

						(*itr)->SetServerVelocityX(VelX);
						(*itr)->SetServerVelocityY(VelY);
						(*itr)->SetAngularVelocity(AngVel);

						(*itr)->DoInterpolateUpdate();
#else
						bs.Read(x);
						bs.Read(y);
						bs.Read(w);
						(*itr)->setLocation( x, y, w ); 

						// Lab 7 Task 1 : Read Extrapolation Data velocity x, velocity y & angular velocity
						bs.Read(temp);
						(*itr)->SetVelocityX( temp );
						bs.Read(temp);
						(*itr)->SetVelocityY( temp );
						bs.Read(temp);
						(*itr)->SetAngularVelocity( temp );
#endif

						break;
					}
				}
			}
			break;

		case ID_COLLIDE:
			{
				unsigned int shipid;
				float x, y;
				bs.Read(shipid);
				
				if( shipid == ships_.at(0)->GetID() )
				{
					std::cout << "collided with someone!" << std::endl;
					bs.Read(x);
					bs.Read(y);
					ships_.at(0)->SetX( x );
					ships_.at(0)->SetY( y );
					bs.Read(x);
					bs.Read(y);
					ships_.at(0)->SetVelocityX( x );
					ships_.at(0)->SetVelocityY( y );
#ifdef INTERPOLATEMOVEMENT
					bs.Read(x);
					bs.Read(y);
					ships_.at(0)->SetServerVelocityX( x );
					ships_.at(0)->SetServerVelocityY( y );
#endif	
				}
			}
			break;


		// Lab 10 Task 10 : new cases to handle missile on application side
		
			case ID_NEWMISSILE:
	{
		float x, y, w;
		int id;

		bs.Read(id);
		bs.Read(x);
		bs.Read(y);
		bs.Read(w);

		missiles_.push_back( new Missile( "missile.png", x, y, w, id ) );
	}
	break;
	case ID_UPDATEMISSILE:
	{
		float x,y,w;
		int id;
		char deleted;

		bs.Read(id);
		bs.Read(deleted);

		for (MissileList::iterator itr = missiles_.begin(); itr != missiles_.end(); ++itr)
		{
			if( (*itr)->GetOwnerID() == id )
			{
				if( deleted == 1)
				{
					delete *itr;
					missiles_.erase(itr);
				}
				else
				{
					bs.Read( x );
					bs.Read( y );
					bs.Read( w );
					(*itr)->UpdateLoc( x, y, w );
					bs.Read( x );
					(*itr)->SetVelocityX(x);
					bs.Read(y);
					(*itr)->SetVelocityY(y);
				}
				break;
			}
		}
	}

	case ID_REJECT:
	{
		ExitGameMessage = true;
	}
	break;
	case ID_MESSAGE:
	{
		char Message[50];
		bs.Read(Message);
		theChat->ReceiveString(Message);
	}
	break;

	default:
		std::cout << "Unhandled Message Identifier: " << (int)msgid << std::endl;

		}
		rakpeer_->DeallocatePacket(packet);
	}

	return false;
}

void Application::Send()
{
	if (RakNet::GetTime() - timer_ > 100)
	{
		timer_ = (unsigned int)RakNet::GetTime();
		unsigned char msgid = ID_MOVEMENT;
		
#ifdef INTERPOLATEMOVEMENT

		ShipPacket theShip;
		theShip.theID = ID_MOVEMENT;
		theShip.ShipID = ships_.at(0)->GetID();
		theShip.ServerX = ships_.at(0)->GetServerX();
		theShip.ServerY = ships_.at(0)->GetServerY();
		theShip.ServerW = ships_.at(0)->GetServerW();
		theShip.ServerVelX = ships_.at(0)->GetServerVelocityX();
		theShip.ServerVelY = ships_.at(0)->GetServerVelocityY();
		theShip.AngularVelocity = ships_.at(0)->GetAngularVelocity();

		char data[sizeof(ShipPacket)];
		theShip.Serialize(data);


		//bs2.Write(ships_.at(0)->GetID());
		//bs2.Write(ships_.at(0)->GetServerX());
		//bs2.Write(ships_.at(0)->GetServerY());
		//bs2.Write(ships_.at(0)->GetServerW());
		//bs2.Write(ships_.at(0)->GetServerVelocityX());
		//bs2.Write(ships_.at(0)->GetServerVelocityY());
		//bs2.Write(ships_.at(0)->GetAngularVelocity());

#else
		bs2.Write(ships_.at(0)->GetID());
		bs2.Write(ships_.at(0)->GetX());
		bs2.Write(ships_.at(0)->GetY());
		bs2.Write(ships_.at(0)->GetW());
		// Lab 7 Task 1 : Add Extrapolation Data velocity x, velocity y & angular velocity
		bs2.Write(ships_.at(0)->GetVelocityX());
		bs2.Write(ships_.at(0)->GetVelocityY());
		bs2.Write(ships_.at(0)->GetAngularVelocity());
#endif

		rakpeer_->Send((char *)data, strlen(data) + 1, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);


		// Lab 10 Task 11 : send missile update 
		if( mymissile )
		{
			MissilePacket theMissile;
			theMissile.theID = ID_UPDATEMISSILE;
			char deleted = 0;

			theMissile.OwnerID = mymissile->GetOwnerID();
			theMissile.deleted = deleted;
			theMissile.X = mymissile->GetX();
			theMissile.Y = mymissile->GetY();
			theMissile.W = mymissile->GetW();
			theMissile.VelX = mymissile->GetVelocityX();
			theMissile.VelY = mymissile->GetVelocityY();
			
			char Message[sizeof(MissilePacket)];
			theMissile.Serialize(Message);
			/*	bs3.Write(mymissile->GetOwnerID());
			bs3.Write(deleted);
			bs3.Write(mymissile->GetX());
			bs3.Write(mymissile->GetY());
			bs3.Write(mymissile->GetW());
			bs3.Write(mymissile->GetVelocityX());
			bs3.Write(mymissile->GetVelocityY());*/

			rakpeer_->Send(Message, strlen(Message) + 1, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
		}

		std::string Message = theChat->StringtoSend();
		if (Message.size() > 0)
		{
			char ID [100];
			itoa(ships_[0]->GetID(), ID, 10);
			std::string theID(ID);
			Message = "Ship " + theID + " : " + Message;

			MessagePacket thePacket;
			thePacket.theID = ID_MESSAGE;

			char data[sizeof(MessagePacket)];

			strncpy(thePacket.Message, (char *)Message.c_str(), sizeof(thePacket.Message));
			thePacket.Message[sizeof(thePacket.Message) - 1] = '\0';
			thePacket.Serialize(data);

			rakpeer_->Send(data, strlen(data) + 1, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
		}
	}
}

bool Application::Keyboard(float timedelta)
{

	if (hge_->Input_GetKeyState(HGEK_ESCAPE))
		return true;

	if (!theChat->ChatTime)
	{
		ships_.at(0)->SetAngularVelocity(0.0f);

		if (hge_->Input_GetKeyState(HGEK_LEFT))
		{
			ships_.at(0)->Accelerate(DEFAULT_ACCELERATION, timedelta);
		}

		if (hge_->Input_GetKeyState(HGEK_RIGHT))
		{
			ships_.at(0)->Accelerate(-DEFAULT_ACCELERATION, timedelta);
		}

		if (hge_->Input_GetKeyState(HGEK_UP))
		{
			//ships_.at(0)->Accelerate(DEFAULT_ACCELERATION, timedelta);
		}

		if (hge_->Input_GetKeyState(HGEK_DOWN))
		{
			//ships_.at(0)->Accelerate(-DEFAULT_ACCELERATION, timedelta);
		}

		// Lab 10 Task 4 : Add a key to shoot missiles
		if (hge_->Input_GetKeyState(HGEK_SPACE))
		{
			if (!keydown_space)
			{
				CreateMissile(ships_.at(0)->GetX(), ships_.at(0)->GetY(), ships_.at(0)->GetW(), ships_.at(0)->GetID());
				keydown_space = true;
			}
		}
		else
		{
			if (keydown_space)
			{
				keydown_space = false;
			}
		}

		if (hge_->Input_GetKeyState(HGEK_ENTER))
		{
			if (!keydown_enter)
			{
				std::cout << "ITS CHAT TIME FOOLS " << std::endl;
				theChat->ChatTime = true;
				keydown_enter = true;
			}
		}
		else
		{
			if (keydown_enter)
			{
				keydown_enter = false;
			}
		}
	}
	else
	{
		theChat->KeyboardInput(keydown_enter);
	}
	return false;
}