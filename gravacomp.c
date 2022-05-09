#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

int gravacomp (int nstructs, void* valores, char* descritor, FILE* arquivo)
{
  int i = 0; // função que controla o loop para percorrer todas as strucs
  char *aux = descritor; // ponteiro que permite percorrer a string descritora sem alterar seu ponteiro
  int *tmp; // cursor para percorrer o vetor de valores (structs)
  
  valores = (char *) valores; // cast de void* para char*, o que permiter percorrer o vetor
 
  int n = 0; // variável auxiliar para usar na aritmética de ponteiros e percorrer o vetor valores
  
  int size = 0; // variável para armazenar o tamanho de cada string na struct (se houver);
  
  // fwrite nao permite casting dentro do argumento, entao usei uma variavel para dar um cast de int para unsigned char
  unsigned char uchar_nstructs = (unsigned char) nstructs;

  // escreve byte com número de structs no arquivo
  fwrite(&uchar_nstructs, 1, 1, arquivo);

  // loop para percorrer o vetor de structs
  for (i = 0; i < nstructs; i++)
  {
    // loop para percorrer os elementos de cada struct
    for (aux = descritor; aux != NULL; aux++)
    {
      // O continue em cada if pula para a próxima iteração do loop sem passar pelos if abaixo, para melhorar a perfomance um pouco
      // O break no último if serve para impedir que ocorra segfault 

      // printf("n = %d\n", n);

      // quando o descritor for i, o valor é uma signed int
      if (*aux == 'i')
      {
        // usa aritmética de ponteiros para mover o cursor n posições
        // n é o número de bytes que já foram percorridos
        tmp = valores + n;
        
        // printf("--------------\n");        
        
        // printf("%d\n", *tmp);
        
        // printf("--------------\n");        

        // escreve cabeçalho para ints, só funciona para números positivos
        int_escreve_cabecalho(aux, arquivo, *tmp);

        // escreve unsigned ints sem padding no arquivo
        // funciona para signed ints positivas também
        escreve_uint(arquivo, *tmp);

        // incremento o cursor por 4 unidades, já que na struct cada int ocupa 4 bytes
        n = n + 4;

        // força a próxima iteração do loop
        continue; 
      }

      // quando o descritor for u, o valor é uma unsigned int
      if (*aux == 'u')
      { 
        tmp = valores + n;
       
        // printf("--------------\n");

        // printf("u = %u\n", *tmp);
       
        // printf("--------------\n");        

        // escreve cabeçalho para unsigned ints
        uint_escreve_cabecalho(aux, arquivo, *tmp);

        // escreve unsigned ints sem padding no arquivo
        escreve_uint(arquivo, *tmp);

        // printf("--------------\n");        

        // incremento o cursor por 4 unidades, já que na struct cada unsigned int ocupa 4 bytes
        n = n + 4;
        
        continue;
      }

      // quando o descritor for s, o valor é uma string
      if (*aux == 's')
      {
        tmp = valores + n;
        // pega os dois últimos caracteres do descrior de string e converte para int, assim encontrando o tamnho máximo da string
        size = atoi(aux+1); 
        
        // printf("Tamanho string = %d\n", size);

        // escreve cabeçalho para strings, o tamnho passado usa strlen pois é o número de caracteres realmente presentes na string
        str_escreve_cabecalho(aux, arquivo, strlen(valores + n));

        // printf("--------------\n");        
        
        // escreve string sem padding (caracteres == NULL) no arquivo
        escreve_string(tmp, size, arquivo);

        // printf("\n");
        // printf("--------------\n");        

        // calcula padding que deve ser adicionado para ir para o próximo valor 
        n = n + (4 - (size % 4)) + size;
        
        continue;
      }

      // se o descritor for \0, ele chegou ao final, então o for é encerrado e a próxima struct é percorrida
      if (*aux == '\0')
      {
        break;
      }
    }
  }
  
  return 0;
}

