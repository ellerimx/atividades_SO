// Atividade 6 - Coordenacao por espera ocupada
// Uso: ./espera_ocupada <modo> <segundos> <imprime>
//   modo: 0 = sem sincronizacao, 1 = alternancia estrita, 2 = Peterson
//   segundos: duracao da execucao (padrao 5)
//   imprime: 1 = imprime cada operacao (padrao), 0 = modo silencioso
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>
#include <unistd.h>
#include "queue.h"

#define TAM 10

typedef struct item_t
{
   struct item_t *prev, *next;   // deve ser o 1o campo (compativel com queue_t)
   int valor;
} item_t;

static item_t itens[TAM];
static item_t *fila = NULL;

static int modo = 0, imprime = 1;
static atomic_int parar = 0;

// alternancia estrita
static atomic_int vez = 0;
// Peterson
static atomic_int quer[2] = {0, 0};
static atomic_int turno = 0;

static long insercoes[2][16];    // contadores (separados para evitar false sharing)
static long erros[2][16];        // falhas detectadas pela biblioteca de fila


static void entra (int t)
{
   int o = 1 - t;
   if (modo == 1)
   {
      while (atomic_load (&vez) != t && !atomic_load (&parar)) ;
   }
   else if (modo == 2)
   {
      atomic_store (&quer[t], 1);
      atomic_store (&turno, o);
      while (atomic_load (&quer[o]) && atomic_load (&turno) == o && !atomic_load (&parar)) ;
   }
}

static void sai (int t)
{
   if (modo == 1) atomic_store (&vez, 1 - t);
   else if (modo == 2) atomic_store (&quer[t], 0);
}

static void *tarefa (void *arg)
{
   int t = (int) (long) arg;
   unsigned int semente = 1234 + t;

   while (!atomic_load (&parar))
   {
      entra (t);
      if (modo != 0 && atomic_load (&parar)) { sai (t); break; }

      item_t *it = fila;                  // primeiro elemento
      int velho = it ? it->valor : -1;
      if (queue_remove ((queue_t **) &fila, (queue_t *) it) < 0) { erros[t][0]++; sai (t); continue; }
      int novo = rand_r (&semente) % 100;
      it->valor = novo;
      if (queue_append ((queue_t **) &fila, (queue_t *) it) < 0) { erros[t][0]++; sai (t); continue; }
      insercoes[t][0]++;

      if (imprime)
      {
         printf ("thread %d: tira %d, p\303\265e %d, fila: ", t, velho, novo);
         if (fila)
         {
            item_t *p = fila; int n = 0;
            do { printf ("%d ", p->valor); p = p->next; } while (p && p != fila && ++n < 64);
         }
         printf ("\n");
      }
      sai (t);
   }
   return NULL;
}

int main (int argc, char **argv)
{
   int segundos = 5;
   if (argc > 1) modo = atoi (argv[1]);
   if (argc > 2) segundos = atoi (argv[2]);
   if (argc > 3) imprime = atoi (argv[3]);

   srand (42);
   for (int i = 0; i < TAM; i++)
   {
      itens[i].valor = rand () % 100;
      itens[i].prev = itens[i].next = NULL;
      queue_append ((queue_t **) &fila, (queue_t *) &itens[i]);
   }

   pthread_t th[2];
   struct timespec a, b;
   clock_gettime (CLOCK_MONOTONIC, &a);
   for (long i = 0; i < 2; i++) pthread_create (&th[i], NULL, tarefa, (void *) i);
   sleep (segundos);
   atomic_store (&parar, 1);
   for (int i = 0; i < 2; i++) pthread_join (th[i], NULL);
   clock_gettime (CLOCK_MONOTONIC, &b);

   double dt = (b.tv_sec - a.tv_sec) + (b.tv_nsec - a.tv_nsec) / 1e9;
   long total = insercoes[0][0] + insercoes[1][0];
   int tam = queue_size ((queue_t *) fila);

   fprintf (stderr, "\n--- resultado (modo %d) ---\n", modo);
   fprintf (stderr, "tempo: %.3f s\n", dt);
   fprintf (stderr, "insercoes: thread 0 = %ld, thread 1 = %ld, total = %ld\n",
            insercoes[0][0], insercoes[1][0], total);
   fprintf (stderr, "insercoes por segundo: %.0f\n", total / dt);
   fprintf (stderr, "erros detectados na fila: %ld\n", erros[0][0] + erros[1][0]);
   fprintf (stderr, "tamanho final da fila: %d (esperado %d)\n", tam, TAM);
   return 0;
}
