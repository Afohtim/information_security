#pragma once
#include "aes.h"


namespace aes
{
	template<class T>
	void xor_array(std::vector<T>& arr_in, const std::vector<T>& arr_xor_with)
	{
		(arr_in.size() == arr_xor_with.size());
		
		for (int i = 0; i < arr_in.size(); ++i)
		{
			arr_in[i] ^= arr_xor_with[i];
		}
	}

	void aes_ecb_encrypt(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext, const std::vector<uint32_t>& dw);
	void aes_ecb_decrypt(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext, const std::vector<uint32_t>& dw);

	void aes_cbc_encrypt(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& iv, const std::vector<uint32_t>& dw);
	void aes_cbc_decrypt(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& iv, const std::vector<uint32_t>& dw);

	void aes_cfb_encrypt(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& iv, const std::vector<uint32_t>& dw, int s);
	void aes_cfb_decrypt(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& iv, const std::vector<uint32_t>& dw, int s);

	void aes_ofb_encrypt(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& iv, const std::vector<uint32_t>& dw);
	void aes_ofb_decrypt(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& iv, const std::vector<uint32_t>& dw);

	void ctr_iv_gen(std::vector<uint8_t>& iv, int len, int rand_const = 0);
	void aes_ctr_encrypt(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& iv, const std::vector<uint32_t>& dw);
	void aes_ctr_decrypt(const std::vector<uint8_t>& plaintext, std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& iv, const std::vector<uint32_t>& dw);

}