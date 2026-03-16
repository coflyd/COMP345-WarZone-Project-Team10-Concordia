#include "Map.h"
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <unordered_set>
#include <algorithm>
#include <random>
#include <vector>
#include <filesystem>
#include <string>

using namespace std;
namespace fs = std::filesystem;

// ======== MapLoader =======

MapLoader::MapLoader(const std::string &mapFileName) {
	std::ifstream mapFile(mapFileName);
	if (!mapFile.is_open()) {
		std::cerr << "Error: Unable to open the file " << mapFileName
				<< std::endl;
		return;
	}

	std::cout << "Opening the file: " << mapFileName << std::endl;

	std::string line;
	std::string currentSection;
	int continent_number = 1;

	while (getline(mapFile, line)) {
		if (!line.empty() && line.back() == '\r') {
			line.pop_back(); // For files in Windows format (\r\n)
		}
		if (!line.empty() && line.back() == '\n') {
			line.pop_back(); // For files in Unix format (\n)
		}
		std::cout << "Ligne lue : " << line << std::endl; // Debug
		// Ignore comments and blank lines
		if (line.empty() || line[0] == ';' || line[0] == ' ') {
			continue;
		}

		// Section detection
		if (line[0] == '[' && line.back() == ']') {
			currentSection = line.substr(1, line.size() - 2);
			std::cout << "\n--- Start of section : [" << currentSection
					<< "] ---" << std::endl;
			continue;
		}

		// Processing of sections
		if (currentSection == "files") {
			this->files.push_back(line);
			std::cout << "File added : " << line << std::endl;

		} else if (currentSection == "continents") {
			std::istringstream iss(line);
			Continent *continent = new Continent;
			continent->id = continent_number++;
			iss >> continent->name >> continent->army >> continent->color;
			this->continents.push_back(continent);
			std::cout << "Continent added : " << continent->id << " "
					<< continent->name << " (army: " << continent->army
					<< ", color: " << continent->color << ")" << std::endl;

		} else if (currentSection == "countries") {
			std::istringstream iss(line);
			Country *country = new Country;
			iss >> country->id >> country->name >> country->continentNumber
					>> country->x >> country->y;
			country->playerName = nullptr;
			this->countries.push_back(country);
			std::cout << "Country added : " << country->id << " - "
					<< country->name << " (continent: "
					<< country->continentNumber << ", contact details: "
					<< country->x << ", " << country->y << ")" << std::endl;

		} else if (currentSection == "borders") {
			std::istringstream iss(line);
			int countryId;
			iss >> countryId;
			std::vector<int> neighbors;
			int neighborId;
			while (iss >> neighborId) {
				neighbors.push_back(neighborId);
			}
			this->borders[countryId] = neighbors;
			std::cout << "Connections added for country ID " << countryId
					<< " : ";
			for (int neighbor : neighbors) {
				std::cout << neighbor << " ";
			}
			std::cout << std::endl;
		}
	}

	mapFile.close();
	std::cout << "\nClosing the file. Loading complete." << std::endl;
}

MapLoader::~MapLoader() {
	for (Continent *continent : continents) {
		delete continent;
	}
	for (Country *country : countries) {
		delete country;
	}
}

const std::map<int, std::vector<int>>& MapLoader::getBorders() const {
	return borders;
}

const std::vector<Continent*>& MapLoader::getContinents() const {
	return continents;
}

const std::vector<Country*>& MapLoader::getCountries() const {
	return countries;
}

