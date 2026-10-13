/*
Cálculo de Pi usando threads POSIX (série de Leibniz-Gregory).

    pi/4 = soma_{n=0}^{inf} (-1)^n / (2n + 1)

Uso:  ./calcpi [num_threads] [num_termos]
      (padrão: 1 thread, 1 bilhão de termos)

Compilar com gcc -Wall -O2 calcpi.c -o calcpi -lm -lpthread

---------------------------------------------------------------------------
Funcionamento
---------------------------------------------------------------------------
O espaço de termos [0, num_termos) é dividido uniformemente entre as N
threads: a thread i calcula os termos [ini_i, fim_i), em que cada bloco tem
num_termos / N termos (o resto da divisão é distribuído, um termo a mais,
entre as primeiras threads). Cada thread soma seus termos em uma variável
LOCAL (na sua própria pilha) e, só ao final, deposita a soma parcial em
result[i]. Como cada thread escreve em uma posição diferente do vetor, não
há condição de disputa e nenhum mecanismo de sincronização é necessário.
O programa principal espera todas as threads (pthread_join), soma os
resultados parciais e multiplica por 4.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define MAX_THREADS 1024
#define TERMOS_PADRAO 1000000000LL   // 10^9 termos

// variáveis globais (apenas lidas pelas threads, exceto result[i])
long long num_termos ;               // total de termos da série
int       num_threads ;              // número de threads de cálculo
double    result [MAX_THREADS] ;     // resultados parciais, um por thread

// corpo das threads de cálculo
void *threadBody (void *id)
{
  long i = (long) (intptr_t) id ;    // índice desta thread (0..N-1)

  // cópias locais das variáveis globais (ficam na pilha da thread)
  long long total = num_termos ;
  int       n     = num_threads ;

  // calcula o intervalo de termos [ini, fim) desta thread
  long long bloco = total / n ;
  long long resto = total % n ;
  long long ini   = i * bloco + (i < resto ? i : resto) ;
  long long fim   = ini + bloco + (i < resto ? 1 : 0) ;

  // soma parcial local; o sinal de (-1)^k depende da paridade de k
  double soma  = 0.0 ;
  double sinal = (ini % 2 == 0) ? 1.0 : -1.0 ;
  for (long long k = ini; k < fim; k++)
  {
    soma  += sinal / (2.0 * k + 1.0) ;
    sinal  = -sinal ;
  }

  // deposita o resultado parcial na posição exclusiva desta thread
  result[i] = soma ;
  return (NULL) ;                    // equivale a pthread_exit (NULL)
}

int main (int argc, char *argv[])
{
  pthread_t thread [MAX_THREADS] ;
  long i ;
  int status ;

  num_threads = (argc > 1) ? atoi (argv[1]) : 1 ;
  num_termos  = (argc > 2) ? atoll (argv[2]) : TERMOS_PADRAO ;

  if (num_threads < 1 || num_threads > MAX_THREADS || num_termos < 1)
  {
    fprintf (stderr, "Uso: %s [num_threads 1..%d] [num_termos]\n",
             argv[0], MAX_THREADS) ;
    exit (1) ;
  }

  // cria as threads de cálculo
  for (i = 0; i < num_threads; i++)
  {
    status = pthread_create (&thread[i], NULL, threadBody, (void *) (intptr_t) i) ;
    if (status)
    {
      perror ("pthread_create") ;
      exit (1) ;
    }
  }

  // espera o término de todas as threads
  for (i = 0; i < num_threads; i++)
  {
    status = pthread_join (thread[i], NULL) ;
    if (status)
    {
      perror ("pthread_join") ;
      exit (1) ;
    }
  }

  // soma os resultados parciais (pi/4) e apresenta o resultado
  double soma = 0.0 ;
  for (i = 0; i < num_threads; i++)
    soma += result[i] ;
  double pi = 4.0 * soma ;

  printf ("threads: %d  termos: %lld\n", num_threads, num_termos) ;
  printf ("pi calculado: %.15f\n", pi) ;
  printf ("erro:         %.3e\n", fabs (pi - M_PI)) ;

  return (0) ;
}
