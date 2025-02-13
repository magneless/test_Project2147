#include "validation.h"

bool validate(const crow::json::rvalue& reqJson) {
	const std::vector<std::string> requiredFileds = {"name", "type",
													 "coordinates"};

	if (!reqJson.has(requiredFileds[0])) return false;
	if (!reqJson.has(requiredFileds[1])) return false;

	if (!reqJson.has(requiredFileds[2])) return false;
	if (reqJson[requiredFileds[2]].t() != crow::json::type::List) return false;
	auto coordinates = reqJson[requiredFileds[2]].lo();
	if (coordinates.size() != 2) return false;
	if (coordinates[0].t() != crow::json::type::Number ||
		coordinates[1].t() != crow::json::type::Number)
		return false;
	
	return true;
}