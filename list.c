#include <stdio.h>
#include <stdlib.h>


#include "list.h"


SList* ListCreate()
{
    SList* list = (SList*) malloc(sizeof(SList));
    list->first = NULL;
    list->last = NULL;
    list->size = 0;

    return list;
}

void ListDestroy (SList** list)
{
    if (list == NULL || *list == NULL)
        return;

    ListClear (*list);

    free (*list);
    *list = NULL;
}

void ListAddElement (SList* list, void* value)
{
    if (list == NULL)
        return;

    SListElement* newElement = (SListElement*) malloc(sizeof(SListElement));
    newElement->value = value;
    newElement->next = NULL;
    list->size++;

    // if it is first element
    if (list->first == NULL)
    {
        newElement->prev = NULL;
        list->first = newElement;
        list->last = newElement;

        //printf ("add as first\n");
    }
    else
    {
        newElement->prev = list->last;
        list->last->next = newElement;

        list->last = newElement;

        //printf ("add to end\n");
    }
}

void ListDeleteElementByValue (SList* list, void* value)
{
    if (list == NULL || value == NULL)
        return;

    if (list->first == NULL)
        return;

    // пробегаемся по всем элементам, пока не наткнемся на значение или на конец...
    SListElement* element = list->first;
    while ((element->value != value) && (element != NULL))
        element = element->next;

    // если что-то нашёл, то удаляем элемент и сцепляем соседей
    if (element != NULL && element->value == value)
    {
        //printf ("try to delete value...");
        if (element->value)
        {
            free (element->value);
            element->value = NULL;
        }
        //printf ("\n\t\tdone\n");

        list->size --;

        // элемент где-то между головой и хвостом
        if (element->prev != NULL && element->next != NULL)
        {
            //printf ("deleting anything...");
            element->prev->next = element->next;
            element->next->prev = element->prev;
            //printf ("\n\t\tdone\n");

            goto element_delete;
        }

        // элемент головной
        if (element->prev == NULL && element->next != NULL)
        {
            //printf ("deleting head...");
            list->first = element->next;
            element->next->prev = NULL;
            //printf ("\n\t\tdone\n");

            goto element_delete;
        }

        // элемент хвостовой
        if (element->prev != NULL && element->next == NULL)
        {
            //printf ("deleting tail...");
            list->last = element->prev;
            element->prev->next = NULL;
            //printf ("\n\t\tdone\n");

            goto element_delete;
        }

        // элемент последний
        if ((list->size <= 0) || (element->prev == NULL && element->next == NULL))
        {
            //printf ("deleting last...");

            // уже всё удалено. Гудбай
            list->first = NULL;
            list->last = NULL;
            //element = NULL;

            list->size = 0;

            //printf ("all deleted! list is empty now!\n");
        }


        element_delete:
        //printf ("try to delete element...");
        if (element)
        {
            free (element);
            element = NULL;
        }
        //printf ("\n\t\tdone\n");
    }
}

void ListDeleteElementByNymber (SList* list, uint32 numOfElement)
{
    if (list == NULL || (numOfElement > list->size))
        return;

    uint32 num = 0;
    SListElement* element = list->first;
    while ((num != numOfElement) && (element != NULL))
        element = element->next;

    if (element != NULL && element->value != NULL)
        ListDeleteElementByValue (list, element->value);
}

void* ListGetElement (SList* list, uint32 numOfElement)
{
    if (list == NULL || (numOfElement > list->size))
        return NULL;

    /* search element for return value */
    SListElement* element = list->first;
    for (uint32 i = 0; i < numOfElement; i++)
    {
        element = element->next;
    }

    if (element == NULL)
        return NULL;
    else
        return element->value;
}

uint32 ListGetSize (SList* list)
{
    if (list == NULL)
        return 0;

    return list->size;
}

void ListClear (SList* list)
{
    if (list == NULL || list->first == NULL)
        return;

    /* проходимся от первого до последнего */
    SListElement* element = list->first;
    SListElement* nextElement;
    while (element != NULL)
    {
        nextElement = element->next;

        /* удаляем значение, если таковое имеется */
        if (element->value)
        {
            //printf("\n\t\ttry to delete value...");
            free (element->value);
            element->value = NULL;
            //printf ("done\n");
        }

        /* удаляем элемент, если есть */
        if (element)
        {
            //printf("\n\t\ttry to delete element...");
            free (element);
            element = NULL;
            //printf ("done\n");
        }

        element = nextElement;
    }

    /* чистим указатели на первый и последний элементы */
    if (list->first)
        list->first = NULL;
    if (list->last)
        list->last = NULL;

    list->size = 0;
}
