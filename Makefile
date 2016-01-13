CXX=g++
THREADS=2

CFLAGS:= \
	-std=c++14 \
	-DPLATFORM_LINUX \
	-Isrc \
	-Isrc/libs \
	-pthread \
	-Wno-format \
	-Wl,--no-as-needed

LDFLAGS:= \
	-lboost_system \
	-lboost_filesystem \

MAKEFLAGS+=-s

BOC_BUILD_SRC_DIRS:= \
	src/build/boc-build \
	src/build/boc-build/entities \
	src/build/boc-build/engine \
	src/build/common \
	src/libs/io \
	src/libs/log \
	src/libs/program \
	src/libs/system \
	src/libs/text \
	src/libs/utility \

BOC_BUILD_SRC_FILES:= \
	$(wildcard $(foreach dir,$(strip $(BOC_BUILD_SRC_DIRS)),$(dir)/*.cpp))

BOC_BUILD_OBJ_FILES:= \
	$(foreach src,$(BOC_BUILD_SRC_FILES),bin/obj/$(patsubst %.cpp,%.o,$(src)))

all:
	$(error Requires 'bootstrap' or 'upgrade' target. See README.md for more information)

bootstrap:
	$(info Bootstrapping build system. This may take some time)
	@$(MAKE) clean-bin
	@$(MAKE) bootstrap-boc
	@$(MAKE) build-final-boc
	@$(MAKE) upgrade

clean-bin:
	@rm -rf bin

bin/obj/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CFLAGS) -c $< -o $@

bin/boc-build: $(BOC_BUILD_OBJ_FILES)
	@$(CXX) $(CFLAGS) $^ -o $@ $(LDFLAGS)

bootstrap-boc: \
	bin/boc-build

build-final-boc:
	$(info Compiling production build system)
	@cd src/build && ../../bin/boc-build -j $(THREADS)

upgrade:
	$(info Upgrading build system)
	@rm -rf bin
	@cp -r pkg/build-bin bin

.PHONY: \
	all \
	bootstrap \
	upgrade \
	clean-bin \
	bootstrap-boc \
	build-final-boc
