#pragma once

enum GameplayPacketType
{
	GPT_HELLO,
	GPT_WELCOME,
	GPT_GAME_START,
	GPT_REPLICATION_DATA,
	GPT_GAME_ENDED,
	GPT_MAX_PACKET
};

enum LobbyPacketType 
{ 
	LPT_LOGIN_DATA, 
	LPT_CREATE_USER,
	LPT_USER_DATA,
	LPT_USER_DISCONNECT,
	LPT_MATCH_DATA,
	LPT_CLIENT_REQUEST,
	LPT_SERVER_RESPONSE,
	LPT_GAME_START,
	LPT_MAX_PACKET
};

enum ServerResponse 
{ 
	SR_LOGIN_OK, 
	SR_LOGIN_WRONG,
	SR_USER_CREATE_SUCCESS,
	SR_USER_ALREADY_EXISTS,
	SR_MAX_RESPONSE
};

enum ClientRequest
{
	CR_SET_READY,
	CR_COLOR_CHANGE,
	CR_MATCH_DATA,
	CR_MAX_REQ
};

enum MatchDataFormat
{
	MDF_ALL_TIME,
	MDF_LAST_MATCH,
	MDF_FIVE_MATCHES,
	MDF_MAX_FORMAT
};