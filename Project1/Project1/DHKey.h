#pragma once
#include <cstdlib>
#include <cmath>
#include <time.h> 
class DHKey
{
	unsigned int SecretInt;
	unsigned int Prime;
	unsigned int Base;

	unsigned int Key;
public:
	DHKey();
	~DHKey();

	unsigned int GetKey();
	void ChooseSecretInt();
	unsigned int GetFirstExchange();
	unsigned int Exchange(unsigned int);

	void ComputeKey(unsigned int);
};

