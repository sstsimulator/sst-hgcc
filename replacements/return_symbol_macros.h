#ifdef hgcc_must_return_free
#define free hgcc_free
#undef hgcc_must_return_free
#endif

#ifdef hgcc_must_return_memset
#undef memset
#define memset hgcc_memset
#undef hgcc_must_return_memset
#endif

#ifdef hgcc_must_return_memcpy
#undef memcpy
#define memcpy hgcc_memcpy
#undef hgcc_must_return_memcpy
#endif

#ifdef hgcc_must_return_gethostname
#undef gethostname
#define gethostname hgcc_gethostname
#undef hgcc_must_return_gethostname
#endif

#ifdef hgcc_must_return_getid
#undef gethostid
#define gethostid hgcc_gethostid
#undef hgcc_must_return_getid
#endif

#ifdef hgcc_must_return_mutex
#define mutex hgcc_mutex
#undef hgcc_must_return_mutex
#endif

#ifdef hgcc_must_return_getenv
#define getenv hgcc_getenv
#undef hgcc_must_return_getenv
#endif

#ifdef hgcc_must_return_setenv
#define setenv hgcc_setenv
#undef hgcc_must_return_setenv
#endif

#ifdef hgcc_must_return_puttenv
#define putenv hgcc_putenv
#undef hgcc_must_return_puttenv
#endif
