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

#if !__has_feature(nullability)
#ifndef _Nullable
#define _Nullable
#endif
#ifndef _Nonnull
#define _Nonnull
#endif
#endif

#if __GNUC__
#define BNR_ATOMIC_DECL        static inline __attribute__((used, always_inline))
#define BNR_ATOMIC_FASTPATH(x) ((typeof(x))__builtin_expect((long)(x), ~0l))
#else
#define BNR_ATOMIC_DECL        static inline
#define BNR_ATOMIC_FASTPATH(x) (x)
#endif

BNR_ATOMIC_DECL void _OSAtomicSpin(void) {
#if defined(__x86_64__) || defined(__i386__)
    __asm__ __volatile__("pause" ::: "memory");
#elif defined(__arm__) || defined(__arm64__)
    __asm__ __volatile__("yield" ::: "memory");
#else
    do {} while (0);
#endif
}

#if __APPLE__
#import <libkern/OSAtomic.h>
#elif __has_extension(c_atomic) && __has_extension(c_generic_selections)

#define BNR_NONVOLATILE_TYPEOF(p) typeof(*_Generic((p), \
    volatile int32_t *:  (int32_t *)(p), \
    volatile int64_t *:  (int64_t *)(p), \
    volatile intptr_t *: (intptr_t *)(p), \
    volatile uint32_t *: (uint32_t *)(p), \
    default:             (void **)(p) \
))

#define BNR_ATOMIC_TYPEOF(p) _Generic((p), \
    volatile int32_t *:  (_Atomic(int32_t) *)(p), \
    volatile int64_t *:  (_Atomic(int64_t) *)(p), \
    volatile intptr_t *: (_Atomic(intptr_t) *)(p), \
    volatile uint32_t *: (_Atomic(uint32_t) *)(p), \
    default:             (_Atomic(void *) *)(p) \
)

#define BNR_DECL_ATOMIC_MATH(_type, _suffix, _cOpName, _operator) \
BNR_ATOMIC_DECL _type OSAtomic ## _suffix (_type amount, volatile _type *_Nonnull address) { \
    return (__c11_atomic_fetch_ ## _cOpName(BNR_ATOMIC_TYPEOF(address), amount, __ATOMIC_RELAXED) _operator amount); \
} \
\
BNR_ATOMIC_DECL _type OSAtomic ## _suffix ## Barrier(_type amount, volatile _type *_Nonnull address) { \
    return (__c11_atomic_fetch_ ## _cOpName(BNR_ATOMIC_TYPEOF(address), amount, __ATOMIC_SEQ_CST) _operator amount); \
} \

#define BNR_DECL_ATOMIC_MATH_ORIG(_type, _suffix, _cOpName) \
BNR_ATOMIC_DECL _type OSAtomic ## _suffix ## Orig (_type amount, volatile _type *_Nonnull address) { \
    return __c11_atomic_fetch_ ## _cOpName(BNR_ATOMIC_TYPEOF(address), amount, __ATOMIC_RELAXED); \
} \
\
BNR_ATOMIC_DECL _type OSAtomic ## _suffix ## OrigBarrier(_type amount, volatile _type *_Nonnull address) { \
    return __c11_atomic_fetch_ ## _cOpName(BNR_ATOMIC_TYPEOF(address), amount, __ATOMIC_SEQ_CST); \
}

BNR_DECL_ATOMIC_MATH(int32_t, Add32, add, +)
BNR_DECL_ATOMIC_MATH(int64_t, Add64, add, +)
BNR_DECL_ATOMIC_MATH(uint32_t, Or32, or, |)
BNR_DECL_ATOMIC_MATH_ORIG(uint32_t, Or32, or)
BNR_DECL_ATOMIC_MATH(uint32_t, And32, and, &)
BNR_DECL_ATOMIC_MATH_ORIG(uint32_t, And32, and)
BNR_DECL_ATOMIC_MATH(uint32_t, Xor32, xor, ^)
BNR_DECL_ATOMIC_MATH_ORIG(uint32_t, Xor32, xor)

#undef BNR_DECL_ATOMIC_MATH_ORIG
#undef BNR_DECL_ATOMIC_MATH

