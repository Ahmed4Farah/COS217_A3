/*--------------------------------------------------------------------*/
/* symtablehash.h                                                     */
/* Author: Ahmed Farah                                                */
/* Implements the Symbol Table abstract data type (ADT), compliant    */
/* with the interface in symtable.h                                   */
/* It uses a hash-table implementation to achieve this                */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "symtable.h"

/* A global variable which specifies the sequence of numbers dictating
the number of buckets our hash table will have when it expands. It
starts out with 509 buckets, and then expands to the next size as
needed. It does not expand any further once it hits 65521 buckets. */
static size_t SIZES[] = {509, 1021, 2039, 4093, 8191, 16381, 32749,
65521};
/* The maximum number of buckets allowed. We won't go above this */
static size_t MAX_SIZE = 65521;

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

/* This is a hash-table implementation of a symbol table. SymTable is
an abstract data structure which has 3 fields. First, Bindings is an
array of pointers to bindings. It is realized as a variable of type
struct Binding **. The second is the size, which is of type size_t,
and stores the current number of elements in the symbol table. The
third is the bucketCountOrder, and it is the index of the element in
SIZES[] which corresponds to the current number of buckets in the
symbol table. It starts off at 0 and is incremented by 1 for every
expansion of Bindindgs. */
struct SymTable {
  /* The buckets of the hash table. An array of pointers to bindings */
  struct Binding ** Bindings;
  /* The current size of the symbol table */
  size_t size;
  /* index of the element in SIZES[] which corresponds to the current
  number of buckets in the symbol table */
  size_t bucketCountOrder;
};

/* The SymTable constructor */
SymTable_T SymTable_new(void) {
  SymTable_T oSymTable;
  oSymTable = (SymTable_T) malloc(sizeof(struct SymTable));
  if (oSymTable == NULL) {
    return NULL;
  }
  oSymTable->Bindings = calloc(SIZES[0], sizeof(struct Binding *));
  if (oSymTable->Bindings == NULL) {
    /* We must remember to free oSymTable, since we won't be actually
    making a symbol table */
    free(oSymTable);
    return NULL;
  }
  oSymTable->bucketCountOrder = 0;
  oSymTable->size = 0;
  return oSymTable;
}

/* Helper function which frees up an array of pointers to bindings. It
takes in an array of pointers to bindings (of type struct Binding **)
called Bindings, and a size_t variable called size, which is the size
of this array. It then frees up all the memory associated with Bindings,
and returns nothing. It is called by SymTable_free as well as
SymTable_expand */
static void SymTable_free_Bindings(struct Binding ** Bindings,
size_t size) {
  struct Binding * psCurrentBinding;
  struct Binding * psNextBinding;
  size_t hash;
  assert(Bindings != NULL);
  assert(size > 0);

  for (hash = 0; hash < size; hash++){
    for (psCurrentBinding = Bindings[hash];
      psCurrentBinding != NULL; psCurrentBinding = psNextBinding) {
      psNextBinding = psCurrentBinding->psNextBinding;
      /* Since we create a defensive copy of the key, we have to free-up
      the memory allocation of the key as well */
      free((void *) psCurrentBinding->Key);
      free(psCurrentBinding);
    }
  }
  free(Bindings);
}

/* A Helper function which takes in a string, pcKey, and a size_t called
uBucketCount, which represents the number of buckets available. It
then returns a hash code for pcKey that is between 0 and uBucketCount-1,
inclusive, as a size_t variable. It is called directly by
SymTable_expand, SymTable_find, and SymTable_remove */
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount) {
  const size_t HASH_MULTIPLIER = 65599;
  size_t u;
  size_t uHash = 0;
  assert(pcKey != NULL);
  assert(uBucketCount > 0);

  for (u = 0; pcKey[u] != '\0'; u++) {
    uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];
  }
  return uHash % uBucketCount;
}

