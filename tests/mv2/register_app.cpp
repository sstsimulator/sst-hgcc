/* Registers test_mv2_sendrecv main (C TU avoids hg++/libc++); build with system clang++. */

extern "C" int main(int, char**);

typedef int (*main_fxn)(int, char**);
extern int userSkeletonMainInitFxn(const char* name, main_fxn fxn);

static int _app_registered =
    userSkeletonMainInitFxn("test_mv2_sendrecv", main);
