INSTALL_DIR ?=/usr/local/bin

cpuid_info : cpuid_info.cpp
	$(CXX) -O3 -o cpuid_info cpuid_info.cpp

install : cpuid_info
	cp cpuid_info $(INSTALL_DIR)/.

.PHONY : clean test

test : cpuid_info
	./cpuid_info

clean :
	rm -f cpuid_info