// Copy constructor MapLoader
MapLoader::MapLoader(const MapLoader &other) :
		files(other.files), borders(other.borders) {
	for (Continent *continent : other.continents) {
		continents.push_back(new Continent(*continent));
	}
	for (Country *country : other.countries) {
		countries.push_back(new Country(*country));
	}
}
bool Map::areAdjacent(int countryId1, int countryId2) const {
	auto it = neighboursEdge.find(countryId1);
	if (it == neighboursEdge.end()) return false;
	for (int neighbor : it->second) {
		if (neighbor == countryId2) return true;
	}
	return false;
}
// MapLoader assignment operator
MapLoader& MapLoader::operator=(const MapLoader &other) {
	if (this != &other) {
		files = other.files;
		borders = other.borders;

		// Free existing memory
		for (Continent *continent : continents) {
			delete continent;
		}
		for (Country *country : countries) {
			delete country;
		}
		continents.clear();
		countries.clear();

		// Deep copy
		for (Continent *continent : other.continents) {
			continents.push_back(new Continent(*continent));
		}
		for (Country *country : other.countries) {
			countries.push_back(new Country(*country));
		}
	}
	return *this;
}

// Insertion operator in a stream for MapLoader
std::ostream& operator<<(std::ostream &os, const MapLoader &mapLoader) {
	os << "=== MapLoader Summary ===" << std::endl;
	os << "Files: ";
	for (const auto &file : mapLoader.files) {
		os << file << " ";
	}
	os << std::endl;

	os << "Continents (" << mapLoader.continents.size() << "):" << std::endl;
	for (Continent *continent : mapLoader.continents) {
		os << "  - ID " << continent->id << " " << continent->name << " (Army: "
				<< continent->army << ", Color: " << continent->color << ")"
				<< std::endl;
	}

	os << "Countries (" << mapLoader.countries.size() << "):" << std::endl;
	for (Country *country : mapLoader.countries) {
		os << "  - " << country->id << ": " << country->name << " (Continent: "
				<< country->continentNumber << ", Coords: " << country->x
				<< ", " << country->y << ")" << std::endl;
	}

	os << "Borders (" << mapLoader.borders.size() << " entries):" << std::endl;
	for (const auto& [countryId, neighbors] : mapLoader.borders) {
		os << "  - Country ID " << countryId << " neighbors: ";
		for (int neighborId : neighbors) {
			os << neighborId << " ";
		}
		os << std::endl;
	}
	return os;
}

// Printing of MapLoader
void MapLoader::display() {
	std::cout << "=== Continent(s) ===" << std::endl;
	for (Continent *continent : continents) {
		std::cout << "ID: " << continent->id << ", Name: " << continent->name
				<< ", Army: " << continent->army << ", Color: "
				<< continent->color << std::endl;
	}

	std::cout << "\n=== Country ===" << std::endl;
	for (Country *country : countries) {
		std::cout << "ID: " << country->id << ", Name: " << country->name
				<< ", Continent: " << country->continentNumber << ", Coords: ("
				<< country->x << ", " << country->y << ")" << std::endl;
	}

	std::cout << "\n=== Borders ===" << std::endl;
	for (const auto& [countryId, neighbors] : borders) {
		std::cout << "Country ID " << countryId << " is connected to: ";
		for (int neighborId : neighbors) {
			std::cout << neighborId << " ";
		}
		std::cout << std::endl;
	}
}

// Function to list .map files in a directory and its subdirectories
string MapLoader::selectMapFile(const string &directoryPath) {
	vector<string> mapFiles;
	vector<string> mapFilesPaths;

	for (const auto &entry : fs::recursive_directory_iterator(directoryPath)) {
		if (entry.is_regular_file() && entry.path().extension() == ".map") {
			mapFiles.push_back(entry.path().filename().string());
			mapFilesPaths.push_back(entry.path().string());
		}
	}

	cout << "Found .map files:\n";
	for (size_t i = 0; i < mapFiles.size(); ++i) {
		cout << (i + 1) << ": " << mapFiles[i] << "\n";
	}

	if (mapFiles.empty()) {
		cerr << "No .map files found in the specified directory.\n";
		return "";
	}

	int choice;
	cout << "Choose a file (enter the number): ";
	cin >> choice;

	if (choice < 1 || choice > static_cast<int>(mapFiles.size())) {
		cerr << "Invalid choice.\n";
		return "";
	}

	return mapFilesPaths[choice - 1];
}

// ======== Map ========

