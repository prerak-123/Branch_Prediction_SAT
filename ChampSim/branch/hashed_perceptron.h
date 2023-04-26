#include <stdio.h>
#include <cstdint>
#include <string.h>
#include <math.h>
#include <stdlib.h>

// this many tables

#define NTABLES	16

// maximum history length

#define MAXHIST	232

// minimum history length (for table 1; table 0 is biases)

#define MINHIST	3

// speed for dynamic threshold setting

#define SPEED	18


// 12-bit indices for the tables

#define LOG_TABLE_SIZE	12
#define TABLE_SIZE	(1<<LOG_TABLE_SIZE)

// this many 12-bit words will be kept in the global history

#define NGHIST_WORDS	(MAXHIST/LOG_TABLE_SIZE+1)

// tables of 8-bit weights

class HPBasePred
{public:
    // geometric global history lengths

    int history_lengths[NTABLES] = { 0, 3, 4, 6, 8, 10, 14, 19, 26, 36, 49, 67, 91, 125, 170, MAXHIST };
    int tables[NTABLES][TABLE_SIZE];

    // words that store the global history

    unsigned int ghist_words[NGHIST_WORDS];

    // remember the indices into the tables from prediction to update

    unsigned int indices[NTABLES];

    // initialize theta to something reasonable, 
    int 
        theta, 

    // initialize counter for threshold setting algorithm
        tc, 	

    // perceptron sum
        yout;

    uint8_t predicted;

    void initialize_branch_predictor();
    uint8_t predict_branch(uint64_t pc);
    void last_branch_result(uint64_t pc, uint8_t taken);


};

void HPBasePred::initialize_branch_predictor() {
	// zero out the weights tables

	memset (tables, 0, sizeof (tables));

	// zero out the global history

	memset (ghist_words, 0, sizeof (ghist_words));

	// make a reasonable theta

	theta = 10;
    return;
}

uint8_t HPBasePred::predict_branch(uint64_t pc) {

	// initialize perceptron sum

	yout = 0;

	// for each table...

	for (int i=0; i<NTABLES; i++) {

		// n is the history length for this table

		int n = history_lengths[i];

		// hash global history bits 0..n-1 into x by XORing the words from the ghist_words array

		unsigned int x = 0;

		// most of the words are 12 bits long

		int most_words = n / LOG_TABLE_SIZE;

		// the last word is fewer than 12 bits

		int last_word = n % LOG_TABLE_SIZE;

		// XOR up to the next-to-the-last word

		int j;
		for (j=0; j<most_words; j++) x ^= ghist_words[j];

		// XOR in the last word

		x ^= ghist_words[j] & ((1<<last_word)-1);

		// XOR in the PC to spread accesses around (like gshare)

		x ^= pc;

		// stay within the table size

		x &= TABLE_SIZE-1;

		// remember this index for update

		indices[i] = x;

		// add the selected weight to the perceptron sum

		yout += tables[i][x];
	}
    this->predicted = yout >=1;
	return yout >= 1;
}

void HPBasePred::last_branch_result(uint64_t pc, uint8_t taken) {

	// was this prediction correct?

	bool correct = taken == (yout >= 1);

	// insert this branch outcome into the global history

	bool b = taken;
	for (int i=0; i<NGHIST_WORDS; i++) {

		// shift b into the lsb of the current word

		ghist_words[i] <<= 1;
		ghist_words[i] |= b;

		// get b as the previous msb of the current word

		b = !!(ghist_words[i] & TABLE_SIZE);
		ghist_words[i] &= TABLE_SIZE-1;
	}

	// get the magnitude of yout

	int a = (yout < 0) ? -yout : yout;

	// perceptron learning rule: train if misprediction or weak correct prediction

	if (!correct || a < theta) {
		// update weights
		for (int i=0; i<NTABLES; i++) {
			// which weight did we use to compute yout?

			int *c = &tables[i][indices[i]];

			// increment if taken, decrement if not, saturating at 127/-128

			if (taken) {
				if (*c < 127) (*c)++;
			} else {
				if (*c > -128) (*c)--;
			}
		}

		// dynamic threshold setting from Seznec's O-GEHL paper

		if (!correct) {

			// increase theta after enough mispredictions

			tc++;
			if (tc >= SPEED) {
				theta++;
				tc = 0;
			}
		} else if (a < theta) {

			// decrease theta after enough weak but correct predictions

			tc--;
			if (tc <= -SPEED) {
				theta--;
				tc = 0;
			}
		}
	}
}