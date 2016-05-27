#ifndef _FUNCTION_H_  //handling multiple inclusions
#define _FUNCTION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


void modprod(char * poly1, char * poly2);
void modprodcheck(char * poly1, char * poly2);
unsigned char bigdot(unsigned char a, unsigned char b);

int checkHex(char * poly);
void tablecheck(char * filename);
#define xtime(x) ((x<<1) ^ (((x>>7) & 1) * 0x1b))

void encryption(char * tfilename, char * key , char *ipfilename);
void keyexpand(char * key, char * filename, int flag);

void decryption(char * tfilename, char * key , char *ipfilename);
void inverse(char * p);

#endif
