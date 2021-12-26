//-----------------------------------------------------------------------------
/** @file libboardgame_test/Main.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libboardgame_base/Log.h"
#include "libboardgame_test/Test.h"

using namespace libboardgame_test;

//-----------------------------------------------------------------------------

/** Main function that runs all tests (if no arguments) or only the tests
    given as arguments. */
int main(int argc, char* argv[])
{
    libboardgame_base::LogInitializer log_initializer;
    if (argc < 2)
        return run_all_tests() ? 0 : 1;
    int result = 0;
    for (int i = 1; i < argc; ++i)
        if (! run_test(argv[i]))
            result = 1;
    return result;
}

//-----------------------------------------------------------------------------
