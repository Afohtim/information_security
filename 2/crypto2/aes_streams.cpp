#include <vector>
#include "aes_streams.h"


namespace aes
{
	void aes_ecb_encrypt(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext, const std::vector<uint32_t>& dw)
	{
		ciphertext.clear();
		if (plaintext.size() % 16 != 0)
		{
			throw "wrong plaintext length";
		}

		for (int i = 0; i < plaintext.size(); i += 16)
		{
			std::vector<uint8_t> plaintext_slice(plaintext.begin() + i, plaintext.begin() + i + 16), res(16, 0);
			cipher(plaintext_slice, res, dw);
			for (auto c : res)
				ciphertext.push_back(c);
			//ciphertext.insert(ciphertext.end(), res.begin(), res.end());
		}
	}

	void aes_ecb_decrypt(const std::vector<uint8_t>& ciphertext, std::vector<uint8_t>& plaintext, const std::vector<uint32_t>& dw)
	{
		plaintext.clear();
		if (ciphertext.size() % 16 != 0)
		{
			throw "wrong plaintext length";
		}

		for (int i = 0; i < ciphertext.size(); i += 16)
		{
			std::vector<uint8_t> ciphertext_slice(ciphertext.begin() + i, ciphertext.begin() + i + 16), res(16, 0);
			inv_cipher(ciphertext_slice, res, dw);
			for (auto c : res)
				plaintext.push_back(c);
			//plaintext.insert(plaintext.end(), res.begin(), res.end());
		}
	}
}

namespace aes
{
	void aes_cbc_encrypt(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& iv, const std::vector<uint32_t>& dw)
	{
		ciphertext.clear();
		if (plaintext.size() % 16 != 0)
		{
			throw "wrong plaintext length";
		}

		std::vector<uint8_t> current_vec = iv;
		for (int i = 0; i < plaintext.size(); i += 16)
		{
			std::vector<uint8_t> plaintext_slice(plaintext.begin() + i, plaintext.begin() + i + 16), res(16, 0);
			xor_array(plaintext_slice, current_vec);
			cipher(plaintext_slice, res, dw);
			for (auto c : res)
				ciphertext.push_back(c);
			//ciphertext.insert(ciphertext.end(), res.begin(), res.end());
			current_vec = res;
		}
	}

	void aes_cbc_decrypt(const std::vector<uint8_t>& ciphertext, std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& iv, const std::vector<uint32_t>& dw)
	{
		plaintext.clear();
		if (ciphertext.size() % 16 != 0)
		{
			throw "wrong plaintext length";
		}

		std::vector<uint8_t> current_vec = iv;
		for (int i = 0; i < ciphertext.size(); i += 16)
		{
			std::vector<uint8_t> ciphertext_slice(ciphertext.begin() + i, ciphertext.begin() + i + 16), res(16, 0);
			inv_cipher(ciphertext_slice, res, dw);
			xor_array(res, current_vec);
			for (auto c : res)
				plaintext.push_back(c);
			//plaintext.insert(plaintext.end(), res.begin(), res.end());
			current_vec = ciphertext_slice;
		}
	}
}


namespace aes
{
	void aes_cfb_encrypt(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& iv, const std::vector<uint32_t>& dw, int s)
	{
		ciphertext.clear();
		if (plaintext.size() % s != 0)
		{
			throw "wrong plaintext length";
		}

		std::vector<uint8_t> current_vec = iv;
		for (int i = 0; i < plaintext.size(); i += s)
		{
			std::vector<uint8_t> plaintext_slice(plaintext.begin() + i, plaintext.begin() + i + s), res(16, 0);
			while (plaintext_slice.size() < 16)
				plaintext_slice.push_back(0);
			cipher(current_vec, res, dw);
			std::vector<uint8_t> next_vec = res;
			xor_array(res, plaintext_slice);
			for (auto c : res)
				ciphertext.push_back(c);
			//ciphertext.insert(ciphertext.end(), res.begin(), res.end());
			current_vec = next_vec;
		}
	}

