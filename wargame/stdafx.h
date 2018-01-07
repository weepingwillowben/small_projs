// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
//undefining is necessary to get the stl min and max avaliable instead of the stupid things
#include <windows.h>
#undef min
#undef max
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory>
#include <inttypes.h>
#include <tchar.h>
#include <fstream>
#include <cmath>
#include <functional>
#include <string>
#include <set>

#include <vector>
#include <algorithm>
#include <array>
#include <map>
#include <unordered_map>
#include <boost/container/flat_set.hpp>
#include <boost/container/flat_map.hpp>
#include "../../../../Visual Studio 2013/RangeIterator.h"
#include "../../../../Visual Studio 2013/two_d_array.h"
#include "../../../../Visual Studio 2013/for_each_extend.hpp"
using namespace std;
// TODO: reference additional headers your program requires here

#define BoardSizeX 45
#define BoardSizeY 30
template<typename Type>
inline Type Square(Type Num){
    return Num * Num;
}
