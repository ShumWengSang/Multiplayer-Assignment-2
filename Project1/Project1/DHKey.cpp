#include "DHKey.h"


DHKey::DHKey()
{
	srand(time(NULL));
	Prime = 23;
	Base = 4;
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
	unsigned int Temp = pow(Base, SecretInt);
	return Temp % Prime;
}

unsigned int DHKey::GetKey()
{
	return Key;
}

void DHKey::ComputeKey(unsigned int temp)
{
	unsigned int temp2 = pow(temp, SecretInt);
	Key = temp2 % Prime;
}