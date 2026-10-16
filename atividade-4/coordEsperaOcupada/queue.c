#include <stdio.h>
#include "queue.h"

#define LIMITE 1000   // evita laco infinito caso a fila esteja corrompida

int queue_size (queue_t *queue)
{
   if (queue == NULL) return 0;
   int n = 1;
   queue_t *p = queue->next;
   while (p != NULL && p != queue && n < LIMITE) { n++; p = p->next; }
   return n;
}

void queue_print (char *name, queue_t *queue, void print_elem (void *))
{
   printf ("%s [", name);
   if (queue != NULL)
   {
      int n = 0;
      queue_t *p = queue;
      do {
         print_elem (p);
         p = p->next;
         if (++n >= LIMITE) break;
         if (p != queue && p != NULL) printf (" ");
      } while (p != NULL && p != queue);
   }
   printf ("]\n");
}

int queue_append (queue_t **queue, queue_t *elem)
{
   if (queue == NULL) { fprintf (stderr, "Erro: fila inexistente\n"); return -1; }
   if (elem == NULL)  { fprintf (stderr, "Erro: elemento inexistente\n"); return -1; }
   if (elem->next != NULL || elem->prev != NULL)
   { fprintf (stderr, "Erro: elemento ja pertence a uma fila\n"); return -1; }

   if (*queue == NULL)
   {
      elem->next = elem->prev = elem;
      *queue = elem;
   }
   else
   {
      queue_t *first = *queue, *last = first->prev;
      if (last == NULL) { fprintf (stderr, "Erro: fila corrompida\n"); return -1; }
      last->next = elem;
      elem->prev = last;
      elem->next = first;
      first->prev = elem;
   }
   return 0;
}

int queue_remove (queue_t **queue, queue_t *elem)
{
   if (queue == NULL || *queue == NULL) { fprintf (stderr, "Erro: fila inexistente ou vazia\n"); return -1; }
   if (elem == NULL) { fprintf (stderr, "Erro: elemento inexistente\n"); return -1; }
   if (elem->next == NULL || elem->prev == NULL)
   { fprintf (stderr, "Erro: elemento nao pertence a nenhuma fila\n"); return -1; }

   // confere se o elemento esta nesta fila
   int n = 0, achou = 0;
   queue_t *p = *queue;
   do {
      if (p == elem) { achou = 1; break; }
      p = p->next;
   } while (p != NULL && p != *queue && ++n < LIMITE);
   if (!achou) { fprintf (stderr, "Erro: elemento nao pertence a esta fila\n"); return -1; }

   if (elem->next == elem)
      *queue = NULL;
   else
   {
      elem->prev->next = elem->next;
      elem->next->prev = elem->prev;
      if (*queue == elem) *queue = elem->next;
   }
   elem->next = elem->prev = NULL;
   return 0;
}
