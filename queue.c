#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* static functions used for sorting */
static void divide(queue_t *a, queue_t *b);
static void merge(queue_t *a, volatile queue_t *b);

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q;
    if (!(q = malloc(sizeof(queue_t))))
        return NULL;

    q->head = NULL;
    q->tail = &(q->head);
    q->size = 0;

    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q)
        return;

    list_ele_t *cur;
    while (q->head) {
        cur = q->head;
        q->head = cur->next;

        free(cur->value);
        free(cur);
    }


    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (!q)
        return false;

    size_t slen = strlen(s) + 1;
    list_ele_t *newh;
    if (!(newh = malloc(sizeof(list_ele_t))))
        return false;

    if (!(newh->value = malloc(slen))) {
        free(newh);
        return false;
    }

    memcpy(newh->value, s, slen);

    newh->next = q->head;
    q->head = newh;
    q->size++;
    if (!(newh->next))
        q->tail = &(newh->next);

    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (!q)
        return false;

    size_t slen = strlen(s) + 1;
    list_ele_t *newt;
    if (!(newt = malloc(sizeof(list_ele_t))))
        return false;

    if (!(newt->value = malloc(slen))) {
        free(newt);
        return false;
    }

    memcpy(newt->value, s, slen);

    newt->next = NULL;
    *(q->tail) = newt;
    q->tail = &(newt->next);
    q->size++;

    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || !(q->head))
        return false;

    list_ele_t *rem = q->head;
    size_t remlen = strlen(rem->value);
    size_t cpylen = remlen < bufsize - 1 ? remlen : bufsize - 1;

    if (sp) {
        memcpy(sp, rem->value, cpylen);
        sp[cpylen] = '\0';
    }

    q->head = rem->next;
    q->size--;
    if (!(q->head))
        q->tail = &(q->head);

    free(rem->value);
    free(rem);

    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    return !q ? 0 : q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q || !(q->head) || !(q->head->next))
        return;

    q->tail = &(q->head->next);

    list_ele_t *tmp_head = NULL, *cur;
    while (q->head) {
        cur = q->head;
        q->head = cur->next;

        cur->next = tmp_head;
        tmp_head = cur;
    }

    q->head = tmp_head;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (!q || !(q->head) || !(q->head->next))
        return;

    queue_t b;
    divide(q, &b);
    q_sort(q);
    q_sort(&b);
    merge(q, &b);
}

/*
 * Attempt to divide elements of a into b
 */
static void divide(queue_t *a, queue_t *b)
{
    b->size = a->size / 2;
    b->tail = a->tail;

    list_ele_t *cut = a->head;
    for (size_t i = b->size; i > 1; i--)
        cut = cut->next;

    b->head = cut->next;

    cut->next = NULL;
    a->tail = &(cut->next);
    a->size -= b->size;
}

/*
 * Attempt to merge a and b into a in acensing order.
 */
static void merge(queue_t *a, volatile queue_t *b)
{
    list_ele_t *tmp, **ori_tail = a->tail;
    for (a->tail = &(a->head); *(a->tail) && b->head;
         a->tail = &((*(a->tail))->next)) {
        if (strcmp((*(a->tail))->value, b->head->value) > 0) {
            tmp = b->head;
            b->head = b->head->next;

            tmp->next = *(a->tail);
            *(a->tail) = tmp;
        }
    }

    if (b->head) {
        *(a->tail) = b->head;
        a->tail = b->tail;
    } else
        a->tail = ori_tail;

    a->size += b->size;
}