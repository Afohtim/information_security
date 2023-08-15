#include "kalyna.h"
#include <vector>
#include <algorithm>
#include "kanyna_lookup_tables.h"

namespace kalyna
{
	//128-128
	constexpr int block_size = 128;
	constexpr int key_size = 128;
	constexpr int nb = 2;
	constexpr int nr = 10;
	constexpr int nk = 2;
	constexpr int col_size = 8;


	void col_to_word(std::vector<uint8_t>& col, uint64_t& w)
	{
		w = 0;
		for (int j = 0; j < col.size(); ++j)
		{
			w = (w << 8) | col[j];
		}
	}

	void word_to_col(std::vector<uint8_t>& col, uint64_t& w)
	{
		for (int j = col.size() - 1; j >= 0; --j)
		{
			col[j] = w & 0xffff;
			w >>= 8;
		}
	}
	
	void bytes_to_words(std::vector<uint8_t>& bytes, std::vector<uint64_t>& words)
	{
		words = std::vector<uint64_t>(bytes.size() / 4);
		for (int i = 0; i < bytes.size() / 4; ++i)
		{
			uint64_t w = 0;
			for (int j = 3; j >= 0; --j)
			{
				w = (w << 8) | bytes[4*i + j];
				w >>= 8;
			}
		}
	}

	void bytes_to_words(std::vector<uint64_t>& words, std::vector<uint8_t>& bytes)
	{
		bytes = std::vector<uint8_t>(words.size() * 4);
		for (int i = 0; i < bytes.size() / 4; ++i)
		{
			uint64_t w = words[i];
			for (int j = 3; j >= 0; --j)
			{
				bytes[4*i + j] = w & 0xffff;
				w >>= 8;
			}
		}
	}

	void sub_bytes(Kalyna_state& state)
	{
		for (int i = 0; i < nb; ++i)
		{
			for (int j = 0; j < 8; ++j)
			{
				state[i][j] = sboxes_enc[j % 4][state[i][j]];
			}
		}
	}

	void inv_sub_bytes(Kalyna_state& state)
	{
		for (int i = 0; i < nb; ++i)
		{
			for (int j = 0; j < 8; ++j)
			{
				state[i][j] = sboxes_dec[j % 4][state[i][j]];
			}
		}
	}

	void shift_rows(Kalyna_state& state)
	{
		Kalyna_state cstate(state);


		int shift = -1;
		for (int row = 0; row < 8; ++row)
		{
			if (row % (8 / nb) == 0)
			{
				shift++;
			}

			for (int col = 0; col < nb; ++col)
			{
				state[(col + shift) % nb][row] = cstate[col][row];
			}
		}

	}

	void inv_shift_rows(Kalyna_state& state)
	{
		Kalyna_state cstate(state);


		int shift = -1;
		for (int row = 0; row < 8; ++row)
		{
			if (row % (8 / nb) == 0)
			{
				shift++;
			}
			for (int col = 0; col < nb; ++col)
			{
				state[col][row] = cstate[(col + shift) % nb][row];
			}
		}
	}

	uint8_t multiply_GF(uint8_t x, uint8_t y) {
		int i;
		uint8_t r = 0;
		uint8_t hbit = 0;
		for (i = 0; i < 8; ++i) {
			if ((y & 0x1) == 1)
				r ^= x;
			hbit = x & 0x80;
			x <<= 1;
			if (hbit == 0x80)
				x ^= 0x011d;
			y >>= 1;
		}
		return r;
	}

	void matrix_multiply(Kalyna_state& state, uint8_t matrix[8][8]) {


		Kalyna_state cstate(state);

		for (int col = 0; col < nb; ++col) {
			for (int row = 7; row >= 0; --row) {
				uint8_t product = 0;
				for (int b = 7; b >= 0; --b) {
					product ^= multiply_GF(cstate[col][b], matrix[row][b]);
				}
				state[col][row] = product;
			}
		}
	}

	void mix_colums(Kalyna_state& state) {
		matrix_multiply(state, mds_matrix);
	}

	void inv_mix_colums(Kalyna_state& state) {
		matrix_multiply(state, mds_inv_matrix);
	}

	void add_round_key(Kalyna_state& state, std::vector<std::vector<uint64_t>>& round_keys, int round) {
		for (int i = 0; i < nb; ++i) {

			state[i] += round_keys[round][i];
		}
	}

	void sub_round_key(Kalyna_state& state, std::vector<std::vector<uint64_t>>& round_keys, int round) {
		for (int i = 0; i < nb; ++i) {
			state[i] -= round_keys[round][i];
		}
	}

	void xor_round_key(Kalyna_state& state, std::vector<std::vector<uint64_t>>& round_keys, int round) {
		for (int i = 0; i < nb; ++i) {
			state[i] ^= round_keys[round][i];

		}
	}

	void rotate(std::vector<uint64_t>& state)
	{
		uint64_t temp = state[0];

		for (int i = 0; i < state.size() - 1; ++i)
		{
			state[i] = state[i + 1];
		}

		state[state.size() - 1] = temp;
	}

	void rotate_left(std::vector<uint64_t>& state)
	{
		int rotate_bytes = 2 * state.size() + 3;
		int bytes_num = state.size() * 8;
		std::vector<uint8_t> bytes;
		for (int i = 0; i < state.size(); ++i)
		{
			for (int j = 0; j < 8; ++j)
			{
				bytes.push_back((uint8_t)(state[i] >> (j * 8)) & (0xff));
			}
		}
		std::rotate(bytes.begin(), bytes.begin() + rotate_bytes, bytes.end());

		for (int i = 0; i < state.size(); ++i)
		{
			state[i] = 0;
			for (int j = 0; j < 8; ++j)
			{
				state[i] = state[i] | ((uint64_t)bytes[8 * i + j] << (8 * j));
			}
		}
		
	}

