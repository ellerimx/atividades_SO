// Processo A (versao corrigida: uma fila de resposta por processo A)
#include "filas.h"

static char nome_resp[64];

static void termina (int s) { mq_unlink (nome_resp); _exit (0); }

int main (void)
{
   srandom (time (NULL) ^ getpid ());
   snprintf (nome_resp, sizeof (nome_resp), "/fila-mirelle-resposta-%d", getpid ());
   signal (SIGINT, termina);
   signal (SIGTERM, termina);

   mqd_t pedido   = abre_fila (FILA_PEDIDO, O_WRONLY, sizeof (pedido_id_t));
   mqd_t resposta = abre_fila (nome_resp,   O_RDONLY, sizeof (int));

   while (1)
   {
      pedido_id_t p = { getpid (), random () % 100, random () % 100 };
      int soma;
      if (mq_send (pedido, (char *) &p, sizeof (p), 0) < 0) { perror ("mq_send"); exit (1); }
      if (mq_receive (resposta, (char *) &soma, sizeof (soma), NULL) < 0) { perror ("mq_receive"); exit (1); }
      printf ("A[%d]: %d + %d = %d%s\n", getpid (), p.n1, p.n2, soma,
              (p.n1 + p.n2 == soma) ? "" : "   <-- ERRADO");
      fflush (stdout);
   }
}
