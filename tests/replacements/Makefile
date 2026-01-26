# Makefile for replacement header tests
# Compiles using hg++ compiler

# Get SST_ELEMENTS path from hgccvars.py if available
# This can be overridden by setting SST_ELEMENTS environment variable
SST_ELEMENTS ?= $(shell python3 -c "import sys; sys.path.insert(0, '../../build'); import hgccvars; print(hgccvars.includeDirElements.rsplit('/include', 1)[0])" 2>/dev/null)

# Install directory for test .so files
INSTALL_DIR = $(SST_ELEMENTS)/lib/sst-elements-library/tests

# List of all tests
TESTS = \
	test_cstdlib \
	test_cstring \
	test_dlfcn \
	test_fixIntrinsics \
	test_fstream \
	test_ios \
	test_iostream \
	test_istream \
	test_list \
	test_malloc \
	test_map \
	test_memory \
	test_mm_malloc \
	test_mpi \
	test_mutex \
	test_omp \
	test_ostream \
	test_pthread \
	test_queue \
	test_sched \
	test_set \
	test_signal \
	test_sstream \
	test_stack \
	test_stdio \
	test_stdlib \
	test_string \
	test_stringh \
	test_time \
	test_unistd \
	test_unordered_map \
	test_unordered_set \
	test_vector \
	test_wait \
	test_limits

# Generate list of .so files
SO_FILES = $(addsuffix .so, $(TESTS))
O_FILES = $(addsuffix .o, $(TESTS))

# Default target: build all tests
all: $(SO_FILES)

# Install target: copy .so files to SST_ELEMENTS
install: all
	@if [ -z "$(SST_ELEMENTS)" ]; then \
		echo "Error: SST_ELEMENTS not set. Please set SST_ELEMENTS environment variable or ensure hgccvars.py is available."; \
		exit 1; \
	fi
	@mkdir -p $(INSTALL_DIR)
	cp $(SO_FILES) $(INSTALL_DIR)/

# Clean target: remove generated files
clean:
	rm -f $(O_FILES) $(SO_FILES)

# Individual compile rules (.cc to .o)
#test_change_mpi_symbols.o: test_change_mpi_symbols.cc
#hg++ --replacements="test_change_mpi_symbols" -c test_change_mpi_symbols.cc

#test_clear_symbol_macros.o: test_clear_symbol_macros.cc
#	hg++ --replacements="test_clear_symbol_macros" -c test_clear_symbol_macros.cc

test_cstdlib.o: test_cstdlib.cc
	hg++ --replacements="cstdlib" -c test_cstdlib.cc

test_cstring.o: test_cstring.cc
	hg++ --replacements="cstring" -c test_cstring.cc

test_dlfcn.o: test_dlfcn.cc
	hg++ --replacements="dlfcn.h" -c test_dlfcn.cc

test_fixIntrinsics.o: test_fixIntrinsics.cc
	hg++ --replacements="fixIntrinsics.h" -c test_fixIntrinsics.cc

test_fstream.o: test_fstream.cc
	hg++ --replacements="fstream" -c test_fstream.cc

test_ios.o: test_ios.cc
	hg++ --replacements="ios" -c test_ios.cc

test_iostream.o: test_iostream.cc
	hg++ --replacements="iostream" -c test_iostream.cc

test_istream.o: test_istream.cc
	hg++ --replacements="istream" -c test_istream.cc

test_list.o: test_list.cc
	hg++ --replacements="list" -c test_list.cc

test_malloc.o: test_malloc.cc
	hg++ --replacements="malloc.h" -c test_malloc.cc

test_map.o: test_map.cc
	hg++ --replacements="map" -c test_map.cc

test_memory.o: test_memory.cc
	hg++ --replacements="memory" -c test_memory.cc

test_mm_malloc.o: test_mm_malloc.cc
	hg++ --replacements="mm_malloc.h" -c test_mm_malloc.cc

test_mpi.o: test_mpi.cc
	hg++ --replacements="mpi" -c test_mpi.cc

test_mutex.o: test_mutex.cc
	hg++ --replacements="mutex" -c test_mutex.cc

test_omp.o: test_omp.cc
	hg++ --replacements="omp.h" -c test_omp.cc

test_ostream.o: test_ostream.cc
	hg++ --replacements="ostream" -c test_ostream.cc

test_pthread.o: test_pthread.cc
	hg++ --replacements="pthread.h" -c test_pthread.cc

test_queue.o: test_queue.cc
	hg++ --replacements="queue" -c test_queue.cc

#test_return_symbol_macros.o: test_return_symbol_macros.cc
#	hg++ --replacements="test_return_symbol_macros" -c test_return_symbol_macros.cc

test_sched.o: test_sched.cc
	hg++ --replacements="sched.h" -c test_sched.cc

test_set.o: test_set.cc
	hg++ --replacements="set" -c test_set.cc

test_signal.o: test_signal.cc
	hg++ --replacements="signal.h" -c test_signal.cc

test_sstream.o: test_sstream.cc
	hg++ --replacements="sstream" -c test_sstream.cc

test_stack.o: test_stack.cc
	hg++ --replacements="stack" -c test_stack.cc

test_stdio.o: test_stdio.cc
	hg++ --replacements="stdio.h" -c test_stdio.cc

test_stdlib.o: test_stdlib.cc
	hg++ --replacements="stdlib.h" -c test_stdlib.cc

test_string.o: test_string.cc
	hg++ --replacements="string" -c test_string.cc

test_stringh.o: test_stringh.cc
	hg++ --replacements="string.h" -c test_stringh.cc

test_time.o: test_time.cc
	hg++ --replacements="time.h" -c test_time.cc

