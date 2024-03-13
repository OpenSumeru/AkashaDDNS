ifeq ($(Version),debug)
	GCC = g++ -std=c++20 -Iinclude -g
	EXE = AkashaDDNS-d
else ifeq ($(Version),release)
	GCC = g++ -std=c++20 -static -Iinclude -O2
	EXE = AkashaDDNS
else ifeq ($(Version),min_release)
	GCC = g++ -std=c++20 -Iinclude -Os
	EXE = AkashaDDNS-min
else
	GCC = g++ -std=c++20 -Iinclude
	EXE = AkashaDDNS-d
endif

ifeq ($(OS),Windows_NT)
	Link = -lssl \
		   -lcrypto \
		   -lws2_32 \
		   -lcrypt32
    
else
    # Linux/Unix
	Link = -lssl \
		   -lcrypto
    
endif

RM = rm src/*.o
CR = rm AkashaDDNS.exe AkashaDDNS-min.exe AkashaDDNS-d.exe AkashaDDNS-d AkashaDDNS-min AkashaDDNS

obj = src/main.o \
	  src/pch.o \
	  src/cloudflare.o


%.o: %.cpp
	$(GCC) -c $< -o $@ 

AkashaDDNS: $(obj)
	$(GCC) $^ -o $(EXE) $(Link)

clear:
	$(RM)
	$(CR)

clean:
	$(RM)