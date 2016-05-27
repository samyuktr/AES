#include "hw5.h"
/*
Samyukta Satish Rao
University Of Southern California.

Inveres referred from https://github.com/kevincrane/cryptography-projects/blob/master/hw6-aes/inverse.c

*/
//unsigned char table_list[256];
unsigned char S[256];
unsigned char SInv[256];
unsigned char P[256];
unsigned char INVP[256];
unsigned char enc_key[44];
unsigned char Rcon[256];
unsigned char state[4][4];
unsigned char out_e[16];
int Nk = 4;
int Nb = 4;
int Nr = 10;


unsigned char Hex_to_Dec(char * array, int len, int start) 
{
	unsigned char val = 0;
	int i;

	// compute the value
	for (i=0; i<len; i++)
	{
		char c = array[start + i];

		// update value
		if ((c >= '0' && c <= '9')) 
		{
			val = val * 16 + c - '0';
		} 
		else if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) 
		{
			val = val * 16 + c - 'a' + 10;
		} 

	}

	// return
	return val;

}

void keyexpand(char * key, char * filename, int flag)
{
	char c;
	int count = 0;
	int i;

	FILE * fp = fopen(filename, "rb");
	if(fp==NULL)
	{
		fprintf(stderr, "Error opening file\n");
		exit(0);
	}
	for (c = getc(fp); c != EOF; c = getc(fp))
	{
        if (c == '\n') // Increment count if this character is newline
            count = count + 1;
        else if(c==' ' || c=='\t' )
        {
        	fprintf(stderr, "Incorrect table file format\n" );
        	exit(0);
        }
       
	}
	if(count!=3)
	{
		fprintf(stderr, "Incorrect table file format\n");
		exit(0);
	}
		//printf("%d\n", strlen(key));

	if(strlen(key) !=32)
	{
		fprintf(stderr, "Bad key, not enough characters\n");
		exit(0);
	}
	rewind(fp);
//========================================================
	int m;
	char * line = calloc(520, sizeof(char));
	line[0] = '\0';
	char *test = calloc(4, sizeof(char));
	test[0] = '\0';

	for(m=0;m<3;m++) // read table file into arrays S, P and INVP
	{

		if(fgets(line, 520, fp)==NULL)
			break;
		char* pos = strchr(line,'=');
		
		int pos_int = (int)(pos-line);

		strncpy(test, line, pos_int);
		test[pos_int+1] = '\0';
		pos++;
		char * temp = calloc(2, sizeof(char));
		
		if(strncmp(test, "S", 2)==0 )
		{

    		for(i=0;i<256;i++)
			{
				int j;
				strncpy(temp, pos, 2);
				pos+=2;
				//printf("a%d: %s\n",i, temp );


				// initialize value
				unsigned char value;

				// compute the value
				for (j = 0; j < 2; j++)
				{
					char c = temp[j];
					// update value
					if(c >= '0' && c <= '9') 
					{
						value = value * 16 + c - '0';
					}
					else if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
					{
						value = value * 16 + c - 'a' + 10;
					} 

				}
				S[i] = value;
			}

    	}

    	if(strncmp(test, "P", 2)==0 )
		{

    		for(i=0;i<4;i++)
			{
				int j;
				strncpy(temp, pos, 2);
				pos+=2;
				// initialize value
				unsigned char value;

				// compute the value
				for (j = 0; j < 2; j++)
				{
					char c = temp[j];
					// update value
					if(c >= '0' && c <= '9') 
					{
						value = value * 16 + c - '0';
					}
					else if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
					{
						value = value * 16 + c - 'a' + 10;
					} 

				}
				P[i] = value;
			}
    	}


    	if(strncmp(test, "INVP", 2)==0 )
		{

    		for(i=0;i<4;i++)
			{
				int j;
				strncpy(temp, pos, 2);
				pos+=2;

				// initialize value
				unsigned char value;

				// compute the value
				for (j = 0; j < 2; j++)
				{
					char c = temp[j];
					// update value
					if(c >= '0' && c <= '9') 
					{
						value = value * 16 + c - '0';
					}
					else if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
					{
						value = value * 16 + c - 'a' + 10;
					} 

				}
				INVP[i] = value;
			}
    	}

	}
// ================= done polpulating S, P, INVP arrays====================================
	// initializing Rcon array. 
	Rcon[0] = 0x8d;
	for(i=1;i<255;i++) 
	{
		Rcon[i] = bigdot(Rcon[i - 1], 0x02);
	}

	unsigned char  temp_key[4];
	int j;
	int buff;
	// first 4 keys remain same as the given key. 
	for(i=0;i<Nk;i++)
	{
		enc_key[4*i] = Hex_to_Dec(key, 2, i*8);
		enc_key[4*i+1] = Hex_to_Dec(key, 2, i*8+2);
		enc_key[4*i+2] = Hex_to_Dec(key, 2, i*8+4);
		enc_key[4*i+3] =  Hex_to_Dec(key, 2, i*8+6);
	}
	//int l;
		/*printf("Key initial: ");
		for(l=0;l<4;l++)
		{
			printf("%d ", enc_key[l]);
		}
		printf("\n"); */

	for(i=Nk; i< Nb*(Nr+1); i++)
	{
		//printf("%dloop\n",i );
		for(j=0;j<4;j++) // putting the 4th set of 32 bits into temp_key from enc_key i.e., 4 characters(32 bits) from enc_key.
		{
			temp_key[j] = enc_key[(i-1)*4 + j];
		}
		if(i%Nk == 0)
		{
			
			/*printf("Before rotating: \n");
			for(l=0;l<4;l++)
			{
				printf("%d ", temp_key[l] );
			}
			printf("\n"); */
			// rotWord
			buff = temp_key[0];
			temp_key[0] = temp_key[1];
			temp_key[1] = temp_key[2];
			temp_key[2] = temp_key[3];
			temp_key[3] = buff;

			/*printf("After rotating anf before subs: \n");
			for(l=0;l<4;l++)
			{
				printf("%d ", temp_key[l] );
			}
			printf("\n"); */

			//SubWord using S array. Since S is a 1D array with hex values converted to binary, subWord becomes a 1D array lookup. 
			temp_key[0] = S[temp_key[0]];
			temp_key[1] = S[temp_key[1]];
			temp_key[2] = S[temp_key[2]];
			temp_key[3] = S[temp_key[3]];
			/*printf("After subs: \n");
			for(l=0;l<4;l++)
			{
				printf("%d ", temp_key[l] );
			}
			printf("\n"); */
			//printf("temp_key[0]: %02x in %dloop\n", temp_key[0], i );
			//getchar();

			temp_key[0] = temp_key[0] ^ Rcon[i/Nk];

		}

		else if(Nk>6 && (i%Nk)==4) 
		{
			temp_key[0] = S[temp_key[0]];
			temp_key[1] = S[temp_key[1]];
			temp_key[2] = S[temp_key[2]];
			temp_key[3] = S[temp_key[3]];
		}
		// populating next 4 key bytes
		enc_key[4*i + 0] = enc_key[4*(i - Nk) + 0] ^ temp_key[0];
		enc_key[4*i + 1] = enc_key[4*(i - Nk) + 1] ^ temp_key[1];
		enc_key[4*i + 2] = enc_key[4*(i - Nk) + 2] ^ temp_key[2];
		enc_key[4*i + 3] = enc_key[4*(i - Nk) + 3] ^ temp_key[3];

	/*	printf("Key: ");
		for(l=0;l<4;l++)
		{
			printf("%d ", enc_key[i+l]);
		}
		printf("\n");
		getchar();*/

	}
	if(flag==1)
	{
		for (i=0;i<44;i++)
		{
			printf("w[%2d]: %02x%02x%02x%02x\n", i, enc_key[4*i], enc_key[4*i + 1], enc_key[4*i + 2], enc_key[4*i + 3]);
		}	
	}
	
}

