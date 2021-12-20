#include "sha.h"
#include <map>
#include <cstdint>
#include <iostream>
void PoW()
{

	uint64_t c = 0;
	std::string empty;
	std::vector<uint32_t> hashed_empty;
	sha::sha256(empty, hashed_empty);
	uint64_t last8_empty = hashed_empty.back() & 0xffffffff;
	time_t start = clock();
	std::cout << std::hex;
	for (int i = 0; i < hashed_empty.size(); ++i)
	{
		std::cout << hashed_empty[i] << ' ';
	}
	//std::cout << last8_empty << '\n';
	std::cout << '\n' << std::dec;
	while (true)
	{
		if (c % 10000000 == 0)
			std::cout << c << std::endl;
		/*int len = 32;//rand() % 64;
		std::string s;
		for (int i = 0; i < len; ++i)
		{
			s.push_back(rand());
		}*/
		std::string s = std::to_string(c++);
		std::vector<uint32_t> hashed;
		sha::sha256(s, hashed);
		uint64_t last8 = hashed.back() & 0xffffffff;
		//std::cout << s << '\n';
		if (last8_empty == last8)
		{
			time_t elapsed = clock() - start;
			std::cout << std::dec << c - 1 << '\n' << std::hex;

			std::cout << '\n' << std::hex;
			for (int i = 0; i < hashed.size(); ++i)
			{
				std::cout << hashed[i] << ' ';
			}

			std::cout << '\n' << 1.0 * elapsed / CLOCKS_PER_SEC << " secs\n";
			break;
		}
		
	}
}