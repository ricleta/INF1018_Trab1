/*
  Rafael Paladini Meirelles 2111538 3WA 
  Ricardo Bastos Leta Vieira 2110526 3WA 
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gravacomp.h"

// Protótipos

void complemento_a_dois(unsigned char * arr, int tamanho);
void interpreta_cabecalho(int * e_ultimo, char ** tipo, int * tamanho, unsigned char cabecalho);
unsigned char * gera_array(FILE * arq, int tamanho);
void grava_cabecalho(int tamanho, FILE* arquivo, int e_ultimo, char tipo);
int tam_min_inteiro(unsigned char * arr, char tipo);

// Calcula complemento a dois de um vetor de bytes
void complemento_a_dois(unsigned char * arr, int tamanho){
  
// Inverte os bytes bit a bit
  for(int i=0; i<tamanho; i++){
    arr[i] = ~arr[i];
  }
  
  for(int i=tamanho - 1; i >= 0; i--){
    
// Começando do último byte, somamos 1
    arr[i]++;
    /* Se a soma não deu 0, o byte não era "ff", e não precisamos carregar nenhum valor para o próximo byte. Se era, configuramos o ponteiro para o penúltimo elemento, e assim seguimos até que o byte comporte o "+1"*/
    if(arr[i] != 0){
      break;
    }
  }
  
}
// Analisa o byte de cabeçalho e transfere as informações úteis
void interpreta_cabecalho(int * e_ultimo, char ** tipo, int * tamanho, unsigned char cabecalho){
  // Criamos as váriáveis para posterior comparação bitwise
  unsigned char primeiro_bit = 0x80;
  unsigned char segundo_bit = 0x40;
  unsigned char bytes_tamanho;

  // Primeiro bit é igual a 1? Se sim, é último
  *e_ultimo = ((primeiro_bit & cabecalho) != 0);

  // Segundo bit é igual a 1? Se sim, é string, senão é inteiro
  if((segundo_bit & cabecalho) != 0){
    *tipo = "(str)";
    // Se string, os 6 últimos bits representam o tamanho
    bytes_tamanho = 0x3f;
  }
  else{
    // Precisamos analisar o terceiro bit para saber se é signed int ou unsigned int
    unsigned char terceiro_bit = 0x20;
    if((terceiro_bit & cabecalho) == 0){
      *tipo = "(uns)";
    }
    else{
      *tipo = "(int)";
    }
    // Se inteiro, os 5 últimos bits representam o tamanho
    bytes_tamanho = 0x1f;
  }
  *tamanho = bytes_tamanho & cabecalho;
  
}
// Lê bytes de um arquivo e transfere para um vetor de bytes
unsigned char * gera_array(FILE * arq, int tamanho){
  unsigned char * arr = (unsigned char *)malloc(tamanho);
  fread(arr, 1, tamanho, arq);
  return arr;
}
// Descompacta struct e mostra conteúdo
void mostracomp(FILE *arquivo){
  
  unsigned char num_structs;
  
  // Lemos o número de structs
  fread(&num_structs, sizeof(unsigned char), 1, arquivo);
  printf("Estruturas: %u\n", num_structs);
  
  // Iteramos uma vez para cada struct
  for(unsigned char i = 0; i < num_structs; i++){
    while(1){
      
      // Informações relativas a cada campo da struct
      unsigned char cabecalho;
      int e_ultimo;
      char * tipo;
      int tamanho;
      
      // Array de bytes que guardará o conteúdo em si do campo
      unsigned char * arr;
      
      // Lemos o byte de cabeçalho e guardamos as informações úteis
      fread(&cabecalho, 1, 1, arquivo);
      interpreta_cabecalho(&e_ultimo, &tipo, &tamanho, cabecalho);
      
      // Printamos o tipo do campo
      printf("%s ", tipo);
      // Guardamos o conteúdo em si
      arr = gera_array(arquivo, tamanho);
      
      // Se é uma string, basta printar cada char um a um
      if(tipo[1] == 's'){
        for(int j = 0; j < tamanho; j++){
          printf("%c", arr[j]);
        }
        printf("\n");
      }
      
      // Se é um unsigned int, calculamos o valor de cada byte com base em sua posição relativa e exibimos o resultado
      else if(tipo[1] == 'u'){
        unsigned int u = 0;
        for(int j = 0; j < tamanho; j++){
            u += pow(256, (tamanho - j - 1)) * (unsigned int)arr[j];
        }
        printf("%u\n", u);
      }
      else if(tipo[1] == 'i')
      {
        int d = 0;
        
        // Se é um int, buscamos primeiro determinar se é um inteiro negativo ou não
        if((arr[0] & (unsigned char)128) != 0){
          // Se sim, utilizamos complemento a dois para obter o valor positivo, e printamos um "-"
        complemento_a_dois(arr, tamanho);
          printf("-");
          }
        
        // Agora que eliminamos o caso dos negativos, basta printar como unsigned int
        for(int j = 0; j < tamanho; j++){
            d += pow(256, (tamanho - j - 1)) * arr[j];
        }
        printf("%d\n", d);
      }
      
      // Se era o último campo, quebramos o loop
      if(e_ultimo){
        break;
      }
    }
    // Separador de linhas entre structs
    printf("\n");
  }
}

