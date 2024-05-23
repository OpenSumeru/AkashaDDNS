CXX = c++
CXXFLAG = -std=c++20
PCHFLAG = -x c++-header

ifeq ($(Version),release)
	CXXFLAG += -O2
	EXE = AkashaDDNS
else ifeq ($(Version),release-min)
	CXXFLAG += -static -Os
	EXE = AkashaDDNS-min
else
	CXXFLAG += -g
	EXE = AkashaDDNS-d
endif

RM = rm

INCLUDEDIR = include
SRCDIR = src

HEADERPPS = $(wildcard $(INCLUDEDIR)/*.hpp) 
HEADERS = $(wildcard $(INCLUDEDIR)/*.h)
PCHS = $(patsubst $(INCLUDEDIR)/%.hpp,$(INCLUDEDIR)/%.pch,$(HEADERPPS)) \
       $(patsubst $(INCLUDEDIR)/%.h,$(INCLUDEDIR)/%.pch,$(HEADERS))

SRCPPS = $(wildcard $(SRCDIR)/*.cpp)
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(SRCDIR)/%.o,$(SRCPPS)) \
       $(patsubst $(SRCDIR)/%.c,$(SRCDIR)/%.o,$(SRCS))

FLAG = $(CXXFLAG) -I$(INCLUDEDIR)
LINK = -lssl \
	   -lcrypto \
	   -lws2_32 \
       -lcrypt32

%.pch: %.h
	$(CXX) $(PCHFLAG) $(FLAG) $< -o $@ -Winvalid-pch

%.pch: %.hpp
	$(CXX) $(PCHFLAG) $(FLAG) $< -o $@ -Winvalid-pch

%.o: %.c
	$(CXX) $(FLAG) -I include -c $< -o $@

%.o: %.cpp
	$(CXX) $(FLAG) -I include -c $< -o $@

AkashaDDNS: $(PCHS) $(OBJS)
	$(CXX) $(FLAG) $(OBJS) -o $(EXE) $(LINK)

all:
	$(MAKE) AkashaDDNS
	$(MAKE) clean
	$(MAKE) AkashaDDNS Version=release
	$(MAKE) clean
	$(MAKE) AkashaDDNS Version=release-min

clean:
	- $(RM) $(OBJS)
	- $(RM) $(PCHS)

clear:
	- $(MAKE) clean
	- $(RM) AkashaDDNS*
