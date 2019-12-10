#include "emp-sh2pc/emp-sh2pc.h"
#include <new>
#include <stdexcept>
using namespace emp;
using namespace std;


int LEN = 5;

#define POS(x, y) ((X * LEN) + (Y))

void parse_dna(string input, int bitsize, int party, Integer output[]) {
    for ( int i = 0; i < input.size(); i++ ) {
        switch ( input[i] ) {
            case 'A': case 'a': output[i] = Integer(bitsize, 0, party); break;
            case 'T': case 't': output[i] = Integer(bitsize, 1, party); break;
            case 'G': case 'g': output[i] = Integer(bitsize, 2, party); break;
            case 'C': case 'c': output[i] = Integer(bitsize, 3, party); break;
            default: throw invalid_argument("invalid character in input");
        }
    }
}

Integer intMin(Integer a, Integer b, Integer c) {
    if ( a < b ) {
        if ( a < c ) {
            return a;
        }
        else {
            return c;
        }
    }
    else {
        if ( b < c ) {
            return b;
        }
        else {
            return c;
        }
    }
}

void test_editdist(int bitsize, string input_a, string input_b) {
	Integer a[LEN];
	Integer b[LEN];

    parse_dna(input_a, bitsize, ALICE, a);
    parse_dna(input_b, bitsize, BOB, b);

    Integer d[LEN * LEN];

    for ( int i = 0; i < LEN; i++ ) {
        for ( int j = 0; j < LEN; j++ ) {
            d[POS(i, j)] = Integer(bitsize, 0, PUBLIC);
        }
    }

    for ( int i = 1; i < LEN; i++ ) {
        d[POS(i, 0)] = i;
    }

    for ( int i = 1; i < LEN; i++ ) {
        d[POS(0, i)] = i;
    }

    Integer one(bitsize, 1, PUBLIC);

    for ( int j = 1; j < LEN; j++ ) {
        for ( int i = 1; i < LEN; i++ ) {
            Integer cost;

            if ( a[i] == b[j] ) {
                cost = Integer(bitsize, 0, PUBLIC);
            }
            else {
                cost = Integer(bitsize, 1, PUBLIC);
            }

            d[POS(i, j)] = intMin(d[POS(i - 1, j)] + one,
                                  d[POS(i, j - 1)] + one,
                                  d[POS(i - 1, j - 1) + cost]);
        }
    }

    cout << "DIST: " << d[POS(LEN - 1, LEN - 1)].reveal<int>() << endl;
}


int main(int argc, char** argv) {
    int bitsize;

    // run computation with semi-honest model
    int port, party;
    parse_party_and_port(argv, &party, &port);
    NetIO * io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);

    setup_semi_honest(io, party);

    if (argc != 4) {
      cout << "Usage: ./editdist <party> <port> <bitsize>" << endl
           << endl;
      delete io;
      return 0;
    }

    cout << "Calculating inner product of two inputs of length " << LEN << endl;

    bitsize = atoi(argv[3]);

    char fname_a[40];
    char fname_b[40];

    sprintf(fname_a, "../data/innerprod/%d.1.dat", bitsize);
    sprintf(fname_b, "../data/innerprod/%d.2.dat", bitsize);

    ifstream infile_a(fname_a);
    ifstream infile_b(fname_b);

    string inputs_a[LEN];
    string inputs_b[LEN];

    if( infile_a.is_open() && infile_b.is_open()) {
        for( int i=0; i<LEN; i++) {
            getline( infile_a, inputs_a[i]);
            getline( infile_b, inputs_b[i]);
        }
        infile_a.close();
        infile_b.close();
    }

    test_editdist(bitsize, inputs_a, inputs_b, LEN);
    delete io;
}


