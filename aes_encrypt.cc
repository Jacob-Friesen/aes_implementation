//-----------------------------------------
// NAME: Jacob Friesen
// STUDENT NUMBER: 7623030
// COURSE: COMP 4140, SECTION: A01
// INSTRUCTOR: John Van Rees
// ASSIGNMENT: assignment 4, QUESTION: 1) b), also runs a)
// RUN EXAMPLE: g++ -Wall aes_encrypt.cc key_schedule.cc file_util.cc -o aes_encrypt && ./aes_encrypt test1key.txt aes_sbox.txt test1plaintext.txt
//
// REMARKS:
// Takes a 128 bit key and using it generates the round keys for the AES encryption/descryption. Then does an
// AES encryption on the given plaintext file.
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
#include "key_schedule.hh"
#include "file_util.hh"
#include "aes_encrypt.hh"

// this is what mix columns mulitplies column values by
const unsigned char A_X[STATE_WIDTH][STATE_WIDTH] = {
    {0x02, 0x03, 0x01, 0x01},
    {0x01, 0x02, 0x03, 0x01},
    {0x01, 0x01, 0x02, 0x03},
    {0x03, 0x01, 0x01, 0x02}
};

// does 128 bit encryption on the state using the key schedule with the s_box. Prints out each state as outlined by the assignment:
// http://www.cs.umanitoba.ca/~comp4140/assignments/a4/sample_out.txt using state_print.
unsigned char **Cipher(unsigned char **state, uint32_t *w, unsigned char **s_box);

// Shift the first row left by 0 bytes, the next by 1 byte, the next by 2 bytes and so on
unsigned char **ShiftRows(unsigned char **state);

// Takes 2 bytes and multiplies their polynomial representations returning the result
unsigned char polynomial_multiply(unsigned char multiplicand1, unsigned char multiplicand2);

// Transposes the given square matrix into a new matrix which is returned
unsigned char **transpose(unsigned char **to_transpose, int width);

unsigned char **aes_encrypt(uint32_t *K, unsigned char **s_box, uint32_t *w, string plaintext_file){
    unsigned char **state;
    
    cout << "ENCRYPTION PROCESS" << endl;
    cout << "------------------" << endl;
    
    // get and print the original state
    state = read_in_twod_arr(plaintext_file, STATE_WIDTH);
    state = transpose(state, STATE_WIDTH);
    state_print(state, "Plain Text:\n");
    
    // perform the main encryption algorithm and print operations and results
    state = Cipher(state, w, s_box);
    
    return state;
}

unsigned char **Cipher(unsigned char **state, uint32_t *w, unsigned char **s_box){
    uint32_t w_set[STATE_WIDTH];// stores the 4 round key values to be sent into AddRoundKey
    stringstream s_stream;
    
    // Get 4 keys from key schedule to add to round
    for(int i = 0; i < STATE_WIDTH; i += 1)
        w_set[i] = w[i];
    state = AddRoundKey(state, w_set);
    
    // Each round apply a specified set of operations after printing the results of the previous round
    for (int round = 1; round < Nr; round += 1){
        s_stream << round;
        state_print(state, "\nRound " + s_stream.str() +  "\n---------\n");
        s_stream.str("");    
        
        state = SubBytes(state, s_box);
        state = ShiftRows(state);
        state = MixColumns(state, A_X);
        
        // Get 4 keys from key schedule to add to round
        for(int i = 0; i < STATE_WIDTH; i += 1)
            w_set[i] = w[(round * Nb) + i];
        state = AddRoundKey(state, w_set);
    }
    state_print(state, "\nLast Round\n---------\n");
    
    // do final whitening operations
    state = SubBytes(state, s_box);
    state = ShiftRows(state);
    for (int i = 0; i < STATE_WIDTH; i += 1)
        w_set[i] = w[(Nr * 4) + i];
    state = AddRoundKey(state, w_set);
    
    // print final result
    state_print(state, "\nCipherText:\n");
    cout << endl;
    
    return state;
}

unsigned char **AddRoundKey(unsigned char **state, uint32_t w[]){
    unsigned char w_part;
    
    //XOR each column in the state the corresponding column in w
    for (int column = 0; column < STATE_WIDTH; column += 1){
        for (int row = 0; row < STATE_WIDTH; row += 1){
            // seperate correct w byte and xor it with corresponding place in state 
            w_part = w[column] >> 8 * (STATE_WIDTH - (row + 1));
            state[row][column] = w_part ^ state[row][column];
        }
    }
    
    return state;
}

