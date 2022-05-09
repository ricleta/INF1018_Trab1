#include <stdio.h>
#include <string.h>
#include "gravacomp.h"

/*
  Rafael Paladini Meirelles 2111538 3WA 
  Ricardo Bastos Leta Vieira 2110526 3WA 
*/

struct s1 
{
  int i1; 
  char s1[5];
  unsigned int u1;
};

struct s2 
{
  char s1[10];
};

struct s3
{
  unsigned int u1;
};

struct s4
{
  int i1; 
};

struct s5
{
  unsigned int u1;
  int i1; 
  char s1[3];
};

struct s6
{
  char s1[3];
  unsigned int u1;
  int i1; 
};

int main(void) 
{
  FILE *arq = fopen("dados.dat","wb"); // abre arquivo para escrita

  // Teste 1
  // int size = 2;
  // struct s1 exemplo[size]; // struct usada para teste
  // exemplo[0].i1 = -1;
  // strcpy(exemplo[0].s1,"abc\0");
  // exemplo[0].u1 = 258;
  // exemplo[1].i1 = 1; 
  // strcpy(exemplo[1].s1,"ABCD\0");
  // exemplo[1].u1 = 65535;
  // int res = gravacomp(size, exemplo, "is05u", arq);

  // Teste 2
  // int size = 2;
  // struct s2 exemplo[size]; // struct usada para teste
  // strcpy(exemplo[0].s1,"a\0");
  // strcpy(exemplo[1].s1,"abcdefghi\0");
  // int res2 = gravacomp(size, exemplo, "s10", arq);

  // Teste 3
  // int size = 1;
  // struct s3 exemplo[size]; // struct usada para teste
  // exemplo[0].u1 = 26946;
  // int res3 = gravacomp(size, exemplo, "u", arq);

  // Teste 4
  // int size = 2;
  // struct s4 exemplo[size]; // struct usada para teste
  // exemplo[0].i1 = -9;
  // exemplo[1].i1 = -905; 
  // int res4 = gravacomp(size, exemplo, "i", arq);

  // Teste 5
  // int size = 1;
  // struct s5 exemplo[size]; // struct usada para teste
  // exemplo[0].u1 = 1;
  // exemplo[0].i1 = -1;
  // strcpy(exemplo[0].s1,"p\0");
  // int res5 = gravacomp(size, exemplo, "uis03", arq);

  // Teste 6
  // int size = 1;
  // struct s6 exemplo[size]; // struct usada para teste
  // strcpy(exemplo[0].s1,"E\0");
  // exemplo[0].u1 = 255;
  // exemplo[0].i1 = 112;
  // int res6 = gravacomp(size, exemplo, "s03ui", arq);
    
  fclose(arq);
  
  FILE *arq2 = fopen("dados.dat","rb"); // abre arquivo para leitura
  
  mostracomp(arq2);
  fclose(arq2);
  return 0;
}
