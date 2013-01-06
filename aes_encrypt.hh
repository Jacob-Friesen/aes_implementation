#ifndef AES_ENCRYPT
#define AES_ENCRYPT
using namespace std;
 
const int STATE_WIDTH = 4; // and height because its a square
 
// Runs the encryption algorithm returning the ciphertext
unsigned char **aes_encrypt(uint32_t *K, unsigned char **s_box, uint32_t *w, string plaintext_file);

// Prints the state column first on one line seperating each byte by 2 spaces and each row by 4. First prints
// whats in the string to_print, directly.
void state_print(unsigned char **state, string to_print);

// round key is added to state by using a bitwise XOR
unsigned char **AddRoundKey(unsigned char **state, uint32_t w[]);

// Substitutes all the states bytes using each hex as indexes in the s_box table
unsigned char **SubBytes(unsigned char **state, unsigned char **s_box);

// Take each byte of each column in the state and polynomial mulitply it by the corresponding value of the sent in A_X
unsigned char **MixColumns(unsigned char **state, const unsigned char a_x[][STATE_WIDTH]);
 
#endif