// Train Genifer to convert Chinese texts to Cantonese texts
// ==========================================================
// Cantonese is a dialect of Chinese very close to standard Chinese, so only minor
// transliterations are required to turn Chinese into Cantonese.

// TO-DO:

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>

// List of Chinese characters that are recognized by the system
// Chinese characters not in these lists are considered "don't care"
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
		fscanf(fp1, "%d %d", &num1, &num2);
		fscanf(fp2, "%d %d", &num1, &num2);
		i++;
		}
	fclose(fp1);
	fclose(fp2);
	}

wchar_t *char_string_to_wchar_t(const char *src,const char *encoding)
{
    if(setlocale(LC_CTYPE, encoding) == NULL)
    {
        fprintf(stderr, "requested encoding unavailable\n");
        return NULL;
    }
    size_t n = mbstowcs(NULL, src, 0);
    wchar_t *dst = malloc((n + 1) * sizeof *dst);
    if(!dst)
    {
        fprintf(stderr, "memory allocation failed\n");
        return NULL;
    }
    if(mbstowcs(dst, src, n + 1) != n)
    {
        fprintf(stderr, "conversion failed\n");
        free(dst);
        return NULL;
    }
    return dst;
}

char *wchar_t_string_to_char(const wchar_t *src, const char *encoding)
{
    if(setlocale(LC_CTYPE, encoding) == NULL)
    {
        fprintf(stderr, "requested encoding unavailable\n");
        return NULL;
    }
    size_t n = wcstombs(NULL, src, 0);
    char *dst = malloc(n + 1);
    if(dst == NULL)
    {
        fprintf(stderr, "memory allocation failed\n");
        return NULL;
    }
    if(wcstombs(dst, src, n + 1) != n)
    {
        fprintf(stderr, "conversion failed\n");
        free(dst);
        return NULL;
    }
    return dst;
}

//**************************main function***********************//

int main(int argc, char** argv)
	{
    wchar_t wstr[]=L"嗨,你好";
    const char *encoding = "en_HK.UTF-8";
    unsigned char *utf8;
    unsigned char utf8_cp[128];
    wchar_t *utf8touni;
    wchar_t *pp;

	wchar_t myString2[16] = { 0x55E8, 0x3001, 0x4F60, 0x597D};

    setlocale(LC_ALL, "");
    wprintf(L"This is a long string: %ls \n",myString2);

	setlocale(LC_ALL, "utf8");

	FILE *fp1;
	if((fp1 = fopen("/home/yky/NetBeansProjects/conceptual-keyboard/training-set.txt", "r")) == NULL)
		{
		printf("cannot open training-set.txt\n");
		exit(1);
		}

	wchar_t s[100];

	wprintf(L"Testing\n");
	wprintf(L"Test again: 掂唔掂架？\n");

	// wcscpy(s, L"掂唔掂架？");

	// wprintf("wc : %ls \n", s);

		fwscanf(fp1, L"%S", s);
		wprintf(s);
		wprintf(L"\n");
		fwscanf(fp1, L"%S", s);
		wprintf(s);

	// wprintf(L"掂唔掂架？");
	fclose(fp1);

    return 0;

    utf8 = wchar_t_string_to_char(wstr, encoding);
    // strcpy(utf8_cp, utf8);
    utf8touni = char_string_to_wchar_t(utf8_cp, encoding);

    for(pp = utf8touni; *pp; pp++)
        printf("0x%X  ", (unsigned) *pp);

    printf("\n");

    free(utf8);
    free(utf8touni);
    return 0;

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
