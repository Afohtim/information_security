#include "sha.h"
#include "sha_constants.h"
#include <string>
#include <vector>
#include <array>

namespace sha
{

	const int BYTES_IN_SHA256_WORD = 4;
	const int BITS_IN_SHA256_WORD = 32;
	void message_padding(const std::string& message, std::vector<uint32_t>& padded_message)
	{
		uint64_t bit_length = 8ull * message.size();
		uint64_t l = bit_length % 512, k = (448 - l - 1 + 512) % 512;

		uint32_t word = 0;
		padded_message.clear();
		for (int i = 0; i < message.size(); ++i)
		{
			word = word << 8 | (uint8_t)message[i];
			if (i % BYTES_IN_SHA256_WORD == BYTES_IN_SHA256_WORD - 1)
			{
				padded_message.push_back(word);
				word = 0;
			}
		}

		int ck = k;
		if (message.size() % BYTES_IN_SHA256_WORD != 0)
		{
			word = word << (8 * (BYTES_IN_SHA256_WORD - message.size() % BYTES_IN_SHA256_WORD)) | 1 << (8 * (BYTES_IN_SHA256_WORD - message.size() % BYTES_IN_SHA256_WORD) - 1);
			ck -= 8 * (BYTES_IN_SHA256_WORD - message.size() % BYTES_IN_SHA256_WORD) - 1;
			padded_message.push_back(word);
		}
		else
		{
			padded_message.push_back(1ull << (BITS_IN_SHA256_WORD - 1));
			ck -= (BITS_IN_SHA256_WORD - 1);
		}

		while (ck >= BITS_IN_SHA256_WORD)
		{
			padded_message.push_back(0);
			ck -= BITS_IN_SHA256_WORD;
		}
		// assuming size < 2^32
		padded_message.push_back(0);
		padded_message.push_back(message.size() * 8);
				
	}

	uint32_t rotr(int n, uint32_t x)
	{
		n %= BITS_IN_SHA256_WORD;
		return (x >> n) | (x << (BITS_IN_SHA256_WORD - n));
	}

	uint32_t shr(int n, uint32_t x)
	{
		n %= BITS_IN_SHA256_WORD;
		return x >> n;
	}

	uint32_t ch(uint32_t x, uint32_t y, uint32_t z)
	{
		return (x & y) ^ (~x & z);
	}

	uint32_t maj(uint32_t x, uint32_t y, uint32_t z)
	{
		return (x & y) ^ (x & z) ^ (y & z);
	}

	uint32_t s0(uint32_t x)
	{
		return rotr(2, x) ^ rotr(13, x) ^ rotr(22, x);
	}

	uint32_t s1(uint32_t x)
	{
		return rotr(6, x) ^ rotr(11, x) ^ rotr(25, x);
	}

	uint32_t sigma0(uint32_t x)
	{
		return rotr(7, x) ^ rotr(18, x) ^ shr(3, x);
	}

	uint32_t sigma1(uint32_t x)
	{
		return rotr(17, x) ^ rotr(19, x) ^ shr(10, x);
	}


	void sha256(std::vector<uint32_t>& padded_message, std::vector<uint32_t>& message_hash)
	{
		std::array<uint32_t, 8> H = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };
		
		for(int i = 0; i < padded_message.size() / 16; ++i)
		{
			uint32_t a = H[0], b = H[1], c = H[2], d = H[3], e = H[4], f = H[5], g = H[6], h = H[7];
			std::array<uint32_t, 64> w;
			for (int t = 0; t < 16; ++t)
			{
				w[t] = padded_message[16 * i + t];
			}
			for (int t = 16; t < 64; ++t)
			{
				w[t] = sigma1(w[t - 2]) + w[t - 7] + sigma0(w[t - 15]) + w[t - 16];
			}

			for (int t = 0; t < 64; ++t)
			{
				uint32_t T1 = h + s1(e) + ch(e, f, g) + K[t] + w[t];
				uint32_t T2 = s0(a) + maj(a, b, c);
				h = g;
				g = f;
				f = e;
				e = d + T1;
				d = c;
				c = b;
				b = a;
				a = T1 + T2;
			}
			H[0] += a;
			H[1] += b;
			H[2] += c;
			H[3] += d;
			H[4] += e;
			H[5] += f;
			H[6] += g;
			H[7] += h;
		}
		
		message_hash = std::vector<uint32_t>(H.begin(), H.end());

	}

	void sha256(const std::string& s, std::vector<uint32_t>& message_hash)
	{
		std::vector<uint32_t> padded;
		message_padding(s, padded);
		sha256(padded, message_hash);
	}
}