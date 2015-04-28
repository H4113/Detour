#include <iostream>
#include <openssl/des.h>

#include "general.h"
#include "pathfinder.h"
#include "network.h"

/* Triple DES key for Encryption and Decryption */
DES_cblock Key1 = { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11 };
DES_cblock Key2 = { 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22 };
DES_cblock Key3 = { 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33 };
DES_key_schedule SchKey1,SchKey2,SchKey3;

void print_data(const char *tittle, const void* data, int len)
{
	printf("%s : ",tittle);
	const unsigned char * p = (const unsigned char*)data;
	int i = 0;
	
	for (; i<len;++i)
		printf("%02X ", *p++);
	
	printf("\n");
}

void test_des()
{
	/* Input data to encrypt */
	unsigned char input_data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
	
	/* Init vector */
	DES_cblock iv = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	DES_set_odd_parity(&iv);
	
	/* Check for Weak key generation */
	if ( -2 == (DES_set_key_checked(&Key1, &SchKey1) || DES_set_key_checked(&Key2, &SchKey2) || DES_set_key_checked(&Key3, &SchKey3)))
	{
		printf(" Weak key ....\n");
		return 1;
	}
	
	/* Buffers for Encryption and Decryption */
	unsigned char* cipher[sizeof(input_data)];
	unsigned char* text[sizeof(input_data)];
	
	/* Triple-DES CBC Encryption */
	DES_ede3_cbc_encrypt( (unsigned char*)input_data, (unsigned char*)cipher, sizeof(input_data), &SchKey1, &SchKey2, &SchKey3,&iv, DES_ENCRYPT);
	
	/* Triple-DES CBC Decryption */
	memset(iv,0,sizeof(DES_cblock)); // You need to start with the same iv value
	DES_set_odd_parity(&iv);
	DES_ede3_cbc_encrypt( (unsigned char*)cipher, (unsigned char*)text, sizeof(input_data), &SchKey1, &SchKey2, &SchKey3,&iv,DES_DECRYPT);

	/* Printing and Verifying */
	print_data("\n Original ",input_data,sizeof(input_data));
	print_data("\n Encrypted",cipher,sizeof(input_data));
	print_data("\n Decrypted",text,sizeof(input_data));
}

int main(void)
{
	std::cout << "NetworkProject server" << std::endl;

	//startServer();
	//TestPathfinder();

	return 0;
}

