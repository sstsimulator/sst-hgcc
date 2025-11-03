#ifdef __APPLE__
#ifdef __cplusplus
extern "C"
#endif
ssize_t hgcc_sendfile(int out_fd, int in_fd, off_t *offset, size_t count);
#define sendfile hgcc_sendfile
#else
#include_next <sys/sendfile.h>
#endif