void AddRoundKey(int number)
{
	int i,j;
	for(i=0;i<Nb;i++)
	{
		for(j=0;j<4;j++)
		{
			//printf("%02x\n", state[j][i] );
			state[j][i] ^= enc_key[(number*Nb*4) + (Nb*i)+j]; // uses w0-w4 for XOR operation for first round key  - i.e., 0-15 from enc_key
			//printf("%02x %02x\n",enc_key[(number*Nb*4) + (Nb*i)+j], state[j][i]);
		}
	}

}

void SubBytes() 
{
	int i, j;
	for (i=0;i<4;i++) 
	{
		for (j=0;j<4;j++) 
		{
			state[i][j] = S[state[i][j]];
		}
	}

}


void ShiftRows()
{
	unsigned char temp1; 
	unsigned char temp2;
	unsigned char temp;

	//single shift
	temp = state[1][0];
	state[1][0] = state[1][1];
	state[1][1] = state[1][2];
	state[1][2] = state[1][3];
	state[1][3] = temp;

	// double shift
	temp1 = state[2][0];
	temp2 = state[2][1];
	state[2][0] = state[2][2];
	state[2][1] = state[2][3];
	state[2][2] = temp1;
	state[2][3] = temp2;

	//triple shift
	temp = state[3][0];
	state[3][0] = state[3][3];
	state[3][3] = state[3][2];
	state[3][2] = state[3][1];
	state[3][1] = temp;

}

