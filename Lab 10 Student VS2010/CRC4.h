//Sources: http://en.wikipedia.org/wiki/RC4
////www.codeproject.com
#pragma once
#include <array>
#include <cstring>

//Use XOR to swap them around.


class CRC4 
{
private:
    unsigned char sbox[256];      /* the S box*/
    unsigned char key[256],k;     /* key values*/
	int  m, n, i, j, ilen;        
public:

	CRC4()
	{
		for (int i = 0; i < 256; i++)
		{
			sbox[i] = 0;
			key[i] = 0;
		}
	}
	~CRC4()
	{
		for (int i = 0; i < 256; i++)
		{
			sbox[i] = 0;
			key[i] = 0;
		}
	}

	void KeyScheduleAlg(char *pszText, const char *pszKey)
	{

		ilen = (int)strlen(pszKey);

		for (m = 0; m < 256; m++) 
		{
			sbox[m] = m;
		}

		for (m = 0; m < 256; m++)
		{
			n = (n + sbox[m] + key[m % ilen]) & 0xff;
			Swap(sbox[m], sbox[n]);
		}
	}


	char *Encrypt(char *pszText, const char *pszKey)
	{
		i = 0, j = 0, n = 0;
		KeyScheduleAlg(pszText, pszKey);

		ilen = (int)strlen(pszText);
		for (m = 0; m < ilen; m++)
		{
			i = (i + 1) & 0xff;
			j = (j + sbox[i]) & 0xff;
			Swap(sbox[i], sbox[j]);  /* randomly Initialize the key sequence */
			k = sbox[((sbox[i] + sbox[j]) & 0xff)];
			if (k == pszText[m])       /* avoid '\0' beween the decoded text; */
				k = 0;
			//THIS IS WHERE THE MAGIC IS DONE
			pszText[m] ^= k;
			//THIS IS WHERE THE MAGIC IS DONE
		}
		return pszText;
	}

	char *Decrypt(char *pszText, const char *pszKey)
	{
		return Encrypt(pszText, pszKey);  /* using the same function as encoding */
	}

	void Swap(unsigned char & a, unsigned char & b)
	{
		(a) ^= (b);
		(b) ^= (a);
		(a) ^= (b);
	}
};