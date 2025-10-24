TOPDIR=..

ifeq ($(FF_PATH),)
        FF_PATH=${TOPDIR}
endif

ifneq ($(shell pkg-config --exists libdpdk && echo 0),0)
$(error "No installation of DPDK found, maybe you should export environment variable `PKG_CONFIG_PATH`")
endif

PKGCONF ?= pkg-config

CFLAGS += -O0 -g -gdwarf-2 -I${FF_PATH}/lib $(shell $(PKGCONF) --cflags libdpdk)


LIBS+= $(shell $(PKGCONF) --static --libs libdpdk)
LIBS+= -L${FF_PATH}/lib -Wl,--whole-archive,-lfstack,--no-whole-archive
LIBS+= -Wl,--no-whole-archive -lrt -lm -ldl -lcrypto -pthread -lnuma

TARGET="fstack"
all:
	cc ${CFLAGS} -o server/server server.c ${LIBS}
	cc ${CFLAGS} -o client/client client.c ${LIBS}
       


.PHONY: clean
clean:
	rm -f *.o ${TARGET}
~                                                                                             
~                                     
