enum MessageType
{
	MISSILE,
	SHIP,
	MESSAGE
};

struct SecondCustomPacket
{
	MessageType theType;
	char Bits[500];
	virtual void Serialize() = 0;
	virtual void Deserialize() = 0;
	virtual void EndianSwapToNetwork() = 0;
	virtual void EndianSwaptoHost() = 0;
};

struct MissilePacket : public SecondCustomPacket
{
	MissilePacket()
	{
		theType = MISSILE;
	}

	int OwnerID;
	char deleted;
	float X;
	float Y;
	float W;
	float VelX;
	float VelY;

	void Serialize()
	{
		EndianSwapToNetwork(OwnerID);
		EndianSwapToNetwork(X);
		EndianSwapToNetwork(Y);
		EndianSwapToNetwork(W);
		EndianSwapToNetwork(VelX);
		EndianSwapToNetwork(VelY);

	}

	void EndianSwapToNetwork(int &value)
	{
		value = htonl(value);
	}

	void EndianSwapToNetwork(float &value)
	{
		value = htonl(value);
	}
};

struct ShipPacket : public SecondCustomPacket
{
	ShipPacket()
	{
		theType = SHIP;
	}
	unsigned int ShipID;
	float ServerX;
	float ServerY;
	float ServerW;

	float ServerVelX;
	float ServerVelY;
};

struct MessagePacket : public SecondCustomPacket
{
	MessagePacket()
	{
		theType = MESSAGE;
	}
	char Message[100];
};