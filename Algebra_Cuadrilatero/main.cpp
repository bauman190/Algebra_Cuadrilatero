
#include <iostream>
#include <string>
#include <cmath>
#include "raylib.h"

using namespace std;

const int totalLines = 4;
const int possibleIntersections = 3;
const int maxIntersections = totalLines * possibleIntersections;
int intersections = 0;
int linesDrawn = 0;

struct Line
{
	Vector2 origin;
	Vector2 end;
	Vector2 intersections[possibleIntersections];
	int totalLineIntersections = 0;
};

// Sobrecarga del operador == para Vector2
bool operator==(const Vector2& lhs, const Vector2& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

// Sobrecarga del operador != para Vector2
bool operator!=(const Vector2& lhs, const Vector2& rhs)
{
	return lhs.x != rhs.x || lhs.y != rhs.y;
}

// Sobrecarga del operador == para Line
bool operator==(const Line& lhs, const Line& rhs)
{
	return lhs.origin == rhs.origin && lhs.end == rhs.end;
}

// Sobrecarga del operador != para Line
bool operator!=(const Line& lhs, const Line& rhs)
{
	return lhs.origin != rhs.origin || lhs.end != rhs.end;
}


void FillLines(Line lines[], int& clickCounter, int& i);

void DrawLines(Line lines[]);

bool MyCheckCollisionLines(Line line1, Line line2, Vector2& collisionPoint);

void SaveIntersections(Line lines[]);

void DrawIntersections(Line lines[]);

bool CheckMinimumIntersections();

bool CheckMinimumLinesIntersections(Line lines[]);

bool IsQuadrilateral(Line lines[], Vector2 previousIntersections[]);

bool IsNotPreviousLine(Line previousLines[], Line otherLine, int step);

float CalculateArea(Vector2 points[], int n);

float CalculatePerimeter(Vector2 points[], int n);



// Program main entry point
void main()
{
	Line lines[totalLines];
	Vector2 usedIntersections[totalLines];

	float area = 0.0f;
	float perimeter = 0.0f;
	string areaText;
	string perimeterText;

	//Screen
	const int screenWidth = 1366;
	const int screenHeight = 768;

	int clickCounter = 0;
	int i = 0;
	for (int j = 0; j < totalLines; j++)
	{
		lines[j].origin.x = 0;
		lines[j].end.x = 0;
		lines[j].origin.y = 0;
		lines[j].end.y = 0;
	}

	InitWindow(screenWidth, screenHeight, "Ventana de prueba");

	// Main game loop
	while (!WindowShouldClose()) // Detect window close button or ESC key
	{

		FillLines(lines, clickCounter, i);
		SaveIntersections(lines);
		// Update

		// TODO: Update your variables here

		// Draw    
		BeginDrawing();

		DrawLines(lines);
		DrawIntersections(lines);

		if (linesDrawn == 4)
		{
			if (IsQuadrilateral(lines, usedIntersections))
			{
				DrawText("ES CUADRILATERO!!", 5, 5, 30, GREEN);

				//Perimetro y area
				area = CalculateArea(usedIntersections, 4);
				perimeter = CalculatePerimeter(usedIntersections, 4);

				areaText = "Area: " + to_string(perimeter);
				perimeterText = "Perimieter: " + to_string(area);

				DrawText(areaText.data(), 5, GetScreenHeight() - 35, 30, BLUE);
				DrawText(perimeterText.data(), 5, GetScreenHeight() - 75, 30, BLUE);
			}
			else
			{
				DrawText("NO ES CUADRILATERO!!", 5, 5, 30, RED);
			}
		}

		ClearBackground(BLACK);

		EndDrawing();
	}

	// De-Initialization 
	CloseWindow();        // Close window and OpenGL context
}

void FillLines(Line lines[], int& clickCounter, int& i)
{

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && i < 4)
	{

		if (clickCounter % 2 == 0)
		{
			lines[i].origin = GetMousePosition();
			clickCounter++;
		}
		else
		{
			lines[i].end = GetMousePosition();
			i++;
			clickCounter++;
		}
	}
}