// Map Constructor
Map::Map(MapLoader *mapLoader) {
	for (Continent *continent : mapLoader->getContinents()) {
		continents.push_back(new Continent(*continent));
	}
	for (Country *country : mapLoader->getCountries()) {
		countryNode[country->id] = new Country(*country);
	}
	neighboursEdge = mapLoader->getBorders();
}

// Copy constructor Map
Map::Map(const Map &other) {
	for (const auto& [id, country] : other.countryNode) {
		countryNode[id] = new Country(*country);
	}
	neighboursEdge = other.neighboursEdge;
	for (Continent *continent : other.continents) {
		continents.push_back(new Continent(*continent));
	}
}

// Map assignment operator
Map& Map::operator=(const Map &other) {
	if (this != &other) {
		// Free existing memory
		for (auto& [id, country] : countryNode) {
			delete country;
		}
		countryNode.clear();
		for (Continent *continent : continents) {
			delete continent;
		}
		continents.clear();

		// Deep copy
		for (const auto& [id, country] : other.countryNode) {
			countryNode[id] = new Country(*country);
		}
		neighboursEdge = other.neighboursEdge;
		for (Continent *continent : other.continents) {
			continents.push_back(new Continent(*continent));
		}
	}
	return *this;
}

// Destructor Map
Map::~Map() {
	for (auto& [id, country] : countryNode) {
		delete country;
	}
	for (Continent *continent : continents) {
		delete continent;
	}
}

// Insert operator in a stream for Map
std::ostream& operator<<(std::ostream &os, const Map &map) {
	os << "=== Map Summary ===" << std::endl;
	os << "Country Nodes (" << map.countryNode.size() << "):" << std::endl;
	for (const auto& [id, country] : map.countryNode) {
		os << "  - ID: " << id << ", Name: " << country->name << ", Continent: "
				<< country->continentNumber << ", Coords: (" << country->x
				<< ", " << country->y << ")";
		if (country->playerName != nullptr) {
			os << ", Player: " << *country->playerName;
		}
		os << std::endl;
	}

	os << "Neighbours Edge (" << map.neighboursEdge.size() << " entries):"
			<< std::endl;
	for (const auto& [id, neighbors] : map.neighboursEdge) {
		os << "  - Country ID " << id << " neighbors: ";
		for (int neighborId : neighbors) {
			os << neighborId << " ";
		}
		os << std::endl;
	}
	return os;
}
/*
void Map::display() {
	std::cout << "\n=== Country Nodes ===" << std::endl;
	for (const auto& [id, country] : countryNode) {
		std::cout << "ID: " << id << ", Name: " << country->name
				<< ", Continent: " << country->continentNumber << ", Coords: ("
				<< country->x << ", " << country->y << ")";
		if (country->playerName != nullptr) {
			cout << ", Player: " << *country->playerName;
		}
		cout << std::endl;
	}

	std::cout << "\n=== Neighbours Edge ===" << std::endl;
	for (const auto& [id, neighbors] : neighboursEdge) {
		std::cout << "Country ID " << id << " is connected to: ";
		for (int neighborId : neighbors) {
			std::cout << neighborId << " ";
		}
		std::cout << std::endl;
	}
}
*/
// ====== CONNECTIVITY's GRAPH =====
bool Map::isGraphConnected() const {
	if (neighboursEdge.empty()) {
		return false;
	}
	std::queue<int> queue;
	std::unordered_set<int> visited;
	int startCountry = neighboursEdge.begin()->first;
	queue.push(startCountry);
	visited.insert(startCountry);

	while (!queue.empty()) {
		int current = queue.front();
		queue.pop();
		for (int neighbor : neighboursEdge.at(current)) {
			if (visited.find(neighbor) == visited.end()) {
				visited.insert(neighbor);
				queue.push(neighbor);
			}
		}
	}
	return visited.size() == neighboursEdge.size();
}