void MixColumns()
{

	unsigned char temp[4];
	int i;
	for (i=0;i<4;i++) 
	{		
		temp[0] = bigdot(P[3], state[0][i]) ^ bigdot(P[0], state[1][i]) ^ bigdot(P[1], state[2][i]) ^ bigdot(P[2], state[3][i]);
		temp[1] = bigdot(P[2], state[0][i]) ^ bigdot(P[3], state[1][i]) ^ bigdot(P[0], state[2][i]) ^ bigdot(P[1], state[3][i]);
		temp[2] = bigdot(P[1], state[0][i]) ^ bigdot(P[2], state[1][i]) ^ bigdot(P[3], state[2][i]) ^ bigdot(P[0], state[3][i]);
		temp[3] = bigdot(P[0], state[0][i]) ^ bigdot(P[1], state[1][i]) ^ bigdot(P[2], state[2][i]) ^ bigdot(P[3], state[3][i]);	

		state[0][i] = temp[0];
		state[1][i] = temp[1];
		state[2][i] = temp[2];
		state[3][i] = temp[3];
	}

}

void encryption(char * tfilename, char * key , char *ipfilename)
{


	
	int i, j;
	//printf("Hello!\n");
	char * in_str = calloc(18, sizeof(char));
	//	char * in_str;
	FILE * fp;
	if(ipfilename!=NULL)
	{
		fp = fopen(ipfilename, "rb");
	}
	else
	{
		 fp = stdin;
	}
	if(fp==NULL)
	{
		fprintf(stderr, "Error opening file\n");
		exit(0);
	}
	int count_in = 0;
	
	while (!feof(fp))
    {
    	//printf("Print: ");
    	//printf("Count: %d \n", count_in);
    	//printf("%02x ",(unsigned char) c );
        in_str[count_in++] = (unsigned char)getc(fp);
        //printf("Count end: %d \n", count_in);
    }
	//printf("Count: %d \n", count_in);
	if((count_in-1)!=16)
	{
		fprintf(stderr, "Error in input file format.\n");
		exit(0);
	}


	//=======================printing initial statements=================================================
		printf("round[ 0].input    ");
		for(i=0;i<count_in-1;i++)
		{
			printf("%02x", (unsigned char)in_str[i]);
		}
		printf("\n");
		printf("round[ 0].k_sch    ");
		for(i=0;i<strlen(key);i++)
		{
			printf("%c", key[i] );
		}
		printf("\n");

	//================= copying input string to state array in a vertical manner==========================
		int count_copy = 0;
		for(i=0;i<4;i++)
		{
			for (j=0;j<4;j++) 
			{
				state[j][i] = in_str[count_copy++];
			}
		}
		/*for(i=0;i<4;i++)
		{
			for(j=0;j<4;j++)
			{
				printf("%02x", state[i][j]);
			}
			printf("\n");
		} */
	//==============now we need to initalise enc_key global variable.============================
		keyexpand(key, tfilename, 0);
		AddRoundKey(0); // initial round key operation. 
		int count = 1; // counter to print 
		int round;

		for(round=1;round<Nr;round++)
		{
			printf("round[%2d].start    ", count);
			for (i=0;i<Nb;i++) 
			{
				for (j=0;j<4;j++) 
				{
					printf("%02x", state[j][i]);
				}
			}
			printf("\n");
	//==============================================================================
			SubBytes();
			printf("round[%2d].s_box    ", count);
			for (i=0;i<Nb;i++) 
			{
				for (j=0;j<4;j++) 
				{
					printf("%02x", state[j][i]);
				}
			}
			printf("\n");


	//==================================================================================

			ShiftRows();

			printf("round[%2d].s_row    ", count);
			for (i=0;i<Nb;i++) 
			{
				for (j=0;j<4;j++) 
				{
					printf("%02x", state[j][i]);
				}
			}
			printf("\n");
	//====================================================================================
			MixColumns();
			printf("round[%2d].m_col    ", count);
			for (i=0;i<Nb;i++) 
			{
				for (j=0;j<4;j++) 
				{
					printf("%02x", state[j][i]);
				}
			}
			printf("\n");
			
	//===================================================================================
			AddRoundKey(round);
			printf("round[%2d].k_sch    ", count);
			for(i=0;i<Nb;i++) 
			{
				for(j=0;j<4;j++) 
				{
					printf("%02x", enc_key[round*16 + 4*i+j] );
				}
			}
			printf("\n"); 
			count++;
	//===================================================================================



		} // end of for loop. 

		printf("round[%2d].start    ", count);
		for (i=0;i<Nb;i++) 
		{
			for (j=0;j<4;j++) 
			{
				printf("%02x", state[j][i]);
			}
		}
		printf("\n");
	//========================================================================================
		SubBytes();
		printf("round[%2d].s_box    ", count);
		for (i=0;i<Nb;i++) 
		{
			for (j=0;j<4;j++) 
			{
				printf("%02x", state[j][i]);
			}
		}
		printf("\n");
	//=========================================================================================
		ShiftRows();

		printf("round[%2d].s_row    ", count);
		for (i=0;i<Nb;i++) 
		{
			for (j=0;j<4;j++) 
			{
				printf("%02x", state[j][i]);
			}
		}
		printf("\n");
	//===========================================================================================
		AddRoundKey(Nr);
		printf("round[%2d].k_sch    ", count);
		for(i=0;i<Nb;i++) 
		{
			for(j=0;j<4;j++) 
			{
				printf("%02x", enc_key[round*16 + 4*i+j] );
			}
		}
		printf("\n");
	//===========================================================================================
		int count_out_e = 0;
		//printf("round[%2d].output   ", count);
		for(i=0;i<Nb;i++) 
		{
			for(j=0;j<4;j++) 
			{
				//printf("%02x", state[j][i]);
				out_e[count_out_e++] = state[j][i];
			}
		}

		printf("round[%2d].output   ", count);
		for(i=0;i<count_out_e;i++)
		{
			printf("%02x",out_e[i]);
		}
		printf("\n");
	//=================================================================================================

}

