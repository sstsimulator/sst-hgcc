HGCC_FLAGS = --replacements pthread.h -DSST_HG_USE_MERCURY_PTHREAD

all: libtest_tls.so libtest_pthread.so

libtest_tls.so: test_tls.o
	hg++ test_tls.o -o libtest_tls.so

test_tls.o: test_tls.cc
	hg++ $(HGCC_FLAGS) -c test_tls.cc

libtest_pthread.so: test_pthread.o
	hg++ test_pthread.o -o libtest_pthread.so

test_pthread.o: test_pthread.cc
	hg++ $(HGCC_FLAGS) -c test_pthread.cc

run_pthread: libtest_pthread.so
	SST_LIB_PATH=$(CURDIR) sst test_pthread.py

run_tls: libtest_tls.so
	SST_LIB_PATH=$(CURDIR) sst test_tls.py

clean:
	rm -rf *.o *.so
