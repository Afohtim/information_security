#pragma once
#include <cstdint>
#include <vector>

namespace salsa20
{
	void salsa20_cipher(const std::vector<uint32_t>& key, const std::vector<uint32_t>& nonce, uint64_t block_n, std::vector<uint32_t>& out);
}