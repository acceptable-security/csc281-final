#include <fcntl.h>
#include <cstdio>
#include <cstdint>
#include <unistd.h>

#include <functional>
#include <stdexcept>

struct WebSocketHeader {
	uint32_t listener;
	uint32_t type;
	uint32_t size;
} __attribute__((packed));

class WebSocketServer {
	int fd_in;
	int fd_out;

	void parseStrictHeader(struct WebSocketHeader& hdr) {
		hdr.listener = htonl(hdr.listener);
		hdr.type = htonl(hdr.type);
		hdr.size = htonl(hdr.size);
	}

public:
	WebSocketServer(const char* in_path, const char* out_path) {
		fd_in = open(in_path, O_RDONLY | O_NONBLOCK);

		if ( fd_in < 0 ) {
			throw std::invalid_argument("unable to open in-fifo");
		}

		fd_out = open(out_path, O_WRONLY);

		if ( fd_out < 0 ) {
			throw std::invalid_argument("unable to open out-fifo");
		}
	}	

	void send(uint32_t client_id, size_t length, uint8_t* data) {
		struct WebSocketHeader hdr = {
			.listener = htonl(client_id),
			.type = htonl(0x1),
			.size = htonl(length)
		};

		write(fd_out, &hdr, sizeof(hdr));
		write(fd_out, data, length);
	}

	void listen(std::function<bool (uint32_t, uint32_t, uint8_t*)> callback) {
		bool need_cont;

		fd_set set;

		do {
			FD_ZERO(&set);
			FD_SET(fd_in, &set);

			if ( select(fd_in + 1, &set, NULL, NULL, NULL) < 1 ) {
				throw std::runtime_error("failed to execute select()");
			}

			if ( !FD_ISSET(fd_in, &set) ) {
				continue;
			}

			struct WebSocketHeader hdr = {};

			if ( read(fd_in, &hdr, sizeof(hdr)) < 1 ) {
				continue;
			}

			parseStrictHeader(hdr);

			uint8_t* data = new uint8_t[hdr.size];

			if ( read(fd_in, data, hdr.size) < 1 ) {
				std::cout << "Failed to read data" << std::endl;
				delete[] data;
				continue;
			}

			need_cont = callback(hdr.listener, hdr.size, data);
			delete[] data;
		} while (need_cont);
	}
};