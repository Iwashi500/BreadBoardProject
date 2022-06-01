#include "pch.h"
#include "BreadBoard.h"
#include <queue>
#include <vector>
#include <map>

using namespace std;
using namespace cv;


bool operator<(const Connection& a, const Connection& b) noexcept {
	if(a.point1.x != b.point1.x)
		return a.point1.x < b.point1.x;

	if (a.point1.y != b.point1.y)
		return a.point1.y < b.point1.y;

	if (a.point2.x != b.point2.x)
		return a.point2.x < b.point2.x;

	return a.point2.y < b.point2.y;
}

String BreadBoard::getHoleName(Point position) {
	String xName = format("%d", position.x + 1);
	String yName;
	switch (position.y)
	{
	case 0:
		yName = "1-";
		break;
	case 1:
		yName = "1+";
		break;
	case 2:
		yName = "j";
		break;
	case 3:
		yName = "i";
		break;
	case 4:
		yName = "h";
		break;
	case 5:
		yName = "g";
		break;
	case 6:
		yName = "f";
		break;
	case 7:
		yName = "e";
		break;
	case 8:
		yName = "d";
		break;
	case 9:
		yName = "c";
		break;
	case 10:
		yName = "b";
		break;
	case 11:
		yName = "a";
		break;
	case 12:
		yName = "2-";
		break;
	case 13:
		yName = "2+";
		break;
	default:
		yName = "unknow";
		break;
	}

	return xName + "-" + yName;
}

bool BreadBoard::checkUsed(int y, int x) {
	return this->checkUsed(Point(x, y));
}

bool BreadBoard::checkUsed(Point position) {
	if (position.y < 0 || position.x < 0 || position.y > 13 || position.x > 29)
		return false;
	else if ((position.y <= 1 || 12 <= position.y) && (position.x > 25))
		return false;

	return find(usedHoles.begin(), usedHoles.end(), position) != usedHoles.end();
}

vector<Point>::iterator BreadBoard::deleteUsedHole(Point hole) {
	auto ite = find(usedHoles.begin(), usedHoles.end(), hole);

	if (ite != usedHoles.end()) {
		return usedHoles.erase(ite);
	}
	else
		return ite;
}

struct CheckPoint
{
	Point point;
	bool isShort;
};

BreadBoard::ErrorType BreadBoard::checkCircleError() {

	queue<CheckPoint> que;
	map<Connection, bool> dist;

	for (const Connection* connect : connections) {
		CheckPoint cp;
		if (connect->point1.y == 1) {
			cp.point = connect->point2;
			cp.isShort = true;
			que.push(cp);
			Connection c(Point(2, 3), Point(3, 4), PartType::WIRE);
			dist[*connect] = true;
		}
		else if (connect->point2.y == 1) {
			cp.point = connect->point1;
			cp.isShort = true;
			que.push(cp);
			dist[*connect] = true;
		}
		else {
			dist[*connect] = false;
		}
	}

	//VCCなしチェック
	if (que.size() == 0)
		return BreadBoard::ErrorType::NO_VCC;

	queue<CheckPoint> que_con = queue<CheckPoint>(que);
	map<Connection, bool> dist_con = map<Connection, bool>(dist);

	const PartType wire = PartType::WIRE;
	const PartType resistor = PartType::RESISTOR;
	const PartType condensor = PartType::CONDENSER;


	//ショートチェック
	while (!que.empty()) {
		CheckPoint cp = que.front();
		que.pop();

		if (cp.point.y == 12) {
			if (cp.isShort) 
				return ErrorType::SHORT;

			continue;
		}

		for (const pair<Connection, bool> d : dist) {
			//探索済みの線ならパス
			if (d.second == true) continue;

			if (d.first.point1 == cp.point) {
				if (d.first.type.type == resistor.type || d.first.type.type == condensor.type)
					continue;

				CheckPoint ncp;
				ncp.point = d.first.point2;
				ncp.isShort = true;
				que.push(ncp);
				dist[d.first] = true;
			}
			else if (d.first.point2 == cp.point) {
				if (d.first.type.type == resistor.type || d.first.type.type == condensor.type)
					continue;

				CheckPoint ncp;
				ncp.point = d.first.point1;
				ncp.isShort = true;
				que.push(ncp);
				dist[d.first] = true;
			}
		}
	}


	//導通チェック
	bool conduction = false; //導通チェック
	while (!que_con.empty()) {
		CheckPoint cp = que_con.front();
		que_con.pop();

		if (cp.point.y == 12) {
			conduction = true;
			break;
		}

		for (const pair<Connection, bool> d : dist_con) {
			//探索済みの線ならパス
			if (d.second == true) continue;

			if (d.first.point1 == cp.point) {
				CheckPoint ncp;
				ncp.point = d.first.point2;
				ncp.isShort = true;
				que_con.push(ncp);
				dist_con[d.first] = true;
			}
			else if (d.first.point2 == cp.point) {
				CheckPoint ncp;
				ncp.point = d.first.point1;
				ncp.isShort = true;
				que_con.push(ncp);
				dist_con[d.first] = true;
			}
		}
	}

	if (!conduction)
		return BreadBoard::ErrorType::NO_CONDUCTION;

	return BreadBoard::ErrorType::NONE;
}