void getSInv()
{
	int i;
	for(i=0;i<256;i++)
	{
		SInv[S[i]] = i;
	}

}

void invSubBytes() 
{
	int i, j;
	for (i=0;i<4;i++) 
	{
		for (j=0;j<4;j++) 
		{
			state[i][j] = SInv[state[i][j]];
		}
	}

}

void invShiftRows()
{
	unsigned char temp1; 
	unsigned char temp2;
	unsigned char temp;

	//single shift
	temp = state[1][3];
	state[1][3] = state[1][2];
	state[1][2] = state[1][1];
	state[1][1] = state[1][0];
	state[1][0] = temp;

	// double shift
	temp1 = state[2][3];
	temp2 = state[2][2];
	state[2][3] = state[2][1];
	state[2][2] = state[2][0];
	state[2][1] = temp1;
	state[2][0] = temp2;

	//triple shift
	temp = state[3][0];
	state[3][0] = state[3][1];
	state[3][1] = state[3][2];
	state[3][2] = state[3][3];
	state[3][3] = temp;
	//state[3][0] = temp;

}

void invMixColumns()
{

	unsigned char temp[4];
	int i;
	for (i=0;i<4;i++) 
	{		
		temp[0] = bigdot(INVP[3], state[0][i]) ^ bigdot(INVP[0], state[1][i]) ^ bigdot(INVP[1], state[2][i]) ^ bigdot(INVP[2], state[3][i]);
		temp[1] = bigdot(INVP[2], state[0][i]) ^ bigdot(INVP[3], state[1][i]) ^ bigdot(INVP[0], state[2][i]) ^ bigdot(INVP[1], state[3][i]);
		temp[2] = bigdot(INVP[1], state[0][i]) ^ bigdot(INVP[2], state[1][i]) ^ bigdot(INVP[3], state[2][i]) ^ bigdot(INVP[0], state[3][i]);
		temp[3] = bigdot(INVP[0], state[0][i]) ^ bigdot(INVP[1], state[1][i]) ^ bigdot(INVP[2], state[2][i]) ^ bigdot(INVP[3], state[3][i]);	


		state[0][i] = temp[0];
		state[1][i] = temp[1];
		state[2][i] = temp[2];
		state[3][i] = temp[3];

	}

}

