#include "kupyna.h"
#include "kupyna_constants.h"
#include <string>
#include <vector>
#include <array>
#include <algorithm>

namespace kupyna
{
	/*
	* l == k == 256
	* Word size 256
	* Key length 256
	* Rounds 14
	* Rows in state matrix 4
	*/
	const int BYTES_IN_KUPYNA_WORD = 8;
	const int BITS_IN_KUPYNA_WORD = 64;
	const int BITS_IN_BYTE = 8;
	const int ROWS = 4;
	const int COLUMNS = BYTES_IN_KUPYNA_WORD;
	const int ROUNDS = 14;
	const int STATE_BYTE_SIZE = ROWS * BYTES_IN_KUPYNA_WORD;

	void vector64_to_8(const std::vector<uint64_t>& in, std::vector<uint8_t>& out)
	{
		out.clear();
		out.reserve(4 * in.size());
		for (auto c : in)
		{
			uint64_t num = c;
			for (int i = 0; i < 4; ++i)
			{
				out.push_back((uint8_t)(num >> ((3 - i) * 8)));
			}
		}
	}
	void message_padding(const std::string& message, std::vector<uint64_t>& padded_message)
	{
		uint64_t bit_length = 8ull * message.size();
		uint64_t l = bit_length % 512, k = ((512 - 96) - l - 1 + 512) % 512;

		uint64_t word = 0;
		padded_message.clear();
		for (int i = 0; i < message.size(); ++i)
		{
			word = word << 8 | (uint8_t)message[i];
			if (i % BYTES_IN_KUPYNA_WORD == BYTES_IN_KUPYNA_WORD - 1)
			{
				padded_message.push_back(word);
				word = 0;
			}
		}

		int ck = k;
		if (message.size() % BYTES_IN_KUPYNA_WORD != 0)
		{
			word = word << (8 * (BYTES_IN_KUPYNA_WORD - message.size() % BYTES_IN_KUPYNA_WORD)) | 1ull << (8 * (BYTES_IN_KUPYNA_WORD - message.size() % BYTES_IN_KUPYNA_WORD) - 1);
			ck -= 8 * (BYTES_IN_KUPYNA_WORD - message.size() % BYTES_IN_KUPYNA_WORD) - 1;
			padded_message.push_back(word);
		}
		else
		{
			padded_message.push_back(1ull << (BITS_IN_KUPYNA_WORD - 1));
			ck -= (BITS_IN_KUPYNA_WORD - 1);
		}

		while (ck >= BITS_IN_KUPYNA_WORD)
		{
			padded_message.push_back(0);
			ck -= BITS_IN_KUPYNA_WORD;
		}
		// assuming size < 2^64
		while (padded_message.size() < 15)
		{
			padded_message.push_back(0);
		}
		padded_message.push_back(1ull * message.size() * 8);

	}

	uint8_t gf_gield_multiply(uint8_t x, uint8_t y)
	{
		uint8_t r = 0;
		uint8_t hbit = 0;
		for (int i = 0; i < 8; ++i)
		{
			if (y & 1)
			{
				r ^= x;
			}

			hbit = x & 0x80;
			x <<= 1;

			if (hbit == 0x80) // idk how this can be true
			{
				x ^= 0x011d; // equivalent of x^8 + x^4 + x^3 + x^2 + 1
			}

			y >>= 1;
		}
		return r;
	}

	void add_q(std::array<std::array<uint8_t, ROWS>, 2 * BYTES_IN_KUPYNA_WORD>& state, int round)
	{
		uint64_t* s = (uint64_t*)state.data();
		for (int i = 0; i < COLUMNS; ++i)
		{
			s[i] = s[i] + (0x00F0F0F0F0F0F0F3ULL ^
				((((COLUMNS - i - 1) * 0x10ULL) ^ round) << (7 * 8)));
		}
	}

	void add_p(std::array<std::array<uint8_t, ROWS>, 2 * BYTES_IN_KUPYNA_WORD>& state, int round)
	{
		
		for (int i = 0; i < COLUMNS; ++i)
		{
			state[i][0] ^= (i * 0x10, round);
		}
	}

	void sub_bytes(std::array<std::array<uint8_t, ROWS>, 2 * BYTES_IN_KUPYNA_WORD >& state)
	{
		std::array<uint8_t, 2 * BYTES_IN_KUPYNA_WORD> temp;
		for (int i = 0; i < ROWS; ++i) {
			for (int j = 0; j < COLUMNS; ++j) {
				state[j][i] = sboxes[i % 4][state[j][i]];
			}
		}
	}

