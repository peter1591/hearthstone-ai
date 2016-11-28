#pragma once

#include <string>
#include "json/value.h"
#include "ai_invoker.h"

class RequestHandler
{
public:
	bool Initialize();
	void Cleanup();

	void Process(std::string const& input);

private:
	int GetSequenceIdOrDefault(Json::Value const& json, int default_value);
	std::string GetActionOrDefault(Json::Value const& json, std::string const& default_value);

	void SendResponse(Json::Value const& response);
	void SendResponse_Ack(Json::Value const& input);
	void SendResponse_Error(Json::Value const& json, std::string const& error_detail);

	bool ParseAsJson(std::string const& input, Json::Value & json);

private:
	AIInvoker ai_invoker;
};