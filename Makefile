APP = src

SRCS := server.c client.c

CC ?= gcc

TOPDIR = ../..

ifeq ($(FF_PATH),)
    FF_PATH = ${TOPDIR}
endif

ifeq ($(FF_DPDK),)
    FF_DPDK = ${TOPDIR}/dpdk/build
endif

# Detect DPDK via pkg-config
PKGCONF ?= pkg-config
DPDK_CFLAGS :=
DPDK_LDFLAGS :=

LDFLAGS += -Wl,--no-as-needed

ifeq ($(shell $(PKGCONF) --exists libdpdk && echo 0),0)
    DPDK_CFLAGS += $(shell $(PKGCONF) --cflags libdpdk)
    DPDK_LDFLAGS += $(shell $(PKGCONF) --libs libdpdk) -lrte_net_bond
else
    $(error "DPDK not found via pkg-config and no RTE_SDK set")
endif

# F-Stack Libraries
LIBS += -L${FF_PATH}/lib -Wl,--whole-archive,-lfstack,--no-whole-archive

# Final link libraries
LIBS += -Wl,--no-whole-archive -lrt -lm -ldl -lcrypto -pthread -lnuma

# Target name
TARGET = src

all: server client

server:
	$(CC) -O -gdwarf-2 -DINET6 $(DPDK_CFLAGS) -I${TOPDIR}/lib -o ${TARGET}_server server.c $(LIBS) $(DPDK_LDFLAGS)

client:
	$(CC) -O -gdwarf-2 -DINET6 $(DPDK_CFLAGS) -I${TOPDIR}/lib -o ${TARGET}_client client.c $(LIBS) $(DPDK_LDFLAGS)

tools:
	$(MAKE) -C ${TOPDIR}/tools

clean:
	rm -f *.o ${TARGET}_server ${TARGET}_client
	$(MAKE) -C ${TOPDIR}/tools clean