unsigned char **SubBytes(unsigned char **state, unsigned char **s_box){
    int x, y;
    
    // substitute each word directly bitmasking to get each value to index into s_box with
    for (int row = 0; row < STATE_WIDTH; row += 1){
        for (int column = 0; column < STATE_WIDTH; column += 1){
            x = (state[row][column] & 0xf0) >> 4;
            y = state[row][column] & 0x0f;
            state[row][column] = s_box[x][y];
        }
    }
    
    return state;
}

unsigned char **ShiftRows(unsigned char **state){
    unsigned char temp[STATE_WIDTH];
    
    for (int row = 0; row < STATE_WIDTH; row += 1){
        // shift row to the left and place it into a temporary array
        for (int column = STATE_WIDTH - 1; column >= 0; column -= 1)
            temp[column] = state[row][(column + row) % STATE_WIDTH];
        
        // copy temporary column into corresponding row in state
        for (int column = 0; column < STATE_WIDTH; column += 1)
            state[row][column] = temp[column];
    }
    
    return state;
}

unsigned char **MixColumns(unsigned char **state, const unsigned char a_x[][STATE_WIDTH]){
    // previous values cannot affect future ones so easiest way to mitigate is to store results in seperate matrix
    unsigned char **return_arr = new unsigned char *[STATE_WIDTH];
    for(int i = 0; i < STATE_WIDTH; i += 1)
        return_arr[i] = new unsigned char[STATE_WIDTH];
    
    for (int column = 0; column < STATE_WIDTH; column += 1){
        for (int row = 0; row < STATE_WIDTH; row += 1)
            // matrix multiply each column position with the current a_x then add the positions together
            return_arr[row][column] = polynomial_multiply(a_x[row][0], state[0][column]) ^ polynomial_multiply(a_x[row][1], state[1][column])
                                    ^ polynomial_multiply(a_x[row][2], state[2][column]) ^ polynomial_multiply(a_x[row][3], state[3][column]);
    }
    
    // cleanup now unused state array
    for (int i = 0; i < STATE_WIDTH; i += 1)
        delete[] state[i];
    delete[] state;
    
    return return_arr;
}

unsigned char polynomial_multiply(unsigned char multiplicand1, unsigned char multiplicand2){
    unsigned char m2_bitmask = 0x01;// for getting first bit
    unsigned char m1_bitmask = 0x80;// for getting last bit
    unsigned char m1_carry;
    unsigned char product = 0;
    
    for (int bit = 0; bit < 8; bit++){
        // first bit in multiplicand 2 is set so add (bitwise xor) product to multiplicand 1
        if ( (multiplicand2 & m2_bitmask) != 0)
            product = product ^ multiplicand1;
        
        // set carry if mulitplicand1 has its leftmost bit set
        m1_carry = multiplicand1 & m1_bitmask;
        
        // shift multiplicand1 left by 1 so multiplying by 2
        multiplicand1 = multiplicand1 << 1;
        
        // high term of irreducible polynomial + carry mod 2 if carry is not 0
        if (m1_carry != 0)
            multiplicand1 = multiplicand1 ^ IRR_POLYNOMIAL_LAST;
        
        // shift multiplicand2 right by 1 so dividing by 2
        multiplicand2 = multiplicand2 >> 1;
    }
    
    return product;
}

unsigned char **transpose(unsigned char **to_transpose, int width){
    unsigned char **transposed = new unsigned char *[width];
    for(int i = 0; i < width; i += 1)
        transposed[i] = new unsigned char[width];
    
    for (int i = 0; i < width; i += 1){
        for (int j = 0; j < width; j+= 1)
            transposed[i][j] = to_transpose[j][i];
    }
    
    return transposed;
}

void state_print(unsigned char **state, string to_print){
    cout << to_print;
    for (int column = 0; column < STATE_WIDTH; column += 1){
        for (int row = 0; row < STATE_WIDTH; row += 1)
            printf("%02x  ", state[row][column]);
        cout << "  ";
    }
    cout << endl;
}

//int main(int argc, char* argv[]){
//    uint32_t *K = new uint32_t[Nk];
//    unsigned char **s_box;
//    unsigned char **state;
//    uint32_t *w;
//    
//    cout << "\n";
//    
//    // Instruct on correct args if not given
//	if (argc < 4){
//		cout << "Usage: " << argv[0] << " <Key File> <S-Box File> <Plaintext>" << endl;
//		exit(1);
//	}
//    
//    // get key schedule information
//    K = read_in_K(argv[1], Nk);
//    s_box = read_in_twod_arr(argv[2], SCHEDULE_LENGTH);
//    w = key_schedule(K, s_box, true);
//    
//    state = aes_encrypt(K, s_box, w, argv[3]);
//    
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