void DrawLines(Line lines[])
{
	linesDrawn = 0;
	for (int j = 0; j < totalLines; j++)
	{
		if (lines[j].origin.x != 0 && lines[j].origin.y != 0 && lines[j].end.x != 0 && lines[j].end.y != 0)
		{
			DrawLine(lines[j].origin.x, lines[j].origin.y, lines[j].end.x, lines[j].end.y, WHITE);
			linesDrawn++;
		}
	}
}

bool MyCheckCollisionLines(Line line1, Line line2, Vector2& collisionPoint)
{
	Vector2 startPos1;
	Vector2 endPos1;

	Vector2 startPos2;
	Vector2 endPos2;

	startPos1.x = line1.origin.x;
	startPos1.y = line1.origin.y;
	endPos1.x = line1.end.x;
	endPos1.y = line1.end.y;

	startPos2.x = line2.origin.x;
	startPos2.y = line2.origin.y;
	endPos2.x = line2.end.x;
	endPos2.y = line2.end.y;

	bool isIntersection = false;

	//Calculo el determinante
	float determinant = (endPos2.y - startPos2.y) * (endPos1.x - startPos1.x) - (endPos2.x - startPos2.x) * (endPos1.y - startPos1.y);

	if (determinant != 0)
	{
		//Supuestas coordenadas comunes
		float xi = ((startPos2.x - endPos2.x) * (startPos1.x * endPos1.y - startPos1.y * endPos1.x) - (startPos1.x - endPos1.x) * (startPos2.x * endPos2.y - startPos2.y * endPos2.x)) / determinant;
		float yi = ((startPos2.y - endPos2.y) * (startPos1.x * endPos1.y - startPos1.y * endPos1.x) - (startPos1.y - endPos1.y) * (startPos2.x * endPos2.y - startPos2.y * endPos2.x)) / determinant;

		//Verifico que sean parte de la primer linea
		if (((xi > startPos1.x && xi < endPos1.x) || (xi > endPos1.x && xi < startPos1.x)) &&
			((yi > startPos1.y && yi < endPos1.y) || (yi > endPos1.y && yi < startPos1.y)))
		{
			//Verifico que sean parte de las segunda linea
			if (((xi > startPos2.x && xi < endPos2.x) || (xi > endPos2.x && xi < startPos2.x)) &&
				((yi > startPos2.y && yi < endPos2.y) || (yi > endPos2.y && yi < startPos2.y)))
			{
				//Guardo la interseccion
				isIntersection = true;
				collisionPoint.x = xi;
				collisionPoint.y = yi;
			}
		}
	}

	return isIntersection;
}

void SaveIntersections(Line lines[])
{
	//Inicializo en valores imposibles
	for (int i = 0; i < totalLines; i++)
	{
		for (int j = 0; j < possibleIntersections; j++)
		{
			lines[i].intersections[j].x = -1;
			lines[i].intersections[j].y = -1;
		}
	}

	//Guardo las intersecciones
	for (int i = 0; i < totalLines; i++)
	{
		int aux = 0;
		for (int j = 0; j < totalLines; j++)
		{
			if (i != j)
			{
				if (MyCheckCollisionLines(lines[i], lines[j], lines[i].intersections[aux]))
				{
					intersections++;
					lines[i].totalLineIntersections++;
					aux++;
				}
			}
		}
	}
}

void DrawIntersections(Line lines[])
{
	for (int i = 0; i < totalLines; i++)
	{
		for (int j = 0; j < possibleIntersections; j++)
		{
			if (lines[i].intersections[j].x != -1 && lines[i].intersections[j].y != -1)
				DrawCircle(lines[i].intersections[j].x, lines[i].intersections[j].y, 6, RED);
		}
	}
}

bool CheckMinimumIntersections()
{
	return intersections >= 8;
}

