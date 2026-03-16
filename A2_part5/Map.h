#ifndef MAP_H_
#define MAP_H_

#include <iostream>
#include <map>
#include <string>
#include <vector>

struct Continent {
    int id;
    std::string name;
    int army;
    std::string color;
};

struct Country {
    int id;
    std::string name;
    int continentNumber;
    int x;
    int y;
    std::string *playerName;
    int armies = 0;
};

class MapLoader {
private:
    std::vector<std::string> files;
    std::vector<Continent*> continents;
    std::vector<Country*> countries;
    std::map<int, std::vector<int>> borders;

public:
    MapLoader(const std::string &mapFileName);
    ~MapLoader();

    const std::map<int, std::vector<int>>& getBorders() const;
    const std::vector<Continent*>& getContinents() const;
    const std::vector<Country*>& getCountries() const;

    MapLoader(const MapLoader &other);
    MapLoader& operator=(const MapLoader &other);
    friend std::ostream& operator<<(std::ostream &os, const MapLoader &map);
    void display();
    static std::string selectMapFile(const std::string &directoryPath);
};

class Map {
private:
    std::vector<Continent*> continents;
    std::map<int, Country*> countryNode;
    std::map<int, std::vector<int>> neighboursEdge;

public:
    bool areAdjacent(int countryId1, int countryId2) const;
    Map(MapLoader *mapObjects);
    Map(const Map &other);
    Map& operator=(const Map &other);
    ~Map();

    friend std::ostream& operator<<(std::ostream &os, const Map &map);
    /// void display();
    bool validate();
    int getCountriesNumber() const;
    std::string getCountryName(int territoryId) const;
    bool isGraphConnected() const;
    static bool isThisGraphConnected(
        const std::map<int, Country*> &nodes,
        const std::map<int, std::vector<int>> &edges);
    bool isEachContinentIsGraphsConnected() const;
    bool isEachCountryBelongsToOneAndOnlyOneContinent() const;
    Country* getCountryById(int id) const;
};

#endif /* MAP_H_ */
