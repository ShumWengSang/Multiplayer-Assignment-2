#include <string>
struct CustomPacket
{
	unsigned int packet_type;
	std::string Message;
	char theRealMessage[100];

	char * Convert(std::string value)
	{
		return (char *)value.c_str();
	}

	char * Convert(int value)
	{
		char buffer[100];
		sprintf_s(buffer,100,"%i",value);
		return buffer;
	}

	char * Convert(float value)
	{
		char buffer[100];
		sprintf_s(buffer,100,"%f",value);
		return buffer;
	}

	char * Convert(char value)
	{
		return &value;
	}

	void Serialization(char * data)
	{
		memcpy(data, theRealMessage, 104);
	}
	void Deserialization(char * data)
	{
		memcpy(theRealMessage, data, 104);
	}

};