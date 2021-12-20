#include "salsa20.h"

#include <cstdint>
#include <vector>


namespace salsa20
{
	const int ROUNDS = 20;
	uint32_t rotl(uint32_t a, uint16_t shift)
	{
		return (a << shift) | (a >> (32 - shift));
	}

	void qr(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d)
	{
		b ^= rotl(a + d, 7);
		c ^= rotl(b + a, 9);
		d ^= rotl(c + b, 13);
		a ^= rotl(d + c, 18);
	}

	void salsa20_block(uint32_t out[16], uint32_t const in[16])
	{
		int i;
		uint32_t x[16];

		for (i = 0; i < 16; ++i)
			x[i] = in[i];

		for (i = 0; i < ROUNDS; i += 2) {
			qr(x[0], x[4], x[8], x[12]);
			qr(x[5], x[9], x[13], x[1]);
			qr(x[10], x[14], x[2], x[6]);
			qr(x[15], x[3], x[7], x[11]);

			qr(x[0], x[1], x[2], x[3]);
			qr(x[5], x[6], x[7], x[4]);
			qr(x[10], x[11], x[8], x[9]);
			qr(x[15], x[12], x[13], x[14]);
		}
		for (i = 0; i < 16; ++i)
			out[i] = x[i] + in[i];
	}

	void salsa20_block(std::vector<uint32_t>& out, const std::vector<uint32_t>& in)
	{
		uint32_t out_[16], in_[16];
		std::copy(out.begin(), out.end(), out_);
		std::copy(in.begin(), in.end(), in_);
		salsa20_block(out_, in_);
		for (int i = 0; i < out.size(); ++i)
		{
			out[i] = out_[i];
		}
	}

	void salsa20_cipher(const std::vector<uint32_t>& key, const std::vector<uint32_t>& nonce, uint64_t block_n, std::vector<uint32_t>& out)
	{
		std::vector<uint32_t> in =
		{
			0x61707865, key[0], key[1], key[2],
			key[3], 0x3320646e, nonce[0], nonce[1],
			(uint32_t)block_n, (uint32_t)(block_n >> 32), 0x79622d32, key[4],
			key[5], key[6], key[7], 0x6b206574

		};
		salsa20_block(out, in);
	}


}

