main:
	mkdir -p compiled
	g++ src/main.cpp src/2fa.cpp -o compiled/gmodws -std=c++17 -Wl,-R,'$$ORIGIN' -ldl -lpthread -lstdc++fs -lcrypto -Wall -Wextra -O3
