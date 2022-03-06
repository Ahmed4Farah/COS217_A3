/*--------------------------------------------------------------------*/
/* symtablelist.c                                                     */
/* Author: Ahmed Farah                                                */
/* Implements the Symbol Table abstract data type (ADT), compliant    */
/*with the interface in symtable.h                                    */
/* It uses a linked-list implementation to achieve this               */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symtable.h"

typedef struct SymTable * SymTable_T;

/* A Binding is an abstract data structure made up of 3 parts: Key,
a pointer to a string (to store the key), Value, which is of type
void * and is a pointer to the value, and psNextBinding, which points
to another bindind - It allows the bindings to be strung together to
form a singly-linked list */
struct Binding {
  /* Symbol table key */
  const char * Key;
  /* Symbol table value */
  const void * Value;
  /* The next binding  */
  struct Binding * psNextBinding;
};

/* This is a linked-list implementation of a symbol table. SymTable is
an abstract data structure which has 2 fields. First, psFirstBinding,
is a pointer to the first binding in the symbol table, and the second
is the size, which is of type size_t, stores the size of the symbol
table */
struct SymTable {
  struct Binding *psFirstBinding;
  size_t size;
};

/* drafted */
SymTable_T SymTable_new(void) {
  SymTable_T oSymTable;
  oSymTable = (SymTable_T) malloc(sizeof(struct SymTable));
  if (oSymTable == NULL) {
    return NULL;
  }
  oSymTable->psFirstBinding = NULL;
  oSymTable->size = 0;
  return oSymTable;
}

/* drafted */
void SymTable_free(SymTable_T oSymTable) {
  struct Binding *psCurrentBinding;
  struct Binding *psNextBinding;
  assert(oSymTable != NULL);
  for (psCurrentBinding = oSymTable->psFirstBinding;
    psCurrentBinding != NULL; psCurrentBinding = psNextBinding) {
    psNextBinding = psCurrentBinding->psNextBinding;
    free(psCurrentBinding);
  }
  free(oSymTable);
}

/* drafted */
size_t SymTable_getLength(SymTable_T oSymTable) {
  assert(oSymTable != NULL);
  return oSymTable->size;
}

/* drafted */
int SymTable_put(SymTable_T oSymTable, const char *pcKey,
const void *pvValue) {
  struct Binding *psNewBinding;
  assert(oSymTable != NULL);
  assert(pcKey != NULL);

  if (SymTable_contains(oSymTable, pcKey)) {
    return 0;
  }

  psNewBinding = (struct Binding*) malloc(sizeof(struct Binding));
  if (psNewBinding == NULL) {
    return 0;
  }

  char *keyCopy = (char *) calloc(strlen(pcKey) + 1, sizeof(char));
  if (keyCopy == NULL) {
    return 0;
  }
  keyCopy = strcpy(keyCopy, pcKey);

  psNewBinding->Key = keyCopy;
  psNewBinding->Value = pvValue;
  psNewBinding->psNextBinding = oSymTable->psFirstBinding;
  oSymTable->psFirstBinding = psNewBinding;
  oSymTable->size++;

  return 1;
}

/* drafted */
static struct Binding * SymTable_find(SymTable_T oSymTable,
const char *pcKey) {
  struct Binding *psCurrentBinding;
  assert(oSymTable != NULL);
  assert(pcKey != NULL);
  for (psCurrentBinding = oSymTable->psFirstBinding;
    psCurrentBinding != NULL;
    psCurrentBinding = psCurrentBinding->psNextBinding) {
    if (strcmp(psCurrentBinding->Key,pcKey) == 0) {
      return psCurrentBinding;
    }
  }
  return NULL;
}

/* drafted */
void * SymTable_replace(SymTable_T oSymTable, const char *pcKey,
const void *pvValue) {
  assert(oSymTable != NULL);
  assert(pcKey != NULL);

  struct Binding * desiredBinding = SymTable_find(oSymTable, pcKey);
  if (desiredBinding == NULL) {
    return NULL;
  }
  const void * oldValue = desiredBinding->Value;
  desiredBinding->Value = pvValue;
  return (void *) oldValue;
}

/* drafted */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
  assert(oSymTable != NULL);
  assert(pcKey != NULL);
  return (SymTable_find(oSymTable, pcKey) != NULL);
}

/* drafted */
void * SymTable_get(SymTable_T oSymTable, const char *pcKey) {
  assert(oSymTable != NULL);
  assert(pcKey != NULL);
  struct Binding * desiredBinding = SymTable_find(oSymTable, pcKey);
  if (desiredBinding == NULL) {
    return NULL;
  }
  return (void *) desiredBinding->Value;
}

/* drafted */
/* we don't need to store current and previous. Instead we can
just store previous, play around with that */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
  struct Binding *psCurrentBinding;
  struct Binding *psPreviousBinding;
  void * toReturn;
  assert(oSymTable != NULL);
  assert(pcKey != NULL);
  psCurrentBinding = oSymTable->psFirstBinding;
  psPreviousBinding = NULL;
  while (psCurrentBinding != NULL) {
    if (strcmp(psCurrentBinding->Key, pcKey) == 0) {
      break;
    }
    psPreviousBinding = psCurrentBinding;
    psCurrentBinding = psCurrentBinding->psNextBinding;
  }
  if (psCurrentBinding == NULL) {
    return NULL;
  }
  if (psPreviousBinding == NULL) {
    oSymTable->psFirstBinding = psCurrentBinding->psNextBinding;
  }
  else {
    psPreviousBinding->psNextBinding = psCurrentBinding->psNextBinding;
  }
  toReturn = (void *) psCurrentBinding->Value;
  free((void *) psCurrentBinding->Key);
  free(psCurrentBinding);
  oSymTable->size--;
  return toReturn;
}

void SymTable_map(SymTable_T oSymTable,
void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
const void *pvExtra) {
  struct Binding *psCurrentBinding;
  assert(oSymTable != NULL);
  assert(pfApply != NULL);

  for (psCurrentBinding = oSymTable->psFirstBinding;
    psCurrentBinding != NULL;
    psCurrentBinding = psCurrentBinding->psNextBinding) {
    pfApply(psCurrentBinding->Key, (void *) psCurrentBinding->Value,
    (void *) pvExtra);
  }
}