bool CheckMinimumLinesIntersections(Line lines[])
{
	for (int i = 0; i < totalLines; i++)
	{
		int lineIntersections = 0;
		for (int j = 0; j < possibleIntersections; j++)
		{
			if (lines[i].intersections[j].x != -1 && lines[i].intersections[j].y != -1)
				lineIntersections++;
		}

		if (lineIntersections < 2)
			return false;
	}

	return true;
}

bool IsQuadrilateral(Line lines[], Vector2 previousIntersections[]) {
	Line previousLines[totalLines];
	int step = 0;
	Vector2 startIntersection;
	Line startLine;

	float area = 0.0f;
	float perimeter = 0.0f;

	//Reviso las condiciones minimas
	if (!CheckMinimumIntersections() || !CheckMinimumLinesIntersections(lines))
		return false;

	//Encuentra una intersección inicial
	bool startFound = false;
	for (int i = 0; i < totalLines && !startFound; i++)
	{
		for (int j = 0; j < possibleIntersections && !startFound; j++)
		{
			if (lines[i].intersections[j].x != -1 && lines[i].intersections[j].y != -1)
			{
				startIntersection = lines[i].intersections[j];
				startLine = lines[i];
				previousLines[step] = lines[i];
				step++;
				startFound = true;
			}
		}
	}

	Vector2 currentIntersection = startIntersection;

	//Recorre hasta haber encontrado 4 intersecciones relacionadas en las 4 lineas
	while (step < totalLines)
	{
		bool foundNext = false;

		//Busco la siguiente interseccion
		for (int i = 0; i < totalLines && !foundNext; i++)
		{
			//Reviso que no sea una linea que ya haya usado
			if (IsNotPreviousLine(previousLines, lines[i], step) || step == totalLines - 1)
			{
				for (int j = 0; j < possibleIntersections; j++)
				{
					//Encuentra el ultimo vertice
					if (step == totalLines - 1 && lines[i] == startLine && lines[i].intersections[j] == startIntersection)
					{
						return true;
					}
					else if (lines[i].intersections[j] == currentIntersection) //Encuentro en que otra linea esta la interseccion 
					{
						previousIntersections[step - 1] = lines[i].intersections[j];
						previousLines[step] = lines[i];
						currentIntersection = lines[i].intersections[(j + 1) % lines[i].totalLineIntersections]; /*Cambia a la
							siguiente intersección (si es la tercer interseccion de la linea cambia a la primera,
							si es la segunda cambia a la tercera y asi)*/
						foundNext = true;
						break;
					}
				}
			}
		}

		//Quizas esto se puede sacar
		if (!foundNext)
			return false; //Si no encuentra la siguiente intersección, no hay cuadrilátero

		step++;
	}

	/*Nunca llega a este if
	if (currentIntersection == startIntersection && step == totalLines - 1)
	{
		area = CalculateArea(previousIntersections, 4);
		BeginDrawing();
		string areaText = "Area: " + to_string(area);
		DrawText(areaText.data(), 0, GetScreenHeight() - 50, 30, BLUE);
		EndDrawing();
		return true;
	}
	*/
	return false;
}

bool IsNotPreviousLine(Line previousLines[], Line otherLine, int step)
{
	//Reviso no haber trabajado con esta interseccion antes
	for (int i = 0; i < step; i++)
	{
		if (otherLine == previousLines[i])
			return false;
	}

	return true;
}

float CalculateArea(Vector2 points[], int n) {
	float area = 0.0f;

	for (int i = 0; i < n; i++) {
		int j = (i + 1) % n;
		area += points[i].x * points[j].y;
		area -= points[j].x * points[i].y;
	}

	return abs(area) / 2.0f; //Valor absoluto
}

float CalculatePerimeter(Vector2 points[], int n) {
	float perimeter = 0.0f;

	for (int i = 0; i < n; i++) {
		int j = (i + 1) % n;
		perimeter += sqrt(pow(points[j].x - points[i].x, 2) + pow(points[j].y - points[i].y, 2));
	}

	return perimeter;
}