#pragma once

#include <string>

class CommandLineArgs
{
public:
	static bool ProcessCommandLineArgs(int argc, char** argv);

	static bool ShouldShowDebugPos() { return s_ShowPlayerDebugPos; }
	static bool ShouldShowStats() { return s_ShowStats; }
	static float GetPacketLoss() { return s_PacketLossPercent; }

	static const std::string GetServerAddress() { return s_ServerAddress; }
	static bool UseCustomServerAddress() { return s_UseCustomServerAddress; }

private:
	static bool s_ShowPlayerDebugPos;
	static bool s_ShowStats;
	static float s_PacketLossPercent;

	static bool s_UseCustomServerAddress;
	static std::string s_ServerAddress;
};