// Compacta struct num arquivo
int gravacomp (int nstructs, void* valores, char* descritor, FILE* arquivo)
{
  // Guardaremos quantos bytes já foram percorridos
  int percorrido = 0;
  
  // grava byte de nstructs
  unsigned char n = nstructs;
  fwrite(&n, 1, 1, arquivo);
  
  // Damos cast em 'valores', para podermos ler as structs byte a byte
  valores = (unsigned char *)valores;
  
  // Iteramos uma vez para cada struct
  for(int i=0; i<nstructs; i++)
  {
    // Criamos um ponteiro auxiliar para percorrer o descritor sem estragá-lo
    char * descritor_auxiliar = descritor;
    
    // Iteramos cada campo, até o último da struct
    while(1)
    {
      // Váriáveis para guardar informações importantes para o cabeçalho
      char tipo_do_campo;
      int e_ultimo = 0;

      // Guardamos o tipo e avançamos na string descritora
      tipo_do_campo = *descritor_auxiliar;
      descritor_auxiliar++;
     
      // Se o campo é uma string
      if(tipo_do_campo == 's')
      {
        // Guardaremos o tamanho declarado da string e o tamanho de fato ocupado
        int tamanho_maximo_string;
        int tamanho_real_string = 0;

        // Lemos dois caracteres e ajustamos as posições relativas para registrar o tamanho
        tamanho_maximo_string = 10 * (*descritor_auxiliar - '0');
        descritor_auxiliar++;
        tamanho_maximo_string += (*descritor_auxiliar - '0');
        descritor_auxiliar++;

        // Se chegamos ao fim do descritor, é o último campo
        if (*descritor_auxiliar == '\0')
        {
          e_ultimo = 1;
        }

        // Copiamos o ponteiro para valores para descobrir o tamanho real da string sem estragar o original
        unsigned char * copia_valores = valores;
        while(*copia_valores)
        {
          copia_valores++;
          tamanho_real_string++;
        }
        // Gravamos o cabeçalho
        grava_cabecalho(tamanho_real_string, arquivo, e_ultimo, 's');

        // Escrevemos os valores em si
        fwrite(valores, 1, tamanho_real_string, arquivo);
        
        // Incrementamos o ponteiro pelo tamanho declarado da string, para acessar o próximo campo
        valores+=tamanho_maximo_string;
        
        // Aumentamos o número de bytes percorrido de acordo
        percorrido += tamanho_maximo_string;
      }
     else
      {
        // Se o descritor chegou ao fim, é o último campo
        if (*descritor_auxiliar == '\0')
        {
          e_ultimo = 1;
        }
        
        // Caso o padding não esteja correto, incrementamos o ponteiro de acordo
        if(percorrido % 4 != 0)
        {
          valores += (4 - (percorrido % 4));
          percorrido += (4 - (percorrido % 4));
        }
        
       // Copiamos o ponteiro de valores para iterá-lo sem estragá-lo
       unsigned char * copia_valores = valores;
       
      // Variável para guardar os 4 bytes de um inteiro num vetor, separados byte a byte
       unsigned char * quebrado = (unsigned char *)malloc(4);

       // Colocamos os valores corretos nesse array
       quebrado[3] = *copia_valores;
       copia_valores++;
       
       quebrado[2] = *copia_valores;
       copia_valores++;
       
       quebrado[1] = *copia_valores;
       copia_valores++;

       quebrado[0] = *copia_valores;
       copia_valores++;

       // Calculamos quais os bytes carregam informação útil
       int tam_min = tam_min_inteiro(quebrado, tipo_do_campo);
       
       // Gravamos o cabeçalho
       grava_cabecalho(tam_min, arquivo, e_ultimo, tipo_do_campo);
       
       // Pulamos os bytes que não carregam informação útil
       quebrado += (4 - tam_min);
       
       // Escrevemos os valores em si
       fwrite(quebrado, 1, tam_min, arquivo);
       
       // Incrementamos o ponteiro no tamanho de um inteiro
       valores += 4;
       percorrido += 4;
     }
      
     // Se era o último campo, quebramos o loop
     if(e_ultimo)
     {
       break;
     }
   }
  }
  return 0;
}
// Determina tamanho mínimo para representar um inteiro
int tam_min_inteiro(unsigned char * arr, char tipo)
{
  int tam_min = 0;
  int counter = 0;

  // checa se todos os bytes são iguais e se são iguais a 0
  // se forem, a int pode ser armazenada em um único byte
  if ((*arr | *(arr+1) | *(arr+2) | *(arr+3)) == 0)
  {
    return 1;
  }
  
  // Pulamos os bytes que não carregam informações úteis para aquele tipo
  // 00, no de caso de unsigned ints e ints positivas, e 0xff, no caso de ints negativas
  if(tipo == 'i' && ((*arr & 0b10000000) != 0))
  {
    // checa se todos os bytes são iguais e se são iguais a 0xff
    // se forem, a int pode ser armazenada em um único byte
    if ((*arr & *(arr+1) & *(arr+2) & *(arr+3)) == 0xff)
    {
      return 1;
    }
  
    while(*arr == 0xff)
    {
      arr++;
      counter++;
    }

  }
  else
  {
    while(*arr == 0x00)
    {
      arr++;
      counter++;
    }
  }
  
  // Contamos os que sobraram
  while(counter < 4)
  {
    counter++;
    tam_min++;
  }
  return tam_min;
}

// Grava o byte de cabeçalho
void grava_cabecalho(int tamanho, FILE* arquivo, int e_ultimo, char tipo)
{
  unsigned char cabecalho = 0b00000000;
  
  // Se é o último campo, o primeiro bit é um
  if(e_ultimo)
  {
    cabecalho |= (unsigned char)0b10000000;
  }
  
  // Configuramos os bits de tipo
  if(tipo == 'i')
  {
    cabecalho |= (unsigned char)0b00100000;
  }
  else if(tipo == 's')
  {
    cabecalho |= (unsigned char)0b01000000;
  }
  
  // Configuramos os bits de tamanho
  cabecalho |= (unsigned char)tamanho;
 
  // Escrevemos o cabeçalho
  fwrite(&cabecalho, 1, 1, arquivo);
}

