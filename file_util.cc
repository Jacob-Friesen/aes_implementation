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

#include "file_util.hh"

string read_in_file(string file_name){
    ifstream file;
    string line;
    string contents;
    
    try{
        file.open(file_name.c_str());
        
        if (file.is_open()){
            while (file.good()){
                getline(file, line);
                contents += line;
            }
            
            file.close();
        }
        else
            throw runtime_error("Couldn't open file \"" + file_name + "\"");
    }
    catch(exception& e){
        cout << "File Error: " << e.what() << endl;
    }
    
    return contents;
}

unsigned char **read_in_twod_arr(string file_name, int dimensions){
    unsigned char **twod_arr = new unsigned char *[dimensions];
    for(int i = 0; i < dimensions; i += 1)
        twod_arr[i] = new unsigned char[dimensions];
    
    string file_string;
    int row_index = 0;
    int column_index = 0;
    string temp;
    int temp_i;
    
    file_string = read_in_file(file_name);
    
    // split the string
    istringstream string_stream(file_string);
    while(!string_stream.eof()){
        getline(string_stream, temp, ' ');
        if (temp != ""){
            // convert to unsigned char and add to array
            sscanf(temp.c_str(), "%02x", &temp_i);
            twod_arr[row_index][column_index] = (unsigned char) temp_i;
            
            column_index += 1;
            if(column_index == dimensions){
                column_index = 0;
                row_index += 1;
            }
          }
    }
    
    return twod_arr;
}

uint32_t *read_in_K(string file_name, int height){
    uint32_t *the_array = new uint32_t[height];
    string file_string, temp;
    string int_string;//stores Nk * 2 hexidecimals for easy conversion to 32bit
    int hex_count = 1;
    
    file_string = read_in_file(file_name);
    
    // split the string
    istringstream string_stream(file_string);
    while(!string_stream.eof()){
        getline(string_stream, temp, ' ');
        int_string += temp;
        
        // when there are Nk (should be 4 for normal inputs) in int_string convert it to an int and place in K position.
        if(hex_count > 0 && hex_count % height == 0){
            sscanf(int_string.c_str(), "%x", &the_array[(hex_count/height) - 1]);
            int_string = "";
        }
        
        hex_count += 1;
    }
    
    return the_array;
}