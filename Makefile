##############################################################
#               CMake Project Wrapper Makefile               #
############################################################## 

SHELL := /bin/bash
RM    := rm -rf

export THIRD_PARTY_INSTALL_DIR=$(abspath third_party_dist)
export LD_LIBRARY_PATH=$(THIRD_PARTY_INSTALL_DIR)/lib
export PKG_CONFIG_PATH=$(THIRD_PARTY_INSTALL_DIR)/lib/pkgconfig

all: build/Makefile
	$(MAKE) -C build
.PHONY: all

build/Makefile: third_party_dist
	mkdir -p build && cd build && cmake ..

third_party_dist:
	$(MAKE) -C third_party all
.PHONY: third_party_dist

clean:
	- $(MAKE) -C build clean
.PHONY: clean

distclean:
	- $(RM) ./build
.PHONY: distclean