#pragma once

#include "defines.h"

typedef struct SList_tag
{
    void* value;

    struct SList_tag* prev;
    struct SList_tag* next;

} SListElement;

typedef struct
{
    uint32 size;

    SListElement* first;        /* head */
    SListElement* last;         /* tail */
} SList;

SList* ListCreate ();
void ListDestroy (SList** list);

void ListAddElement (SList* list, void* value);
void ListDeleteElementByValue (SList* list, void* value);
void ListDeleteElementByNymber (SList* list, uint32 numOfElement);

void* ListGetElement (SList* list, uint32 numOfElement);
uint32 ListGetSize (SList* list);

void ListClear (SList* list);
