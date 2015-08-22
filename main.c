// TO-DO:

#include <stdio.h>
#include <stdlib.h>

char inChars[1024];
char outChars[1024];

// *******************read training data and testing data**********************

void getTrainingAndTestData(int argc, char **path)
	{
	if(argc != 3)
		{
		printf("Usage: program training_data_file testing_data_file\n");
		exit(0);
		}

	FILE *fp1, *fp2;
	if((fp1 = fopen(path[1], "r")) == NULL)
		{
		printf("cannot open %s\n", path[1]);
		exit(1);
		}
	if((fp2 = fopen(path[2], "r")) == NULL)
		{
		printf("cannot open %s\n", path[2]);
		exit(1);
		}

	int i = 0;
	int num1, num2;
	while(i < 800)
		{
		fscanf(fp1, "%d %d %lf %lf", &num1, &num2);
		fscanf(fp2, "%d %d %lf %lf", &num1, &num2);
		i++;
		}
	fclose(fp1);
	fclose(fp2);
	}

//**************************main function***********************//

int main2(int argc, char** argv)
	{

	//output data to a file
	FILE *fout;
	if ((fout = fopen("randomtest_1.txt", "w")) == NULL)
		{
		fprintf(stderr, "file open failed.\n");
		exit(1);
		}

	fclose(fout);

	return 0;
	}
