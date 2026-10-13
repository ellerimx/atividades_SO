/*
Criação de threads POSIX em UNIX, com operação join().

Compilar com gcc -Wall thread-join.c -o thread-join -lpthread

Carlos Maziero, DINF/UFPR 2020

---------------------------------------------------------------------------
Exercício 2 - comentários
---------------------------------------------------------------------------
Mesmo programa do exercício 1, mas agora a thread main ESPERA o término de
cada uma das threads criadas, usando pthread_join(), antes de imprimir
"Main: fim". É o equivalente, para threads, do wait() usado com processos.

As threads são criadas com um atributo (attr) que as define explicitamente
como "joinable" (PTHREAD_CREATE_JOINABLE), ou seja, threads cujo término
pode ser aguardado por outra thread com pthread_join(). Uma thread
"detached" (PTHREAD_CREATE_DETACHED) libera seus recursos sozinha ao
terminar e não pode ser aguardada.

Os joins são feitos em ordem (t00, t01, ..., t15): main bloqueia no join
de t00 até que t00 termine; quando isso acontece as demais threads em geral
já terminaram também (todas dormem os mesmos 3 s), e os joins seguintes
retornam imediatamente. Assim, "Main: fim" é sempre a última mensagem.
*/

#include <pthread.h>  // pthread_*()
#include <stdio.h>    // printf(), perror()
#include <stdlib.h>   // exit()
#include <unistd.h>   // sleep()

#define NUM_THREADS 16

// corpo das threads (igual ao do exercício 1)
void *threadBody (void *id)
{
  long tid = (long) id ;  // ID da thread, recebido por valor

  printf ("t%02ld: Olá!\n", tid) ;
  sleep (3) ;             // suspende somente esta thread
  printf ("t%02ld: Tchau!\n", tid) ;
  pthread_exit (NULL) ;   // termina; o valor NULL seria entregue a quem
                          // fizer pthread_join() sobre esta thread
}

int main (int argc, char *argv[])
{
  pthread_t thread [NUM_THREADS] ;  // identificadores das threads
  pthread_attr_t attr ;             // atributos usados na criação
  long i, status ;

  // para permitir a operação "join" sobre as threads
  pthread_attr_init (&attr) ;       // inicializa attr com valores padrão
  pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE) ;
                                    // threads criadas com attr poderão
                                    // ser aguardadas com pthread_join()

  for (i=0; i<NUM_THREADS; i++)
  {
    printf ("Main: criando thread %02ld\n", i) ;
    // cria a thread i usando os atributos definidos em attr
    status = pthread_create (&thread[i], &attr, threadBody, (void *) i) ;
    if (status)
    {
      perror ("pthread_create") ;
      exit (1) ;
    }
  }

  for (i=0; i<NUM_THREADS; i++)
  {
    printf ("Main: aguardando thread %02ld\n", i);
    // bloqueia main até que thread[i] termine (se ela já terminou,
    // retorna imediatamente). O segundo parâmetro (NULL) indica que o
    // valor de retorno da thread não interessa. Após o join, os recursos
    // da thread terminada (pilha, descritor) são liberados.
    status = pthread_join (thread[i], NULL) ;
    if (status)
    {
      perror ("pthread_join") ;
      exit (1) ;
    }
  }
  // aqui é garantido que todas as threads já terminaram
  printf ("Main: fim\n") ;
  pthread_attr_destroy (&attr) ;    // libera o objeto de atributos
  pthread_exit (NULL) ;             // (aqui, return 0 teria o mesmo efeito,
                                    // pois não restam outras threads)
}
