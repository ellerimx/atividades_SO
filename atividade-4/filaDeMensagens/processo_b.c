// Processo B (versao basica: 2 filas)
#include "filas.h"

int main (void)
{
   mqd_t pedido   = abre_fila (FILA_PEDIDO,   O_RDONLY, sizeof (pedido_t));
   mqd_t resposta = abre_fila (FILA_RESPOSTA, O_WRONLY, sizeof (int));

   while (1)
   {
      pedido_t p;
      if (mq_receive (pedido, (char *) &p, sizeof (p), NULL) < 0) { perror ("mq_receive"); exit (1); }
      int soma = p.n1 + p.n2;
      printf ("B[%d]: recebeu %d e %d\n", getpid (), p.n1, p.n2);
      fflush (stdout);
      sleep (1);
      if (mq_send (resposta, (char *) &soma, sizeof (soma), 0) < 0) { perror ("mq_send"); exit (1); }
   }
}