void decryption(char * tfilename, char * key , char *ipfilename)
{
	int i, j;
	//printf("Hello!\n");
	char * in_str = calloc(18, sizeof(char));
	//	char * in_str;
	FILE * fp;
	if(ipfilename!=NULL)
	{
		fp = fopen(ipfilename, "rb");
	}
	else
	{
		 fp = stdin;
	}
	if(fp==NULL)
	{
		fprintf(stderr, "Error opening file\n");
		exit(0);
	}
	int count_in = 0;

	while (!feof(fp))
    {
    	//printf("Print: ");
    	//printf("Count: %d \n", count_in);
    	//printf("%02x ",(unsigned char) c );
        in_str[count_in++] = (unsigned char)getc(fp);
        //printf("Count end: %d \n", count_in);
    }
	//printf("Count: %d \n", count_in);
	if((count_in-1)!=16)
	{
		fprintf(stderr, "Error in input file format.\n");
		exit(0);
	}

	//=====================Initial print statements========================================================

	printf("round[ 0].iinput   ");
	for(i=0;i<count_in-1;i++)
	{
		printf("%02x", (unsigned char)in_str[i]);
	}
	printf("\n");

	//printf("Key: %s\n", key);
	keyexpand(key, tfilename, 0);
	printf("round[ 0].ik_sch   ");
	for (i=0;i<Nb;i++) 
	{
		for (j=0;j<4;j++) 
		{
			printf("%02x", enc_key[Nr*16 + 4*i+j]);
		}
	}
	printf("\n");

	//===============================================================================
	int count_copy = 0;
	// initialising state array with the cipher text. 
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			state[j][i] = in_str[count_copy++];
		}
	}


	// getting inverse of the S table. 
	getSInv();
	//=================================================================================

	AddRoundKey(Nr);

	int round;
	int count = 1;
	for(round=Nr-1;round>0;round--)
	{
		
		printf("round[%2d].istart   ", count);
		for (i=0;i<Nb;i++) 
		{
			for (j=0;j<4;j++) 
			{
				printf("%02x", state[j][i]);
			}
		}
		printf("\n");
		//============================================================================
		invShiftRows();

		printf("round[%2d].is_row   ", count);
		for (i=0;i<Nb;i++) 
		{
			for (j=0;j<4;j++) 
			{
				printf("%02x", state[j][i]);
			}
		}
		printf("\n");
		//==============================================================================
		invSubBytes();
		printf("round[%2d].is_box   ", count);
		for (i=0;i<Nb;i++) 
		{
			for (j=0;j<4;j++) 
			{
				printf("%02x", state[j][i]);
			}
		}
		printf("\n");

		//====================================================================================

		AddRoundKey(round);
		printf("round[%2d].ik_sch   ", count);
		for(i=0;i<Nb;i++) 
		{
			for(j=0;j<4;j++) 
			{
				printf("%02x", enc_key[round*16 + 4*i+j] );
			}
		}
		printf("\n"); 
		//=====================================================================================
		printf("round[%2d].ik_add   ", count);
		for (i=0;i<Nb;i++) 
		{
			for (j=0;j<4;j++) 
			{
				printf("%02x", state[j][i]);
			}
		}
		printf("\n");
		//count++;
		//getchar();

		invMixColumns();
		count++;
	} // end of for loop
	//=======================================================================================

	printf("round[%2d].istart   ", count);
	for (i=0;i<Nb;i++) 
	{
		for (j=0;j<4;j++) 
		{
			printf("%02x", state[j][i]);
		}
	}
	printf("\n");

	//========================================================================================
	invShiftRows();

	printf("round[%2d].is_row   ", count);
	for (i=0;i<Nb;i++) 
	{
		for (j=0;j<4;j++) 
		{
			printf("%02x", state[j][i]);
		}
	}
	printf("\n");
	//==========================================================================================
	invSubBytes();
	printf("round[%2d].is_box   ", count);
	for (i=0;i<Nb;i++) 
	{
		for (j=0;j<4;j++) 
		{
			printf("%02x", state[j][i]);
		}
	}
	printf("\n");

	//=========================================================================================
	
	printf("round[%2d].ik_sch   ", count);
	for(i=0;i<Nb;i++) 
	{
		for(j=0;j<4;j++) 
		{
			printf("%02x", enc_key[round*16 + 4*i+j] );
		}
	}
	printf("\n"); 

	AddRoundKey(0);

	//===============================================================================================

	printf("round[%2d].ioutput  ", count);
	for (i=0;i<4;i++) 
	{
		for (j=0;j<4;j++) 
		{
			printf("%02x", state[j][i]);
		}
	}
	printf("\n");

} 
//===========================================referred from https://github.com/kevincrane/cryptography-projects/blob/master/hw6-aes/inverse.c===============================================
unsigned int getinv(int index) 
{

	char* table_list; 
	table_list = (char*)"00018df6cb527bd1e84f29c0b0e1e5c7\
74b4aa4b992b605f583ffdccff40eeb2\
3a6e5af1554da8c9c10a98153044a2c2\
2c45926cf3396642f235206f77bb5919\
1dfe37672d31f569a764ab135425e909\
ed5c05ca4c2487bf183e22f051ec6117\
165eafd349a63643f44791df3393213b\
79b7978510b5ba3cb670d006a1fa8182\
837e7f809673be569b9e95d9f702b9a4\
de6a326dd88a84722a149f88f9dc899a\
fb7c2ec38fb8654826c8124acee7d262\
0ce01fef11757871a58e763dbdbc8657\
0b282fa3dad4e40fa92753041bfcace6\
7a07ae63c5dbe2ea948bc4d59df8906b\
b10dd6ebc60ecfad084ed7e35d501eb3\
5b2338346846038cdd9c7da0cd1a411c";

	// Read each pair of hexvals, store as ASCII vals in inv_table
	char temp_val[3];
	temp_val[0] = table_list[index * 2];
	temp_val[1] = table_list[index * 2 + 1];
	temp_val[2] = ' ';

	return strtol(temp_val, NULL, 16);
}
void inverse(char * poly)
{
	if(strlen(poly)!=8)
	{
		fprintf(stderr, "Input polynomial not in right format. Need to have 8 characters.\n");
		exit(0);
	}
	int i;
	int flag;
	for(i=0;i<strlen(poly);i++)
	{
		char c = poly[i];
		if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
			flag = 1;
		else
		{
			fprintf(stderr, "Input polynomial not in hex format\n");
			exit(0);
		}
	}
	
		
	char * temp = calloc(2, sizeof(char));
	unsigned char poly_num[4];
	int  j;
	for(i=0;i<4;i++)
	{
		int j;
		unsigned char value;
		strncpy(temp, poly, 2);
		poly+=2;

		// compute the value
		for (j=0; j<2; j++)
		{
			char c = temp[j];
			// update value
			if(c >= '0' && c <= '9') 
			{
				value = value * 16 + c - '0';
			}
			else if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
			{
				value = value * 16 + c - 'a' + 10;
			} 
			}
		poly_num[i] = value;

	}





	//Initially we know 2 rows of rem, and 2 rows of aux. 
	unsigned char rem[6][5] = { { 0x01, 0x00, 0x00, 0x00, 0x01 }, { 0x00, poly_num[0], poly_num[1], poly_num[2], poly_num[3] } };
	unsigned char quo[6][4] = { { 0 } };
	unsigned char aux[6][4] = { { 0 }, { rem[0][1], rem[0][2], rem[0][3], rem[0][4] } };
	//printing what we know in the initial 2 rows. 

	printf("i=1, rem[i]={%02x}{%02x}{%02x}{%02x}, quo[i]={00}{00}{00}{00}, aux[i]={00}{00}{00}{00}\n", rem[0][1], rem[0][2], rem[0][3], rem[0][4]);
	printf("i=2, rem[i]={%02x}{%02x}{%02x}{%02x}, quo[i]={00}{00}{00}{00}, aux[i]={%02x}{%02x}{%02x}{%02x}\n", poly_num[0], poly_num[1], poly_num[2], poly_num[3], aux[1][0], aux[1][1], aux[1][2],	aux[1][3]);

	for (i=2;i<6;i++) // populating the tabe from 2 to 6. 
	{
		// first division i.e., with one value in quotient quo[0]
		unsigned char temp[6 - i];
		quo[i][2] = bigdot(getinv(rem[i-1][i-1]), rem[i-2][i-2]); // getting the inverse of the first coeff when i=2. 
		for (j=0;j<(6 - i);j++) {
			temp[j] = rem[i-2][j+i-1] ^ bigdot(rem[i-1][j+i], quo[i][2]); // bigdot with inverse and xor with what is in the dividend's place at position j
		}

		// Till the 5th row we do the next steps. 
		if (i<5) 
		{
			temp[5-i] = rem[i-2][4];
			quo[i][3] = bigdot(getinv(rem[i-1][i-1]), temp[0]); // getting a value that will give {01} while temp[0] has what was calculated in the first bigdot operation
		} 
		else // if its the 5th row, check if it multiplies to 0x01
		{
			quo[i][3] = bigdot(getinv(rem[i-1][i-1]), temp[0] ^ 0x01); 
			rem[i][i-1] = 0x01;
		}
		// Compute remainder from quotient and previous remainders
		for (j=i;j<5;j++) 
		{
			rem[i][j] = temp[j-i+1] ^ bigdot(rem[i-1][j], quo[i][3]);
		}
		//Now we have all quo values. For aux, we need to get the mod product of value at position i in quo and i-1 in aux for every coeff and xor them 

		aux[i][3] = bigdot(quo[i][3], aux[i - 1][3]) ^ bigdot(quo[i][0], aux[i - 1][2]) ^ bigdot(quo[i][1], aux[i - 1][1]) ^ bigdot(quo[i][2], aux[i - 1][0]);
		aux[i][2] = bigdot(quo[i][2], aux[i - 1][3]) ^ bigdot(quo[i][3], aux[i - 1][2]) ^ bigdot(quo[i][0], aux[i - 1][1]) ^ bigdot(quo[i][1], aux[i - 1][0]);
		aux[i][1] = bigdot(quo[i][1], aux[i - 1][3]) ^ bigdot(quo[i][2], aux[i - 1][2]) ^ bigdot(quo[i][3], aux[i - 1][1]) ^ bigdot(quo[i][0], aux[i - 1][0]);
		aux[i][0] = bigdot(quo[i][0], aux[i - 1][3]) ^ bigdot(quo[i][1], aux[i - 1][2]) ^ bigdot(quo[i][2], aux[i - 1][1]) ^ bigdot(quo[i][3], aux[i - 1][0]);

		for (j=0;j<4;j++) 
		{
			aux[i][j] ^= aux[i-2][j];
		}
		// Output current results
		printf("i=%d, rem[i]={%02x}{%02x}{%02x}{%02x}",i + 1, rem[i][1], rem[i][2], rem[i][3], rem[i][4]);
		printf(", quo[i]={%02x}{%02x}{%02x}{%02x}", quo[i][0], quo[i][1], quo[i][2], quo[i][3]);
		printf(", aux[i]={%02x}{%02x}{%02x}{%02x}\n", aux[i][0], aux[i][1], aux[i][2], aux[i][3]);

		// Invalid coefficient, no inverse
		if (i != 5 && rem[i][i] == 0x00) {
			printf("{%02x}{%02x}{%02x}{%02x} does not have a multiplicative inverse.\n",
					poly_num[0], poly_num[1], poly_num[2], poly_num[3]);
			
		}
	}

	printf("Multiplicative inverse of {%02x}{%02x}{%02x}{%02x} is {%02x}{%02x}{%02x}{%02x}\n",
			poly_num[0], poly_num[1], poly_num[2], poly_num[3], aux[5][0], aux[5][1],
			aux[5][2], aux[5][3]);	
	

}


