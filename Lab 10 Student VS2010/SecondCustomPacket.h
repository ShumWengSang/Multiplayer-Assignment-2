#include "../Lab 10 Student VS2010/MyMsgIDs.h"
struct SecondCustomPacket
{
	MyMsgIDs theID;
	virtual void Serialize(char * data) = 0;
	virtual void Deserialize(char * data) = 0;

	void EndianSwapToNetwork(int &value)
	{
		value = htonl(value);
	}

	void EndianSwapToNetwork(float &value)
	{
		value = htonl(value);
	}

	void EndianSwapToHost(int &value)
	{
		value = ntohl(value);
	}

	void EndianSwapToHost(float &value)
	{
		value = ntohl(value);
	}
};

struct MissilePacket : public SecondCustomPacket
{
	int OwnerID;
	char deleted;
	float X;
	float Y;
	float W;
	float VelX;
	float VelY;

	void Serialize(char * data)
	{
		EndianSwapToNetwork(OwnerID);
		EndianSwapToNetwork(X);
		EndianSwapToNetwork(Y);
		EndianSwapToNetwork(W);
		EndianSwapToNetwork(VelX);
		EndianSwapToNetwork(VelY);

		memcpy(data, this, sizeof(MissilePacket));
	}

	void Deserialize(char * data)
	{
		memcpy(this, data, sizeof(MissilePacket));

		EndianSwapToHost(OwnerID);
		EndianSwapToHost(X);
		EndianSwapToHost(Y);
		EndianSwapToHost(W);
		EndianSwapToHost(VelX);
		EndianSwapToHost(VelY);
	}


};

struct ShipPacket : public SecondCustomPacket
{
	int ShipID;
	float ServerX;
	float ServerY;
	float ServerW;

	float ServerVelX;
	float ServerVelY;
	float AngularVelocity;

	void Serialize(char * data)
	{
		EndianSwapToNetwork(ShipID);
		EndianSwapToNetwork(ServerX);
		EndianSwapToNetwork(ServerY);
		EndianSwapToNetwork(ServerW);
		EndianSwapToNetwork(ServerVelX);
		EndianSwapToNetwork(ServerVelY);
		EndianSwapToNetwork(AngularVelocity);

		memcpy(data, this, sizeof(ShipPacket));
	}

	void Deserialize(char * data)
	{
		memcpy(this, data, sizeof(ShipPacket));

		EndianSwapToHost(ShipID);
		EndianSwapToHost(ServerX);
		EndianSwapToHost(ServerY);
		EndianSwapToHost(ServerW);
		EndianSwapToHost(ServerVelX);
		EndianSwapToHost(ServerVelY);
		EndianSwapToHost(AngularVelocity);
	}
};

struct MessagePacket : public SecondCustomPacket
{
	void Serialize(char * data)
	{
		memcpy(data, this, sizeof(MessagePacket));
	}
	void Deserialize(char * data)
	{
		memcpy(this, data, sizeof(MessagePacket));
	}

	char Message[100];
};