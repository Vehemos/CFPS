#include <iostream>
#include <chrono>
using namespace std;

#include <Windows.h>

int sWidth = 120;
int sHt= 40;

float pX= 8.0f;
float pY = 8.0f;
float pA = 0.0f;

int mapHt= 16;
int mapWidth = 16;

float fov = 3.14159 / 4.0;
float depth = 16.0f;

int main()
{
	wchar_t *screen = new wchar_t[sHt*sWidth];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	wstring map;

	map += L"################";
	map += L"#..............#";
	map += L"####...........#";
	map += L"#..............#";
	map += L"#.........######";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#......#.......#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#........#######";
	map += L"#..............#";
	map += L"#..............#";
	map += L"################";

	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	while (1)
	{
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float eTime = elapsedTime.count();


		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			pA -= (1.0f) * eTime;
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			pA += (1.0f) * eTime;
		if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		{
			pX+= sinf(pA) * 5.0f * eTime;
			pY += cosf(pA) * 5.0f * eTime;
			if (map[(int)pY * mapWidth + (int)pX] == '#')
			{
				pX-= sinf(pA) * 5.0f * eTime;
				pY -= cosf(pA) * 5.0f * eTime;
			}
		}
		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{
			pX-= sinf(pA) * 5.0f * eTime;
			pY -= cosf(pA) * 5.0f * eTime;
			if (map[(int)pY * mapWidth + (int)pX] == '#')
			{
				pX+= sinf(pA) * 5.0f * eTime;
				pY += cosf(pA) * 5.0f * eTime;
			}
		}
		
		for (int x = 0; x < sWidth; x++)
		{
			float rayAng = (pA - fov / 2.0f) + ((float)x / (float)sWidth)*fov;

			float DistWall = 0;
			bool wallHit = false;
			bool boundary = false;

			float eX = sinf(rayAng);
			float eY = cosf(rayAng);

			while (!wallHit && DistWall < depth)
			{
				DistWall += 0.1f;

				int nTestX = (int)(pX+ eX * DistWall);
				int nTestY = (int)(pY + eY * DistWall);

				if (nTestX < 0 || nTestX >= mapWidth || nTestY < 0 || nTestY >= mapHt)
				{
					wallHit = true;
					DistWall = depth;
				}
				else
				{
					if (map[nTestY * mapWidth + nTestX] == '#')
					{
						wallHit = true;
					}
				}

			}

			int ceiling = (float)(sHt/ 2.0) - sHt/ ((float)DistWall);
			int nFloor = sHt - ceiling;

			short shade = ' ';

			if (DistWall <= depth / 4.0f)			
				shade = 0x2588;
			else if (DistWall < depth / 3.0f)		
				shade = 0x2593;
			else if (DistWall < depth / 2.0f)		
				shade = 0x2592;
			else if (DistWall < depth)				
				shade = 0x2591;
			else
				shade = ' ';


			for (int y = 0; y < sHt; y++)
			{
				if (y <= ceiling)
					screen[y*sWidth + x] = ' ';
				else if (y > ceiling && y <= nFloor)
					screen[y*sWidth + x] = shade;
				else
				{
					float b = 1.0f - (((float)y - sHt/ 2.0f) / ((float)sHt/ 2.0f));
					if (b < 0.25)		
						shade = '#';
					else if (b < 0.5)	
						shade = 'x';
					else if (b < 0.75)	
						shade = '.';
					else if (b < 0.9)	
						shade = '-';
					else				
						shade = ' ';
					screen[y*sWidth + x] = shade;
				}
			}
		}

		screen[sHt* sWidth - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, sHt*sWidth, { 0,0 }, &dwBytesWritten);
	}


	return 0;
}