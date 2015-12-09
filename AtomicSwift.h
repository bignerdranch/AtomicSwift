//
//  AtomicSwift.h
//
//  Created by Zachary Waldowski on 12/7/15.
//  Copyright © 2015 Big Nerd Ranch. Some rights reserved. Licensed under MIT.
//

#ifndef __BNR_ATOMIC_SHIMS__
#define __BNR_ATOMIC_SHIMS__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef __has_feature
#define __has_feature(x) 0
#endif

#ifndef __has_extension
#define __has_extension(x) 0
#endif

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#if __GNUC__
#if __has_attribute(swift_private)
#define BNR_ATOMIC_DECL        static inline __attribute__((used, always_inline, swift_private))
#else
#define BNR_ATOMIC_DECL        static inline __attribute__((used, always_inline))
#endif
#define BNR_ATOMIC_FASTPATH(x) ((typeof(x))__builtin_expect((long)(x), ~0l))
#else
#define BNR_ATOMIC_DECL        static inline
#define BNR_ATOMIC_FASTPATH(x) (x)
#endif

#if __has_feature(objc_fixed_enum) || __has_extension(cxx_strong_enums)
#define BNR_ATOMIC_ENUM(name, type, ...) typedef enum : type { __VA_ARGS__ } name##_t
#else
#define BNR_ATOMIC_ENUM(name, type, ...) enum { __VA_ARGS__ }; typedef type name##_t
#endif

BNR_ATOMIC_ENUM(bnr_atomic_memory_order, int32_t,
    bnr_atomic_memory_order_relaxed,
    bnr_atomic_memory_order_consume,
    bnr_atomic_memory_order_acquire,
    bnr_atomic_memory_order_release,
    bnr_atomic_memory_order_acq_rel,
    bnr_atomic_memory_order_seq_cst
);

#if __has_extension(c_atomic) && __has_extension(c_generic_selections)

#define __bnr_atomic_base(p) typeof(*_Generic((p), \
    volatile int32_t *: (int32_t *)(p), \
    volatile int64_t *: (int64_t *)(p), \
    default:            (void   **)(p) \
))

#define __bnr_cast_atomic(p) _Generic((p), \
    volatile int32_t *: (volatile _Atomic(int32_t) *)(p), \
    volatile int64_t *: (volatile _Atomic(int64_t) *)(p), \
    default:            (volatile _Atomic(void *)  *)(p) \
)

#define __bnr_atomic_load(ptr, model) \
    __c11_atomic_load(__bnr_cast_atomic(ptr), bnr_atomic_memory_order_##model)

#define __bnr_atomic_store(ptr, val, model) \
    __c11_atomic_store(__bnr_cast_atomic(ptr), val, bnr_atomic_memory_order_##model)

#define __bnr_atomic_compare_and_swap(ptr, expected, desired, model) ({ \
    __bnr_atomic_base(ptr) _expected = (expected); \
    __c11_atomic_compare_exchange_strong(__bnr_cast_atomic(ptr), &_expected, (desired), bnr_atomic_memory_order_##model, bnr_atomic_memory_order_relaxed); \
})

#define __bnr_atomic_math(ptr, value, model, opName, operator) ({ \
    __bnr_atomic_base(ptr) _value = (value); \
    (__c11_atomic_fetch_##opName(__bnr_cast_atomic(ptr), _value, bnr_atomic_memory_order_##model) operator _value); \
})

#define __bnr_atomic_math_orig(ptr, value, model, opName, operator) \
    __c11_atomic_fetch_##opName(__bnr_cast_atomic(ptr), (value), bnr_atomic_memory_order_##model);

BNR_ATOMIC_DECL int32_t bnr_atomic_add_32(volatile int32_t *address, int32_t amount) {
    return __bnr_atomic_math(address, amount, seq_cst, add, +);
}

BNR_ATOMIC_DECL int32_t bnr_atomic_increment_32(volatile int32_t *address) {
    return bnr_atomic_add_32(address, 1);
}

BNR_ATOMIC_DECL int32_t bnr_atomic_sub_32(volatile int32_t *address, int32_t amount) {
    return __bnr_atomic_math(address, amount, seq_cst, sub, -);
}

BNR_ATOMIC_DECL int32_t bnr_atomic_decrement_32(volatile int32_t *address) {
    return bnr_atomic_sub_32(address, 1);
}

BNR_ATOMIC_DECL int32_t bnr_atomic_or_32(volatile int32_t *address, int32_t mask) {
    return __bnr_atomic_math(address, mask, seq_cst, or, |);
}

BNR_ATOMIC_DECL int32_t bnr_atomic_or_32_orig(volatile int32_t *address, int32_t mask) {
    return __bnr_atomic_math_orig(address, mask, seq_cst, or, |);
}

