/*
 * alloc_template.h
 *
 *  Created on: 2019Äê6ÔÂ15ÈÕ
 *      Author: bc155
 */

#ifndef ALLOC_TEMPLATE_H_
#define ALLOC_TEMPLATE_H_

#include <stddef.h>
#include <stdlib.h>

//first level
template <int inst>
class __malloc_alloc_template {
private:
	static void *oom_malloc(size_t);
	static void *oom_realloc(void *, size_t);
	static void (* __malloc_alloc_oom_handler)();

public:
	static void * allocate(size_t n) {
		void *result = malloc(n);
		if(0 == result) {
			result = oom_malloc(n);
		}
		return result;
	}

	static void deallocate(void *p, size_t n) {
		free(p);
	}

	static void * reallocate(void *p, size_t old_sz, size_t new_sz) {
		void * result = realloc(p, new_sz);
		if(0 == result) result = oom_realloc(p, new_sz);
		return result;
	}

	static void (* set_malloc_handler(void (*f)())) () {
		void (* old)() = __malloc_alloc_oom_handler;
		__malloc_alloc_oom_handler = f;
		return (old);
	}
};

template <int inst>
void (* __malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

template <int inst>
void * __malloc_alloc_template<inst>::oom_malloc(size_t n) {
	void (* my_malloc_handler)();
	void *result;

	for(;;) {
		my_malloc_handler = __malloc_alloc_oom_handler;
		if(0 == my_malloc_handler) { __THROW_BAD_ALLOC; }
		(*my_malloc_handler)();
		result = malloc(n);
		if(result)
			return (result);
	}
}

template <int inst>
void * __malloc_alloc_template<inst>::oom_realloc(void *p, size_t n) {
	void (* my_malloc_handler)();
	void *result;

	for(;;) {
		my_malloc_handler = __malloc_alloc_oom_handler;
		if(0 == my_malloc_handler) { __THROW_BAD_ALLOC; }
		(*my_malloc_handler)();
		result = realloc(p, n);
		if(result)
			return (result);
	}
}

typedef __malloc_alloc_template<0> malloc_alloc;

//second level

enum {__ALIGN = 8};
enum {__MAX_BYTES = 128};
enum {__NPREELISTS = __MAX_BYTES/__ALIGN};

template <bool threads, int inst>
class __default_alloc_template {
private:
	static size_t ROUND_UP(size_t bytes) {
		return (((bytes) + __ALIGN - 1) & ~(__ALIGN - 1));
	}

	union obj {
		union obj * free_list_link;
		char client_data[1];
	};

	static obj * volatile free_list[__NPREELISTS];

	static size_t FREELIST_INDEX(size_t bytes) {
		return (((bytes) + __ALIGN - 1) / __ALIGN - 1);
	}

	static void *refill(size_t n);
	static char *chunk_alloc(size_t size, int &nobjs);


};

#endif /* ALLOC_TEMPLATE_H_ */
