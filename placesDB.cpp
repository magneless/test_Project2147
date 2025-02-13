#include "placesDB.h"

#include <stdexcept>

PlacesDB::PlacesDB(const std::string& dbPath) {
	int rc = sqlite3_open(dbPath.c_str(), &db);
	if (rc != SQLITE_OK) throw std::runtime_error("Error during db connection");
}

PlacesDB::~PlacesDB() { sqlite3_close(db); }

void PlacesDB::create() {
	const char* sql =
		"CREATE TABLE IF NOT EXISTS places ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"name TEXT NOT NULL, "
		"type TEXT NOT NULL, "
		"latitude REAL NOT NULL, "
		"longitude REAL NOT NULL);";

	char* errMsg = nullptr;
	int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
	if (rc != SQLITE_OK) {
		std::string err = errMsg;
		sqlite3_free(errMsg);
		throw std::runtime_error("Error during table creation" + err);
	};
}

void PlacesDB::insert(const Place& place) {
	const char* sql =
		"INSERT INTO places (name, type, latitude, longitude) "
		"VALUES (?, ?, ?, ?);";

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
		throw std::runtime_error(
			"Error during request preparation to insert place");

	sqlite3_bind_text(stmt, 1, place.name.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, place.type.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_double(stmt, 3, place.latitude);
	sqlite3_bind_double(stmt, 4, place.longitude);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		sqlite3_finalize(stmt);
		throw std::runtime_error(
			"Error during request execution to insert place");
	}

	sqlite3_finalize(stmt);
}

std::vector<Place> PlacesDB::get(const std::string& type) {
	std::string sql = "SELECT name, type, latitude, longitude FROM places";
	if (type != "")
		sql += " WHERE type = ?;";
	else
		sql += ";";

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
		throw std::runtime_error(
			"Error during request preparation to get places");

	if (type != "") sqlite3_bind_text(stmt, 1, type.c_str(), -1, SQLITE_STATIC);

	std::vector<Place> places;
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		places.push_back(
			{reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)),
			 reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
			 sqlite3_column_double(stmt, 2), sqlite3_column_double(stmt, 3)});
	}
	if (rc != SQLITE_DONE) {
		sqlite3_finalize(stmt);
		throw std::runtime_error("Error during row iteration");
	}
	sqlite3_finalize(stmt);
	return places;
}
