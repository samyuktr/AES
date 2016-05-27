#include "hw5.h"
/*
Samyukta Satish Rao
University Of Southern California.

*/


unsigned char Scheck[256];

int testS(char * line) // tests S table
{
	if(!checkHex(line))
	{
		fprintf(stderr, "Error in S table. Values not in hex format\n");
		exit(0);
	}
	return 1;
}

int testP(char * line) // tests P input
{
	
	if(strlen(line)-1!=8)
	{
		fprintf(stderr, "P table does not contain value with length 8\n");
		exit(0);
	}
	if(!checkHex(line))
	{
		fprintf(stderr, "Error in P table\n");
		exit(0);
	}
	
	return 1;
}

int testINVP(char * line) // tests inverse P input
{

	if(strlen(line)-1!=8)
	{
		fprintf(stderr, "INVP table does not contain value with length 8\n");
		exit(0);
	}

	if(!checkHex(line))
	{
		fprintf(stderr, "Error in INVP table\n");
		exit(0);
	}
	return 1;
}


void tablecheck(char * filename) // does necessary table check. 
{
	int flag_p=0, flag_ip=0;
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
        	fprintf(stderr, "Incorrect table file format. Incorrect entries.\n" );
        	exit(0);
        }
       
	}
	if(count!=3)
	{
		fprintf(stderr, "Incorrect table file format. S, P and INVP don't exist in the table.\n");
		exit(0);
	}
	rewind(fp);


	//========================================================

	char * line = calloc(520, sizeof(char));
	line[0] = '\0';
	char *test = calloc(4, sizeof(char));
	test[0] = '\0';
	char * poly1= calloc(8, sizeof(char));
	char * poly2 = calloc(8, sizeof(char));
	for(i=0;i<3;i++)
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

    		if(!testS(pos))
    		{
    			fprintf(stderr, "S table error!\n");

    			exit(0);
    		}
    		int m;
    		for(m=0;m<256;m++)
			{
				int j;
				unsigned char value;
				strncpy(temp, pos, 2);
				pos+=2;
	

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

				if(Scheck[value]==1)
				{
					fprintf(stderr, "Repeated entries in S.\n");
					exit(0);
				}
				else
				{
					Scheck[value] = 1;
				}
			
				
			}


    	}
    	else if(strncmp(test, "P", 1)==0)
    	{
    			
    			if(!testP(pos))
    			{
    				fprintf(stderr, "P table error!\n");
    				exit(0);
    			}
    			strncpy(poly1, pos, 8);
    			flag_p = 1;
    	}
    	else if(strncmp(test, "INVP", 4)==0)
    	{
    			
    			if(!testINVP(pos))
    			{
    				fprintf(stderr, "INVP table error!\n");
    				exit(0);
    			}
    			strncpy(poly2, pos, 8);
    			flag_ip = 1;
    	}
  

    }
   // printf("poly1: %s\n", poly1);
   // printf("poly2: %s\n", poly2);
    if(flag_ip==0)
    {
    	fprintf(stderr, "INVP table does not exist :(\n");
    	exit(0);
    }
    modprodcheck(poly1, poly2);
}
