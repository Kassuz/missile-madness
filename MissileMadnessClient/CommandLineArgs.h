#pragma once

class CommandLineArgs
{
public:
	static void ProcessCommandLineArgs(int argc, char** argv);

	static bool ShouldShowDebugPos() { return s_ShowPlayerDebugPos; }
	static bool ShouldShowStats() { return s_ShowStats; }
	static float GetPacketLoss() { return s_PacketLossPercent; }

private:
	static bool s_ShowPlayerDebugPos;
	static bool s_ShowStats;
	static float s_PacketLossPercent;
};

