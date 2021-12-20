#pragma once

#include <vector>
#include <string>


bool test1();

void hex_to_vector(std::string str, std::vector<uint8_t>& vec);
bool test_aes(std::string in_, std::string key_, std::string enc_);
double encrypt_random_message_aes(std::vector<uint32_t>& w);
double time_test_aes(int rounds, std::string str_key);
bool test_kalyna(std::string str_key, std::string str_plaintext, std::string str_ciphertext);
double encrypt_random_message_kalyna(std::vector<std::vector<uint64_t>>& w);
double time_test_kalyna(int rounds, std::string str_key);