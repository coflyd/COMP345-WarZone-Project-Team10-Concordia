#include "Map.h"
#include <iostream>
#include <vector>

using namespace std;

int main() {
	auto mapLoader = std::make_unique<MapLoader>("data/europe_map/europe.map");
	Map europe(std::move(mapLoader));
	europe.initialCountryDistribution(4);
	europe.display();
	cout << europe << endl;
	europe.validate();
	return 0;
}