test_unistd.o: test_unistd.cc
	hg++ --replacements="unistd.h" -c test_unistd.cc

test_unordered_map.o: test_unordered_map.cc
	hg++ --replacements="unordered_map" -c test_unordered_map.cc

test_unordered_set.o: test_unordered_set.cc
	hg++ --replacements="unordered_set" -c test_unordered_set.cc

test_vector.o: test_vector.cc
	hg++ --replacements="vector" -c test_vector.cc

test_wait.o: test_wait.cc
	hg++ --replacements="wait.h" -c test_wait.cc

test_limits.o: test_limits.cc
	hg++ --replacements="linux/limits.h" -c test_limits.cc

# Individual link rules (.o to .so)
#test_change_mpi_symbols.so: test_change_mpi_symbols.o
#	hg++ --replacements="test_change_mpi_symbols" test_change_mpi_symbols.o -o test_change_mpi_symbols.so

#test_clear_symbol_macros.so: test_clear_symbol_macros.o
#	hg++ --replacements="test_clear_symbol_macros" test_clear_symbol_macros.o -o test_clear_symbol_macros.so

test_cstdlib.so: test_cstdlib.o
	hg++ --replacements="cstdlib" test_cstdlib.o -o test_cstdlib.so

test_cstring.so: test_cstring.o
	hg++ --replacements="cstring" test_cstring.o -o test_cstring.so

test_dlfcn.so: test_dlfcn.o
	hg++ --replacements="dlfcn.h" test_dlfcn.o -o test_dlfcn.so

test_fixIntrinsics.so: test_fixIntrinsics.o
	hg++ --replacements="fixIntrinsics.h" test_fixIntrinsics.o -o test_fixIntrinsics.so

test_fstream.so: test_fstream.o
	hg++ --replacements="fstream" test_fstream.o -o test_fstream.so

test_ios.so: test_ios.o
	hg++ --replacements="ios" test_ios.o -o test_ios.so

test_iostream.so: test_iostream.o
	hg++ --replacements="iostream" test_iostream.o -o test_iostream.so

test_istream.so: test_istream.o
	hg++ --replacements="istream" test_istream.o -o test_istream.so

test_list.so: test_list.o
	hg++ --replacements="list" test_list.o -o test_list.so

test_malloc.so: test_malloc.o
	hg++ --replacements="malloc.h" test_malloc.o -o test_malloc.so

test_map.so: test_map.o
	hg++ --replacements="map" test_map.o -o test_map.so

test_memory.so: test_memory.o
	hg++ --replacements="memory" test_memory.o -o test_memory.so

test_mm_malloc.so: test_mm_malloc.o
	hg++ --replacements="mm_malloc.h" test_mm_malloc.o -o test_mm_malloc.so

test_mpi.so: test_mpi.o
	hg++ --replacements="mpi" test_mpi.o -o test_mpi.so

test_mutex.so: test_mutex.o
	hg++ --replacements="mutex" test_mutex.o -o test_mutex.so

test_omp.so: test_omp.o
	hg++ --replacements="omp.h" test_omp.o -o test_omp.so

test_ostream.so: test_ostream.o
	hg++ --replacements="ostream" test_ostream.o -o test_ostream.so

test_pthread.so: test_pthread.o
	hg++ --replacements="pthread.h" test_pthread.o -o test_pthread.so

test_queue.so: test_queue.o
	hg++ --replacements="queue" test_queue.o -o test_queue.so

test_return_symbol_macros.so: test_return_symbol_macros.o
	hg++ --replacements="test_return_symbol_macros" test_return_symbol_macros.o -o test_return_symbol_macros.so

test_sched.so: test_sched.o
	hg++ --replacements="sched.h" test_sched.o -o test_sched.so

test_set.so: test_set.o
	hg++ --replacements="set" test_set.o -o test_set.so

test_signal.so: test_signal.o
	hg++ --replacements="signal.h" test_signal.o -o test_signal.so

test_sstream.so: test_sstream.o
	hg++ --replacements="sstream" test_sstream.o -o test_sstream.so

test_stack.so: test_stack.o
	hg++ --replacements="stack" test_stack.o -o test_stack.so

test_stdio.so: test_stdio.o
	hg++ --replacements="stdio.h" test_stdio.o -o test_stdio.so

test_stdlib.so: test_stdlib.o
	hg++ --replacements="stdlib.h" test_stdlib.o -o test_stdlib.so

test_string.so: test_string.o
	hg++ --replacements="string" test_string.o -o test_string.so

test_stringh.so: test_stringh.o
	hg++ --replacements="string.h" test_stringh.o -o test_stringh.so

test_time.so: test_time.o
	hg++ --replacements="time.h" test_time.o -o test_time.so

test_unistd.so: test_unistd.o
	hg++ --replacements="unistd.h" test_unistd.o -o test_unistd.so

test_unordered_map.so: test_unordered_map.o
	hg++ --replacements="unordered_map" test_unordered_map.o -o test_unordered_map.so

test_unordered_set.so: test_unordered_set.o
	hg++ --replacements="unordered_set" test_unordered_set.o -o test_unordered_set.so

test_vector.so: test_vector.o
	hg++ --replacements="vector" test_vector.o -o test_vector.so

test_wait.so: test_wait.o
	hg++ --replacements="wait.h" test_wait.o -o test_wait.so

test_limits.so: test_limits.o
	hg++ --replacements="linux/limits.h" test_limits.o -o test_limits.so

# Convenience targets: allow "make test_foo" to build "test_foo.so"
$(TESTS): %: %.so

.PHONY: all clean install $(TESTS)
