#include "DHKey.h"


DHKey::DHKey()
{
	srand(time(NULL));
	////This prime is supposed to be randomly generated, and the Base also
	Prime = 23;
	Base = 4;
	/////////////////////////////////////////////////////////////
	ChooseSecretInt();
}


DHKey::~DHKey()
{
}

void DHKey::ChooseSecretInt()
{
	SecretInt = rand() % 20;
}

unsigned int DHKey::GetFirstExchange()
{
	unsigned int Temp = pow((float)Base, (float)SecretInt);
	return Temp % Prime;
}

unsigned int DHKey::GetKey()
{
	return Key;
}

void DHKey::ComputeKey(unsigned int temp)
{
	unsigned int temp2 = pow((float)temp, (float)SecretInt);
	Key = temp2 % Prime;
}