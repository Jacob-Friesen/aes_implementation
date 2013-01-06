AES Implementation
==================

This is my AES implementation. It demonstrates my ability to code complex programs in a C style of environment. 
Given a key a sbox, inverse sbox and some plaintext, encrypts then decrpyts the plaintext, outputting the results in each round. Encryption, decryption, and key schedule generation only rely on previous components to  work if you uncomment thier main functions e.g. decryption only requires the key schedule files. Look in aes.cc for the main file that gets everything started and how to run the program.

*Note: I am not a Cryptographer and this is coded for readability above efficiency, so don't use this for serious projects.*
