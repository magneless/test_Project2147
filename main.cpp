#include <sqlite3.h>

#include <cstring>

#include "crow.h"
#include "placesDB.h"
#include "validation.h"

int main() {
	std::string pathToDB = "storage.db";
	PlacesDB* db;
	try {
		db = new PlacesDB(pathToDB);
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error: " << e.what();
		return 1;
	}
	try {
		db->create();
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error: " << e.what();
		return 1;
	}

	crow::SimpleApp app;
	CROW_ROUTE(app, "/places")
		.methods(crow::HTTPMethod::POST)([&db](const crow::request& req) {
			auto reqJson = crow::json::load(req.body);
			if (!validate(reqJson))
				return crow::response(crow::status::BAD_REQUEST);
			std::string name = reqJson["name"].s();
			std::string type = reqJson["type"].s();
			auto coordinates = reqJson["coordinates"].lo();
			double latitude = coordinates[0].d();
			double longitude = coordinates[1].d();

			try {
				db->insert({name, type, latitude, longitude});
			} catch (const std::exception& e) {
				CROW_LOG_ERROR << "Error: " << e.what();
				return crow::response(crow::status::INTERNAL_SERVER_ERROR);
			}

			crow::response res;
			res.code = crow::status::OK;
			res.body = "Place added!";
			return res;
		});
	CROW_ROUTE(app, "/places")
		.methods(crow::HTTPMethod::GET)([&db](const crow::request& req) {
			std::string type;
			if (req.url_params.get("type")) type = req.url_params.get("type");

			std::vector<Place> places;
			try {
				places = db->get(type);
			} catch (const std::exception& e) {
				CROW_LOG_ERROR << "Error: " << e.what();
				return crow::response(crow::status::INTERNAL_SERVER_ERROR);
			}
			crow::json::wvalue respJson;
			int i = 0;
			for (const auto& place : places) {
				respJson[i] = {
					{"name", place.name},
					{"type", place.type},
					{"coordinates", crow::json::wvalue::list(
										{place.latitude, place.longitude})}};
				i++;
			}

			crow::response res;
			res.code = crow::status::OK;
			res.body = respJson.dump();
			return res;
		});

	app.bindaddr("127.0.0.1").port(8080).multithreaded().run();
}