	void aes_cfb_decrypt(const std::vector<uint8_t>& ciphertext, std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& iv, const std::vector<uint32_t>& dw, int s)
	{
		plaintext.clear();
		if (ciphertext.size() % 16 != 0)
		{
			throw "wrong plaintext length";
		}

		std::vector<uint8_t> current_vec = iv;
		for (int i = 0; i < ciphertext.size(); i += 16)
		{
			std::vector<uint8_t> ciphertext_slice(ciphertext.begin() + i, ciphertext.begin() + i + 16), res(16, 0);
			cipher(current_vec, res, dw);
			std::vector<uint8_t> next_vec = res;
			xor_array(res, ciphertext_slice);
			for (int j = 0; j < s; ++j)
				plaintext.push_back(res[j]);
			//plaintext.insert(plaintext.end(), res.begin(), res.end());
			current_vec = next_vec;
		}
	}
}


namespace aes
{
	void aes_ofb_encrypt(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& iv, const std::vector<uint32_t>& dw)
	{
		ciphertext.clear();
		if (plaintext.size() % 16 != 0)
		{
			throw "wrong plaintext length";
		}

		std::vector<uint8_t> current_vec = iv;
		for (int i = 0; i < plaintext.size(); i += 16)
		{
			std::vector<uint8_t> plaintext_slice(plaintext.begin() + i, plaintext.begin() + i + 16), res(16, 0);
			cipher(current_vec, res, dw);
			std::vector<uint8_t> next_vec = res;
			xor_array(res, plaintext_slice);
			for (auto c : res)
				ciphertext.push_back(c);
			//ciphertext.insert(ciphertext.end(), res.begin(), res.end());
			current_vec = next_vec;
		}
	}

	void aes_ofb_decrypt(const std::vector<uint8_t>& ciphertext, std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& iv, const std::vector<uint32_t>& dw)
	{
		plaintext.clear();
		if (ciphertext.size() % 16 != 0)
		{
			throw "wrong plaintext length";
		}

		std::vector<uint8_t> current_vec = iv;
		for (int i = 0; i < ciphertext.size(); i += 16)
		{
			std::vector<uint8_t> ciphertext_slice(ciphertext.begin() + i, ciphertext.begin() + i + 16), res(16, 0);
			cipher(current_vec, res, dw);
			std::vector<uint8_t> next_vec = res;
			xor_array(res, ciphertext_slice);
			for (auto c : res)
				plaintext.push_back(c);
			//plaintext.insert(plaintext.end(), res.begin(), res.end());
			current_vec = next_vec;
		}
	}
}


namespace aes
{
	void ctr_iv_gen(std::vector<uint8_t>& iv, int len, int rand_const)
	{
		iv.clear();
		iv.resize(len);
		int c = rand_const;
		srand(c);
		for (int i = 0; i < len; ++i)
		{
			iv[i] = rand();
		}
	}


	void aes_ctr_encrypt(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& iv, const std::vector<uint32_t>& dw)
	{
		ciphertext.clear();
		if (plaintext.size() % 16 != 0)
		{
			throw "wrong plaintext length";
		}

		for (int i = 0; i < plaintext.size(); i += 16)
		{
			std::vector<uint8_t> plaintext_slice(plaintext.begin() + i, plaintext.begin() + i + 16), iv_slice(iv.begin() + i, iv.begin() + i + 16), res(16, 0);
			cipher(iv_slice, res, dw);
			xor_array(res, plaintext_slice);
			for (auto c : res)
				ciphertext.push_back(c);
			//ciphertext.insert(ciphertext.end(), res.begin(), res.end());
		}
	}

	void aes_ctr_decrypt(const std::vector<uint8_t>& ciphertext, std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& iv, const std::vector<uint32_t>& dw)
	{
		plaintext.clear();
		if (ciphertext.size() % 16 != 0)
		{
			throw "wrong plaintext length";
		}

		for (int i = 0; i < ciphertext.size(); i += 16)
		{
			std::vector<uint8_t> ciphertext_slice(ciphertext.begin() + i, ciphertext.begin() + i + 16), iv_slice(iv.begin() + i, iv.begin() + i + 16), res(16, 0);
			inv_cipher(iv_slice, res, dw);
			xor_array(res, ciphertext_slice);
			for (auto c : res)
				plaintext.push_back(c);
			//plaintext.insert(plaintext.end(), res.begin(), res.end());
		}
	}
}