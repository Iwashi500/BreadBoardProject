#pragma once
#include <list>
#include "MyPoint.h"
#include "ChainPoint.h"
#include "OnLine.h"

class BreadBoard
{
private:
	std::list<OnLine> onLines;

public:
	void push(OnLine onLine) {
		onLines.push_back(onLine);
	}

	std::list<OnLine> getOnLines() {
		return this->onLines;
	}

	OnLine getOnLine(MyPoint point);
};



