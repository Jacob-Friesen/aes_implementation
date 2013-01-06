#ifndef KEY_SCHEDULE
#define KEY_SCHEDULE
using namespace std;
 
// Runs key schedule using the given s_box and key and prints the results if the param is given. Returns results in either case as an array of 32 bit values.
uint32_t *key_schedule(uint32_t *K, unsigned char **s_box, bool print_it);

// Takes the schedule and prints each value, 4 to a row
void print_schedule(uint32_t w[]);
 
#endif