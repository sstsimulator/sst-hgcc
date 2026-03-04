/* Clear all Mercury/HG pthread macros before including system headers.
   After the system header, include hgcc_pthread_return.h to restore them. */

#ifdef SST_HG_LIBRARIES_PTHREAD_HG_PTHREAD_MACRO_H

/* --- Undef type macros --- */
#undef pthread_t
#undef pthread_attr_t
#undef pthread_mutex_t
#undef pthread_mutexattr_t
#undef pthread_cond_t
#undef pthread_condattr_t
#undef pthread_spinlock_t
#undef pthread_once_t
#undef pthread_key_t
#undef pthread_rwlock_t
#undef pthread_rwlockattr_t
#undef pthread_barrier_t
#undef pthread_barrierattr_t

/* --- Undef constant macros --- */
#undef PTHREAD_ONCE_INIT
#undef PTHREAD_MUTEX_INITIALIZER
#undef PTHREAD_COND_INITIALIZER
#undef PTHREAD_RWLOCK_INITIALIZER
#undef PTHREAD_CREATE_DETACHED
#undef PTHREAD_CREATE_JOINABLE
#undef PTHREAD_MUTEX_NORMAL
#undef PTHREAD_MUTEX_RECURSIVE
#undef PTHREAD_MUTEX_ERRORCHECK
#undef PTHREAD_MUTEX_DEFAULT
#undef PTHREAD_PROCESS_SHARED
#undef PTHREAD_PROCESS_PRIVATE
#undef PTHREAD_SCOPE_PROCESS
#undef PTHREAD_SCOPE_SYSTEM
#undef PTHREAD_BARRIER_SERIAL_THREAD

/* --- Undef function macros --- */
#undef pthread_create
#undef pthread_join
#undef pthread_exit
#undef pthread_self
#undef pthread_equal
#undef pthread_mutex_init
#undef pthread_mutex_destroy
#undef pthread_mutex_lock
#undef pthread_mutex_trylock
#undef pthread_mutex_unlock
#undef pthread_mutexattr_init
#undef pthread_mutexattr_destroy
#undef pthread_mutexattr_gettype
#undef pthread_mutexattr_settype
#undef pthread_mutexattr_getpshared
#undef pthread_mutexattr_setpshared
#undef pthread_spin_init
#undef pthread_spin_destroy
#undef pthread_spin_lock
#undef pthread_spin_trylock
#undef pthread_spin_unlock
#undef pthread_cond_init
#undef pthread_cond_destroy
#undef pthread_cond_wait
#undef pthread_cond_timedwait
#undef pthread_cond_signal
#undef pthread_cond_broadcast
#undef pthread_condattr_init
#undef pthread_condattr_destroy
#undef pthread_condattr_getpshared
#undef pthread_condattr_setpshared
#undef pthread_once
#undef pthread_key_create
#undef pthread_key_delete
#undef pthread_setspecific
#undef pthread_getspecific
#undef pthread_attr_init
#undef pthread_attr_destroy
#undef pthread_attr_getdetachstate
#undef pthread_attr_setdetachstate
#undef pthread_attr_setscope
#undef pthread_attr_getscope
#undef pthread_attr_setaffinity_np
#undef pthread_attr_getaffinity_np
#undef pthread_attr_getstack
#undef pthread_detach
#undef pthread_yield
#undef pthread_testcancel
#undef pthread_kill
#undef pthread_rwlock_init
#undef pthread_rwlock_destroy
#undef pthread_rwlock_rdlock
#undef pthread_rwlock_tryrdlock
#undef pthread_rwlock_wrlock
#undef pthread_rwlock_trywrlock
#undef pthread_rwlock_unlock
#undef pthread_rwlockattr_init
#undef pthread_rwlockattr_destroy
#undef pthread_barrier_init
#undef pthread_barrier_destroy
#undef pthread_barrier_wait
#undef pthread_barrierattr_init
#undef pthread_barrierattr_destroy
#undef pthread_barrierattr_getpshared
#undef pthread_barrierattr_setpshared
#undef pthread_setconcurrency
#undef pthread_getconcurrency
#undef pthread_atfork
#undef pthread_setaffinity_np
#undef pthread_getaffinity_np
#undef pthread_cleanup_push
#undef pthread_cleanup_pop

#endif
