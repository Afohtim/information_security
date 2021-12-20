#include "tests.h"
#include "aes.h"
#include "kalyna.h"

#include <vector>
#include <iostream>
#include <time.h>


bool test1()
{
	std::vector<uint8_t> key = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
	int nr = 10;
	std::vector<uint32_t> w = std::vector<uint32_t>(4 * (nr + 1));

	aes::key_expansion(key, w);

	/*std::cout << "round key:\n";
	for (int i = 0; i < w.size(); ++i)
	{
		std::cout << std::dec << i << ' ' << std::hex << w[i] << '\n';
	}*/

	std::vector<uint8_t> in = { 0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34 };
	std::vector<uint8_t> out1 = std::vector<uint8_t>(16, 0);
	std::vector<uint8_t> out2 = std::vector<uint8_t>(16, 0);

	aes::cipher(in, out1, w);

	/*for (int i = 0; i < out1.size(); ++i)
	{
		std::cout << (int)out1[i] << ' ';
	}
	std::cout << '\n';*/

	aes::inv_cipher(out1, out2, w);

	/*for (int i = 0; i < out2.size(); ++i)
	{
		std::cout << (int)out2[i] << ' ';
	}
	std::cout << '\n';*/
	bool ans = true;
	for (int i = 0; i < in.size(); ++i)
	{
		if (in[i] != out2[i])
			ans = false;
	}
	return ans;
}

void hex_to_vector(std::string str, std::vector<uint8_t>& vec)
{
	for (int i = 0; i < str.size(); i += 2)
	{
		uint8_t x = 0;
		if (str[i] >= '0' && str[i] <= '9')
		{
			x += str[i] - '0';
		}
		else if (str[i] >= 'a' && str[i] <= 'f')
		{
			x += str[i] - 'a' + 10;
		}
		else
		{
			x += str[i] - 'A' + 10;
		}
		x <<= 4;
		if (str[i + 1] >= '0' && str[i + 1] <= '9')
		{
			x += str[i + 1] - '0';
		}
		else if (str[i + 1] >= 'a' && str[i + 1] <= 'f')
		{
			x += str[i + 1] - 'a' + 10;
		}
		else
		{
			x += str[i + 1] - 'A' + 10;
		}
		vec.push_back(x);
	}
}

bool test_aes(std::string in_, std::string key_, std::string enc_)
{
	std::vector<uint8_t> in;
	hex_to_vector(in_, in);
	std::vector<uint8_t> key;
	hex_to_vector(key_, key);
	std::vector<uint8_t> enc;
	hex_to_vector(enc_, enc);

	int nr = key.size() / 4 + 6;
	std::vector<uint32_t> w = std::vector<uint32_t>(4 * (nr + 1));
	aes::key_expansion(key, w);
	std::cout << key.size() << ' ' << w.size() << '\n';

	std::vector<uint8_t> out1 = std::vector<uint8_t>(16, 0);
	std::vector<uint8_t> out2 = std::vector<uint8_t>(16, 0);

	aes::cipher(in, out1, w);
	aes::inv_cipher(out1, out2, w);

	bool ans = true;
	for (int i = 0; i < in.size(); ++i)
	{
		if (in[i] != out2[i] || enc[i] != out1[i])
		{
			ans = false;
		}
	}
	return ans;
}

double encrypt_random_message_aes(std::vector<uint32_t>& w)
{
	std::string alphabet = "0123456789abcdef";
	std::string random_data;
	for (int i = 0; i < 32; ++i)
	{
		random_data.push_back(alphabet[rand() % alphabet.size()]);
	}

	std::vector<uint8_t> in;
	hex_to_vector(random_data, in);
	std::vector<uint8_t> out = std::vector<uint8_t>(16, 0);
	std::clock_t timer = std::clock();
	aes::cipher(in, out, w);
	timer = std::clock() - timer;
	return timer;
}

double time_test_aes(int rounds, std::string str_key)
{
	std::vector<uint8_t> key;
	hex_to_vector(str_key, key);

	int nr = key.size() / 4 + 6;
	std::vector<uint32_t> w = std::vector<uint32_t>(4 * (nr + 1));
	aes::key_expansion(key, w);

	std::clock_t timer = std::clock();
	long double ans = 0;
	for (int i = 0; i < rounds; ++i)
	{
		ans += (long double)encrypt_random_message_aes(w);
	}
	timer = std::clock() - timer;
	return double(timer) / CLOCKS_PER_SEC;
}


bool test_kalyna(std::string str_key, std::string str_plaintext, std::string str_ciphertext)
{
	int kalyna_nr = 10, kalyna_nb = 2;

	std::vector<uint8_t> key;
	hex_to_vector(str_key, key);

	std::vector<uint8_t> ciphertext_check;
	hex_to_vector(str_ciphertext, ciphertext_check);

	std::vector<uint8_t> plaintext;
	hex_to_vector(str_plaintext, plaintext);
	std::vector<uint8_t> ciphertext(plaintext.size(), 0), plaintext2(plaintext.size(), 0);

	std::vector<std::vector<uint64_t>> w(kalyna_nr + 1, std::vector<uint64_t>(kalyna_nb));

	kalyna::key_expansion(key, w);

	kalyna::cipher(plaintext, ciphertext, w);
	kalyna::inv_cipher(ciphertext, plaintext2, w);

	bool res = true;
	for (int i = 0; i < plaintext.size(); ++i)
	{
		if (plaintext[i] != plaintext2[i] || ciphertext[i] != ciphertext_check[i])
			res = false;
	}
	return res;
}

double encrypt_random_message_kalyna(std::vector<std::vector<uint64_t>>& w)
{
	std::string alphabet = "0123456789abcdef";
	std::string random_data;
	for (int i = 0; i < 32; ++i)
	{
		random_data.push_back(alphabet[rand() % alphabet.size()]);
	}

	std::vector<uint8_t> in;
	hex_to_vector(random_data, in);
	std::vector<uint8_t> out = std::vector<uint8_t>(16, 0);
	std::clock_t timer = std::clock();
	kalyna::cipher(in, out, w);
	timer = std::clock() - timer;
	return timer;
}


double time_test_kalyna(int rounds, std::string str_key)
{
	int kalyna_nr = 10, kalyna_nb = 2;
	std::vector<uint8_t> key;
	hex_to_vector(str_key, key);

	std::vector<std::vector<uint64_t>> w(kalyna_nr + 1, std::vector<uint64_t>(kalyna_nb));
	kalyna::key_expansion(key, w);

	std::clock_t timer = std::clock();
	long double ans = 0;
	for (int i = 0; i < rounds; ++i)
	{
		ans += (long double)encrypt_random_message_kalyna(w);
	}
	timer = std::clock() - timer;
	return double(timer) / CLOCKS_PER_SEC;
}
