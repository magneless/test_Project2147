#include <sqlite3.h>

#include <cstring>
#include <memory>

#include "crow.h"
#include "placesApp.h"

int main() {
	const std::string dbPath = "storage.db";
	PlacesApp placesApp(dbPath);

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
