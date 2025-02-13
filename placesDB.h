#pragma once

#include <sqlite3.h>

#include <string>
#include <vector>

struct Place {
	std::string name;
	std::string type;
	double latitude;
	double longitude;
};

class PlacesDB {
   public:
	PlacesDB(const std::string& dbPath);
	~PlacesDB();

	void create();
	void insert(const Place& place);
	std::vector<Place> get(const std::string& type = "");

   private:
	sqlite3* db;
};
