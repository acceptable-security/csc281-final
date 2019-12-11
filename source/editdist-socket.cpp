#include "emp-sh2pc/emp-sh2pc.h"
#include "websock.hpp"
#include <new>
#include <stdexcept>
#include <cstdlib>
using namespace emp;
using namespace std;

int LEN = 5;

#define POS(X, Y) (((Y) * LEN) + (X))

Integer bitToInt(Bit bit, int owner) {
    Integer x(32, 0, owner);
    x[0] = bit;
    return x;
}

void parse_dna(string input, int party, Integer output[]) {
    if ( input.size() != LEN )  {
        throw invalid_argument("invalid input length");
    }

    for ( int i = 0; i < LEN; i++ ) {
        switch ( input[i] ) {
            case 'A': case 'a': output[i] = Integer(32, 0, party); break;
            case 'T': case 't': output[i] = Integer(32, 1, party); break;
            case 'G': case 'g': output[i] = Integer(32, 2, party); break;
            case 'C': case 'c': output[i] = Integer(32, 3, party); break;
            default: throw invalid_argument("invalid character in input");
        }
    }
}

Integer intMin2(Integer a, Integer b) {
    return b + bitToInt(a < b, PUBLIC) * (a - b);
}

Integer intMin3(Integer a, Integer b, Integer c) {
    return intMin2(intMin2(a, b), c);
}

void test_editdist(string input_a, string input_b) {
	Integer a[LEN];
	Integer b[LEN];

    cout << "Parsing DNA sequence" << endl;

    parse_dna(input_a, ALICE, a);
    parse_dna(input_b, BOB, b);

    Integer d[(LEN + 1) * (LEN + 1)];

    for ( int i = 0; i < LEN + 1; i++ ) {
        for ( int j = 0; j < LEN + 1; j++ ) {
            d[POS(i, j)] = Integer(32, 0, PUBLIC);
        }
    }

    for ( int i = 1; i < LEN + 1; i++ ) {
        d[POS(i, 0)] = Integer(32, i, PUBLIC);
    }

    for ( int i = 1; i < LEN + 1; i++ ) {
        d[POS(0, i)] = Integer(32, i, PUBLIC);
    }

    // Are these necessary?
    Integer one(32, 1, PUBLIC);

    for ( int j = 1; j < LEN + 1; j++ ) {
        for ( int i = 1; i < LEN + 1; i++ ) {
            // 0 if both are equal, 1 if not.
            Integer cost = one - bitToInt(a[i - 1].equal(b[j - 1]), PUBLIC);

            d[POS(i, j)] = intMin3(d[POS(i - 1, j)]     + one,
                                   d[POS(i,     j - 1)] + one,
                                   d[POS(i - 1, j - 1)] + cost);
        }
    }

    cout << "DIST: " << d[POS(LEN, LEN)].reveal<int>() << endl;
}

int do_setup(char* input, int party, int port) {
    NetIO* io = new NetIO(party == ALICE ? nullptr : "127.0.0.1", port);

    setup_semi_honest(io, party);

    cout << "Calculating edit distance of length " << LEN << endl;

    // Fake input for other party
    string inputs_a(input);
    string inputs_b(input);

    int out = test_editdist(inputs_a, inputs_b);
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
        int output = do_setup(&data[1], data[0] == 'A' ? ALICE : BOB, port);

        char str_output[33];
        itoa(output, (char*) str_output, 10);

        sock.send(client_id, strlen(str_output) + 1, str_output);
    });
}
