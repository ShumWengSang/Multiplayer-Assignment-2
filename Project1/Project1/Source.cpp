#include <iostream>
#include <vector>
enum PacketType
{
	MISSLE,
	SHIP,
	CHATMESSAGE
};

struct CustomPacket
{
	unsigned int packet_type;
	std::vector<std::string> Message;
	char theRealMessage[100];
	unsigned int NumOfContents;

	CustomPacket()
	{
		NumOfContents = 0;
	}

	std::string Convert(int value)
	{
		char buffer[100];
		sprintf_s(buffer,100,"%i",value);
		return std::string(buffer);
	}

	std::string Convert(float value)
	{
		char buffer[100];
		sprintf_s(buffer,100,"%f",value);
		return std::string(buffer);
	}

	void Add(std::string Adder)
	{
		NumOfContents++;
		Message.push_back(Adder);
	}

	void Ready()
	{
		int howMany;
		int Type;
	}
	void Serialization()
	{
		memcpy(data, theRealMessage, 104);
	}
	void Deserialization()
	{
		memcpy(theRealMessage, data, 104);
	}

};

void main ()
{
	CustomPacket thePacket;
	int i = 10;

	std::string  yolo = thePacket.Convert(i);
	std::string yolo2 = thePacket.Convert(20.f);

}