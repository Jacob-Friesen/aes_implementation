#ifndef AES_DECRYPT
#define AES_DECRYPT
using namespace std;

// Takes ciphertext in the for of a 4x4 byte array and generates the plaintext according to the aes decryption algorithm
unsigned char **aes_decrypt(unsigned char **state, uint32_t *w, unsigned char **inv_s_box);

#endif