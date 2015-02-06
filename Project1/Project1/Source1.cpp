

/*************************************************************
Ref::http://en.wikipedia.org/wiki/Diffie%E2%80%93Hellman_key_exchange
***********************************************************/
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <stdint.h>
using namespace std;
#include "DiffieHelmanKey.h"


int main()
{
	DiffieHelmanKey Alice;
	DiffieHelmanKey Bob;
	DiffieHelmanKey Charlie;

	int AlicePubKey = Alice.CreatePublicKey(Alice.p_number, Alice.Base, Alice.random);
	int PubA = Bob.CreatePublicKey(Bob.p_number, AlicePubKey, Bob.random);
	int KeyC = Charlie.SharedSecretKey(Charlie.p_number, Charlie.random, PubA);

	int CharliePubKay = Charlie.CreatePublicKey(Charlie.p_number, Charlie.Base, Charlie.random);
	int PubC = Alice.CreatePublicKey(Alice.p_number, CharliePubKay, Alice.random);
	int KeyA = Bob.SharedSecretKey(Bob.p_number, Bob.random, PubC);

	int BobPubKey = Bob.CreatePublicKey(Bob.p_number, Bob.Base, Bob.random);
	int PubB = Charlie.CreatePublicKey(Charlie.p_number, BobPubKey, Charlie.random);
	int KeyB = Alice.SharedSecretKey(Alice.p_number, Alice.random, PubB);


	cout << "Pub Key Sent around A is : " << PubA << " B is: " << PubB << " : C is " << PubC << endl;
	cout << "Secret Key from A is: " << KeyA << " : B is " << KeyB <<  " : C is " <<  KeyC << endl;
	system("pause");
}