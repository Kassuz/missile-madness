#include "Crypto.h"

#define SODIUM_STATIC = 1
#define SODIUM_EXPORT = 1
#include <sodium.h>

#include "Debug.h"

bool Crypto::s_HasInit = false;

bool Crypto::InitCrypto()
{
	if (!s_HasInit)
	{
		if (sodium_init() == -1)
		{
			return false;
		}
		else
		{
			s_HasInit = true;
			return true;
		}
	}
	return true;
}

bool Crypto::HashPassword(const std::string& password, std::string& outHash)
{
	if (!s_HasInit)
	{
		Debug::LogError("Crypto has not been initialized! Call Crypto::InitCrypto() first!");
		return false;
	}

	char pwHash[crypto_pwhash_STRBYTES];

	if (crypto_pwhash_str(pwHash, password.c_str(), password.length(), crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
	{
		Debug::LogError("Password hashing run out of memory!");
		return false;
	}
	else
	{
		outHash = std::string(pwHash);
		return true;
	}
}

bool Crypto::VerifyPassword(const std::string& password, const std::string& hash)
{
	if (!s_HasInit)
	{
		Debug::LogError("Crypto has not been initialized! Call Crypto::InitCrypto() first!");
		return false;
	}

	return crypto_pwhash_str_verify(hash.c_str(), password.c_str(), password.length()) == 0;
}