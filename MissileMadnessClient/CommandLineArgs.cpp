#include "CommandLineArgs.h"

#include <cstdio>
#include <string>

bool CommandLineArgs::s_ShowPlayerDebugPos = false;
bool CommandLineArgs::s_ShowStats = false;
float CommandLineArgs::s_PacketLossPercent = 0.0f;

void CommandLineArgs::ProcessCommandLineArgs(int argc, char** argv)
{
	if (argc == 1) return;

	for (int i = 1; i < argc; ++i)
	{
		std::string arg = argv[i];
		
		printf("Arg %i: %s\n", i, arg.c_str());

		// Show Debug pos
		if (arg == "--debug-pos")
		{
			printf("Show player debug position!\n");
			s_ShowPlayerDebugPos = true;
		}
		// Packet loss
		else if (arg == "--PL")
		{
			i++;
			try
			{
				s_PacketLossPercent = std::stof(argv[i]);
				printf("Set Packet loss to : %.1f\n", s_PacketLossPercent);
			}
			catch (std::exception e)
			{
				printf("--PL: Can't convert arg %s to float\n", argv[i]);
			}
		}
		else if (arg == "--stats")
		{
			printf("Show debug stats!\n");
			s_ShowStats = true;
		}
	}
}
