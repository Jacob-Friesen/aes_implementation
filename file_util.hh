#ifndef FILE_UTIL
#define FILE_UTIL
using namespace std;

// Reads in specified file contents then transforms the values into an array of 4 32 bit ints.
uint32_t *read_in_K(string file_name, int height);

// Reads in specified file contents then transforms the values into  a 2d array of unsigned chars of dimensions
unsigned char **read_in_twod_arr(string file_name, int dimensions);

// Reads file into a string and returns that
string read_in_file(string file_name);

#endif