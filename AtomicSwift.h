//
//  AtomicSwift.h
//
//  Created by Zachary Waldowski on 12/7/15.
//  Copyright © 2015 Big Nerd Ranch. Some rights reserved. Licensed under MIT.
//

#ifndef __BNR_ATOMIC_SHIMS__
#define __BNR_ATOMIC_SHIMS__

#include <stddef.h>
#include <sys/cdefs.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef __has_extension
#define __has_extension(name)
#endif

#if __GNUC__
#define BNR_ATOMIC_USED __attribute__((__used__))
#define BNR_ATOMIC_INLINE static __inline__ __attribute__((always_inline))
#else
#define BNR_ATOMIC_USED
#define BNR_ATOMIC_INLINE static inline
#endif

#if __has_extension(c_atomic)

#include <stdatomic.h>

typedef atomic_int_least32_t bnr_atomic_int32_t;
typedef atomic_uint_least32_t bnr_atomic_uint32_t;
typedef atomic_int_least64_t bnr_atomic_int64_t;
typedef atomic_uint_least64_t bnr_atomic_uint64_t;
typedef volatile _Atomic(void *) atomic_ptr_t;
typedef atomic_flag bnr_spinlock_t;
#define BNR_SPINLOCK_INIT ATOMIC_FLAG_INIT

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int32_t __bnr_atomic_load_32(bnr_atomic_int32_t *address) {
    return atomic_load(address);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int32_t __bnr_atomic_add_32(bnr_atomic_int32_t *address, int32_t amount) {
    return atomic_fetch_add(address, amount);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int32_t __bnr_atomic_increment_32(bnr_atomic_int32_t *address) {
    return __bnr_atomic_add_32(address, 1);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int32_t __bnr_atomic_sub_32(bnr_atomic_int32_t *address, int32_t amount) {
    return atomic_fetch_sub(address, amount);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int32_t __bnr_atomic_decrement_32(bnr_atomic_int32_t *address) {
    return __bnr_atomic_sub_32(address, 1);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int32_t __bnr_atomic_or_32(bnr_atomic_uint32_t *address, uint32_t mask) {
    return atomic_fetch_or(address, mask) | mask;
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int32_t __bnr_atomic_or_32_orig(bnr_atomic_uint32_t *address, uint32_t mask) {
    return atomic_fetch_or(address, mask);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int32_t __bnr_atomic_xor_32(bnr_atomic_uint32_t *address, uint32_t mask) {
    return atomic_fetch_or(address, mask) ^ mask;
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int32_t __bnr_atomic_xor_32_orig(bnr_atomic_uint32_t *address, uint32_t mask) {
    return atomic_fetch_or(address, mask);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int32_t __bnr_atomic_and_32(bnr_atomic_uint32_t *address, uint32_t mask) {
    return atomic_fetch_and(address, mask) & mask;
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int32_t __bnr_atomic_and_32_orig(bnr_atomic_uint32_t *address, uint32_t mask) {
    return atomic_fetch_and(address, mask);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED bool __bnr_atomic_compare_and_swap_32(bnr_atomic_int32_t *address, int32_t expected, int32_t desired) {
    return atomic_compare_exchange_strong(address, &expected, desired);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int64_t __bnr_atomic_load_64(bnr_atomic_int64_t *address) {
    return atomic_load(address);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int64_t __bnr_atomic_add_64(bnr_atomic_int64_t *address, int64_t amount) {
    return atomic_fetch_add(address, amount);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int64_t __bnr_atomic_increment_64(bnr_atomic_int64_t *address) {
    return __bnr_atomic_add_64(address, 1);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int64_t __bnr_atomic_sub_64(bnr_atomic_int64_t *address, int64_t amount) {
    return atomic_fetch_sub(address, amount);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int64_t __bnr_atomic_decrement_64(bnr_atomic_int64_t *address) {
    return __bnr_atomic_sub_64(address, 1);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED bool __bnr_atomic_compare_and_swap_64(bnr_atomic_int64_t *address, int64_t expected, int64_t desired) {
    return atomic_compare_exchange_strong(address, &expected, desired);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED void *__bnr_atomic_load_ptr(atomic_ptr_t *address) {
    return atomic_load(address);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED bool __bnr_atomic_compare_and_swap_ptr(atomic_ptr_t *address, void *expected, void **desired) {
    return atomic_compare_exchange_strong(address, expected, desired);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED bool __bnr_spinlock_try(bnr_spinlock_t *address) {
    return atomic_flag_test_and_set_explicit(address, memory_order_acquire);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED void __bnr_spinlock_lock(bnr_spinlock_t *address) {
    while (!__bnr_spinlock_try(address)) {
#if (defined(__x86_64__) || defined(__i386__))
        __asm __volatile("pause":::"memory");
#elif (defined(__arm__) || defined(__arm64__))
        __asm __volatile("wfe");
#else
        do {} while (0)
#endif
    }
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED void __bnr_spinlock_unlock(bnr_spinlock_t *address) {
    atomic_flag_clear_explicit(address, memory_order_release);
}

#elif __APPLE__

#import <libkern/OSAtomic.h>

typedef volatile int32_t bnr_atomic_int32_t;
typedef volatile uint32_t bnr_atomic_uint32_t;
typedef volatile int64_t bnr_atomic_int64_t;
typedef volatile uint64_t bnr_atomic_uint64_t;
typedef void *volatile atomic_ptr_t;
typedef OSSpinLock bnr_spinlock_t;
#define BNR_SPINLOCK_INIT OS_SPINLOCK_INIT

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int32_t __bnr_atomic_load_32(bnr_atomic_int32_t *address) {
    return *address;
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int32_t __bnr_atomic_add_32(bnr_atomic_int32_t *address, int32_t amount) {
    return OSAtomicAdd32(amount, address);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int32_t __bnr_atomic_increment_32(bnr_atomic_int32_t *address) {
    return OSAtomicIncrement32(address);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int32_t __bnr_atomic_sub_32(bnr_atomic_int32_t *address, int32_t amount) {
    return OSAtomicAdd32(-amount, address);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int32_t __bnr_atomic_decrement_32(bnr_atomic_int32_t *address) {
    return OSAtomicDecrement32(address);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int32_t __bnr_atomic_or_32(bnr_atomic_uint32_t *address, uint32_t mask) {
    return OSAtomicOr32(mask, address);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int32_t __bnr_atomic_or_32_orig(bnr_atomic_uint32_t *address, uint32_t mask) {
    return OSAtomicOr32Orig(mask, address);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int32_t __bnr_atomic_xor_32(bnr_atomic_uint32_t *address, uint32_t mask) {
    return OSAtomicXor32(mask, address);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int32_t __bnr_atomic_xor_32_orig(bnr_atomic_uint32_t *address, uint32_t mask) {
    return OSAtomicXor32Orig(mask, address);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int32_t __bnr_atomic_and_32(bnr_atomic_uint32_t *address, uint32_t mask) {
    return OSAtomicAnd32(mask, address);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int32_t __bnr_atomic_and_32_orig(bnr_atomic_uint32_t *address, uint32_t mask) {
    return OSAtomicAnd32Orig(mask, address);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED bool __bnr_atomic_compare_and_swap_32(volatile int32_t *address, int32_t expected, int32_t desired) {
    return OSAtomicCompareAndSwap32(expected, desired, address);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int64_t __bnr_atomic_load_64(bnr_atomic_int64_t *address) {
    return *address;
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int64_t __bnr_atomic_add_64(bnr_atomic_int64_t *address, int64_t amount) {
    return OSAtomicAdd64(amount, address);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int64_t __bnr_atomic_increment_64(bnr_atomic_int64_t *address) {
    return OSAtomicIncrement64(address);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int64_t __bnr_atomic_sub_64(bnr_atomic_int64_t *address, int64_t amount) {
    return OSAtomicAdd64(-amount, address);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED int64_t __bnr_atomic_decrement_64(bnr_atomic_int64_t *address) {
    return OSAtomicDecrement64(address);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED bool __bnr_atomic_compare_and_swap_64(bnr_atomic_int64_t *address, int64_t expected, int64_t desired) {
    return OSAtomicCompareAndSwap64(expected, desired, address);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED void *__bnr_atomic_load_ptr(atomic_ptr_t *address) {
    return *address;
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED bool __bnr_atomic_compare_and_swap_ptr(atomic_ptr_t *address, void *expected, void *desired) {
    return OSAtomicCompareAndSwapPtr(expected, desired, address);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED bool __bnr_spinlock_try(bnr_spinlock_t *address) {
    return OSSpinLockTry(address);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED void __bnr_spinlock_lock(bnr_spinlock_t *address) {
    return OSSpinLockLock(address);
}

BNR_ATOMIC_INLINE BNR_ATOMIC_USED void __bnr_spinlock_unlock(bnr_spinlock_t *address) {
    return OSSpinLockUnlock(address);
}

#else
#error Unsupported platform for atomic locking primitives
#endif
#endif
