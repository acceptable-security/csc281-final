#include <iostream>

#include "websock.hpp"

int main(int argc, char* argv[]) {
	if ( argc != 3 ) {
		std::cout << "Usage: " << argv[0] << " <fifoin> <fifoout>" << std::endl;
		return 1;
	}

	WebSocketServer sock(argv[1], argv[2]);

	sock.listen([&](uint32_t client_id, uint32_t size, uint8_t* data) -> bool {
		std::cout << "Received from " << client_id << " message of size " << size << std::endl;
		std::cout << data << std::endl;

		sock.send(client_id, size, data);

		return true;
	});
}