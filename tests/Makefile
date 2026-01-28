libtest_tls.so: test_tls.o
	hg++ test_tls.o -o libtest_tls.so

test_tls.o: test_tls.cc
	hg++ -c test_tls.cc

clean:
	rm -rf *.o *.so
