#include "common.h"
#include "alloc.h"
#include "buf.h"

#include <assert.h>
#include <string.h>


len_t fb_len(fixed_buf b)
{
	static_assert(sizeof (uintptr_t) == 8, "");
	return b >> 48;
}

void *fb_mem(fixed_buf b)
{
	return (void *) SEXTEND(b, 16);
}

void fb_set_len(fixed_buf *b, len_t l)
{
	*b = (*b & BITS(48)) | (l << 48);
}

void fb_set_mem(fixed_buf *b, void *m)
{
	*b = (*b & BITRANGE(48, 64)) | ((uintptr_t) m & BITS(48));
}

fixed_buf fb_set(len_t len, void *mem)
{
	return (len << 48) | ((uintptr_t) mem & BITS(48));
}

len_t sm_len(small_buf b)
{
	return b >> 48;
}

len_t sm_cap(small_buf b)
{
	return 1 << (b & 0xF);
}

void *sm_mem(small_buf b)
{
	return (void *) SEXTEND(b & BITRANGE(4, 48), 16);
}

void sm_set_mem(small_buf *b, void *mem)
{
	assert(((uintptr_t) mem & 0xF) == 0);
	assert(((uintptr_t) mem >> 48) == 0);
	*b = (*b & ~BITRANGE(4, 48)) | (uintptr_t) mem;
}

void *sm_add(allocator al, small_buf *b, void *obj, len_t objsz)
{
	len_t len = sm_len(*b);
	if (!*b || len + 1 > sm_cap(*b))
		sm_resize(al, b, objsz);
	void *dst = (char *) sm_mem(*b) + len * objsz;
	memcpy(dst, obj, objsz);
	len++;
	*b = (len << 48) | (*b & BITS(48));
	return dst;
}

void *sm_resize(allocator al, small_buf *b, len_t objsz)
{
	len_t cap = sm_cap(*b) * 2;
	void *new = xrealloc(sm_mem(*b), cap * objsz);
	assert(cap < (1 << 15));
	++*b;
	sm_set_mem(b, new);
	return new;
}

void *sm_shrink_into(allocator al, small_buf *restrict dst, small_buf src, len_t objsz)
{
	len_t len = sm_len(src);
	len_t log2_cap = 63 - __builtin_clzll(len);
	void *new = xrealloc(sm_mem(src), (1 << log2_cap) * objsz);
	sm_set_mem(dst, new);
	*dst |= (len << 48);
	*dst |= log2_cap;
	return new;
}


