#include "ServerApp.h"
#include "RakPeerInterface.h"
#include "Bitstream.h"
#include "GetTime.h"
#include "../MyMsgIDs.h"
#include <iostream>
#include "../SecondCustomPacket.h"

ServerApp::ServerApp() : 
	rakpeer_(RakPeerInterface::GetInstance()),
	newID(0)
{
	rakpeer_->Startup(100, &SocketDescriptor(1691, 0), 1);
	rakpeer_->SetMaximumIncomingConnections(100);
	rakpeer_->SetOccasionalPing(true);
	std::cout << "Server Started" << std::endl;
	NumofPlayers = 0;
}

ServerApp::~ServerApp()
{
	rakpeer_->Shutdown(100);
	RakPeerInterface::DestroyInstance(rakpeer_);
}

void ServerApp::Loop()
{
	if (Packet* packet = rakpeer_->Receive())
	{
		RakNet::BitStream bs(packet->data, packet->length, false);

		if(packet->length == sizeof(ShipPacket))
		{
			ShipPacket thePacket;
			thePacket.Deserialize(packet->data);

			switch (thePacket.theID)
			{
				case ID_MOVEMENT:
					{
						float x = thePacket.ServerX;  float y = thePacket.ServerY;
						unsigned int shipid = thePacket.ShipID;
						UpdatePosition(packet->systemAddress, x, y);
						char data[sizeof(ShipPacket)];
						thePacket.Serialize(data);
						rakpeer_->Send(data, sizeof(ShipPacket), HIGH_PRIORITY, RELIABLE, 0, packet->systemAddress, true);
					}
					break;
				case ID_INITIALPOS:
					{
						float x, y;
						int type;
						std::cout << "ProcessInitialPosition" << std::endl;
						x = thePacket.ServerX;
						y = thePacket.ServerY;
						type = thePacket.ShipType;
						ProcessInitialPosition(packet->systemAddress, x, y, type);
					}
					break;
				case ID_COLLIDE:
					{
						char data[sizeof(ShipPacket)];
						thePacket.Serialize(data);
						rakpeer_->Send(data, sizeof(ShipPacket), HIGH_PRIORITY, RELIABLE, 0, packet->systemAddress, true);
					}
					break;
				default:
					break;
			}
		}
		else if (packet->length == sizeof(MissilePacket))
		{
			MissilePacket thePacket;
			thePacket.Deserialize(packet->data);

			switch (thePacket.theID)
			{
				case ID_NEWMISSILE:
					{
						char data[sizeof(MissilePacket)];
						thePacket.Serialize(data);
						rakpeer_->Send(data, sizeof(MissilePacket), HIGH_PRIORITY, RELIABLE, 0, packet->systemAddress, true);
					}
					break;
				case ID_UPDATEMISSILE:
					{
						char data[sizeof(MissilePacket)];
						thePacket.Serialize(data);
						rakpeer_->Send(data, sizeof(MissilePacket), HIGH_PRIORITY, RELIABLE, 0, packet->systemAddress, true);
					}
					break;
				default:
					break;
			}

		}
		unsigned char msgid = 0;
		RakNet::Time timestamp = 0;

		bs.Read(msgid);

		if (msgid == ID_TIMESTAMP)
		{
			bs.Read(timestamp);
			bs.Read(msgid);
		}

		switch (msgid)
		{
		case ID_NEW_INCOMING_CONNECTION:
			if (NumofPlayers < 2)
				SendWelcomePackage(packet->systemAddress);
			else
				SendRejectPackage(packet->systemAddress);
			break;

		case ID_DISCONNECTION_NOTIFICATION:
		case ID_CONNECTION_LOST:
			SendDisconnectionNotification(packet->systemAddress);
			break;

		case ID_MESSAGE:
		{
			bs.ResetReadPointer();
			rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, packet->systemAddress, true);
		}
		break;

		default:
			std::cout << "Unhandled Message Identifier: " << (int)msgid << std::endl;
		}

		rakpeer_->DeallocatePacket(packet);
	}
}

void ServerApp::SendWelcomePackage(SystemAddress& addr)
{
	++newID;
	NumofPlayers++;
	unsigned int shipcount = static_cast<unsigned int>(clients_.size());
	unsigned char msgid = ID_WELCOME;
	
	RakNet::BitStream bs;
	bs.Write(msgid);
	bs.Write(newID);
	bs.Write(shipcount);

	for (ClientMap::iterator itr = clients_.begin(); itr != clients_.end(); ++itr)
	{
		std::cout << "Ship " << itr->second.id << " pos" << itr->second.x_ << " " << itr->second.y_ << std::endl;
		bs.Write( itr->second.id );
		bs.Write( itr->second.x_ );
		bs.Write( itr->second.y_ );
		bs.Write( itr->second.type_ );
	}

	rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED,0, addr, false);

	bs.Reset();

	GameObject newobject(newID);

	clients_.insert(std::make_pair(addr, newobject));

	std::cout << "New guy, assigned id " << newID << std::endl;
	std::cout << "PLAYERS: " << NumofPlayers << std::endl;
}

void ServerApp::SendDisconnectionNotification(SystemAddress& addr)
{

	ClientMap::iterator itr = clients_.find(addr);
	if (itr == clients_.end())
		return;

	unsigned char msgid = ID_LOSTSHIP;
	ShipPacket thePacket;
	thePacket.theID = ID_LOSTSHIP;
	thePacket.ShipID = itr->second.id;
	char data[sizeof(ShipPacket)];
	thePacket.Serialize(data);

	rakpeer_->Send(data, sizeof(ShipPacket), HIGH_PRIORITY, RELIABLE_ORDERED, 0, addr, true);

	std::cout << itr->second.id << " has left the game" << std::endl;
	NumofPlayers--;
	clients_.erase(itr);
	std::cout << "PLAYERS: " << NumofPlayers << std::endl;

}

void ServerApp::ProcessInitialPosition( SystemAddress& addr, float x_, float y_, int type_)
{
	unsigned char msgid;
	RakNet::BitStream bs;
	ClientMap::iterator itr = clients_.find(addr);
	if (itr == clients_.end())
		return;

	itr->second.x_ = x_;
	itr->second.y_ = y_;
	itr->second.type_ = type_;
	
	std::cout << "Received pos" << itr->second.x_ << " " << itr->second.y_ << std::endl;
	std::cout << "Received type" << itr->second.type_ << std::endl;

	ShipPacket newShip;
	newShip.theID = ID_NEWSHIP;
	newShip.ShipID = itr->second.id;
	newShip.ServerX = itr->second.x_;
	newShip.ServerY = itr->second.y_;
	newShip.ShipType = itr->second.type_;
	char data[sizeof(ShipPacket)];
	newShip.Serialize(data);
	rakpeer_->Send(data, sizeof(ShipPacket), HIGH_PRIORITY, RELIABLE_ORDERED, 0, addr, true);
}

void ServerApp::UpdatePosition( SystemAddress& addr, float x_, float y_ )
{
	ClientMap::iterator itr = clients_.find(addr);
	if (itr == clients_.end())
		return;

	itr->second.x_ = x_;
	itr->second.y_ = y_;
}

void ServerApp::SendRejectPackage(SystemAddress& addr)
{
	//unsigned int shipcount = static_cast<unsigned int>(clients_.size());
	unsigned char msgid = ID_REJECT;

	RakNet::BitStream bs;
	bs.Write(msgid);

	rakpeer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, addr, false);

	bs.Reset();

	std::cout << "Connection Rejected" << newID << std::endl;

	std::cout << "PLAYERS: " << NumofPlayers << std::endl;
}