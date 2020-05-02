main:
	g++ src/main.c -o compiled/gmodws -std=c++17 -I ../bootil/include -Wl,-R,'$$ORIGIN' -L ../bootil/lib/linux/gmake -lm -ldl -lpthread -lbootil_static
