// Processo B (versao corrigida: responde na fila do A que fez o pedido)
#include "filas.h"

int main (void)
{
   mqd_t pedido = abre_fila (FILA_PEDIDO, O_RDONLY, sizeof (pedido_id_t));

   while (1)
   {
      pedido_id_t p;
      if (mq_receive (pedido, (char *) &p, sizeof (p), NULL) < 0) { perror ("mq_receive"); exit (1); }
      int soma = p.n1 + p.n2;
      printf ("B[%d]: recebeu %d e %d de A[%d]\n", getpid (), p.n1, p.n2, p.pid);
      fflush (stdout);
      sleep (1);

      char nome[64];
      snprintf (nome, sizeof (nome), "/fila-mirelle-resposta-%d", p.pid);
      mqd_t resposta = mq_open (nome, O_WRONLY);
      if (resposta == (mqd_t) -1) { perror ("mq_open (resposta)"); continue; }  // A ja terminou
      if (mq_send (resposta, (char *) &soma, sizeof (soma), 0) < 0) perror ("mq_send");
      mq_close (resposta);
   }
}
