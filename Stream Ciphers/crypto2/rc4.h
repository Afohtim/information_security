#pragma once
#include <vector>
#include <string>

namespace rc4
{
	void key_schedule(std::string& s, std::string key);
	void PRGA(std::string& s, std::string plaintext, std::string& ciphertext);
}