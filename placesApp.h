#pragma once
#include <memory>
#include <string>

#include "crow.h"
#include "placesDB.h"

class PlacesApp {
   public:
	PlacesApp(const std::string& dbPath);
	crow::response addPlace(const crow::request& req);
	crow::response getPlaces(const crow::request& req);

   private:
	std::unique_ptr<PlacesDB> db;
};
