##############################################################
#               CMake Project Wrapper Makefile               #
############################################################## 

SHELL := /bin/bash
RM    := rm -rf

export THIRD_PARTY_INSTALL_DIR=$(abspath third_party_dist)
export THIRD_PARTY_LIB_DIR=$(THIRD_PARTY_INSTALL_DIR)/lib
export LD_LIBRARY_PATH=$(THIRD_PARTY_LIB_DIR)
export PKG_CONFIG_PATH=$(THIRD_PARTY_INSTALL_DIR)/lib/pkgconfig

BUILD_DIR:=$(abspath build)
DUKE_BIN:=$(abspath build/src/duke/duke)
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
	mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Release ..

third_party_dist:
	$(MAKE) -C third_party all
.PHONY: third_party_dist

clean:
	- if [ -d $(BUILD_DIR) ]; then $(MAKE) -C $(BUILD_DIR) clean; fi
	- $(RM) $(DIST_DIR)
.PHONY: clean

distclean: clean
	- $(RM) $(BUILD_DIR)
.PHONY: distclean