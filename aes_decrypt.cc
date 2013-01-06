//-----------------------------------------
// REMARKS:
// Takes a 128 bit key and using it generates the round keys for the AES encryption/descryption. Then
// prints the results if the print flag is given.
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

// this is what mix columns mulitplies column values by
const unsigned char INV_A_X[STATE_WIDTH][STATE_WIDTH] = {
    {0x0e, 0x0b, 0x0d, 0x09},
    {0x09, 0x0e, 0x0b, 0x0d},
    {0x0d, 0x09, 0x0e, 0x0b},
    {0x0b, 0x0d, 0x09, 0x0e}
};

// Does 128 bit decryption on the state using the key schedule with the s_box. Prints out each state as outlined by the assignment:
// http://www.cs.umanitoba.ca/~comp4140/assignments/a4/sample_out.txt using state_print.
unsigned char **InvCipher(unsigned char **state, uint32_t *w, unsigned char **inv_s_box);

// Shift the first row right by 0 bytes, the next by 1 byte, the next by 2 bytes and so on
unsigned char **InvShiftRows(unsigned char **state);

unsigned char **aes_decrypt(unsigned char **state, uint32_t *w, unsigned char **inv_s_box){
	cout << "DECRYPTION PROCESS" << endl;
    cout << "------------------" << endl;
	state_print(state, "CipherText:\n");
	
	state = InvCipher(state, w, inv_s_box);
	
    return state;
}

unsigned char **InvCipher(unsigned char **state, uint32_t *w, unsigned char **inv_s_box){
    uint32_t w_set[STATE_WIDTH];// stores the 4 round key values to be sent into AddRoundKey
    stringstream s_stream;
    
    // Get 4 keys from key schedule to add to round, from the end backwards this time
    for (int i = 0; i < STATE_WIDTH; i += 1)
        w_set[i] = w[(Nr * 4) + i];
    state = AddRoundKey(state, w_set);

	// In reverse; each round apply a specified set of operations after printing the results of the previous round
    for (int round = Nr - 1; round > 0; round -= 1){
        s_stream << round;
        state_print(state, "\nRound " + s_stream.str() +  "\n---------\n");
        s_stream.str("");
        
        state = InvShiftRows(state);
	    state = SubBytes(state, inv_s_box);// only difference between inv and non-inv is the input s_box
		
		// Get 4 keys from key schedule to add to round
		for(int i = 0; i < STATE_WIDTH; i += 1)
			w_set[i] = w[(round * Nb) + i];
		state = AddRoundKey(state, w_set);
		
		state = MixColumns(state, INV_A_X);// only difference between inv and non-inv is the a_x input 
    }
    state_print(state, "\nLast Round\n---------\n");
    
    // do final dewhitening operations
    state = InvShiftRows(state);
	state = SubBytes(state, inv_s_box);
    for (int i = 0; i < STATE_WIDTH; i += 1)
        w_set[i] = w[i];
    state = AddRoundKey(state, w_set);
    
    // print final result
    state_print(state, "\nPlaintext:\n");
    cout << endl;
    
    return state;
}

unsigned char **InvShiftRows(unsigned char **state){
    unsigned char temp[STATE_WIDTH];
    
    for (int row = 0; row < STATE_WIDTH; row += 1){
		// shift row left and place in temporary array
        for (int column = 0; column < STATE_WIDTH; column += 1){
			if (column - row < 0)// row reaches past end
				temp[column] = state[row][STATE_WIDTH + (column - row)];
			else
				temp[column] = state[row][column - row];
        }
        
		// copy temporary array into state
        for (int column = 0; column < STATE_WIDTH; column += 1)
            state[row][column] = temp[column];
    }
    
    return state;
}

//int main(int argc, char* argv[]){
//	uint32_t *K = new uint32_t[Nk];
//    unsigned char **s_box;
//    unsigned char **state;
//    uint32_t *w;
//    
//    cout << "\n";
//    
//    // Instruct on correct args if not given
//	if (argc < 5){
//		cout << "Usage: " << argv[0] << " <Key File> <S-Box File> <Inv S-Box File> <Plaintext File>" << endl;
//		exit(1);
//	}
//	
//	cout << "Plaintext Filename: " << argv[4] << endl;
//    cout << "Key Filename: " << argv[1] << endl;
//	
//	// get key schedule information
//    K = read_in_K(argv[1], Nk);
//    s_box = read_in_twod_arr(argv[2], SCHEDULE_LENGTH);
//    w = key_schedule(K, s_box, true);
//    
//	state = aes_encrypt(K, s_box, w, argv[4]);
//	
//	// get inverse s_box, key schedule stays the same
//    s_box = read_in_twod_arr(argv[3], SCHEDULE_LENGTH);
//	
//	state = aes_decrypt(state, w, s_box);
//    
//	// clean up
//    for (int i = 0; i < STATE_WIDTH; i += 1)
//        delete[] state[i];
//    delete[] state;
//	for (int i = 0; i < SCHEDULE_LENGTH; i += 1)
//        delete[] s_box[i];
//    delete[] s_box;
//	delete []w;
//	delete []K;
//    
//    cout << "Program completed successfully...\n";
//}
