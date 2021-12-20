#include "rc4.h"
#include <vector>
#include <algorithm>
#include <string>

namespace rc4
{
	void key_schedule(std::vector<unsigned char>& s, std::vector<unsigned char> key)
	{
		for (int i = 0; i < 256; ++i)
		{
			s[i] = i;
		}
		int j = 0;
		for (int i = 0; i < 256; ++i)
		{
			j = (j + (unsigned char)s[i] + key[i % key.size()]) % 256;
			std::swap(s[i], s[j]);
		}
	}

	void key_schedule(std::string& s, std::string key)
	{
		std::vector<unsigned char> s_(s.begin(), s.end()), key_(key.begin(), key.end());

		key_schedule(s_, key_);

		for (int i = 0; i < s.size(); ++i)
		{
			s[i] = s_[i];
		}
	}

	void PRGA(std::vector<unsigned char>& s, std::vector<unsigned char> plaintext, std::vector<unsigned char>& ciphertext)
	{

		int i = 0, j = 0;
		for (int k = 0; k < plaintext.size(); ++k)
		{
			i = (i + 1) % 256;
			j = (j + s[i]) % 256;
			std::swap(s[i], s[j]);

			unsigned char n = s[(s[i] + s[j]) % 256];

			ciphertext[k] = plaintext[k] ^ n;
		}
	}

	void PRGA(std::string& s, std::string plaintext, std::string& ciphertext)
	{
		std::vector<unsigned char> s_(s.begin(), s.end()), plaintext_(plaintext.begin(), plaintext.end()), ciphertext_(ciphertext.begin(), ciphertext.end());
		PRGA(s_, plaintext_, ciphertext_);

		for (int i = 0; i < s.size(); ++i)
			s[i] = s_[i];

		for (int i = 0; i < ciphertext.size(); ++i)
			ciphertext[i] = ciphertext_[i];
	}
}