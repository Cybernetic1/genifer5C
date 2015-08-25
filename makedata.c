#include <stdio.h>
#include <math.h>
 
#define PI 3.14159
 
int main()
{ 
  int i,j;
  for(j=0; j<3; j++) {
    for(i=0; i<60; i++) {
      printf("%f\t", sin(i * PI / 30.0 + j * PI / 1.5));
    }
    printf("\n");
  }

  return 0;
}
