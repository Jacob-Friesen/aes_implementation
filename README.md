AES Implementation
==================

This is my AES implementation. It demonstrates my ability to code complex programs in a C style of environment. Given a key, sbox, inverse sbox and some plaintext, encrypts then decrpyts the plaintext, outputting the results in each round. Encryption, decryption, and key schedule generation only rely on previous components to  work if you uncomment thier main functions e.g. decryption only requires the key schedule files. The code was based on the [FIPS AES specification](http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf). Look in aes.cc for the main file that gets everything started and how to run the program.

*Note: I am not a Cryptographer and this is coded for readability above efficiency, so don't use this for serious projects.*

Running
=======
aes.cc is the main file, to compile and run:
    g++ -Wall aes.cc aes_decrypt.cc aes_encrypt.cc key_schedule.cc file_util.cc -o aes && ./aes test1key.txt aes_sbox.txt aes_inv_sbox.txt test1plaintext.txt

I have included a few other test files so you can see how the algorithm works with different inputs.