BNR_ATOMIC_DECL int32_t bnr_atomic_xor_32(volatile int32_t *address, int32_t mask) {
    return __bnr_atomic_math(address, mask, seq_cst, xor, ^);
}

BNR_ATOMIC_DECL int32_t bnr_atomic_xor_32_orig(volatile int32_t *address, int32_t mask) {
    return __bnr_atomic_math_orig(address, mask, seq_cst, xor, ^);
}

BNR_ATOMIC_DECL int32_t bnr_atomic_and_32(volatile int32_t *address, int32_t mask) {
    return __bnr_atomic_math(address, mask, seq_cst, and, &);
}

BNR_ATOMIC_DECL int32_t bnr_atomic_and_32_orig(volatile int32_t *address, int32_t mask) {
    return __bnr_atomic_math_orig(address, mask, seq_cst, and, &);
}

BNR_ATOMIC_DECL _Bool bnr_atomic_compare_and_swap_32(volatile int32_t *address, int32_t expected, int32_t desired) {
    return __bnr_atomic_compare_and_swap(address, expected, desired, seq_cst);
}

BNR_ATOMIC_DECL int64_t bnr_atomic_add_64(volatile int64_t *address, int64_t amount) {
    return __bnr_atomic_math(address, amount, seq_cst, add, +);
}

BNR_ATOMIC_DECL int64_t bnr_atomic_increment_64(volatile int64_t *address) {
    return bnr_atomic_add_64(address, 1);
}

BNR_ATOMIC_DECL int64_t bnr_atomic_sub_64(volatile int64_t *address, int64_t amount) {
    return __bnr_atomic_math(address, amount, seq_cst, sub, -);
}

BNR_ATOMIC_DECL int64_t bnr_atomic_decrement_64(volatile int64_t *address) {
    return bnr_atomic_sub_64(address, 1);
}

BNR_ATOMIC_DECL _Bool bnr_atomic_compare_and_swap_64(volatile int64_t *address, int64_t expected, int64_t desired) {
    return __bnr_atomic_compare_and_swap(address, expected, desired, seq_cst);
}

BNR_ATOMIC_DECL _Bool bnr_atomic_compare_and_swap_ptr(void *volatile *address, void *expected, void **desired) {
    return __bnr_atomic_compare_and_swap(address, expected, desired, seq_cst);
}

typedef struct {
    _Atomic(_Bool) _value;
} bnr_spinlock_t;
#define BNR_SPINLOCK_INIT { 0 }

BNR_ATOMIC_DECL _Bool bnr_spinlock_try(volatile bnr_spinlock_t *address) {
    return !__c11_atomic_exchange(&address->_value, 1, __ATOMIC_ACQUIRE);
}

BNR_ATOMIC_DECL void bnr_spinlock_lock(volatile bnr_spinlock_t *address) {
    while (!BNR_ATOMIC_FASTPATH(bnr_spinlock_try(address))) {
#if (defined(__x86_64__) || defined(__i386__))
        __asm __volatile("pause":::"memory");
#elif (defined(__arm__) || defined(__arm64__))
        __asm __volatile("wfe");
#else
        do {} while (0);
#endif
    }
}

BNR_ATOMIC_DECL void bnr_spinlock_unlock(volatile bnr_spinlock_t *address) {
    __c11_atomic_store(&address->_value, 0, __ATOMIC_RELEASE);
}

#elif __APPLE__

#import <libkern/OSAtomic.h>

#define __bnr_atomic_load(ptr, model) ({ \
    typeof(*(ptr)) _ret = *(ptr); \
    switch (bnr_atomic_memory_order_##model) { \
    case bnr_atomic_memory_order_seq_cst: \
        OSMemoryBarrier(); /* fallthrough */ \
    case bnr_atomic_memory_order_relaxed: \
        break; \
    default: \
        __bnr_atomic_unimplemented(); \
        break; \
    } \
    _ret; \
})

#define __bnr_atomic_store(ptr, val, model) ({ \
    switch (bnr_atomic_memory_order_##model) { \
    case bnr_atomic_memory_order_release: \
    case bnr_atomic_memory_order_seq_cst: \
        OSMemoryBarrier(); /* fallthrough */ \
    case bnr_atomic_memory_order_relaxed: \
        *(ptr) = (val); \
        break; \
    default: \
        __bnr_atomic_unimplemented(); \
        break; \
    } \
    \
    switch (bnr_atomic_memory_order_##model) { \
    case bnr_atomic_memory_order_seq_cst: \
        OSMemoryBarrier(); break; \
    default: \
        break; \
    } \
})

#define __bnr_atomic_unimplemented() \
    __asm__(".err unimplemented")

BNR_ATOMIC_DECL int32_t bnr_atomic_add_32(volatile int32_t *address, int32_t amount) {
    return OSAtomicAdd32(amount, address);
}