// ====== CONNECTIVITY's SUBGRAPH =====
bool Map::isThisGraphConnected(const std::map<int, Country*> &nodes,
		const std::map<int, std::vector<int>> &edges) {

	if (edges.empty() || nodes.empty()) {
		return false;
	}

	std::queue<int> queue;
	std::unordered_set<int> visited;
	int startNode = edges.begin()->first;
	queue.push(startNode);
	visited.insert(startNode);

	while (!queue.empty()) {
		int current = queue.front();
		queue.pop();
		for (int neighbor : edges.at(current)) {
			if (nodes.find(neighbor) != nodes.end()
					&& visited.find(neighbor) == visited.end()) {
				visited.insert(neighbor);
				queue.push(neighbor);
			}
		}
	}

	if (visited.size() == nodes.size()) {
		cout << "Graph is connected" << endl;
		return true;
	} else {
		cerr << "Graph is not connected" << endl;
		return false;
	}
}

// ====== CONNECTIVITY's SUBGRAPH by CONTINENT =====
bool Map::isEachContinentIsGraphsConnected() const {
	for (Continent *continent : continents) {
		int currentContinentId = continent->id;
		std::map<int, Country*> currentCountryNode;
		std::map<int, std::vector<int>> currentNeighboursEdge;

		for (const auto& [id, country] : countryNode) {
			if (country->continentNumber == currentContinentId) {
				currentCountryNode[id] = country;
			}
		}

		for (const auto& [id, neighbors] : neighboursEdge) {
			if (currentCountryNode.find(id) != currentCountryNode.end()) {
				std::vector<int> continentNeighbors;
				for (int neighborId : neighbors) {
					if (currentCountryNode.find(neighborId)
							!= currentCountryNode.end()) {
						continentNeighbors.push_back(neighborId);
					}
				}
				currentNeighboursEdge[id] = continentNeighbors;
			}
		}

		if (!Map::isThisGraphConnected(currentCountryNode,
				currentNeighboursEdge)) {
			std::cerr << "The continent " << continent->name << " (ID: "
					<< currentContinentId << ") is not connected !"
					<< std::endl;
			return false;
		} else {
			std::cout << "The continent " << continent->name << " (ID: "
					<< currentContinentId << ") is fully connected !"
					<< std::endl;
		}
	}
	return true;
}

bool Map::isEachCountryBelongsToOneAndOnlyOneContinent() const {
	std::vector<int> continentsId;
	for (Continent *c : continents) {
		continentsId.push_back(c->id);
	}
	for (const auto& [id, country] : countryNode) {
		if (std::find(continentsId.begin(), continentsId.end(),
				country->continentNumber) == continentsId.end()) {
			std::cerr << country->name
					<< " is not connected to an existing continent "
					<< std::endl;
			return false;
		} else {
			std::cout << country->name
					<< " is connected with an existing continent " << std::endl;
		}
	}
	return true;
}

bool Map::validate() {
	if (isGraphConnected() && isEachContinentIsGraphsConnected()
			&& isEachCountryBelongsToOneAndOnlyOneContinent()) {
		cout << "\nMap is connected" << endl;
		return true;
	} else {
		cerr << "\nMap is not connected" << endl;
		return false;
	}
}

int Map::getCountriesNumber() const {
	return countryNode.size();
}

std::string Map::getCountryName(int territoryId) const {
	return countryNode.at(territoryId)->name;
}
/*
void Map::initialCountryDistribution(unsigned int numPlayers) {
	int i = 0;
	for (auto& [id, country] : countryNode) {
		country->playerName = new std::string(
				"Player_" + std::to_string((i % numPlayers) + 1));
		i++;
	}
}

void Map::initialRandomCountryDistribution(unsigned int numPlayers) {
	std::vector<int> countryBag;
	for (const auto& [id, country] : countryNode) {
		countryBag.push_back(id);
	}
	std::shuffle(countryBag.begin(), countryBag.end(), std::mt19937 {
			std::random_device { }() });
	int i = 0;
	for (int idCountry : countryBag) {
		countryNode[idCountry]->playerName = new std::string(
				"Player_" + std::to_string((i % numPlayers) + 1));
		i++;
	}
}
*/
Country* Map::getCountryById(int id) const {
	auto it = countryNode.find(id);
	if (it != countryNode.end()) {
		return it->second;
	}
	return nullptr;
}
