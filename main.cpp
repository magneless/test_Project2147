#include <sqlite3.h>

#include <cstring>

#include "crow.h"
#include "validation.h"

int main() {
	sqlite3* db;

	int rc = sqlite3_open("storage.db", &db);
	if (rc != SQLITE_OK) return 1;

	const char* sql =
		"CREATE TABLE IF NOT EXISTS places ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"name TEXT NOT NULL, "
		"type TEXT NOT NULL, "
		"latitude REAL NOT NULL, "
		"longitude REAL NOT NULL);";

	rc = sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
	if (rc != SQLITE_OK) return 1;

	crow::SimpleApp app;
	CROW_ROUTE(app, "/places")
		.methods(crow::HTTPMethod::POST)([db](const crow::request& req) {
			auto reqJson = crow::json::load(req.body);
			if (!validate(reqJson))
				return crow::response(crow::status::BAD_REQUEST);
			std::string name = reqJson["name"].s();
			std::string type = reqJson["type"].s();
			auto coordinates = reqJson["coordinates"].lo();
			double latitude = coordinates[0].d();
			double longitude = coordinates[1].d();

			const char* sql =
				"INSERT INTO places (name, type, latitude, longitude) "
				"VALUES (?, ?, ?, ?);";
			sqlite3_stmt* stmt;
			int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK)
				return crow::response(crow::status::INTERNAL_SERVER_ERROR);

			sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_text(stmt, 2, type.c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_double(stmt, 3, latitude);
			sqlite3_bind_double(stmt, 4, longitude);

			rc = sqlite3_step(stmt);
			if (rc != SQLITE_DONE)
				return crow::response(crow::status::INTERNAL_SERVER_ERROR);
			sqlite3_finalize(stmt);

			crow::response res;
			res.body = "OK!";
			return res;
		});
	CROW_ROUTE(app, "/places")
		.methods(crow::HTTPMethod::GET)([db](const crow::request& req) {
			sqlite3_stmt* stmt;
			int rc;
			auto type = req.url_params.get("type");
			if (type) {
				const char* sql =
					"SELECT name, type, latitude, longitude FROM places WHERE "
					"type = (?)";
				rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
				sqlite3_bind_text(stmt, 1, type, -1, SQLITE_STATIC);
			} else {
				const char* sql =
					"SELECT name, type, latitude, longitude FROM places;";
				rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
			}

			if (rc != SQLITE_OK)
				return crow::response(crow::status::INTERNAL_SERVER_ERROR);

			crow::json::wvalue respJson;
			int i = 0;
			while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
				const char* name =
					reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
				const char* type =
					reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
				double latitude = sqlite3_column_double(stmt, 2);
				double longitude = sqlite3_column_double(stmt, 3);

				respJson[i] = {{"coordinates", crow::json::wvalue::list(
												   {latitude, longitude})},
							   {"type", type},
							   {"name", name}};
				i++;
			}

			sqlite3_finalize(stmt);
			crow::response res;
			res.body = respJson.dump();
			return res;
		});
	app.bindaddr("127.0.0.1").port(8080).multithreaded().run();

	sqlite3_close(db);
}