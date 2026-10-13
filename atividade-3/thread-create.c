/*
Criação de threads POSIX em UNIX.

Compilar com gcc -Wall thread-create.c -o thread-create -lpthread

Carlos Maziero, DINF/UFPR 2020

---------------------------------------------------------------------------
Exercício 1 - comentários
---------------------------------------------------------------------------
O programa cria NUM_THREADS (16) threads dentro do MESMO processo. Ao
contrário do fork(), que duplica o processo inteiro, pthread_create() cria
apenas um novo fluxo de execução, que compartilha com as demais threads o
código, as variáveis globais, o heap e os arquivos abertos; cada thread tem
apenas a sua própria pilha e o seu próprio contexto de CPU (registradores).

Cada thread executa a função threadBody(), recebendo como parâmetro um
número de identificação (0..15). Ela imprime "Olá", dorme 3 segundos e
imprime "Tchau". A thread principal (main) apenas cria as threads, imprime
"Main: fim" e encerra com pthread_exit(), sem esperar pelas outras.

Como main termina com pthread_exit() (e não com return/exit()), somente a
thread main termina: o processo continua existindo até que a última thread
encerre. Se main usasse return ou exit(), o processo inteiro (com todas as
threads) seria encerrado imediatamente e as mensagens "Tchau" (e talvez
algumas "Olá") nunca seriam impressas.
*/

#include <pthread.h>  // pthread_create(), pthread_exit(), pthread_t
#include <stdio.h>    // printf(), perror()
#include <stdlib.h>   // exit()
#include <unistd.h>   // sleep()

#define NUM_THREADS 16

// Corpo das threads: função executada por cada thread criada.
// Recebe e retorna void*, conforme exigido por pthread_create().
void *threadBody (void *id)
{
  // o identificador foi passado "disfarçado" como ponteiro; aqui ele é
  // convertido de volta para inteiro (tid fica na pilha DESTA thread)
  long tid = (long) id ; // ID da thread

  printf ("t%02ld: Olá!\n", tid) ;
  sleep (3) ;            // suspende apenas esta thread por 3 s; as demais
                         // continuam executando normalmente
  printf ("t%02ld: Tchau!\n", tid) ;

  // encerra esta thread (NULL = nenhum valor de retorno para quem fizer
  // join); equivale a "return NULL"
  pthread_exit (NULL) ;
}

int main (int argc, char *argv[])
{
  pthread_t thread [NUM_THREADS] ;  // identificadores das threads criadas
  long i, status ;

  for (i=0; i<NUM_THREADS; i++)
  {
    printf ("Main: criando thread %02ld\n", i) ;

    // cria uma nova thread:
    //  - &thread[i]: onde será guardado o identificador da nova thread;
    //  - NULL:       atributos padrão (a thread é "joinable" por padrão);
    //  - threadBody: função que a nova thread executará;
    //  - (void *) i: argumento repassado a threadBody. O VALOR de i é
    //    passado (e não &i), pois i muda a cada iteração e é
    //    compartilhado: passar &i faria as threads lerem valores errados.
    // A nova thread fica pronta e pode começar a executar a qualquer
    // momento: antes, durante ou depois do retorno de pthread_create().
    status = pthread_create (&thread[i], NULL, threadBody, (void *) i) ;

    if (status)  // retorno != 0 indica erro (o código de erro é o próprio
    {            // valor retornado; pthread_create não usa errno)
      perror ("pthread_create") ;
      exit (1) ;
    }
  }
  printf ("Main: fim\n") ;

  // encerra apenas a thread main; o processo só termina quando a última
  // thread em execução terminar
  pthread_exit (NULL) ;
}