/* A Helper function which Expands Bindings, the array of binding
pointers that SymTable_T oSymTable uses.
It is called by SymTable_put. It takes in a SymTable_T oSymTable and
has no return value. It expands the relevant array if it can, but
returns and leaves oSymTable in its original condition if it can't
expand it. It exaapnds according to the sequence of sizes recorded in
SIZES by increasing the number of buckets by one level in that sequence
of numbers (unless it's already reached the highest of them) */
static void SymTable_expand(SymTable_T oSymTable) {
  struct Binding ** newBindings;
  struct Binding * psCurrentBinding;
  struct Binding * psNewBinding;
  size_t hash;
  size_t newHash;
  char * keyCopy;
  assert(oSymTable != NULL);

  /* If we've already hit the max number of buckets,
  we don't expand any further */
  if (SIZES[oSymTable->bucketCountOrder] == MAX_SIZE){
    return;
  }

  /* We allocate the memory for the new bindings array */
  newBindings = calloc(
    SIZES[oSymTable->bucketCountOrder + 1], sizeof(struct Binding *));
  if(newBindings == NULL) {
    return;
  }

  /* We recompute the hashes for every single key (since they're gonna
  change as we've increased the number of buckets) make a new copy of
  each binding, and place it in the new bindings table */
  for (hash = 0; hash < SIZES[oSymTable->bucketCountOrder]; hash++){
    for (psCurrentBinding = (oSymTable->Bindings)[hash];
    psCurrentBinding != NULL;
    psCurrentBinding = psCurrentBinding->psNextBinding) {
      psNewBinding = (struct Binding*) malloc(sizeof(struct Binding));
      if (psNewBinding == NULL) {
        /* If ever we can't allocate more memory, we have to free up
        the entire table we just created */
        SymTable_free_Bindings(newBindings,
        SIZES[oSymTable->bucketCountOrder + 1]);
        return;
      }
      keyCopy = (char *) calloc(strlen(psCurrentBinding->Key) + 1,
      sizeof(char));
      if (keyCopy == NULL) {
        /* If ever we can't allocate more memory, we have to free up
        the entire table we just created */
        free(psNewBinding);
        SymTable_free_Bindings(newBindings,
        SIZES[oSymTable->bucketCountOrder + 1]);
        return;
      }
      newHash = SymTable_hash(psCurrentBinding->Key,
      SIZES[oSymTable->bucketCountOrder + 1]);
      keyCopy = strcpy(keyCopy, psCurrentBinding->Key);
      psNewBinding->Key = keyCopy;
      psNewBinding->Value = psCurrentBinding->Value;
      psNewBinding->psNextBinding = newBindings[newHash];
      newBindings[newHash] = psNewBinding;
    }
  }
  /* We free the old table */
  SymTable_free_Bindings(oSymTable->Bindings,
  SIZES[oSymTable->bucketCountOrder]);

  /* We link the new table and increment bucketCountOrder */
  oSymTable->Bindings = newBindings;
  oSymTable->bucketCountOrder++;
}

/* A helper function used by SymTable_replace, SymTable_contains,
and SymTable_get. It takes in a SymTable_T oSymTable and a char * pcKey.
If oSymTable contains a binding with the key pcKey, it returns a pointer
to that binding. Otherwise, it returns Null. It does not change the
contents of oSymTable */
static struct Binding * SymTable_find(SymTable_T oSymTable,
const char *pcKey) {
  struct Binding *psCurrentBinding;
  size_t hash;
  assert(oSymTable != NULL);
  assert(pcKey != NULL);
  hash = SymTable_hash(pcKey, SIZES[oSymTable->bucketCountOrder]);
  for (psCurrentBinding = (oSymTable->Bindings)[hash];
    psCurrentBinding != NULL;
    psCurrentBinding = psCurrentBinding->psNextBinding) {
    if (strcmp(psCurrentBinding->Key,pcKey) == 0) {
      return psCurrentBinding;
    }
  }
  return NULL;
}

/* The SymTable deconstructor */
void SymTable_free(SymTable_T oSymTable) {
  assert(oSymTable != NULL);
  SymTable_free_Bindings(oSymTable->Bindings,
  SIZES[oSymTable->bucketCountOrder]);
  /* here we free up the rest of the table */
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
  size_t hash;
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

  /* If we need to expand the bindings array, expand it */
  if (oSymTable->size >= SIZES[oSymTable->bucketCountOrder]) {
    SymTable_expand(oSymTable);
  }

  /* We hash the key to get the index of where to place the binding */
  hash = SymTable_hash(pcKey, SIZES[oSymTable->bucketCountOrder]);
  /* We fill the binding and add it to the front of the linked list
  of the corresponding bucket */
  psNewBinding->Key = keyCopy;
  psNewBinding->Value = pvValue;
  psNewBinding->psNextBinding = (oSymTable->Bindings)[hash];
  (oSymTable->Bindings)[hash] = psNewBinding;
  oSymTable->size++;

  return 1;
}

/* implements the SymTable_replace() function */
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

/* implements the SymTable_contains() function */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
  assert(oSymTable != NULL);
  assert(pcKey != NULL);
  return (SymTable_find(oSymTable, pcKey) != NULL);
}

/* implements the SymTable_get() function */
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
  size_t hash;
  assert(oSymTable != NULL);
  assert(pcKey != NULL);
  hash = SymTable_hash(pcKey, SIZES[oSymTable->bucketCountOrder]);
  psCurrentBinding = oSymTable->Bindings[hash];
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
    oSymTable->Bindings[hash] = psCurrentBinding->psNextBinding;
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
  size_t hash;
  assert(oSymTable != NULL);
  assert(pfApply != NULL);

  for (hash = 0; hash < SIZES[oSymTable->bucketCountOrder]; hash++) {
    for (psCurrentBinding = (oSymTable->Bindings)[hash];
      psCurrentBinding != NULL;
      psCurrentBinding = psCurrentBinding->psNextBinding) {
      pfApply(psCurrentBinding->Key, (void *) psCurrentBinding->Value,
      (void *) pvExtra);
    }
  }
}
