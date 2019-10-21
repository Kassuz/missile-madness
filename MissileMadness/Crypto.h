#pragma once

#include <string>

class Crypto
{
public:

	static bool InitCrypto();

	static bool HashPassword(const std::string& password, std::string& outHash);
	static bool VerifyPassword(const std::string& password, const std::string& hash);

private:
	static bool s_HasInit;
};
