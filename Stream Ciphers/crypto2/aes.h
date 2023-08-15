#pragma once
#include <vector>
#include <cstdint>
#include <array>



namespace aes
{
	struct Aes_word
	{
		uint32_t word;

		Aes_word()
		{
			word = 0;
		}

		Aes_word(uint32_t w)
		{
			word = w;
		}

		uint8_t& operator[](int i)
		{
			return ((uint8_t*)&word)[3-i];
		}

		uint32_t operator+(uint32_t other)
		{
			return word + other;
		}

		uint32_t operator-(uint32_t other)
		{
			return word + other;
		}

		uint32_t operator^(uint32_t other)
		{
			return word ^ other;
		}

		uint32_t operator=(uint32_t other)
		{
			return word = other;
		}

		uint32_t operator+=(uint32_t other)
		{
			return word += other;
		}

		uint32_t operator-=(uint32_t other)
		{
			return word -= other;
		}

		uint32_t operator^=(uint32_t other)
		{
			return word ^= other;
		}
	};

	struct Aes_state
	{
		std::array<Aes_word, 4> state;

		Aes_state() {}


		Aes_state(Aes_state& other)
		{
			state = std::array<Aes_word, 4>(other.state);
		}

		Aes_state(std::vector<uint32_t> state_vector)
		{
			for (int i = 0; i < 4; ++i)
			{
				state[i] = state_vector[i];
			}

		}


		int size()
		{
			return state.size();
		}

		Aes_word& operator[](int i)
		{
			return state[i];
		}
	};

	void key_expansion(const std::vector<uint8_t>& key, std::vector<uint32_t>& w);

	void cipher(const std::vector<uint8_t>& in, std::vector<uint8_t>& out, const std::vector<uint32_t>& dw);

	void inv_cipher(const std::vector<uint8_t>& in, std::vector<uint8_t>& out, const std::vector<uint32_t>& dw);
}
