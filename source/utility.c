#pragma once

//******************** Config ********************//

#ifndef UT_INLINE
#define UT_INLINE inline
#endif // #ifndef UT_INLINE

#define UT_FN LOCAL
#define UT_INLINE_FN UT_INLINE UT_FN

//******************** Data ********************//

typedef struct {
	u8 *start, *end;
	u8 *low, *hi;
} mem_pool;

//******************** Functions ********************//

UT_INLINE_FN mem_pool new_mem_pool(void *memory, u64 size) {
	mem_pool result;
	result.start = result.low = (u8 *)memory;
	result.end = result.hi = (u8 *)memory + size;

	return result;
}

UT_INLINE_FN void *mem_push(mem_pool *pool, u64 size) {
	u8 *new_low = pool->low + size;
	cn_assert(new_low <= pool->hi);
	void *result = pool->low;
	pool->low = new_low;

	return result;
}

UT_INLINE_FN void *mem_push_back(mem_pool *pool, u64 size) {
	u8 *new_hi = pool->hi - size;
	cn_assert(new_hi >= pool->low);
	pool->hi = new_hi;

	return new_hi;
}

UT_FN u64 load_file(const char* path, void* memory, u64 bytes_to_load) {
    u64 nbytes_read = 0;
    SDL_RWops *file = SDL_RWFromFile(path, "rb");
    if (file)
    {
        nbytes_read = SDL_RWread(file, memory, 1, bytes_to_load);
        SDL_RWclose(file);
    }

    return nbytes_read;
}

UT_FN u64 get_file_size(const char* path) {
    SDL_RWops *file = SDL_RWFromFile(path, "rb");
    u64 size = SDL_RWsize(file);
    SDL_RWclose(file);

    return size;
}

UT_FN void* load_file_p(const char *path, mem_pool *pool) {
    cn_assert(path && pool);
    u64 file_size = get_file_size(path);
    void *file_data = mem_push(pool, file_size);
    load_file(path, file_data, file_size);

    return file_data;
}

#define DEFINE_MAX(fn_name, type)                                                                  \
	UT_INLINE_FN type fn_name(type a, type b) {                                                    \
		return a > b ? a : b;                                                                      \
	}

#define DEFINE_MIN(fn_name, type)                                                                  \
	UT_INLINE_FN type fn_name(type a, type b) {                                                    \
		return a < b ? a : b;                                                                      \
	}

#define DEFINE_ABS(fn_name, type)                                                                  \
	UT_INLINE_FN type fn_name(type x) {                                                            \
		return x > 0 ? x : -x;                                                                     \
	}

DEFINE_MIN(minf, float);
DEFINE_MAX(maxf, float);
DEFINE_ABS(absf, float);

DEFINE_MIN(minu, u32);
DEFINE_MAX(maxu, u32);