BNR_ATOMIC_DECL int32_t OSAtomicIncrement32(volatile int32_t *_Nonnull address) {
    return OSAtomicAdd32(1, address);
}

BNR_ATOMIC_DECL int32_t OSAtomicIncrement32Barrier(volatile int32_t *_Nonnull address) {
    return OSAtomicAdd32Barrier(1, address);
}

BNR_ATOMIC_DECL int32_t OSAtomicDecrement32(volatile int32_t *_Nonnull address) {
    return OSAtomicAdd32(-1, address);
}

BNR_ATOMIC_DECL int32_t OSAtomicDecrement32Barrier(volatile int32_t *_Nonnull address) {
    return OSAtomicAdd32Barrier(-1, address);
}

BNR_ATOMIC_DECL int64_t OSAtomicIncrement64(volatile int64_t *_Nonnull address) {
    return OSAtomicAdd64(1, address);
}

BNR_ATOMIC_DECL int64_t OSAtomicIncrement64Barrier(volatile int64_t *_Nonnull address) {
    return OSAtomicAdd64Barrier(1, address);
}

BNR_ATOMIC_DECL int64_t OSAtomicDecrement64(volatile int64_t *_Nonnull address) {
    return OSAtomicAdd64(-1, address);
}

BNR_ATOMIC_DECL int64_t OSAtomicDecrement64Barrier(volatile int64_t *_Nonnull address) {
    return OSAtomicAdd64Barrier(-1, address);
}

#define BNR_DECL_ATOMIC_COMPARE_AND_SWAP(_type, _suffix) \
BNR_ATOMIC_DECL _Bool OSAtomicCompareAndSwap ## _suffix (_type expected, _type desired, volatile _type *_Nonnull target) { \
    return __c11_atomic_compare_exchange_strong(BNR_ATOMIC_TYPEOF(target), &expected, desired, __ATOMIC_RELAXED, __ATOMIC_RELAXED); \
} \
\
BNR_ATOMIC_DECL _Bool OSAtomicCompareAndSwap ## _suffix ## Barrier (_type expected, _type desired, volatile _type *_Nonnull target) { \
    return __c11_atomic_compare_exchange_strong(BNR_ATOMIC_TYPEOF(target), &expected, desired, __ATOMIC_SEQ_CST, __ATOMIC_RELAXED); \
}

BNR_DECL_ATOMIC_COMPARE_AND_SWAP(int32_t, 32)
BNR_DECL_ATOMIC_COMPARE_AND_SWAP(void *_Nullable, Ptr)
BNR_DECL_ATOMIC_COMPARE_AND_SWAP(int, Int)
BNR_DECL_ATOMIC_COMPARE_AND_SWAP(long, Long)
BNR_DECL_ATOMIC_COMPARE_AND_SWAP(int64_t, 64)

#undef BNR_DECL_ATOMIC_COMPARE_AND_SWAP

#undef BNR_ATOMIC_TYPEOF
#undef BNR_NONVOLATILE_TYPEOF

typedef struct {
    _Atomic(_Bool) _value;
} OSSpinLock;

#define OS_SPINLOCK_INIT { 0 }

BNR_ATOMIC_DECL _Bool OSSpinLockTry(volatile OSSpinLock *_Nonnull address) {
    return !__c11_atomic_exchange(&address->_value, 1, __ATOMIC_ACQUIRE);
}

BNR_ATOMIC_DECL void OSSpinLockLock(volatile OSSpinLock *_Nonnull address) {
    while (!BNR_ATOMIC_FASTPATH(OSSpinLockTry(address))) {
        _OSAtomicSpin();
    }
}

BNR_ATOMIC_DECL void OSSpinLockUnlock(volatile OSSpinLock *_Nonnull address) {
    __c11_atomic_store(&address->_value, 0, __ATOMIC_RELEASE);
}

BNR_ATOMIC_DECL void OSMemoryBarrier(void) {
    __c11_atomic_thread_fence(__ATOMIC_SEQ_CST);
}

#else
#error Unsupported platform for atomics
#endif

#endif
