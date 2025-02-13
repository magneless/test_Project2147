#include <sqlite3.h>

#include <memory>

#include "crow.h"
#include "placesApp.h"
#include "placesDB.h"

int main() {
	const std::string dbPath = "storage.db";
	std::shared_ptr<PlacesDB> db;
	try {
		db = std::make_shared<PlacesDB>(dbPath);
		db->create();
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error: " << e.what();
		return 1;
	}
	PlacesApp placesApp(db);

	crow::SimpleApp app;
	CROW_ROUTE(app, "/places")
		.methods(crow::HTTPMethod::POST)(
			[&placesApp](const crow::request& req) {
				return placesApp.addPlace(req);
			});
	CROW_ROUTE(app, "/places")
		.methods(crow::HTTPMethod::GET)([&placesApp](const crow::request& req) {
			return placesApp.getPlaces(req);
		});

	app.bindaddr("127.0.0.1").port(8080).multithreaded().run();
}
