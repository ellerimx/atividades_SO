#ifndef FILAS_H
#define FILAS_H
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define FILA_PEDIDO   "/fila-mirelle-pedido"    // A -> B
#define FILA_RESPOSTA "/fila-mirelle-resposta"  // B -> A (versao basica)

// mensagem de pedido (versao basica: apenas os dois numeros)
typedef struct { int n1, n2; } pedido_t;

// mensagem de pedido (versao corrigida: identifica quem pediu)
typedef struct { pid_t pid; int n1, n2; } pedido_id_t;

static inline mqd_t abre_fila (const char *nome, int flags, size_t tam)
{
   struct mq_attr attr;
   attr.mq_flags = 0;
   attr.mq_maxmsg = 10;
   attr.mq_msgsize = tam;
   attr.mq_curmsgs = 0;
   mqd_t q = mq_open (nome, flags | O_CREAT, 0666, &attr);
   if (q == (mqd_t) -1) { perror ("mq_open"); exit (1); }
   return q;
}
#endif
