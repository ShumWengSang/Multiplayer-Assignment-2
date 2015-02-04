#include <iostream>
#include <vector>
#include <string>
#include <WinSock2.h>
#include "DHKey.h"
#include "CRC4.h"
/////////////////////////////////
///////////FORMAT OF CUSTOM PACKET
/////////////////////////////////
//
//-------------------------------------------------
//NumOfContents||||||||data||||||||data...
//-------------------------------------------------
//
///////////////////////////////////

int swap_uint32(int val)
{
	val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
	return (val << 16) | (val >> 16);
}

struct CustomPacket
{
	std::vector<std::string> Message;
	char theRealMessage[200];
	unsigned int NumOfContents;
	std::string BufferMessage;
	char * NextTarget;

	CustomPacket()
	{
		NumOfContents = 0;
		NextTarget = NULL;
	}

	std::string Convert(std::string value)
	{
		value = "S" + value;
		return value;
	}

	std::string Convert(int value)
	{
		char buffer[100];
		sprintf_s(buffer,100,"I%i",value);
		return std::string(buffer);
	}

	std::string Convert(float value)
	{
		char buffer[100];
		sprintf_s(buffer,100,"F%f",value);
		return std::string(buffer);
	}

	void Add(std::string Adder)
	{
		NumOfContents++;
		Message.push_back(Adder);
	}

	void Ready()
	{
		unsigned int * howMany;
		howMany = new unsigned int[Message.size()];
		//0 for NULL, 1 for int, 2 for float, 3 for string
		unsigned int * Type;
		Type = new unsigned int[Message.size()];

		char NumContents[4];
		char CType[4];
		char CSize[4];



		for (int i = 0; i < Message.size(); i++)
		{
			howMany[i] = Message[i].size();
		}

		itoa(NumOfContents, NumContents, 10);
		BufferMessage = NumContents;
		for (int i = 0; i < Message.size(); i++)
		{
			itoa((howMany[i]), CSize, 10);
			std::string SHowMany(CSize);

			BufferMessage = BufferMessage /*+ "~" + CSize*/ + "~" + Message[i];
		}
	}

	void DeReady()
	{
		char * Buffer = NULL;
		if (theRealMessage != NULL)
		{
			Buffer = strtok_s(theRealMessage, "~", &NextTarget);
		}
	}


	bool Get(int & value)
	{
		char * Buffer = NULL;
		if (NextTarget != NULL)
		{
			Buffer = strtok_s(NextTarget, "~", &NextTarget);
			if (Buffer[0] == 'I')
			{
				value = atoi(++Buffer);
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	bool Get(float & value)
	{
		char * Buffer = NULL;
		if (NextTarget != NULL)
		{
			Buffer = strtok_s(NextTarget, "~", &NextTarget);
			if (Buffer[0] == 'F')
			{
				value = atof(++Buffer);
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	bool Get(std::string & value)
	{
		char * Buffer = NULL;
		if (NextTarget != NULL)
		{
			Buffer = strtok_s(NextTarget, "~", &NextTarget);
			if (Buffer[0] == 'S')
			{
				std::string SBuffer(++Buffer);
				value = SBuffer;
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	void EndianSwapHtoNL(char * data, int size)
	{
		for (int i = 0; i < size; i++)
		{
			//data[i] = htonl(data[i]);
		}
	}

	void EndianSwapNtoHL(char * data, int size)
	{
		for (int i = 0; i < size; i++)
		{
			//data [i] = ntohl(data[i]);
		}
	}
	void Serialization()
	{
		Ready();
		memcpy(theRealMessage, BufferMessage.c_str(), /*sizeof(BufferMessage)*/200);
		EndianSwapHtoNL(theRealMessage, 200);
	}
	void Deserialization(char * data)
	{
		memcpy(theRealMessage, data, 200);
	}

	void UnPack()
	{
		DeReady();
		EndianSwapHtoNL(theRealMessage, 200);
	}


};

void main ()
{
	//CustomPacket thePacket;
	//int i = 10;

	//CRC4 RC4;

	//std::string  yolo = thePacket.Convert(i);
	//std::string yolo2 = thePacket.Convert(20.f);
	//std::string yolo3 = thePacket.Convert(5.f);
	//std::string yolo4 = thePacket.Convert(std::string("hello"));
	//thePacket.Add(yolo);
	//thePacket.Add(yolo2);
	//thePacket.Add(yolo3);
	//thePacket.Add(yolo4);
	//thePacket.Serialization();

	//std::cout << thePacket.theRealMessage << std::endl;

	//RC4.Encrypt(thePacket.theRealMessage, "key");

	//std::cout << thePacket.theRealMessage << std::endl;

	//CustomPacket theSecondPacket;
	//theSecondPacket.Deserialization(thePacket.theRealMessage);
	////std::cout << theSecondPacket.theRealMessage << theSecondPacket.NextTarget << std::endl;

	//RC4.Decrypt(theSecondPacket.theRealMessage, "key");

	//theSecondPacket.UnPack();
	//std::cout << theSecondPacket.theRealMessage << theSecondPacket.NextTarget << std::endl;
	//int a;
	//float b, c;
	//std::string testing;
	//theSecondPacket.Get(a);
	//theSecondPacket.Get(b);
	//theSecondPacket.Get(c);
	//theSecondPacket.Get(testing);
	//std::cout << a <<" " << b << " "<< c << " " << testing << std::endl;
	//int q = sizeof(char);

	DHKey first;
	DHKey second;

	int i = first.GetFirstExchange();
	int k = second.GetFirstExchange();

	first.ComputeKey(k);
	second.ComputeKey(i);

	std::cout << "First: " << first.GetKey() << " : " << "Second: " << second.GetKey();

	system("pause");
}