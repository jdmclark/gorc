BOC_BUILD_SRC_DIRS:= \
	src/build/boc-build \
	src/build/boc-build/entities \
	src/build/boc-build/engine \
	src/build/common \
	src/libs/ast \
	src/libs/io \
	src/libs/log \
	src/libs/program \
	src/libs/system \
	src/libs/text \
	src/libs/utility \

BOC_BUILD_CFLAGS:= \
	-std=c++11 \
	-DPLATFORM_LINUX \
	-Isrc \
	-Isrc/libs \
	-pthread \
	-Wno-format \
	-Wl,--no-as-needed

BOC_BUILD_LDFLAGS:= \
	-lboost_system \
	-lboost_filesystem \
	-lboost_thread \

all:
	$(error Requires 'bootstrap' or 'upgrade' target. See README.md for more information)

bootstrap: \
	clean-bin \
	bootstrap-boc \
	build-final-boc \
	upgrade

clean-bin:
	rm -rf bin

bootstrap-boc:
	mkdir -p bin
	g++ $(BOC_BUILD_CFLAGS) \
		$(foreach dir,$(BOC_BUILD_SRC_DIRS),$(dir)/*.cpp) \
		-o bin/boc-build \
		$(BOC_BUILD_LDFLAGS)

build-final-boc:
	cd src/build && ../../bin/boc-build

upgrade:
	rm -rf bin
	cp -r pkg/build-bin bin

.PHONY: \
	all \
	bootstrap \
	upgrade \
	clean-bin \
	bootstrap-boc \
	build-final-boc
