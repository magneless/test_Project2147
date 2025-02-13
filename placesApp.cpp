#include "placesApp.h"

#include "validation.h"

PlacesApp::PlacesApp(std::shared_ptr<PlacesDB> db) : db(db) {}

crow::response PlacesApp::addPlace(const crow::request& req) {
	auto reqJson = crow::json::load(req.body);
	if (!validate(reqJson)) return crow::response(crow::status::BAD_REQUEST);

	try {
		db->insert({reqJson["name"].s(), reqJson["type"].s(),
					reqJson["coordinates"].lo()[0].d(),
					reqJson["coordinates"].lo()[1].d()});
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error: " << e.what();
		return crow::response(crow::status::INTERNAL_SERVER_ERROR);
	}

	return crow::response(crow::status::OK, "Place added successfully");
}

crow::response PlacesApp::getPlaces(const crow::request& req) {
	std::string type =
		req.url_params.get("type") ? req.url_params.get("type") : "";
	std::vector<Place> places;
	try {
		places = db->get(type);
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error: " << e.what();
		return crow::response(crow::status::INTERNAL_SERVER_ERROR);
	}
	crow::json::wvalue respJson;
	for (size_t i = 0; i < places.size(); ++i) {
		respJson[i] = {
			{"name", places[i].name},
			{"type", places[i].type},
			{"coordinates", crow::json::wvalue::list(
								{places[i].latitude, places[i].longitude})}};
	}
	return crow::response(crow::status::OK, respJson.dump());
}