	void shift_bytes(std::array<std::array<uint8_t, ROWS>, 2 * BYTES_IN_KUPYNA_WORD >& state)
	{
		std::array<uint8_t, 2 * BYTES_IN_KUPYNA_WORD> temp;
		int shift = -1;

		for (int i = 0; i < ROWS; ++i) {
			if ((i == ROWS - 1) && (COLUMNS == 2 * BYTES_IN_KUPYNA_WORD)) {
				shift = 11;
			}
			else {
				++shift;
			}
			for (int j = 0; j < COLUMNS; ++j) {
				temp[(j + shift) % COLUMNS] = state[j][i];
			}
			for (int j = 0; j < COLUMNS; ++j) {
				state[j][i] = temp[j];
			}
		}
	}

	void mix_colums(std::array<std::array<uint8_t, ROWS>, 2 * BYTES_IN_KUPYNA_WORD >& state)
	{
		std::array<uint8_t, ROWS> res;
		uint8_t product;
		for (int col = 0; col < COLUMNS; ++col) {
			std::fill(std::begin(res), std::end(res), 0);
			for (int row = ROWS - 1; row >= 0; --row) {
				product = 0;
				for (int b = ROWS - 1; b >= 0; --b) {
					product ^= gf_gield_multiply(state[col][b], mds_matrix[row][b]);
				}
				res[row] = product;
			}
			for (int i = 0; i < ROWS; ++i) {
				state[col][i] = res[i];
			}
		}
	}

	


	void P(std::array<std::array<uint8_t, ROWS>, 2 * BYTES_IN_KUPYNA_WORD>& state)
	{
		for (int i = 0; i < ROUNDS; ++i)
		{
			add_p(state, i);
			sub_bytes(state);
			shift_bytes(state);
			mix_colums(state);
		}


	}

	void Q(std::array<std::array<uint8_t, ROWS>, 2 * BYTES_IN_KUPYNA_WORD>& state)
	{
		for (int i = 0; i < ROUNDS; ++i)
		{
			add_q(state, i);
			sub_bytes(state);
			shift_bytes(state);
			mix_colums(state);
		}
	}

	void kupyna256(std:: vector<uint8_t> padded_message, std::vector<uint64_t>& hash)
	{
		std::array<std::array<uint8_t, ROWS>, 2 * BYTES_IN_KUPYNA_WORD> state;// = { 1ull << 62, 0, 0, 0, 0, 0, 0, 0 }; //
		for (int i = 0; i < state.size(); ++i)
		{
			std::fill(std::begin(state[i]), std::end(state[i]), 0);

		}
		state[0][0] = STATE_BYTE_SIZE;
		std::array<std::array<uint8_t, ROWS>, 2 * BYTES_IN_KUPYNA_WORD> temp1, temp2;
		for (int t = 0; t < padded_message.size() / 8; t += STATE_BYTE_SIZE)
		{
			for (int i = 0; i < ROWS; ++i)
			{
				for (int j = 0; j < COLUMNS; ++j)
				{
					temp1[j][i] = state[j][i] ^ padded_message[t + j * ROWS + j];
					temp2[j][i] = padded_message[t + j * ROWS + j];
				}
			}

			P(temp1);
			Q(temp2);

			for (int i = 0; i < ROWS; ++i)
			{
				for (int j = 0; j < COLUMNS; ++j)
				{
					state[j][i] ^= temp1[j][i] ^ temp2[j][i];
				}
			}
		}

		uint8_t temp[2 * BYTES_IN_KUPYNA_WORD][ROWS];
		memcpy(temp, ctx->state, ROWS * NB_1024);
		P(ctx, temp);
		for (i = 0; i < ROWS; ++i) {
			for (j = 0; j < ctx->columns; ++j) {
				ctx->state[j][i] ^= temp[j][i];
			}
		}
		Trunc(ctx, hash_code);
		
	}

	void kupuna256(std::string s, std::vector<uint64_t>& hash)
	{
		std::vector<uint64_t> padded64;
		std::vector<uint8_t> padded8;
		message_padding(s, padded64);
		vector64_to_8(padded64, padded8);
		kupyna256(padded8, hash);
	}


	
}