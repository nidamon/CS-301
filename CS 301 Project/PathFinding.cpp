/*Path Finding
Nathan Damon
11/21/2020
This is the hpp file for the PathFinding Class.
*/


#include "PathFinding.h"

#include <iomanip>
using std::setw;

void PathFinding::Algorithm()
{
    SetStartinPathGrid(_StartPos),
        _alg_values.push_back(_StartPos);
    while (!_PathEnd && !_CreatPath)
    {
        int Count = _DataCount;
        _DataCount = 0;
        for (int Direction = 0; Direction < 4; Direction++) // Check each North, West, East, and South
        {
            for (unsigned int i = 0; i < Count + 1; i++)
            {
                int RET = NeighborCheck(_alg_values[_alg_values_location + i], // Gets the neighboring cell in the given direction
                    Direction, _TargetPos, _IterCount);
                if (RET == 1)
                {
                    int temp = NeighborRetrieve(_alg_values[_alg_values_location + i], Direction); // Gets the neighbor after confirmed accessable
                    if (temp < 0) // Runs the following code in case of error to aid in finding the reason for the error
                    {
                        cout << "ERROR in NeighborRetrieve, PathFinding.cpp: Value returned below 0." << endl;

                        cout << "Position: " << _alg_values[_alg_values_location + i] << endl;
                        for (int y = 0; y < 64; y++)
                        {
                            for (int x = 0; x < 64; x++)
                            {
                                // Set specified value to its opposite
                                cout << setw(3) << PathIntGridGet(x, y) << " "; // (rcx, rdx)
                            }
                            cout << endl;
                        }
                    }

                    _alg_values.push_back(temp);
                    _DataCount++; // Increase the count of data
                }
                if (RET == 2) // Finish found
                {
                    _CreatPath = true;
                    break;
                }
            }
            if (_CreatPath)
                break;
        }
        if (_DataCount == 0) // If there is no new data, then everything that can be moved to has been checked
        {
            _PathEnd = true;
        }
        _alg_values_location += Count; // Increase the starting point for where the next set of data points is read from
        _IterCount++;
    }
}

void PathFinding::Create_Path()
{
    _DirectPath.clear(); // Empty the path
    _DirectPath.push_back(_TargetPos); // Set the first position in the path to the start position
    int NewPos = 0;

    int prevent_overrun = 0;
    while (NewPos != _StartPos)
    {
        NewPos = NeighborCheckPath(_DirectPath.back()); // Finds the best direction to move in and returns it
        if (NewPos != _DirectPath.back())
            _DirectPath.push_back(NewPos);
        prevent_overrun++;
        if (prevent_overrun > _IterCount)
            break;
    }

    PathIntGridReset(); // Resets the grid for reuse
}

vector<int> PathFinding::Path_Get()
{
    _PathEnd = false;
    if (IntegralGridAreaSumGet(_StartPos % Width, _StartPos / Width, _TargetPos % Width, _TargetPos / Width) == 0)
        return { _TargetPos };
    Algorithm();
    if (_PathEnd)
    {
        PathIntGridReset(); // Because the original reset was in Create_Path() we need to call it before leaving in the case that we don't get to Create_Path()
        return { -1 };
    }
    Create_Path();
    return _DirectPath; // Return the calculated path
}