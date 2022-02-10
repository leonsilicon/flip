#include <iterator>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <bitset>
using namespace std;

// Each card in a card state is represented as 5 bits

// 00000

/*
The first bit indicates whether the card is face-up or
face-down (0 = face down, 1 = face up)
The remaining bits represent the value of the card at
that position:

A = 0001
2 = 0010
3 = 0011
4 = 0100
5 = 0101
6 = 0110
7 = 0111
8 = 1000
9 = 1001
T = 1010 // ten

10 cards * 5 bits/card = 50 bits

Bits 0-4 represent the top card in the deck, bits 5-9 the second
card, etc.
*/

typedef unsigned long long ull;

unordered_set<ull> visited_states;
ull num_cards = 10;

ull get_next_state(ull cur_state);

bool check_all_cards_face_down(ull state) {
	for (ull i = 0; i < num_cards; i += 1) {
		ull is_face_up = (state >> (i * 5)) & 0b10000ULL;
		if (is_face_up) return false;
	}
	return true;
}

ull LOSS = 0;
ull WIN = ~0;

// Loss from encountering an ace
int SIMULATE_ACE_LOSS = 0;
// Loss from ending in an infinite loop
int SIMULATE_INF_LOSS = 1;
// Win
int SIMULATE_WIN = 2;

int simulate(ull initial_state) {
	// Maintain a hashset of visited states so we don't end
	// up in an infinite loop
	unordered_set<ull> visited_states;

	// Flip the card face up before starting
	// (or else the game will categorize it as a win)
	initial_state |= 0b10000ULL;
	visited_states.insert(initial_state);

	ull cur_state = initial_state;
	while (true) {
		cur_state = get_next_state(cur_state);

		if (cur_state == LOSS) {
			return SIMULATE_ACE_LOSS;
		}

		if (cur_state == WIN) {
			return SIMULATE_WIN;
		}

		if (visited_states.count(cur_state) == 1) {
			return SIMULATE_INF_LOSS;
		}

		visited_states.insert(cur_state);
	}
}

/**
 * Returns the next state. A value of 0 represents a loss,
 * and a value of ~0 represents a win.
 */
ull get_next_state(ull cur_state) {
	// If all the cards are face down, you win
	if (check_all_cards_face_down(cur_state)) {
		return WIN;
	}

	ull next_state = cur_state;

	// Ensure the top card of the stack is face up
	next_state |= 0b10000ULL;

	// Get the value of the top card of the stack
	ull card_value = next_state & 0b1111ULL;

	// If the top card is an ace
	if (card_value == 1) {
		// If all your other cards are face down, you win
		ull cur_state_with_ace_face_down = cur_state ^ 0b10000ULL;
		if (check_all_cards_face_down(cur_state_with_ace_face_down)) {
			return WIN;
		}
		// Otherwise, you enter an infinite loop with the ace and lose
		else {
			return LOSS;
		}
	}

	// Otherwise, flip the number of cards indicated by the
	// value of the card

	// Iterating through the top cards and swapping them
	for (ull i = 0; i < (card_value + 1) / 2; i += 1) {
		// Getting the card closer to the top of the dock
		ull top_card = (cur_state >> i * 5) & 0b11111ULL;
		// Flipping the card's orientation
		top_card ^= 0b10000ULL;

		// Getting the card to swap with
		ull swap_card = (cur_state >> (card_value - i - 1) * 5) & 0b11111ULL;
		// Flipping the card's orientation
		swap_card ^= 0b10000ULL;

		// Inserting the swap_card in the top_card position
		ull top_card_bit_mask = 0b11111ULL << i * 5;
		next_state = (next_state & ~top_card_bit_mask) | swap_card << i * 5;

		// Inserting the top_card in the swap_card position
		ull swap_card_bit_mask = 0b11111ULL << (card_value - i - 1) * 5;
		next_state = (next_state & ~swap_card_bit_mask) | top_card << (card_value - i - 1) * 5;
	}

	return next_state;
}

int main() {
	// The value of the card at index i is cur_arrangement[i]
	vector<ull> cur_arrangement;
	for (ull i = 1; i <= num_cards; i += 1) {
		cur_arrangement.push_back(i);
	}

	ull result_counts[3] = {0, 0, 0};
	do {
		ull initial_state = 0;
		for (ull i = 0; i < num_cards; i += 1) {
			ull card_value = cur_arrangement[i];
			initial_state |= card_value << i * 5;
		}

		int result = simulate(initial_state);
		result_counts[result] += 1;
	} while (
		next_permutation(cur_arrangement.begin(), cur_arrangement.end())
	);

  cout << "Ace losses: " << result_counts[SIMULATE_ACE_LOSS] << endl;
  cout << "Inf losses: " << result_counts[SIMULATE_INF_LOSS] << endl;
  cout << "Wins: " << result_counts[SIMULATE_WIN] << endl;
}