#include "CommandLineArgs.h"

#include <cstdio>

bool CommandLineArgs::s_ShowPlayerDebugPos = false;
bool CommandLineArgs::s_ShowStats = false;
float CommandLineArgs::s_PacketLossPercent = 0.0f;

bool CommandLineArgs::s_UseCustomServerAddress = false;
std::string CommandLineArgs::s_ServerAddress = "";


bool CommandLineArgs::ProcessCommandLineArgs(int argc, char** argv)
{
	if (argc == 1) return true;

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
				return false;
			}
		}
		// Show stats, fps and such
		else if (arg == "--stats")
		{
			printf("Show debug stats!\n");
			s_ShowStats = true;
		}
		// Set custom server address
		else if (arg == "--server-address")
		{
			i++;
			s_ServerAddress = argv[i];
			s_UseCustomServerAddress = true;
			printf("Set server address to %s\n", s_ServerAddress.c_str());
		}
		else
		{
			printf("Unknown command line argunent %s\n", arg.c_str());
			return false;
		}
	}

	printf("\n\n");

	return true;
}
