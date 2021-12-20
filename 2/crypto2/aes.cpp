#include <vector>
#include <cstdint>
#include "aes.h"
#include "aes_lookup_tables.h"

namespace aes
{
	int nb = 4, nk = 4, nr = 10;


	// y should be 2, 3, 9, 11, 13, 14
	uint8_t mul(uint8_t x, uint8_t y)
	{
		uint8_t a = x >> 4, b = x & 15;
		switch (y)
		{
		case 2: return mul2[a][b];
		case 3: return mul3[a][b];
		case 9: return mul9[a][b];
		case 11: return mul11[a][b];
		case 13: return mul13[a][b];
		case 14: return mul14[a][b];
		default:
			throw;
		}
	}

	uint32_t new_word(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
	{
		uint32_t res = (((uint32_t)a) << (3 * 8)) ^ (((uint32_t)b) << (2 * 8)) ^ (((uint32_t)c) << (1 * 8)) ^ ((uint32_t)d);
		return res;
	}

	uint32_t sub_word(uint32_t word)
	{
		uint32_t ans =
			(((uint32_t)sub[(uint8_t)(word >> 28) & 15][(uint8_t)(word >> 24) & 15])) << 24 |
			(((uint32_t)sub[(uint8_t)(word >> 20) & 15][(uint8_t)(word >> 16) & 15])) << 16 |
			(((uint32_t)sub[(uint8_t)(word >> 12) & 15][(uint8_t)(word >> 8) & 15])) << 8 |
			(((uint32_t)sub[(uint8_t)(word >> 4) & 15][(uint8_t)(word >> 0) & 15])) << 0;
		return ans;
	}

	uint32_t rot_word(uint32_t word)
	{
		uint32_t ans = (word << 8) | (word >> 24);
		return ans;
	}



	void key_expansion(const std::vector<uint8_t>& key, std::vector<uint32_t>& w)
	{
		uint32_t temp;
		nk = key.size() / 4;
		nr = key.size() / 4 + 6;

		int i = 0;
		while (i < nk)
		{
			w[i] = new_word(key[4 * i], key[4 * i + 1], key[4 * i + 2], key[4 * i + 3]);
			++i;
		}

		i = nk;

		uint32_t cur_rcon = 0x01000000;

		while (i < nb * (nr + 1))
		{
			temp = w[i - 1];
			if (i % nk == 0)
			{
				temp = sub_word(rot_word(temp)) ^ cur_rcon;
				cur_rcon = (mul2[cur_rcon >> 28][(cur_rcon >> 24) & 15]) << 24;
			}
			else if (nk > 6 && i % nk == 4)
			{
				temp = sub_word(temp);
			}
			w[i] = w[i - nk] ^ temp;
			i++;
		}

	}

	void sub_bytes(Aes_state& state)
	{
		for (int i = 0; i < state.size(); ++i) {
			for (int j = 0; j < 4; ++j)
			{
				state[i][j] = sub[state[i][j] >> 4][state[i][j] & 15];
			}
		}
	}

	void inv_sub_bytes(Aes_state& state)
	{

		for (int i = 0; i < state.size(); ++i) {
			for (int j = 0; j < 4; ++j)
			{
				state[i][j] = inv_sub[state[i][j] >> 4][state[i][j] & 15];
			}
		}
	}

	void shift_rows(Aes_state& state)
	{
		for (int i = 1; i < state.size(); ++i)
		{

			state[i] = (state[i].word << (8 * i)) | (state[i].word >> (32 - 8 * i));
		}
	}

	void inv_shift_rows(Aes_state& state)
	{
		for (int i = 1; i < state.size(); ++i)
		{
			state[i] = (state[i].word >> (8 * i)) | (state[i].word << (32 - 8 * i));
		}
	}

	void mix_column(std::vector<uint8_t>& r) {
		std::vector<uint8_t> rc(r.begin(), r.end());

		r[0] = mul(rc[0], 2) ^ mul(rc[1], 3) ^ rc[2] ^ rc[3];
		r[1] = rc[0] ^ mul(rc[1], 2) ^ mul(rc[2], 3) ^ rc[3];
		r[2] = rc[0] ^ rc[1] ^ mul(rc[2], 2) ^ mul(rc[3], 3);
		r[3] = mul(rc[0], 3) ^ rc[1] ^ rc[2] ^ mul(rc[3], 2);
	}

	void mix_columns(Aes_state& state)
	{
		Aes_state cstate = state;


		for (int i = 0; i < 4; ++i)
		{
			std::vector<uint8_t> col;
			for (int j = 0; j < state.size(); ++j)
			{
				col.push_back(cstate[j][i]);
			}
			mix_column(col);
			for (int j = 0; j < state.size(); ++j)
			{
				state[j][i] = col[j];
			}
		}
	}

	void inv_mix_column(std::vector<uint8_t>& r)
	{
		std::vector<uint8_t> rc(r.begin(), r.end());

		r[0] = mul(rc[0], 14) ^ mul(rc[1], 11) ^ mul(rc[2], 13) ^ mul(rc[3], 9);
		r[1] = mul(rc[0], 9) ^ mul(rc[1], 14) ^ mul(rc[2], 11) ^ mul(rc[3], 13);
		r[2] = mul(rc[0], 13) ^ mul(rc[1], 9) ^ mul(rc[2], 14) ^ mul(rc[3], 11);
		r[3] = mul(rc[0], 11) ^ mul(rc[1], 13) ^ mul(rc[2], 9) ^ mul(rc[3], 14);
	}

	void inv_mix_columns(Aes_state& state)
	{
		Aes_state cstate = state;

		for (int i = 0; i < 4; ++i)
		{
			std::vector<uint8_t> col;
			for (int j = 0; j < state.size(); ++j)
			{
				col.push_back(cstate[j][i]);
			}
			inv_mix_column(col);
			for (int j = 0; j < state.size(); ++j)
			{
				state[j][i] = col[j];
			}
		}
	}

	void add_round_key(Aes_state& state, std::vector<uint32_t> round_keys, int round)
	{
		//todo
		Aes_state cstate = state;

		for (int i = 0; i < 4; ++i)
		{
			state[0][i] ^= (uint8_t)(round_keys[round * nb + i] >> 24);
			state[1][i] ^= (uint8_t)(round_keys[round * nb + i] >> 16);
			state[2][i] ^= (uint8_t)(round_keys[round * nb + i] >> 8);
			state[3][i] ^= (uint8_t)(round_keys[round * nb + i] >> 0);
		}

	}

	void cipher(const std::vector<uint8_t>& in, std::vector<uint8_t>& out, const std::vector<uint32_t>& dw)
	{
		Aes_state state;

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < nb; ++j)
			{
				state[i][j] = in[4 * j + i];
			}
		}

		add_round_key(state, dw, 0);

		for (int round = 1; round <= nr - 1; ++round)
		{
			sub_bytes(state);
			shift_rows(state);
			mix_columns(state);
			add_round_key(state, dw, round);
		}

		sub_bytes(state);
		shift_rows(state);
		add_round_key(state, dw, nr);

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < nb; ++j)
			{
				out[4 * j + i] = state[i][j];
			}
		}
	}

	void inv_cipher(const std::vector<uint8_t>& in, std::vector<uint8_t>& out, const std::vector<uint32_t>& dw)
	{
		Aes_state state;

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < nb; ++j)
			{
				state[i][j] = in[4 * j + i];
			}
		}

		add_round_key(state, dw, nr);

		for (int round = nr - 1; round >= 1; --round)
		{
			inv_shift_rows(state);
			inv_sub_bytes(state);
			add_round_key(state, dw, round);
			inv_mix_columns(state);
		}

		inv_shift_rows(state);
		inv_sub_bytes(state);
		add_round_key(state, dw, 0);

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < nb; ++j)
			{
				out[4 * j + i] = state[i][j];
			}
		}
	}
}