void escreve_string(void *p, int n, FILE *arq) 
{
  // cursor para percorrer a string, caractere por caractere
  unsigned char *p1 = p;

  // loop para percorrer a string até o tamanho ser 0
  // a cada iteração, n diminui 1 unidade
  while(n--) 
  {
    // se o caractere for NULL, ir para a próxima iteração do loop
    if (*p1 == 0x00)
    {
      continue;
    }

    // se não for NULL, escreve o caractere no arquivo
    fwrite(p1, 1, 1, arq);
    
    // printf("%p - %02x\n", p1, *p1);

    // move o cursor para o próximo caractere
    p1++;
  }
}

// função que calcula o menor número de bytes necessário para armazenar um inteiro positivo ou um unsigned int (que sempre é positivo)
int tam_int_pos(void *p) 
{
  unsigned char *p1 = p;
  int tam = 4; // armazena o número de bytes necessário para armazenar o número

  // checa se todos os bytes são iguais e se são iguais a 0xff
  // se forem, a int pode ser armazenada em um único byte
  if ((*p1 | *(p1+1) | *(p1+2) | *(p1+3)) == 0xff)
  {
    return 1;
  }

  // checa se todos os bytes são iguais e se são iguais a 0
  // se forem, a int pode ser armazenada em um único byte
  if ((*p1 | *(p1+1) | *(p1+2) | *(p1+3)) == 0)
  {
    return 1;
  }

  // loop para percorrer cada byte do número
  for (int i = 0; i < 4; i++) 
  { 
    // se o byte não for 0, o loop é interrompido, pois os bytes importam para determinar o valor do número
    if (*p1 != 0x00)
    {
      break;
    }
    else
    {
      // se o byte for zero e aparecer antes de um byte diferente de zero
      // ele não importa para o valor do número e pode ser ignorado
      
      tam--; // tamanho - 1;
      p1++; // avança o ponteiro para o próximo byte
    }
  }

  // retorna o número de bytes significativos do número
  return tam;
}

void str_escreve_cabecalho(void *p, FILE *arq, int size)
{
  unsigned char *p1 = p; // cursor para percorrer a string do descritor
  unsigned char c = 0; // variável para armazenar o byte de cabeçalho 

  // seta os bytes mais a direita como o comprimento da string a ser armazenda
  c |= size << c;

  // seta o sexto bit para 1, por ser uma string
  c |= 1 << 6;

  // se depois do descritor de string (sXX), não houver caractere, ela é o último campo da struct
  if (*(p1 + 3) == '\0')
  {
    c |= 1 << 7; // seta o sétimo bit para 1 
  }

  fwrite(&c, 1, 1, arq); // escreve o cabeçalho no arquivo
}

void int_escreve_cabecalho(char* p, FILE *arq, int num)
{
  // o (num & 0x80000000) verifica se o bit mais a esquerda da int é 1, se for, (num & 0x80000000) == 1 e o número é negativo, caso contrário é positivo
  if((num & 0x80000000) == 0)
  {
    unsigned char c = 0; // variável para armazenar o byte de cabeçalho 
    int size = 0; // variável para armazenar o tamanho do inteiro
    
    // variável para indicar se o número é negativo ou positivo
    // se negativo, é igual 1, caso contrário é igual a 0 
    int e_negativo; 
    
    unsigned char * quebrado; // vetor para armazenar os bytes da int separadamente

    // se depois do descritor de int (i), não houver caractere, ela é o último campo da struct
    if (*(p + 1) == '\0')
    {
      c |= 1 << 7; // seta o sétimo bit para 1
    }
  
    c |= 0b00100000; // seta o quinto bit para 1, indicando que é uma int
    
    quebrado = quebra_uint(num); // separa os bytes da int em uma array de unsigned chars
    size = tam_int_pos(quebrado); // determina o tamanho da int

    decimal_binary(size); // converte o tamanho de decimal para binário
    
    c |= size << c; // seta os bytes mais a direita como o tamanho da int a ser armazenda
    
    fwrite(&c, 1, 1, arq); // escreve o cabeçalho no arquivo
  }
  else
  {
    printf("Nao funciona com numeros negativos\n");
  }
}

