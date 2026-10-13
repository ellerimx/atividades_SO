/*
Criação de threads POSIX em UNIX, com impressão de variáveis.

Compilar com gcc -Wall thread-print.c -o thread-print -lpthread

Carlos Maziero, DINF/UFPR 2020

---------------------------------------------------------------------------
Exercício 3 - comentários
---------------------------------------------------------------------------
Mesmo esquema do exercício 1, mas agora as threads manipulam uma variável
GLOBAL x. Como as threads de um processo compartilham o mesmo espaço de
endereçamento, existe UMA ÚNICA variável x, vista e alterada por todas as
16 threads: cada incremento feito por uma thread é visível às demais.

Cada thread incrementa x duas vezes (antes e depois do sleep), então o
valor final esperado seria 2 x 16 = 32. Entretanto x++ não é atômico: ele
é composto por três operações (ler x da memória, somar 1, escrever x).
Se duas threads executarem x++ ao mesmo tempo, ambas podem ler o mesmo
valor e um dos incrementos se perde (condição de disputa / race
condition). Por isso, em algumas execuções aparecem valores repetidos e o
valor final fica menor que 32. Para evitar isso seria necessário um
mecanismo de sincronização (mutex, operação atômica, etc.).

Compare com o fork-print.c do Lab 2: lá cada processo tinha a SUA cópia de
x e a alteração do filho não era vista pelo pai.
*/

#include <pthread.h>  // pthread_*()
#include <stdio.h>    // printf(), perror()
#include <stdlib.h>   // exit()
#include <unistd.h>   // sleep()

#define NUM_THREADS 16

int x = 0 ;  // variável global: uma única instância, compartilhada por
             // todas as threads do processo

void *threadBody (void *id)
{
  long tid = (long) id ;  // variável local: cada thread tem a sua (pilha)

  x++ ;  // incrementa a variável COMPARTILHADA (operação não atômica:
         // lê x, soma 1, escreve x; sujeita a condição de disputa)
  printf ("t%02ld: Olá!   (x=%02d)\n", tid, x) ;  // mostra o valor atual,
         // que já pode ter sido alterado por outras threads
  sleep (3) ;
  x++ ;  // segundo incremento: soma-se aos 16 primeiros e aos das threads
         // que acordaram antes desta
  printf ("t%02ld: Tchau! (x=%02d)\n", tid, x) ;
  pthread_exit (NULL) ;
}

int main (int argc, char *argv[])
{
  pthread_t thread [NUM_THREADS] ;
  long i, status ;

  for (i=0; i<NUM_THREADS; i++)
  {
    printf ("Main: criando thread %02ld\n", i) ;
    // cria a thread i, que executará threadBody(i)
    status = pthread_create (&thread[i], NULL, threadBody, (void *) i) ;
    if (status)
    {
      perror ("pthread_create") ;
      exit (1) ;
    }
  }
  printf ("Main: fim\n") ;
  pthread_exit (NULL) ;   // termina main; as threads continuam e o
                          // processo acaba quando a última terminar
}
