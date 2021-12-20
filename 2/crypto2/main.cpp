#include "rc4.h"
#include "salsa20.h"
#include "aes.h"
#include "aes_streams.h"
#include <iostream>

bool test_rc4()
{
	std::vector<std::string> keys = { "Key", "Secret" };
	std::vector<std::string> plaintexts = { "Plaintext", "Attack at dawn" };

	std::vector<std::string> keystreams(keys.size(), std::string(256, char(0)));
	std::vector<std::string> ciphertexts(plaintexts.size());

	std::vector<std::vector<unsigned char> > check_ciphertexts = { {0xBB, 0xF3, 0x16, 0xE8, 0xD9, 0x40, 0xAF, 0x0A, 0xD3} , {0x45, 0xA0, 0x1F, 0x64, 0x5F, 0xC3, 0x5B, 0x38, 0x35, 0x52, 0x54, 0x4B, 0x9B, 0xF5} };

	for (int i = 0; i < ciphertexts.size(); ++i)
	{
		ciphertexts[i] = std::string(plaintexts[i], char(0));
	}

	bool res = true;
	for (int i = 0; i < keys.size(); ++i)
	{
		rc4::key_schedule(keystreams[i], keys[i]);
		rc4::PRGA(keystreams[i], plaintexts[i], ciphertexts[i]);

		for (int j = 0; j < ciphertexts[i].size(); ++j)
		{
			if ((unsigned char)ciphertexts[i][j] != check_ciphertexts[i][j])
			{
				res = false;
			}
		}
	}

	
	return res;
}

int to_big_endian(int x)
{
	int res = 0;
	for (int i = 0; i < 4; ++i)
	{
		res = res << 8 | ((x >> (i * 8)) & 0xff);
	}
	return res;
}

bool test_salsa20()
{

	std::vector<uint32_t> key(8), nonce(2);
	std::vector<uint32_t> out(16);
	salsa20::salsa20_cipher(key, nonce, 0, out);

	

	std::vector<uint32_t> check =
	{
		0x9a97f65b, 0x9b4c721b, 0x960a6721, 0x45fca8d4,
		0xe32e67f9, 0x111ea979, 0xce9c4826, 0x806aeee6,
		0x3de9c0da, 0x2bd7f91e, 0xbcb2639b, 0xf989c625,
		0x1b29bf38, 0xd39a9bdc, 0xe7c55f4b, 0x2ac12a39
	};

	bool res = true;
	for (int i = 0; i < out.size(); ++i)
	{
		if (to_big_endian(out[i]) != check[i])
			res = false;

	}

	return res;
}

bool test_aes_streams()
{
	int len = 4;
	std::vector<uint8_t> plaintext(16 * len, 0), iv_block(16, 0), iv_ctr;
	aes::ctr_iv_gen(iv_ctr, 16 * len);
	plaintext[0] = 42;
	std::vector<uint8_t> key = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
	int nr = 10;
	std::vector<uint32_t> w = std::vector<uint32_t>(4 * (nr + 1));
	aes::key_expansion(key, w);


	// ECB
	std::vector<uint8_t> ecb_cipher, ecb_reverse_plain;

	aes::aes_ecb_encrypt(plaintext, ecb_cipher, w);
	aes::aes_ecb_decrypt(ecb_cipher, ecb_reverse_plain, w);

	for (int i = 0; i < 16 * len; ++i)
	{
		std::cout << ((plaintext[i] == ecb_reverse_plain[i]) ? "OK " : "SAD ");
	}
	std::cout << '\n';

	// CBC
	std::vector<uint8_t> cbc_cipher, cbc_reverse_plain;

	aes::aes_cbc_encrypt(plaintext, cbc_cipher, iv_block, w);
	aes::aes_cbc_decrypt(cbc_cipher, cbc_reverse_plain, iv_block, w);

	for (int i = 0; i < 16 * len; ++i)
	{
		std::cout << ((plaintext[i] == cbc_reverse_plain[i]) ? "OK " : "SAD ");
	}
	std::cout << '\n';

	// CFB
	std::vector<uint8_t> cfb_cipher, cfb_reverse_plain;

	aes::aes_cfb_encrypt(plaintext, cfb_cipher, iv_block, w, 8);
	aes::aes_cfb_decrypt(cfb_cipher, cfb_reverse_plain, iv_block, w, 8);

	for (int i = 0; i < 16 * len; ++i)
	{
		std::cout << ((plaintext[i] == cfb_reverse_plain[i]) ? "OK " : "SAD ");
	}
	std::cout << '\n';

	//OFB
	std::vector<uint8_t> ofb_cipher, ofb_reverse_plain;

	aes::aes_ofb_encrypt(plaintext, ofb_cipher, iv_block, w);
	aes::aes_ofb_decrypt(ofb_cipher, ofb_reverse_plain, iv_block, w);

	for (int i = 0; i < 16 * len; ++i)
	{
		std::cout << ((plaintext[i] == ofb_reverse_plain[i]) ? "OK " : "SAD ");
	}
	std::cout << '\n';

	//CTR
	std::vector<uint8_t> ctr_cipher, ctr_reverse_plain;

	aes::aes_cbc_encrypt(plaintext, ctr_cipher, iv_ctr, w);
	aes::aes_cbc_decrypt(ctr_cipher, ctr_reverse_plain, iv_ctr, w);

	for (int i = 0; i < 16 * len; ++i)
	{
		std::cout << ((plaintext[i] == ctr_reverse_plain[i]) ? "OK " : "SAD ");
	}
	std::cout << '\n';

	std::vector<uint8_t> in = { 0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34 };
	std::vector<uint8_t> out1 = std::vector<uint8_t>(16, 0);

	aes::cipher(in, out1, w);

	return true;
}

int main()
{
	std::cout << std::hex;
	std::cout << "rc4: " << (test_rc4() ? "OK" : "FAILED") << '\n';
	std::cout << "salsa20: " << (test_salsa20() ? "OK" : "FAILED") << '\n';

	test_aes_streams();
}