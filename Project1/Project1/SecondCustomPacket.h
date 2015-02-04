enum MessageType
{
	MISSILE,
	SHIP,
	MESSAGE
};

struct SecondCustomPacket
{
	MessageType theType;
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
};

struct ShipPacket : public SecondCustomPacket
{
	ShipPacket()
	{
		theType = SHIP;
	}
};

struct MessagePacket : public SecondCustomPacket
{
	MessagePacket()
	{
		theType = MESSAGE;
	}
};