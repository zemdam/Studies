#ifndef JNP_HASH_H
#define JNP_HASH_H

#ifdef __cplusplus

#include <cstddef>
#include <cstdint>
#include <iostream> // Avoiding not initialized cerr.

namespace jnp1 {
    extern "C" {
    using hash_function_t = uint64_t (*)(uint64_t const *, size_t);

#else

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h> // Avoiding not initialized cerr.

typedef uint64_t (*hash_function_t)(uint64_t const *, size_t);

#endif

    unsigned long hash_create(hash_function_t hash_function);
    void hash_delete(unsigned long id);
    size_t hash_size(unsigned long id);
    bool hash_insert(unsigned long id, uint64_t const *seq, size_t size);
    bool hash_remove(unsigned long id, uint64_t const *seq, size_t size);
    void hash_clear(unsigned long id);
    bool hash_test(unsigned long id, uint64_t const *seq, size_t size);

#ifdef __cplusplus
    } // extern "C"
} // namespace jnp1
#endif

#endif //JNP_HASH_H
