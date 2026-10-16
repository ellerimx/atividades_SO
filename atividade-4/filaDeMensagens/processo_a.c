// Processo A (versao basica: 2 filas)
#include "filas.h"

int main (void)
{
   srandom (time (NULL) ^ getpid ());
   mqd_t pedido   = abre_fila (FILA_PEDIDO,   O_WRONLY, sizeof (pedido_t));
   mqd_t resposta = abre_fila (FILA_RESPOSTA, O_RDONLY, sizeof (int));

   while (1)
   {
      pedido_t p = { random () % 100, random () % 100 };
      int soma;
      if (mq_send (pedido, (char *) &p, sizeof (p), 0) < 0) { perror ("mq_send"); exit (1); }
      if (mq_receive (resposta, (char *) &soma, sizeof (soma), NULL) < 0) { perror ("mq_receive"); exit (1); }
      printf ("A[%d]: %d + %d = %d%s\n", getpid (), p.n1, p.n2, soma,
              (p.n1 + p.n2 == soma) ? "" : "   <-- ERRADO (soma de outro pedido)");
      fflush (stdout);
   }
}