void uint_escreve_cabecalho(char* p, FILE *arq, unsigned int num)
{
  unsigned char c = 0; // variável para armazenar o byte de cabeçalho 
  int size = 0; // variável para armazenar o tamanho do inteiro
    
  // variável para indicar se o número é negativo ou positivo
  // se negativo, é igual 1, caso contrário é igual a 0 
  int e_negativo; 
    
  unsigned char * quebrado; // vetor para armazenar os bytes da int separadamente

  // se depois do descritor de int (i), não houver caractere, ela é o último campo da struct
  if (*(p+1) == '\0')
  {
    c |= 1 << 7; // seta o sétimo bit para 1
  }
  
  quebrado = quebra_uint(num); // separa os bytes da unsigned int em uma array de unsigned chars
  size = tam_int_pos(quebrado); // determina o tamanho da unsigned int, que é sempre um inteiro positivo

  decimal_binary(size); // converte o tamanho de decimal para binário
  
  c |= size << c; // seta os bytes mais a direita como o tamanho da int a ser armazenda

  // OBS: Como o quinto e sexto bits de c devem ser 0 para indicar que é uma unsigned int, não é necessário setá-los
}

// converte um número em base 10 para um em base 2 (binário) usando recursão (método de divisões sucessivas)
int decimal_binary(int n)
{
    // se n for 0, não é necessário dividir mais o número, então o resto é zero
    if (n == 0)
    {
      return 0;
    }
    else
    {
      // acha o resto da divisão de n por 2 e soma ao resto da divisão de n/2 por 2 * 10
      return ((n % 2) + 10 * decimal_binary(n/2)); 
    }
}

// separa os bytes de uma unsigned int em um array
unsigned char * quebra_uint(unsigned int inteiro)
{
  unsigned char *arr = (unsigned char *)malloc(4);

  arr[0] = inteiro / 0x01000000;
  arr[1] = ((inteiro) / 0x00010000) % 0x100;
  arr[2] = ((inteiro) / 0x00000100) % 0x100;
  arr[3] = 0x000000ff & inteiro;
  
  return arr;
}

unsigned char * quebra_int(int inteiro, int * e_negativo)
{
  // se o inteiro for negativo, seta e_negativo para 1
  // caso contrário, seta para 0
  if((inteiro & 0x80000000) != 0)
  {
    *e_negativo = 1;
  }
  else
  {
    *e_negativo = 0;
  }

  // transforma a int em um arrray de bytes
  unsigned char * arr = quebra_uint((unsigned int) inteiro);
  
  return arr; // retorna o array
}

void escreve_uint(FILE *arq, unsigned int u)
{
  // cria um array de chars para armazenar os bytes de u
  unsigned char * arr = quebra_uint(u);
  int tamanho = tam_int_pos(arr); // armazena o número de bytes necessário de u
  arr += (4 - tamanho); // seta o array para começar a partir do primeiro byte necessário para determinar o valor de u 
  fwrite(arr, 1, tamanho, arq); // escreve o array de bytes no arquivo
}

void escreve_int(FILE *arq, int d)
{
  int e_negativo; // variável que indica se um número é negativo ou não
  unsigned char * arr = quebra_int(d, &e_negativo); // separa a int em um array de unsigned chars
  int tamanho = 0; // inicia o tamanho da int como 0

  // se o número for negativo, multipla ele por menos -1 para torná-lo positivo
  if(e_negativo)
  {
    d *= -1;
  }
  else
  {
    // caso contrário, só calcula o seu tamanho
    tamanho = tam_int_pos(&d);
  }

  arr += (4 - tamanho); // seta o array para começar a partir do primeiro byte necessário para determinar o valor de d
  
  complemento_a_dois(arr, tamanho); // Calcula complemento a dois do número
  
  fwrite(arr, 1, tamanho, arq); // escreve o número
}

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
