#pragma once
//
#include <string>
#include <vector>

namespace sha
{
	void message_padding(const std::string& message, std::vector<uint32_t>& padded_message);
	void sha256(std::vector<uint32_t>& padded_message, std::vector<uint32_t>& message_hash);
	void sha256(const std::string& s, std::vector<uint32_t>& message_hash);
}