BNR_ATOMIC_DECL int32_t bnr_atomic_increment_32(volatile int32_t *address) {
    return OSAtomicIncrement32(address);
}

BNR_ATOMIC_DECL int32_t bnr_atomic_sub_32(volatile int32_t *address, int32_t amount) {
    return OSAtomicAdd32(-amount, address);
}

BNR_ATOMIC_DECL int32_t bnr_atomic_decrement_32(volatile int32_t *address) {
    return OSAtomicDecrement32(address);
}

BNR_ATOMIC_DECL int32_t bnr_atomic_or_32(volatile int32_t *address, int32_t mask) {
    return OSAtomicOr32(mask, (volatile uint32_t *)address);
}

BNR_ATOMIC_DECL int32_t bnr_atomic_or_32_orig(volatile int32_t *address, int32_t mask) {
    return OSAtomicOr32Orig(mask, (volatile uint32_t *)address);
}

BNR_ATOMIC_DECL int32_t bnr_atomic_xor_32(volatile int32_t *address, int32_t mask) {
    return OSAtomicXor32(mask, (volatile uint32_t *)address);
}

BNR_ATOMIC_DECL int32_t bnr_atomic_xor_32_orig(volatile int32_t *address, int32_t mask) {
    return OSAtomicXor32Orig(mask, (volatile uint32_t *)address);
}

BNR_ATOMIC_DECL int32_t bnr_atomic_and_32(volatile int32_t *address, int32_t mask) {
    return OSAtomicAnd32(mask, (volatile uint32_t *)address);
}

BNR_ATOMIC_DECL int32_t bnr_atomic_and_32_orig(volatile int32_t *address, int32_t mask) {
    return OSAtomicAnd32Orig(mask, (volatile uint32_t *)address);
}

BNR_ATOMIC_DECL _Bool bnr_atomic_compare_and_swap_32(volatile int32_t *address, int32_t expected, int32_t desired) {
    return OSAtomicCompareAndSwap32(expected, desired, address);
}

BNR_ATOMIC_DECL int64_t bnr_atomic_add_64(volatile int64_t *address, int64_t amount) {
    return OSAtomicAdd64(amount, address);
}

BNR_ATOMIC_DECL int64_t bnr_atomic_increment_64(volatile int64_t *address) {
    return OSAtomicIncrement64(address);
}

BNR_ATOMIC_DECL int64_t bnr_atomic_sub_64(volatile int64_t *address, int64_t amount) {
    return OSAtomicAdd64(-amount, address);
}

BNR_ATOMIC_DECL int64_t bnr_atomic_decrement_64(volatile int64_t *address) {
    return OSAtomicDecrement64(address);
}

BNR_ATOMIC_DECL _Bool bnr_atomic_compare_and_swap_64(volatile int64_t *address, int64_t expected, int64_t desired) {
    return OSAtomicCompareAndSwap64(expected, desired, address);
}

BNR_ATOMIC_DECL _Bool bnr_atomic_compare_and_swap_ptr(void *volatile *address, void *expected, void *desired) {
    return OSAtomicCompareAndSwapPtr(expected, desired, address);
}

typedef OSSpinLock        bnr_spinlock_t;
#define BNR_SPINLOCK_INIT OS_SPINLOCK_INIT

BNR_ATOMIC_DECL _Bool bnr_spinlock_try(volatile bnr_spinlock_t *address) {
    return OSSpinLockTry(address);
}

BNR_ATOMIC_DECL void bnr_spinlock_lock(volatile bnr_spinlock_t *address) {
    return OSSpinLockLock(address);
}

BNR_ATOMIC_DECL void bnr_spinlock_unlock(volatile bnr_spinlock_t *address) {
    return OSSpinLockUnlock(address);
}

#else
#error Unsupported platform for atomic locking primitives
#endif

BNR_ATOMIC_DECL int32_t bnr_atomic_load_32(volatile int32_t *address) {
    return __bnr_atomic_load(address, seq_cst);
}

BNR_ATOMIC_DECL void bnr_atomic_store_32(volatile int32_t *address, int32_t value) {
    __bnr_atomic_store(address, value, seq_cst);
}

BNR_ATOMIC_DECL int64_t bnr_atomic_load_64(volatile int64_t *address) {
    return __bnr_atomic_load(address, seq_cst);
}

BNR_ATOMIC_DECL void bnr_atomic_store_64(volatile int64_t *address, int64_t value) {
    __bnr_atomic_store(address, value, seq_cst);
}

BNR_ATOMIC_DECL void *bnr_atomic_load_ptr(void *volatile *address) {
    return __bnr_atomic_load(address, seq_cst);
}

BNR_ATOMIC_DECL void bnr_atomic_store_ptr(void *volatile *address, void *value) {
    __bnr_atomic_store(address, value, seq_cst);
}

#endif
