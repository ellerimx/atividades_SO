// ==================================
// Atividade 1 - Biblioteca de filas
// Disciplina: SO
// Aluna: Mirelle Casimiro
// ==================================

#include <stdio.h>
#include "queue.h"

// verifica se o elemento pertence a fila indicada
// retorno: 1 se pertence, 0 senao
static int queue_contains(queue_t *queue, queue_t *elem)
{
    if (queue == NULL || elem == NULL)
        return 0;

    queue_t *aux = queue;

    do
    {
        if (aux == elem)
            return 1;

        aux = aux->next;

    } while (aux != queue);

    return 0;
}

//===========
// insere um elemento no final da fila
// retorno: 0 se sucesso ou <0 se ocorreu algum erro

int queue_append(queue_t **queue, queue_t *elem)
{
    // a fila deve existir
    if (queue == NULL)
    {
        fprintf(stderr, "queue_append: o ponteiro da fila eh nulo. nao eh possivel realizar a insercao\n");
        return -1;
    }

    // o elemento deve existir
    if (elem == NULL)
    {
        fprintf(stderr, "queue_append: o ponteiro do elemento eh nulo. nao eh possivel realizar a insercao\n");
        return -2;
    }

    // o elemento nao pode ja pertencer a uma fila
    if (elem->next != NULL || elem->prev != NULL)
    {
        fprintf(stderr, "queue_append: o elemento ja esta numa fila. nao eh possivel realizar a insercao\n");
        return -3;
    }

    // fila vazia
    if (*queue == NULL)
    {
        elem->next = elem;
        elem->prev = elem;
        *queue = elem;
    }
    else
    {
        // insere no final da fila circular
        elem->next = *queue;
        elem->prev = (*queue)->prev;

        (*queue)->prev->next = elem;
        (*queue)->prev = elem;
    }

    return 0;
}

//===========
// remove o elemento indicado da fila, sem destruir
// retorno: 0 se sucesso ou <0 se ocorreu algum erro

int queue_remove(queue_t **queue, queue_t *elem)
{
    // a fila deve existir
    if (queue == NULL)
    {
        fprintf(stderr, "queue_remove: o ponteiro da fila eh nulo. nao eh possivel realizar a remocao\n");
        return -1;
    }

    // a fila nao pode estar vazia
    if (*queue == NULL)
    {
        fprintf(stderr, "queue_remove: a fila esta vazia. nao eh possivel realizar a remocao\n");
        return -2;
    }

    // o elemento deve existir
    if (elem == NULL)
    {
        fprintf(stderr, "queue_remove: o ponteiro do elemento eh nulo. nao eh possivel realizar a remocao\n");
        return -3;
    }

    // o elemento deve pertencer a fila indicada
    if (!queue_contains(*queue, elem))
    {
        fprintf(stderr, "queue_remove: o elemento nao esta na fila. nao eh possivel realizar a remocao\n");
        return -4;
    }

    // fila com apenas 1 elemento
    if (elem->next == elem && elem->prev == elem)
    {
        *queue = NULL;
    }
    else
    {
        // remove o elemento da lista circular
        elem->prev->next = elem->next;
        elem->next->prev = elem->prev;

        // se era o primeiro elemento, atualiza o inicio da fila
        if (*queue == elem)
        {
            *queue = elem->next;
        }
    }

    // desconecta o elemento removido
    elem->next = NULL;
    elem->prev = NULL;

    return 0;
}

//==========
// conta o numero de elementos na fila

int queue_size(queue_t *queue)
{
    if (queue == NULL)
        return 0;

    int cont = 0;
    queue_t *aux = queue;

    do
    {
        cont++;
        aux = aux->next;

    } while (aux != queue);

    return cont;
}

// =========
// percorre a fila e imprime seu conteudo

void queue_print(char *name, queue_t *queue, void print_elem(void *))
{
    if (name != NULL)
        printf("%s", name);

    printf("[");

    if (queue != NULL && print_elem != NULL)
    {
        queue_t *aux = queue;

        do
        {
            print_elem(aux);
            aux = aux->next;

            if (aux != queue)
                printf(" ");

        } while (aux != queue);
    }

    printf("]\n");
}