#pragma once
#include <memory>
#include <string>

#include "crow.h"
#include "placesDB.h"

class PlacesApp {
   public:
	PlacesApp(std::shared_ptr<PlacesDB> db);
	crow::response addPlace(const crow::request& req);
	crow::response getPlaces(const crow::request& req);

   private:
	std::shared_ptr<PlacesDB> db;
};
