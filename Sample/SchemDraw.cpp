#include "pch.h"
#include "SchemDraw.h"

void SchemDraw::addPart(PartType pt, Direction d, CStringW labelString) {
	
	CStringW part = L"";
	CStringW direction = L"";
	CStringW label = L"";

	switch (pt)
	{
	case SchemDraw::Line:
		part = L".Line()";
		break;
	case SchemDraw::Resister:
		part = L".Resistor()";
		break;
	case SchemDraw::LED:
		part = L".LED()";
		break;
	default:
		break;
	}

	switch (d)
	{
	case SchemDraw::UP:
		direction = L".up()";
		break;
	case SchemDraw::DOWN:
		direction = L".down()";
		break;
	case SchemDraw::LEFT:
		direction = L".left()";
		break;
	case SchemDraw::RIGHT:
		direction = L".right()";
		break;
	default:
		break;
	}

	if (!labelString.IsEmpty()) {
		label = L".label('" + labelString + L"')";
	}

	CStringW txt = L"d += elm"
		+ part + direction + label
		+ L"\n";
	fputws(txt, fp);

}
