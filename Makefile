main:
	g++ src/main.c -o compiled/gmodws -std=c++17 -Wl,-R,'$$ORIGIN' -ldl -lpthread
