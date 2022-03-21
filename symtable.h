/*--------------------------------------------------------------------*/
/* symtable.h                                                         */
/* Author: Ahmed Farah                                                */
/* Interface for a Symbol Table abstract data type (ADT)              */
/*--------------------------------------------------------------------*/

/* To prevent double inclusions */
#ifndef SYMTABLE_INCLUDED
#define SYMTABLE_INCLUDED

/* allows us to use size_t as the return type of SymTable_getLength */
#include <stdlib.h>

/* defines an alias for struct SymTable * */
typedef struct SymTable * SymTable_T;

/* The constructor. It takes in no parameters and
returns an empty SymTable_T structure.
A symbol table is a set of key-value pairs which supports a variety of
operations such as put, get, replace... etc. */
SymTable_T SymTable_new(void);

/* The deconstructor. Take in a SymTable_T called oSymTable and frees
all memory associated with it. Doesn't return anything.
Runs in linear time */
void SymTable_free(SymTable_T oSymTable);

/* Takes in oSymTable, of type SymTable_T, and returns its size as a
size_t variable. Runs in constant time */
size_t SymTable_getLength(SymTable_T oSymTable);

/* Takes a SymTable_T called oSymTable, and a key-value pair: a string
pcKey, and a pointer to the value, pvValue, which is of type void *,
both of which are presumed to be constant.
It adds that key-value pair to the symbol table. It returns 1 if it
succeeds in adding the key-value pair to the symbol table. It returns 0
if it fails to do so, which could occurs if it can't allocate memory or
if the key is already in the table. */
int SymTable_put(SymTable_T oSymTable, const char *pcKey,
const void *pvValue);

/* Takes a SymTable_T called oSymTable, and a key-value pair: a string
pcKey, and a pointer to the value, pvValue, which is of type void *,
both of which are presumed to be constant.
If the symbol table oSymTable doesn't contain the key pcKey, it returns
Null. Otherwise, it gives it a new value, pvValue, and returns the old
value as a void * variable. */
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
const void *pvValue);

/* Takes a SymTable_T called oSymTable, and a key, which is a constant
string pcKey.
If the symbol table oSymTable doesn't contain the key pcKey, it returns
0 as an int. If it does, it returns the int 1. */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/* Takes a SymTable_T called oSymTable, and a key, which is a constant
string pcKey.
If the symbol table oSymTable doesn't contain the key pcKey, it returns
Null. If it does, it returns the value to which that key corresponds
as a void * variable. */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/* Takes a SymTable_T called oSymTable, and a key, which is a constant
string pcKey.
If the symbol table oSymTable doesn't contain the key pcKey, it returns
Null. If it does, it removes the key-value pair from the symbol table
oSymTable, frees up the associated memory, and returns the value which
corresponds to the key pcKey as a void * variable. */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/* Takes a SymTable_T called oSymTable, a function of return type void
called pfApply() of the signature seen below, and an extra parameter
pvExtra of type void *.
It traverses the symbol table oSymTable and applies the function
pfApply() to each key-value pair in the table, with the additional
parameter pvExtra. It returns nothing and does not alter the contents
of the symbol table. */
void SymTable_map(SymTable_T oSymTable,
void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
const void *pvExtra);

#endif
