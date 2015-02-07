#include "DiffieHelmanKey.h"

int64_t DiffieHelmanKey::is_prime(int64_t iVal)
{
	char bPrime;
	bPrime = true;
	for (int i = 2; i < iVal / 2; i++)
	{
		if ((iVal % i) == 0)
		{
			bPrime = false;
			break;
		}
	}
	return bPrime;
}

int64_t DiffieHelmanKey::get_prime_number(int64_t MAX)
{
	int64_t rPrime;
	char result;
	srand(time(NULL));
	while (1)
	{
		rPrime = rand() % MAX + 1;
		if (rPrime % 2 == 0)
		{
			//Add 1 to check if it is prime, since it is even number.
			//Try to save memory.
			rPrime = rPrime + 1;

			result = is_prime(rPrime);
			if (result == true)
			{
				break;
			}
		}
		else
		{
			result = is_prime(rPrime);
			if (result == true){
				break;
			}
		}
	}
	return rPrime;
}


DiffieHelmanKey::DiffieHelmanKey()
{
	//int p_number = get_prime_number(99999999);
	//Prime number should be randomed, but would take too much computation.
	p_number = 1039;
	Base = 5; // public[base number should set to 2 or 5]
	random = rand() % 20;
	std::cout << "Rand: " << random << std::endl;
}


DiffieHelmanKey::~DiffieHelmanKey()
{
}

int DiffieHelmanKey::CreatePublicKey(int pn, int gn, int s_key)
{
	//WE ARE NOT USING POWS AND ABS BECAUSE THEY GIVE US ROUNDED NUMBERS (SOMETIMES)
	//WEIRD ERROR STUFF
	int i;
	int pub_key = 1;
	for (i = 0; i < s_key; i++)
	{
		pub_key = (pub_key * gn) % pn;
	}
	return pub_key;
}
int DiffieHelmanKey::SharedSecretKey(int pn, int s_key, int pub_key)
{
	int i;
	int shared_secret_key = 1;
	for (i = 0; i < s_key; i++)
	{
		shared_secret_key = (shared_secret_key * pub_key) % pn;
	}
	SecretKey = shared_secret_key;
	return shared_secret_key;
}
int DiffieHelmanKey::GetSecretKey()
{
	return SecretKey;
}

char * DiffieHelmanKey::GetSecretKeyC()
{
	char c[(sizeof(int)*CHAR_BIT - 1) / 3 + 3];
	sprintf_s(c, "%i", SecretKey);
	return c;
}