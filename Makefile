main:
	g++ src/main.c -o compiled/gmodws -std=c++17 -I ../bootil/include -Wl,-R,'$$ORIGIN' -lm -ldl -lpthread
