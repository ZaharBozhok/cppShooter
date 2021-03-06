// Shooter.cpp : Defines the entry point for the console application.
//
#include <Windows.h>
#include <chrono>
#include <string>
#include <vector>
#include <math.h>

int nScreenWidth = 240;
int nScreenHeight = 64;

float fPlayerX = 1.0f;
float fPlayerY = 1.0f;
float fPlayerA = -4.75f;

int nMapHeight = 30;
int nMapWidth = 30;

const float Pi = 3.14159;
float fFOV = Pi / 4.0;
float fDepth = 16.0f;

inline bool EqualsAbout(const float& what, const float& with, const float& plusMinus)
{
    return what >= with - plusMinus && what <= with + plusMinus;
}
int main()
{
    wchar_t *screen = new wchar_t[nScreenHeight*nScreenWidth];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    std::wstring map;

    map += L"##############################";
    map += L"#.#..........................#";
    map += L"#.#..........................#";
    map += L"#....#######....########.....#";
    map += L"############....########.....#";
    map += L"##...............######......#";
    map += L"#.....###..........##........#";
    map += L"#....###########...#.........#";
    map += L"##.....###.....#...##........#";
    map += L"###......###.###....###......#";
    map += L"#......###............####...#";
    map += L"#..#####.....#######....######";
    map += L"#####...............##.....###";
    map += L"#..#.....#######......##....##";
    map += L"#..#....##....#####....##....#";
    map += L"#.....###........####........#";
    map += L"#...###....##...###.....###..#";
    map += L"##..##...####..........#####.#";
    map += L"##...##.................###..#";
    map += L"###...####...............##..#";
    map += L"##......##.........##.....#..#";
    map += L"#.....##......#...####.......#";
    map += L"#..###.....#############.....#";
    map += L"#.#..........##......##..##..#";
    map += L"#.#..####.....##..##..##..#..#";
    map += L"#. #.......##....####..##....#";
    map += L"#.#...###...##..##..##..##...#";
    map += L"#.#..#####...##......##..##..#";
    map += L"#.#..#####...##......##..##..#";
    map += L"##############################";

    auto tp1 = std::chrono::system_clock::now();
    auto tp2 = std::chrono::system_clock::now();
    char olddirSign = 'V';
    POINT oldpoint;
    GetCursorPos(&oldpoint);
    int pseudoZ = 0;
    bool jump = false;
    std::chrono::duration<float> jumpTime = std::chrono::system_clock::now() - std::chrono::system_clock::now();
    while (true)
    {
        float height = 0;
        
        tp2 = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2; 
        float fElapsedTime = elapsedTime.count();
        POINT newpoint;
        GetCursorPos(&newpoint);
        if(jump)
        {
            jumpTime += elapsedTime;
            height = 4.0 * jumpTime.count() - 10.0 * jumpTime.count() * jumpTime.count();
            if(height<0)
            {
                height = 0;
                jump = false;
                jumpTime = std::chrono::system_clock::now() - std::chrono::system_clock::now();
            }
        }
        if(newpoint.x != oldpoint.x)
        {
            fPlayerA += (newpoint.x - oldpoint.x) / 15.0 * fElapsedTime;
        }
        if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
        {
            float tmpfPlayerA = fPlayerA + Pi / 2;
            float xVal = 2.0f * sinf(tmpfPlayerA) * fElapsedTime;
            float yVal = 2.0f * cosf(tmpfPlayerA) * fElapsedTime;
            fPlayerX += xVal;
            fPlayerY += yVal;
        }
        if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
        {
            float tmpfPlayerA = fPlayerA - Pi / 2;
            float xVal = 2.0f * sinf(tmpfPlayerA) * fElapsedTime;
            float yVal = 2.0f * cosf(tmpfPlayerA) * fElapsedTime;
            fPlayerX += xVal;
            fPlayerY += yVal;
        }
        if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
        {
            float xVal = 2.0f * sinf(fPlayerA) * fElapsedTime;
            float yVal = 2.0f * cosf(fPlayerA) * fElapsedTime;
            fPlayerX += xVal;
            fPlayerY += yVal;
        }
        if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
        {
            fPlayerX -= 2.0f * sinf(fPlayerA) * fElapsedTime;
            fPlayerY -= 2.0f * cosf(fPlayerA) * fElapsedTime;
        }
        if (GetAsyncKeyState((unsigned short)' ') & 0x8000)
        {
            jump = true;
        }
        oldpoint = newpoint;

        for (int x = 0; x < nScreenWidth; x++)
        {
            float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;
            float fDistanceToWall = 0;
            bool bHitWall = false;
            bool bHitCorner = false;

            float fEyeVectorX = sinf(fRayAngle);
            float fEyeVectorY = cosf(fRayAngle);

            while (!bHitWall && fDistanceToWall < fDepth)
            {
                fDistanceToWall += 0.1f;

                int nTestX = (int)(fPlayerX + fEyeVectorX * fDistanceToWall);
                int nTestY = (int)(fPlayerY + fEyeVectorY * fDistanceToWall);
                bool bBoundary = false;

                if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
                {
                    bHitWall = true;
                    fDistanceToWall = fDepth;
                }
                else
                {
                    if (map[nTestX * nMapWidth + nTestY] == '#')
                    {
                        bHitWall = true;

                        std::vector<std::pair<float, float>> corners;
                        corners.push_back(std::make_pair<float, float>((float)nTestX, (float)nTestY));
                        corners.push_back(std::make_pair<float, float>((float)nTestX + 1, (float)nTestY));
                        corners.push_back(std::make_pair<float, float>((float)nTestX, (float)nTestY + 1));
                        corners.push_back(std::make_pair<float, float>((float)nTestX + 1, (float)nTestY + 1));
                        float eyeX = fPlayerX + fEyeVectorX * fDistanceToWall;
                        float eyeY = fPlayerY + fEyeVectorY * fDistanceToWall;
                        for (const auto& corner : corners)
                        {
                            float plusminus = 0.07f * (1 - fDistanceToWall/fDepth);
                            if(EqualsAbout(eyeX,corner.first, plusminus) && EqualsAbout(eyeY, corner.second, plusminus))
                            {
                                bHitCorner = true;
                                break;
                            }
                        }
                    }
                }
            }
            int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
            int nFloor = nScreenHeight - nCeiling;
            if(jump)
            {
                nCeiling += nScreenHeight * height;
                nFloor += nScreenHeight * height;
            }
            
            short nShade = ' ';
            if (fDistanceToWall <= fDepth / 4.0f)
            {
                nShade = 0x2588;
            }
            else if (fDistanceToWall < fDepth / 3.0f)
            {
                nShade = 0x2593;
            }
            else if (fDistanceToWall < fDepth / 2.0f)
            {
                nShade = 0x2592;
            }
            else if (fDistanceToWall < fDepth)
            {
                nShade = 0x2591;
            }
            wchar_t sign = ' ';
            if (bHitCorner)
            {
                nShade = 'I';
            }
            for (int y = 0; y < nScreenHeight; ++y)
            {
                sign = ' ';
                if (y > nFloor)
                {
                    float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
                    if (b < 0.25) sign = '#';
                    else if (b < 0.5) sign = 'x';
                    else if (b < 0.75) sign = '-';
                    else if (b < 0.9) sign = '.';
                    else  sign = ' ';
                }
                else if (y > nCeiling && y <= nFloor)
                {
                    sign = nShade;
                }
                screen[y*nScreenWidth + x] = sign;
            }
        }
        //drawing buffer
        swprintf_s(screen, 60, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f x=%d y=%d", fPlayerX, fPlayerY, fPlayerA, 1.0f / fElapsedTime,newpoint.x, newpoint.y);
        for(int nx = 0; nx < nMapWidth; nx++)
        {
            for(int ny = 0; ny < nMapWidth; ny++)
            {
                screen[(ny + 1) * nScreenWidth + nx] = map[ny * nMapWidth + nx];
            }
        }
        int xDir = round(sinf(fPlayerA));
        int yDir = round(cosf(fPlayerA));
        char dirSign = olddirSign;
        if (xDir == 1 && yDir == 0) dirSign = 'v';
        else if (xDir == 0 && yDir == 1) dirSign = '>';
        else if (xDir == -1 && yDir == 0) dirSign = '^';
        else if (xDir == 0 && yDir == -1) dirSign = '<';
        olddirSign = dirSign;

        screen[((int)fPlayerX + 1)*nScreenWidth + (int)fPlayerY] = dirSign;
        screen[nScreenWidth*nScreenHeight - 1] = '\0';
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
    }

    return 0;
}

