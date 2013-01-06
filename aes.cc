//-----------------------------------------
// NAME: Jacob Friesen
// RUN EXAMPLE: g++ -Wall aes.cc aes_decrypt.cc aes_encrypt.cc key_schedule.cc file_util.cc -o aes && ./aes test1key.txt aes_sbox.txt aes_inv_sbox.txt test1plaintext.txt
// REMARKS:
// Takes a key, s_box, inverse s_box and plaintext. Uses the s_box and key to generate a key schedule which
// is printed then encrypts the ciphertext using the key schedule and s_box printing each of the encryption
// steps. Finally using the ciphertext generates the plaintext using the inverse s_box and key schedule
// printing each decryption step. The program has a non classed structure to it using a lot of c++ tools, as
// this style seems to match how the implementation of AES is described. Names and variables try to match the
// AES document when possible even when it violates naming standards such as the function MixColumns.
//-----------------------------------------

// reading in and standard stuff
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <cstring>
#include <strings.h>
#include <sstream>
#include <stdlib.h>

// 32 bit ints
#include <stdint.h>

// application headers
#include "constants.hh"
#include "file_util.hh"
#include "key_schedule.hh"
#include "aes_encrypt.hh"
#include "aes_decrypt.hh"

void do_abc(string key_file, string s_box_file, string inv_s_box_file, string plaintext_file){
    uint32_t *K = new uint32_t[Nk];
    unsigned char **s_box;
    unsigned char **state;
    uint32_t *w;
    
    // get key schedule information
    K = read_in_K(key_file, Nk);
    s_box = read_in_twod_arr(s_box_file, SCHEDULE_LENGTH);
    w = key_schedule(K, s_box, true);
    
	state = aes_encrypt(K, s_box, w, plaintext_file);
	
	// get inverse s_box, key schedule stays the same
    s_box = read_in_twod_arr(inv_s_box_file, SCHEDULE_LENGTH);
	
	state = aes_decrypt(state, w, s_box);
    
	// clean up
    for (int i = 0; i < STATE_WIDTH; i += 1)
        delete[] state[i];
    delete[] state;
	for (int i = 0; i < SCHEDULE_LENGTH; i += 1)
        delete[] s_box[i];
    delete[] s_box;
	delete []w;
	delete []K;
}

int main(int argc, char* argv[]){    
    cout << "\n";
    
    // Instruct on correct args if not given
	if (argc < 5){
		cout << "Usage: " << argv[0] << " <Key File> <S-Box File> <Inv S-Box File> <Plaintext File>" << endl;
		exit(1);
	}
	
	cout << "Plaintext Filename: " << argv[4] << endl;
    cout << "Key Filename: " << argv[1] << endl;
	
    do_abc(argv[1], argv[2], argv[3], argv[4]);
    
    cout << "Program completed successfully...\n";
}
