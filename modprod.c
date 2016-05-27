#include "hw5.h"
/*
Samyukta Satish Rao
University Of Southern California.

*/

/*
a3 a0 a1 a2		b3
a2 a3 a0 a1		b2
a1 a2 a3 a0		b1
a0 a1 a2 a3		b0
*/

int checkHex(char * poly) // checks if input is in hex format. 
{
	int i;
	int flag = 0;
	for(i=0;i<strlen(poly);i++)
	{
		char c = poly[i];
		if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
			flag = 1;
		else
			break;
	}
	return flag;
}

unsigned char bigdot(unsigned char a, unsigned char b) // function to perform big dot notation. Computes a(bigdot)b
{

   unsigned char c = 0;
   unsigned char d = b;
   int i;


   for (i=0 ; i < 8 ; i++) 
   {
      if (a%2 == 1)
      	c  = c ^ d;
      a=a/2;
      d = xtime(d);
   }
   return c;
}


void modprod(char * poly1, char * poly2) // calculates and prints mod product given two polynomials. 
{

	unsigned char a[4], b[4];
	char * temp = calloc(2, sizeof(char));
	int i;

	if((!checkHex(poly1)) || (!checkHex(poly2)))	
	{
		fprintf(stderr, "The polynomials should be in hexadecimal format and should be of length 8.\n");
		exit(0);
	}
	else
	{

		//extracting poly1
		for(i=0;i<4;i++)
		{
			int j;
			unsigned char value;
			strncpy(temp, poly1, 2);
			poly1+=2;
		
			// compute the value
			for (j = 0; j < 2; j++)
			{
				char c = temp[j];
				if(c >= '0' && c <= '9') 
				{
					value = value * 16 + c - '0';
				}
				else if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
				{
					value = value * 16 + c - 'a' + 10;
				} 

			}
			a[i] = value;
		}

		//extracting poly2
		for(i=0;i<4;i++)
		{
			int j;
			strncpy(temp, poly2, 2);
			poly2+=2;
			unsigned char value;

			// compute the value
			for (j = 0; j < 2; j++)
			{
				char c = temp[j];
				
				if(c >= '0' && c <= '9') 
				{
					value = value * 16 + c - '0';
				}
				else if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
				{
					value = value * 16 + c - 'a' + 10;
				} 

			}
			b[i] = value;
		}

		unsigned char d[4]; 
		// matrix multiplication
		d[3] = bigdot(a[3], b[3]) ^ bigdot(a[0], b[2]) ^ bigdot(a[1], b[1]) ^ bigdot(a[2], b[0]);
		d[2] = bigdot(a[2], b[3]) ^ bigdot(a[3], b[2]) ^ bigdot(a[0], b[1]) ^ bigdot(a[1], b[0]);
		d[1] = bigdot(a[1], b[3]) ^ bigdot(a[2], b[2]) ^ bigdot(a[3], b[1]) ^ bigdot(a[0], b[0]);
		d[0] = bigdot(a[0], b[3]) ^ bigdot(a[1], b[2]) ^ bigdot(a[2], b[1]) ^ bigdot(a[3], b[0]);

		//printing
		printf("{%02x}{%02x}{%02x}{%02x} CIRCLEX {%02x}{%02x}{%02x}{%02x} = {%02x}{%02x}{%02x}{%02x}\n"
			, a[0], a[1], a[2], a[3], b[0], b[1], b[2], b[3], d[0], d[1], d[2], d[3]);	
		
	}
}



void modprodcheck(char * poly1, char * poly2) // checks if modprod gives required value. This does not print anything. 
{

	unsigned char a[4], b[4];
	char * temp = calloc(2, sizeof(char));
	int i;

	if((!checkHex(poly1)) || (!checkHex(poly2)))	
	{
		fprintf(stderr, "The polynomials should be in hexadecimal format and should be of length 8.\n");
		exit(0);
	}
	else
	{

		//extracting poly1
		for(i=0;i<4;i++)
		{
			int j;
			strncpy(temp, poly1, 2);
			poly1+=2;
			unsigned char value;

			// compute the value
			for (j = 0; j < 2; j++)
			{
				char c = temp[j];
				if(c >= '0' && c <= '9') 
				{
					value = value * 16 + c - '0';
				}
				else if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
				{
					value = value * 16 + c - 'a' + 10;
				} 

			}
			a[i] = value;
		}

		//extracting poly2
		for(i=0;i<4;i++)
		{
			int j;
			strncpy(temp, poly2, 2);
			poly2+=2;
			unsigned char value;

			// compute the value
			for (j = 0; j < 2; j++)
			{
				char c = temp[j];
				if(c >= '0' && c <= '9') 
				{
					value = value * 16 + c - '0';
				}
				else if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
				{
					value = value * 16 + c - 'a' + 10;
				} 

			}
			b[i] = value;
		}

		unsigned char d[4]; 
		// matrix multiplication
		d[3] = bigdot(a[3], b[3]) ^ bigdot(a[0], b[2]) ^ bigdot(a[1], b[1]) ^ bigdot(a[2], b[0]);
		d[2] = bigdot(a[2], b[3]) ^ bigdot(a[3], b[2]) ^ bigdot(a[0], b[1]) ^ bigdot(a[1], b[0]);
		d[1] = bigdot(a[1], b[3]) ^ bigdot(a[2], b[2]) ^ bigdot(a[3], b[1]) ^ bigdot(a[0], b[0]);
		d[0] = bigdot(a[0], b[3]) ^ bigdot(a[1], b[2]) ^ bigdot(a[2], b[1]) ^ bigdot(a[3], b[0]);


		if( (d[0]!=0) || (d[1]!=0) || (d[2]!=0) || (d[3]!=1) )
		{
			fprintf(stderr, "Product of the polynomials represented by P and INVP is not {00}{00}{00}{01}. Invalid characters in INVP\n");
			exit(0);
		}
		
	}
}

