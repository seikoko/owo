#ifndef PTRMAP_H
#define PTRMAP_H

#include "begincpp.h"

#include <stdint.h>
#include <assert.h>

#include "common.h"
#include "alloc.h"


static_assert (sizeof (intptr_t) == 64 / CHAR_BIT, "pointer bits abuse");

typedef uint64_t hash_t;
typedef void *key_t;
typedef hash_t hash_func(key_t k);
typedef int cmp_func(key_t k1, key_t k2);

// TODO: see if a bloom filter for hashes helps in any way
typedef struct ptrmap {
	void *mem;
	len_t len: 56;
	len_t log2_cap: 8;
	allocator upstream;
} ptrmap;

void pmap_init(ptrmap *m, allocator al);
key_t pmap_find(ptrmap *m, hash_t hash, key_t k, cmp_func *cmp, len_t objsz);
key_t pmap_push(ptrmap *m, hash_t hash, len_t objsz);
key_t pmap_intern(ptrmap *m, hash_t hash, key_t k, cmp_func *cmp, len_t objsz);
void pmap_reserve(ptrmap *m, len_t n, len_t objsz);
void pmap_fini(ptrmap *m);


#include "endcpp.h"

#endif /* PTRMAP_H */

