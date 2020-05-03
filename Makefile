main:
	g++ src/main.cpp -o compiled/gmodws -std=c++17 -Wl,-R,'$$ORIGIN' -ldl -lpthread
