 /* Ricardo Bastos Leta Vieira 2110526 3WA */
/* Rafael Paladini Meirelles 2111538 3WA */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "gravacomp.h"

// Structs de teste
struct s 
{
  int i1; 
  // int i2;
  char s1[3];
  unsigned int u1;
  char s2[10];
  // unsigned int u2;
};

int main(void) 
{
  int size = 2;
  
  struct s exemplo[2]; // struct usada para teste
  FILE *arq = fopen("dados.dat","wb"); // abre arquivo para escrita
  
  exemplo[0].i1 = 2000; 
  // exemplo[0].i2 = 3; 
  strcpy(exemplo[0].s1,"h\0");
  exemplo[0].u1 = 2000;
  // exemplo[0].u2 = 12;
  strcpy(exemplo[0].s2,"abacate\0");
  // exemplo[0].u2 = 12;
  

  exemplo[1].i1 = 69; 
  // exemplo[1].i2 = 5; 
  strcpy(exemplo[1].s1,"tt\0");
  exemplo[1].u1 = 257;
  // exemplo[1].u2 = 258;
  strcpy(exemplo[1].s2,"aba\0");
  // exemplo[1].u2 = 258;

  int res = gravacomp(size, exemplo, "is03us10", arq); // chamade de teste
    
  // int res = gravacomp(size, exemplo, "iis03s10", arq); // chamade de teste
  
  // int res = gravacomp(size, exemplo, "iis03us10", arq); // chamade de teste

  fclose(arq); // fecha arq de escrita
  

  // -----------------------------------------------
  // teste de mostracomp
  FILE *arq2 = fopen("testes.dat","wb"); // abre arquivo para escrita

  // struct s {
  //   int  i;
  //   char s1[5];
  //   unsigned int u;
  // };
  
  // e um array com duas estruturas desse tipo. Se os campos da primeira estrutura contiverem, nesta ordem, os valores -1, "abc" e 258 e os da segunda os valores 1, "ABCD" e 65535, o conteúdo armazenado para esse array será (com os valores dos bytes exibidos em hexadecimal):
  
//   unsigned char testes[] = {0x01, 0x41, 0x45, 0x01, 0xff, 0xa1, 0x70};

  
//   fwrite(testes, 20, 1, arq2);

//   fclose(arq2);

//   FILE *arq3 = fopen("testes.dat","rb"); // abre arquivo para leitura
  
  // mostracomp(arq3);
  // fim teste de mostracomp
  
  // fclose(arq2);

//   FILE *arq4 = fopen("testes.dat", "rb");
  
  FILE *arq4 = fopen("dados.dat", "rb");
  
  mostracomp(arq4);

  fclose(arq4);
  return 0;
}