	void shift_left(std::vector<uint64_t>& state)
	{
		for (int i = 0; i < state.size(); ++i)
		{
			state[i] <<= 1;
		}
	}

	void encipher_round(Kalyna_state& state)
	{
		sub_bytes(state);
		shift_rows(state);
		mix_colums(state);
	}

	void decipher_round(Kalyna_state& state)
	{
		inv_mix_colums(state);
		inv_shift_rows(state);
		inv_sub_bytes(state);
	}

	void add_round_key_expand(Kalyna_state& state, std::vector<uint64_t>& value)
	{
		for (int i = 0; i < nb; ++i)
		{
			state[i] = state[i] + value[i];
		}
	}

	void xor_round_key_expand(Kalyna_state& state, std::vector<uint64_t>& value)
	{
		for (int i = 0; i < nb; ++i)
		{
			state[i] = state[i] ^ value[i];
		}
	}

	void key_expansion_kt(const std::vector<uint8_t>& key, std::vector<uint64_t>& kt)
	{
		std::vector<uint64_t> k0(nb), k1(nb);

		// nb == nk
		Kalyna_state state(nb);
		state[0] = nb + nk + 1;
		for (int i = 0; i < key.size() / 8; ++i)
		{
			uint64_t word = 0ull;
			for (int j = 0; j < 8; ++j)
			{
				// if big endian
				//word = (word << 8) | (uint64_t)key[8 * i + j];

				// if little endian
				word = word | ((uint64_t)key[8 * i + j]) << (8 * j);
			}
			k0[i] = k1[i] = word;
		}

		//asoifjasiojfeioajfoesij

		add_round_key_expand(state, k0);
		encipher_round(state);
		xor_round_key_expand(state, k1);
		encipher_round(state);
		add_round_key_expand(state, k0);
		encipher_round(state);

		for (int i = 0; i < nb; ++i)
		{
			kt[i] = state[i].word;
		}
	}
	
	void key_expansion_even(const std::vector<uint8_t>& key, std::vector<uint64_t>& kt, std::vector<std::vector<uint64_t>>& round_keys)
	{
		std::vector<uint64_t> initial_data(nk), kt_round(nb), tmv(nb);
		int round = 0;

		for (int i = 0; i < key.size() / 8; ++i)
		{
			uint64_t word = 0ull;
			for (int j = 0; j < 8; ++j)
			{
				// if big endian
				//word = (word << 8) | (uint64_t)key[8 * i + j];

				// if little endian
				word = word | ((uint64_t)key[8 * i + j]) << (8 * j);
			}
			initial_data[i] = word;
		}

		for (int i = 0; i < nb; ++i)
		{
			tmv[i] = 0x0001000100010001;
		}

		Kalyna_state state;
		while (true)
		{
			state = kt;
			add_round_key_expand(state, tmv);

			for (int i = 0; i < nb; ++i)
			{
				kt_round[i] = state[i].word;
			}
			state = initial_data;

			add_round_key_expand(state, kt_round);
			encipher_round(state);
			xor_round_key_expand(state, kt_round);
			encipher_round(state);
			add_round_key_expand(state, kt_round);

			for(int i = 0; i < nb; ++i)
			{
				round_keys[round][i] = state[i].word;
			}

			if (nr == round)
				break;
			

			//nk == nb
			round += 2;
			shift_left(tmv);
			rotate(initial_data);
		}
	}

	void key_expansion_odd(std::vector<std::vector<uint64_t>>& round_keys)
	{
		for (int i = 1; i < nr; i += 2)
		{
			round_keys[i] = round_keys[i - 1];
			rotate_left(round_keys[i]);
		}
	}
	
	void key_expansion(const std::vector<uint8_t>& key, std::vector<std::vector<uint64_t>>& round_keys)
	{
		std::vector<uint64_t> kt(nb);
		key_expansion_kt(key, kt);
		key_expansion_even(key, kt, round_keys);
		key_expansion_odd(round_keys);
	}

	void cipher(std::vector<uint8_t>& in, std::vector<uint8_t>& out, std::vector<std::vector<uint64_t>>& dw)
	{
		
		Kalyna_state state(nb);

		for (int i = 0; i < nb; ++i)
		{
			for (int j = 0; j < 8; ++j)
			{
				state[i][j] = in[8 * i + j];
			}
		}

		
		add_round_key(state, dw, 0);
		for (int round = 1; round < nr; ++round)
		{
			encipher_round(state);
			xor_round_key(state, dw, round);
		}
		encipher_round(state);
		add_round_key(state, dw, nr);

		for (int i = 0; i < nb; ++i)
		{
			for (int j = 0; j < 8; ++j)
			{
				out[8 * i + j] = state[i][j];
			}
		}
	}

	void inv_cipher(std::vector<uint8_t>& in, std::vector<uint8_t>& out, std::vector<std::vector<uint64_t>>& dw)
	{
		Kalyna_state state(nb);

		for (int i = 0; i < nb; ++i)
		{
			for (int j = 0; j < 8; ++j)
			{
				state[i][j] = in[8 * i + j];
			}
		}

		int nr = 10;

		sub_round_key(state, dw, nr);
		for (int round = nr-1; round > 0; --round)
		{
			decipher_round(state);
			xor_round_key(state, dw, round);
		}
		decipher_round(state);
		sub_round_key(state, dw, 0);

		for (int i = 0; i < nb; ++i)
		{
			for (int j = 0; j < 8; ++j)
			{
				out[8 * i + j] = state[i][j];
			}
		}
	}

}