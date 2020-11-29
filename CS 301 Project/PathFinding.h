/*PathFinding.h
Nathan Damon
11/21/2020
This is the hpp file for the PathFinding Class.
This class will find a path through a given 2d grid THAT MUST have a SOLID BORDER.
*/

#include <iostream>
using std::cout;
using std::cin;
using std::endl;
#include <vector>
using std::vector;

extern "C" int GetWidth(void); // Returns the map width

extern "C" int Height; // Returns the map height
extern "C" int Width; // Returns the map width

extern "C" int SetStartinPathGrid(int _StartPos); // Set value at the given coordinates
extern "C" int PathIntGridGet(int x, int y); // Get value at the given coordinates
extern "C" int PathIntGridReset(void); // Reset the path grid

extern "C" int IntegralGridGet(int x, int y); // Get value at the given coordinates
extern "C" int IntegralGridResetFrom(int x, int y); // Create/update the integral grid from the x and y coordinates to the bottom right corner
extern "C" int IntegralGridAreaSumGet(long x1, long y1, long x2, long y2); // Get sum in the area of the given coordinates

extern "C" int NeighborCheck(int position, int direction, int targetpos, int itercount); // Finds a path from A to B
extern "C" int NeighborRetrieve(int position, int direction); // Gets the neighbor value in the given direction from ASM

extern "C" int NeighborCheckPath(int position); // Gets the neighbor value from ASM

extern "C" int BoolGridGet(int x, int y); // Reads from the bool grid and returns collision bool

class PathFinding
{
public:
    PathFinding(int startx, int starty, int targetx, int targety) :
        _alg_values{}, // list of values in order of distance from start
        _alg_values_location{ 0 }, // current position in agl_values
        _DataCount{ 0 },
        _IterCount{ 1 },
        _PathEnd{ false },
        _CreatPath{ false },
        _StartPos{ startx + starty * Width },
        _TargetPos{ targetx + targety * Width }
    {};
    void Algorithm(); // Produces a grid of values with _start as 0 expanding outward
    void Create_Path(); // Identifies the quickest path
    vector<int> Path_Get(); // Runs the path code and returns the path to follow as a vector

    bool _PathEnd;
    vector<int> _DirectPath;
    bool _CreatPath;

private:
    vector<int> _alg_values;
    unsigned _alg_values_location;
    unsigned _DataCount;
    int _IterCount;
    int _TargetPos;
    int _StartPos;
};

