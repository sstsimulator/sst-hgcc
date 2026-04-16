/* pthread_spinlock_t shim for macOS (mutex-backed); no-op elsewhere. */
#ifndef HGCC_PTHREAD_SPINLOCK_H
#define HGCC_PTHREAD_SPINLOCK_H

#if defined(__APPLE__)
#include <pthread.h>
#include <errno.h>

typedef pthread_mutex_t pthread_spinlock_t;

#ifdef __cplusplus
extern "C" {
#endif

static inline int pthread_spin_init(pthread_spinlock_t *lock, int pshared) {
  (void)pshared;
  return pthread_mutex_init(lock, NULL);
}

static inline int pthread_spin_destroy(pthread_spinlock_t *lock) {
  return pthread_mutex_destroy(lock);
}

static inline int pthread_spin_lock(pthread_spinlock_t *lock) {
  return pthread_mutex_lock(lock);
}

static inline int pthread_spin_trylock(pthread_spinlock_t *lock) {
  int r = pthread_mutex_trylock(lock);
  if (r == 0) return 0;
  if (r == EBUSY) return EBUSY;
  return r;
}

static inline int pthread_spin_unlock(pthread_spinlock_t *lock) {
  return pthread_mutex_unlock(lock);
}

#ifdef __cplusplus
}
#endif

#endif /* __APPLE__ */

#endif /* HGCC_PTHREAD_SPINLOCK_H */
