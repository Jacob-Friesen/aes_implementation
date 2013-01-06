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

// Creates the ten 32 bit Rcon values and returns then in an array form
uint32_t *create_Rcon();

// Follows the key expansion algorithm generation a schedule of Nr keys for each round.Input is K and the
// s_box.
uint32_t *keyExpansion(uint32_t K[], unsigned char **s_box);

// Shifts all bits to the left by 8 bits, not a bitshift because 1st 8 bits are added to the back 
uint32_t RotWord(uint32_t word);

// Substitutes each byte in the sent in word by using the given s_box, the returns the substituted version.
uint32_t SubWord(uint32_t word, unsigned char **s_box);

uint32_t *key_schedule(uint32_t *K, unsigned char **s_box, bool print_it){
    uint32_t *w;
    
    w = keyExpansion(K, s_box);
    if (print_it == true)
        print_schedule(w);
    
    return w;
}

uint32_t *keyExpansion(uint32_t K[], unsigned char **s_box){
    uint32_t *w = new uint32_t[Nb * (Nr + 1)];
    uint32_t temp;
    uint32_t *Rcon;
    int i = 0;
    
    // make sure Rcon is usable
    Rcon = create_Rcon();
    
    // First Nk rounds just use the keys values in order
    for(i = 0; i < Nk; i += 1)
        w[i] = K[i];
    
    // Every 4th round do a sub word rot word then xor with relavent Rcon the xok with previous, otherwise
    // just xor with previous
    for (i = Nk; i < Nb * (Nr + 1); i += 1){
        temp = w[i - 1];
        
        // One of a batch of Nk make to do more than basic Xor
        if (i % Nk == 0)
            temp = SubWord(RotWord(temp), s_box) ^ Rcon[(i/Nk) - 1];
        w[i] = w[i - Nk] ^ temp;
    }
    
    //cleanup
    delete[] Rcon;
    
    return w;
}

uint32_t SubWord(uint32_t word, unsigned char **s_box){
    int x, y; // coordinates for s_box
    unsigned char current_byte;
    uint32_t result = 0;
    
    // go through each byte of the word
    for (int i = 0; i < 4; i += 1){
        // make room for new byte
        result = result << 8;
        
        // Use the first and second hex as indexes into the s_box and add the result to the return string
        current_byte = word >> (24 - (8 * i));
        x = (current_byte & 0xf0) >> 4;
        y = current_byte & 0x0f;
        result = result | s_box[x][y];
    }
    return result;
}

uint32_t RotWord(uint32_t word){
    // shift the word left by one byte and added the shifted byte to the end
    unsigned char start = (word & 0xff000000) >> 24;
    word = word << 8;
    return word | start;
}

void print_schedule(uint32_t w[]){
    string temp;
    
    cout << "\nKey Schedule:" << endl;
    for (int i = 0; i < Nb * (Nr + 1); i += 1){
        if((i + 1) % 4 == 0)
            printf("%08x\n", w[i]);
        else
            printf("%08x,", w[i]);
    }
    cout << endl;
}

uint32_t *create_Rcon(){
    uint32_t *Rcon = new uint32_t[RCON_VALUES];//garranteed to be 16 by the standard
    Rcon[0] = 0x01000000;
    
    // If mulitplying creates an overflow replace the top 2 bits with the irreducable polynomial, otherwise
    // just multiply previous value by 2.
    for (int i = 1; i < RCON_VALUES; i += 1){
        if(Rcon[i - 1] != 0x80000000)
            Rcon[i] = Rcon[i - 1] * 2;
        else
            Rcon[i] = IRR_POLYNOMIAL_LAST << 24;
    }
    
    return Rcon;
}

//int main(int argc, char* argv[]){
//    uint32_t *K = new uint32_t[Nk];
//    unsigned char **s_box;
//    bool print_it = false;
//    
//    cout << "\n";
//    
//    // Instruct on correct args if not given
//	if (argc < 2){
//		cout << "Usage: " << argv[0] << " <Key File> <S-Box File>" << endl;
//		exit(1);
//	}
//    
//    if(argc > 3 && strcmp(argv[3],"print") == 0)
//        print_it = true;
//        
//    // read in key schedule information
//    K = read_in_K(argv[1], Nk);
//    s_box = read_in_twod_arr(argv[2], SCHEDULE_LENGTH);
//    
//    key_schedule(K, s_box, print_it);
//    
//    cout << "Program completed successfully...\n";
//}
