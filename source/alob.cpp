#include "emp-sh2pc/emp-sh2pc.h"
#include "websock.hpp"
#include <new>
#include <stdexcept>
#include <cstdlib>
using namespace emp;
using namespace std;

int LEN = 10;

#define POS(X, Y) (((Y) * LEN) + (X))

Integer bitToInt(Bit bit, int owner) {
    Integer x(32, 0, owner);
    x[0] = bit;
    return x;
}

void parse_choices(string input, int party, Integer output[]) {
    if ( input.size() != LEN )  {
        throw invalid_argument("invalid input length");
    }

    for ( int i = 0; i < LEN; i++ ) {
        switch ( input[i] ) {
            case '0': output[i] = Integer(32, 0, party); break;
            case '1': output[i] = Integer(32, 1, party); break;
            default: throw invalid_argument("invalid character in input");
        }
    }
}

int exec_score(string input_a, string input_b, int _threshold) {
    Integer a[LEN];
    Integer b[LEN];

    Integer threshold(32, _threshold, PUBLIC);

    parse_choices(input_a, ALICE, a);
    parse_choices(input_b, BOB, b);

    Integer acc(32, 0, PUBLIC);

    for ( int i = 0; i < LEN; i++ ) {
        acc = acc + input_a & input_b;
    }

    return bitToInt(acc.geq(threshold, PUBLIC)).reveal<int>();
}

int do_setup(char* input, int party, int port) {
    NetIO* io = new NetIO(party == ALICE ? nullptr : "127.0.0.1", port);

    setup_semi_honest(io, party);

    cout << "Calculating potential match of " << LEN << " options" << endl;

    // Fake input for other party
    string inputs_a(input);
    string inputs_b(input);

    int out = exec_score(inputs_a, inputs_b);
    delete io;

    return out;
}

int main(int argc, char** argv) {
    if ( argc != 4 ) {
        std::cout << "Usage: " << argv[0] << " <fifoin> <fifoout> <port>" << std::endl;
        return 1;
    }

    WebSocketServer sock(argv[1], argv[2]);

    int port = atoi(argv[3]);

    sock.listen([&](uint32_t client_id, uint32_t size, uint8_t* data) -> bool {
        if ( size != 11 ) {
            std::cout << "Invalid message: " << data << std::endl;
            return true;
        }

        std::cout << "Received response from " << client_id << std::endl;

        int output = do_setup((char*) &data[1], data[0] == 'A' ? ALICE : BOB, port);

        uint8_t str_output[33];
        sprintf((char*) str_output, "%d", output);

        sock.send(client_id, strlen((const char*) str_output) + 1, str_output);

        std::cout << "Finished" << std::endl;

        return true;
    });
}
