ifeq ($(OS),Windows_NT)
	GCC = g++ -std=c++20 -static -Iinclude -O2 -ID:\Project\C++\Compiler\MinGW\include
	
	Link = -lssl \
		   -lcrypto \
		   -lws2_32 \
		   -lcrypt32
    RM = del src\*.o
	CR = del AkashaDDNS.exe
else
    # Linux/Unix
	GCC = g++ -std=c++20 -static -Iinclude -O2

	Link = -lssl \
		   -lcrypto
    RM = rm -rf src/*.o
	CR = rm -rf AkashaDDNS
endif


obj = src/main.o \
      src/json11.o \
	  src/pch.o \
	  src/cloudflare.o


%.o: %.cpp
	$(GCC) -c $< -o $@ 

AkashaDDNS: $(obj)
	$(GCC) $^ -o $@ $(Link)

clear:
	$(RM)
	$(CR)

clean:
	$(RM)