#pragma once
#include "ChainPoint.h"
#include <list>

class OnLine {
private:
	std::list<ChainPoint> chains;
	std::list<MyPoint> points;
public:
	void push(ChainPoint chain) {
		if (chain.point1 == chain.point2)
			return;


		auto result = std::find(chains.begin(), chains.end(), chain);
		if (result == chains.end()) {
			chains.push_back(chain);

			auto Presult1 = std::find(points.begin(), points.end(), chain.point1);
			if (Presult1 == points.end())
				points.push_back(chain.point1);
			auto Presult2 = std::find(points.begin(), points.end(), chain.point2);
			if (Presult2 == points.end())
				points.push_back(chain.point2);
		}
	}

	std::list<ChainPoint> getChains() {
		return chains;
	}

	std::list<MyPoint> getPoints() {
		return points;
	}


};

