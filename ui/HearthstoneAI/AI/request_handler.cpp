#include <iostream>
#include <string>
#include "json/reader.h"
#include "json./writer.h"

#include "request_handler.h"

bool RequestHandler::Initialize()
{
	return this->ai_invoker.Initialize();
}

void RequestHandler::Cleanup()
{
	this->ai_invoker.Cleanup();
}

void RequestHandler::Process(std::string const& raw_input)
{
	Json::Value input;
	if (!this->ParseAsJson(raw_input, input)) {
		this->SendResponse_Error(input, "Failed to parse as json");
	}

	std::string action = this->GetActionOrDefault(input, "");


	if (action == "GetBestMove") {
		this->SendResponse_Ack(input);
		if (!input.isMember("game")) this->SendResponse_Error(input, "missing field: 'game'");
		else this->ai_invoker.CreateNewTask(input["game"]);
	}
	else if (action == "Cancel") {
		this->ai_invoker.CancelAllTasks();
		this->SendResponse_Ack(input);
	}
	else if (action == "GetCurrentBestMove") {
		this->ai_invoker.GenerateCurrentBestMoves();
		this->SendResponse_Ack(input);
	}
	else if (action == "UpdateBoard") {
		this->SendResponse_Ack(input);
		if (!input.isMember("game")) this->SendResponse_Error(input, "missing field: 'game'");
		else this->ai_invoker.BoardActionStart(input["game"]);
	}
	else {
		this->SendResponse_Error(input, "Unrecognized action");
	}
}

int RequestHandler::GetSequenceIdOrDefault(Json::Value const& json, int default_value)
{
	int value = default_value;
	try {
		value = json["sequence"].asInt();
	}
	catch (std::exception) {
		return default_value;
	}
	return value;
}

std::string RequestHandler::GetActionOrDefault(Json::Value const& json, std::string const& default_value)
{
	std::string value = default_value;
	try {
		value = json["action"].asString();
	}
	catch (std::exception) {
		return default_value;
	}
	return value;
}

void RequestHandler::SendResponse_Ack(Json::Value const& input)
{
	Json::Value response;
	response["response_to_sequence"] = GetSequenceIdOrDefault(input, -1);
	response["type"] = "ack";

	this->SendResponse(response);
}

void RequestHandler::SendResponse_Error(Json::Value const& json, std::string const& error_detail)
{
	Json::Value response;
	response["response_to_sequence"] = GetSequenceIdOrDefault(json, -1);
	response["type"] = "error";
	response["error"] = error_detail;

	this->SendResponse(response);
}

void RequestHandler::SendResponse(Json::Value const& response)
{
	Json::FastWriter writer;
	std::string json = writer.write(response);

	while (true)
	{
		size_t it = json.find_first_of('\n');
		if (it == std::string::npos) it = json.find_first_of('\r');
		if (it == std::string::npos) break;
		json.replace(it, it + 1, "");
	}
	
	std::cout << json << '\n';
}

bool RequestHandler::ParseAsJson(std::string const& input, Json::Value & json)
{
	Json::Reader reader;
	return reader.parse(input, json, false);
}

