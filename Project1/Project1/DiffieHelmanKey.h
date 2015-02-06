#pragma once
#include <stdint.h>
#include <math.h>
#include <cstdlib>
#include <ctime>
class DiffieHelmanKey
{
	int SecretKey;
public:
	DiffieHelmanKey();
	~DiffieHelmanKey();

	int Base;
	int p_number;
	int random;

	int CreatePublicKey(int pn, int gn, int s_key);
	int SharedSecretKey(int pn, int s_key, int pub_key);
	int GetSecretKey();
};

