// Base64test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "Base64_RC4.h"
void main()
{
	char str[64] = "This is a test for RC4 cypher";
/* Test rc4 encoding and decoding here */
	CRC4 rc4;
	printf("Demo for RC4 \n\n");
	printf("Plain text: %s \n",str);
	rc4.Encrypt(str,"Key");
	printf("Encoded string: %s \n",str);
	rc4.Decrypt(str,"Key");
	printf("Decoded string: %s \n",str);
/* Test Base64  encoding and decoding here */
	strcpy(str, "This is a test for Base64 cypher");
	CBase64  base64;
	char *dst;
	dst = (char*)malloc( base64.B64_length(strlen(str))+1);
	if(dst == NULL)
		return;
	printf("\n\nDemo for Base64 \n\n");
	printf("Plain text: %s \n",str);
	base64.Encrypt(str,strlen(str),dst);
	printf("Encoded string: %s\n",dst);
	memset(str,0,sizeof(str));
	base64.Decrypt(dst,strlen(dst),str);
	printf("Decoded string: %s\n",str);
	free(dst);
	getchar();
}

