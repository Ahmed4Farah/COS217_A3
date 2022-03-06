/*--------------------------------------------------------------------*/
/* symtablelist.c                                                     */
/* Author: Ahmed Farah                                                */
/* Implements the Symbol Table abstract data type (ADT), compliant    */
/*with the interface in symtable.h                                    */
/* It uses a linked-list implementation to achieve this               */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "symtable.h"

/* A Binding is an abstract data structure made up of 3 parts: Key,
a pointer to a string (to store the key), Value, which is of type
void * and is a pointer to the value, and psNextBinding, which points
to another binding - It allows the bindings to be strung together to
form a singly-linked list. */
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
  /* A pointer to the first binding in the symbol table */
  struct Binding *psFirstBinding;
  /* The current size of the symbol table */
  size_t size;
};

/* The SymTable constructor */
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

/* The SymTable deconstructor */
void SymTable_free(SymTable_T oSymTable) {
  struct Binding *psCurrentBinding;
  struct Binding *psNextBinding;
  assert(oSymTable != NULL);

  /* We iterate through the linked list from first to last */
  for (psCurrentBinding = oSymTable->psFirstBinding;
    psCurrentBinding != NULL; psCurrentBinding = psNextBinding) {
    psNextBinding = psCurrentBinding->psNextBinding;

    /* Since we create a defensive copy of the key, we have to free-up
    the memory allocation of the key as well */
    free((void *) psCurrentBinding->Key);
    free(psCurrentBinding);
  }
  free(oSymTable);
}

/* Implements the SymTable_getLength() function */
size_t SymTable_getLength(SymTable_T oSymTable) {
  assert(oSymTable != NULL);
  return oSymTable->size;
}

/* Implements the SymTable_put() function */
int SymTable_put(SymTable_T oSymTable, const char *pcKey,
const void *pvValue) {
  struct Binding *psNewBinding;
  char *keyCopy;
  assert(oSymTable != NULL);
  assert(pcKey != NULL);

  if (SymTable_contains(oSymTable, pcKey)) {
    return 0;
  }

  /* We make a memory allocation for the new binding and create it */
  psNewBinding = (struct Binding*) malloc(sizeof(struct Binding));
  if (psNewBinding == NULL) {
    return 0;
  }

  /* We make a defensive copy of the key */
  keyCopy = (char *) calloc(strlen(pcKey) + 1, sizeof(char));
  if (keyCopy == NULL) {
    /* Since we won't be adding psNewBinding to the symbol table,
    we must free it */
    free(psNewBinding);
    return 0;
  }
  keyCopy = strcpy(keyCopy, pcKey);

  /* We fill the binding and add it to the front of the linked list */
  psNewBinding->Key = keyCopy;
  psNewBinding->Value = pvValue;
  psNewBinding->psNextBinding = oSymTable->psFirstBinding;
  oSymTable->psFirstBinding = psNewBinding;
  oSymTable->size++;

  return 1;
}

/* A helper function used by SymTable_replace, SymTable_contains,
and SymTable_get. It takes in a SymTable_T oSymTable and a char * pcKey.
If oSymTable contains a binding with the key pcKey, it returns a pointer
to that binding. Otherwise, it returns Null. It does not change the
contents of oSymTable */
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

/* implements the SymTable_replace() replace function */
void * SymTable_replace(SymTable_T oSymTable, const char *pcKey,
const void *pvValue) {
  struct Binding * desiredBinding;
  const void * oldValue;
  assert(oSymTable != NULL);
  assert(pcKey != NULL);

  desiredBinding = SymTable_find(oSymTable, pcKey);
  if (desiredBinding == NULL) {
    return NULL;
  }
  oldValue = desiredBinding->Value;
  desiredBinding->Value = pvValue;

  /* Here we have to "cast away the constness" */
  return (void *) oldValue;
}

/* implements the SymTable_replace() replace function */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
  assert(oSymTable != NULL);
  assert(pcKey != NULL);
  return (SymTable_find(oSymTable, pcKey) != NULL);
}

/* implements the SymTable_get() replace function */
void * SymTable_get(SymTable_T oSymTable, const char *pcKey) {
  struct Binding * desiredBinding;
  assert(oSymTable != NULL);
  assert(pcKey != NULL);
  desiredBinding = SymTable_find(oSymTable, pcKey);
  if (desiredBinding == NULL) {
    return NULL;
  }
  return (void *) desiredBinding->Value;
}

/* implements the SymTable_remove() replace function */
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

  /* The case where we didn't find the binding corresponding to pcKey*/
  if (psCurrentBinding == NULL) {
    return NULL;
  }

  /* The case where we did but it was the first one in the linked list
  and hence we never updated  psPreviousBinding*/
  if (psPreviousBinding == NULL) {
    oSymTable->psFirstBinding = psCurrentBinding->psNextBinding;
  }

  /* All other cases*/
  else {
    psPreviousBinding->psNextBinding = psCurrentBinding->psNextBinding;
  }

  toReturn = (void *) psCurrentBinding->Value;

  /* Since we created a defensive copy of the key, we have to free that
  too */
  free((void *) psCurrentBinding->Key);
  free(psCurrentBinding);
  oSymTable->size--;
  return toReturn;
}

/* implements the SymTable_map() replace function */
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
