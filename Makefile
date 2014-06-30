##############################################################
#               CMake Project Wrapper Makefile               #
############################################################## 

SHELL := /bin/bash
RM    := rm -rf

# Third parties
export THIRD_PARTY_INSTALL_DIR=$(abspath third_party_dist)
export THIRD_PARTY_LIB_DIR=$(THIRD_PARTY_INSTALL_DIR)/lib
export THIRD_PARTY_LIB64_DIR=$(THIRD_PARTY_INSTALL_DIR)/lib64
export LD_LIBRARY_PATH=$(THIRD_PARTY_LIB64_DIR):$(THIRD_PARTY_LIB_DIR)
export PKG_CONFIG_PATH=$(THIRD_PARTY_LIB64_DIR)/pkgconfig:$(THIRD_PARTY_LIB_DIR)/pkgconfig

# To compile/package debug variant just call 'DEBUG=1 make'
ifdef DEBUG
	VARIANT +=.debug
	MY_CMAKE_FLAGS += -DCMAKE_BUILD_TYPE:STRING=Debug
else
	VARIANT +=.release
	MY_CMAKE_FLAGS += -DCMAKE_BUILD_TYPE:STRING=Release
endif

BUILD_DIR:=$(abspath build$(VARIANT))
DUKE_BIN:=$(BUILD_DIR)/src/duke/duke
DIST_DIR:=$(abspath dist)
DIST_SHELL:=$(DIST_DIR)/duke.sh

all: $(DUKE_BIN)
.PHONY: all

$(DUKE_BIN): $(BUILD_DIR)/Makefile
	$(MAKE) -C $(BUILD_DIR)

$(DIST_SHELL): $(DUKE_BIN)
	mkdir -p $(DIST_DIR)
	cp $(DUKE_BIN) $(DIST_DIR)
	strip $(DIST_DIR)/duke
	for file in `ldd $(DUKE_BIN) | cut -d'>' -f2 | awk '{print $$1}' | grep "$(THIRD_PARTY_LIB_DIR)"`; do cp $$file $(DIST_DIR); done
	echo 'export DUKE_DIR="$$( cd "$$( dirname "$${BASH_SOURCE[0]}" )" && pwd )";export LD_LIBRARY_PATH=$${DUKE_DIR};$${DUKE_DIR}/duke $$*' > $(DIST_SHELL)
	chmod +x $(DIST_SHELL)

dist: $(DIST_SHELL)
.PHONY: dist

package: dist
	tar cfj `$(DIST_SHELL) -v | tr ' ' '_' | tr '/' '_'`.tar.bz2 dist
.PHONY: package

$(BUILD_DIR)/Makefile: third_party_dist
	mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR) && cmake $(MY_CMAKE_FLAGS) ..

third_party_dist:
	$(MAKE) -C third_party all
.PHONY: third_party_dist

clean:
	- if [ -d $(BUILD_DIR) ]; then $(MAKE) -C $(BUILD_DIR) clean; fi
	- $(RM) $(DIST_DIR)
.PHONY: clean

distclean: clean
	- $(MAKE) -C third_party distclean
	- $(RM) $(BUILD_DIR)
.PHONY: distclean
