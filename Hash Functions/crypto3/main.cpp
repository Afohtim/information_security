#include "sha.h"
#include "kupyna.h"
#include <iostream>

#include <bitset>

#include "PoW.h"

void padding_check()
{
	std::string s = "abc";
	std::vector<uint32_t> padded;
	sha::message_padding(s, padded);

	for (auto c : padded)
	{
		std::bitset<32> cc(c);
		std::cout << cc << ' ';
	}
}

void sha_check()
{
	std::string s = "abc";
	std::vector<uint32_t> hashed;

	sha::sha256(s, hashed);
	std::cout << std::hex;
	for (auto h : hashed)
	{
		std::cout << h << ' ';
	}
}

void kupyna_check()
{
	std::string s = "abc";
	std::vector<uint64_t> hashed;

	kupyna::kupuna256(s, hashed);
	std::cout << std::hex;
	for (auto h : hashed)
	{
		std::cout << h << ' ';
	}
}


int main()
{
	srand(time(0));
	kupyna_check();
	//PoW();
}