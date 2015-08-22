#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include <iostream>
#include <iomanip>

int main()
	{
	setlocale(LC_ALL, "utf8");

	FILE *fp1;
	if((fp1 = fopen("/home/yky/NetBeansProjects/conceptual-keyboard/training-set.txt", "r")) == NULL)
		{
		printf("cannot open training-set.txt\n");
		exit(1);
		}

	wchar_t s[100];

	std::wcout << L"Test 1st: 掂唔掂架？" << std::endl;

	wprintf(L"Testing\n");
	wprintf(L"Test again: 掂唔掂架？\n");

	wcscpy(s, L"掂唔掂架？");

	// wprintf("wc : %ls \n", s);

		fscanf(fp1, "%s", s);
		wprintf(s);
		fscanf(fp1, "%s", s);
		wprintf(s);

	// wprintf(L"掂唔掂架？");
	fclose(fp1);

	/*output data to a file
	FILE *fout;
	if ((fout = fopen("randomtest_1.txt", "w")) == NULL)
		{
		fprintf(stderr, "file open failed.\n");
		exit(1);
		}

	fclose(fout);
	*/

	return 0;
	}
