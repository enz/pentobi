//-----------------------------------------------------------------------------
/** @file unittest/libpentobi_base/PentobiSgfUtilTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libpentobi_base/PentobiSgfUtil.h"

#include <cstring>
#include "libboardgame_test/Test.h"

using namespace libpentobi_base;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(pentobi_base_get_color_id)
{
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::classic, Color(0)), "1") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::classic, Color(1)), "2") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::classic, Color(2)), "3") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::classic, Color(3)), "4") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::classic_2, Color(0)), "1") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::classic_2, Color(1)), "2") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::classic_2, Color(2)), "3") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::classic_2, Color(3)), "4") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::classic_3, Color(0)), "1") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::classic_3, Color(1)), "2") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::classic_3, Color(2)), "3") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::classic_3, Color(3)), "4") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::duo, Color(0)), "B") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::duo, Color(1)), "W") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::junior, Color(0)), "B") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::junior, Color(1)), "W") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::trigon, Color(0)), "1") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::trigon, Color(1)), "2") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::trigon, Color(2)), "3") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::trigon, Color(3)), "4") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::trigon_2, Color(0)), "1") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::trigon_2, Color(1)), "2") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::trigon_2, Color(2)), "3") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::trigon_2, Color(3)), "4") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::trigon_3, Color(0)), "1") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::trigon_3, Color(1)), "2") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::trigon_3, Color(2)), "3") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::nexos, Color(0)), "1") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::nexos, Color(1)), "2") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::nexos, Color(2)), "3") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::nexos, Color(3)), "4") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::nexos_2, Color(0)), "1") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::nexos_2, Color(1)), "2") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::nexos_2, Color(2)), "3") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::nexos_2, Color(3)), "4") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::callisto, Color(0)), "1") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::callisto, Color(1)), "2") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::callisto, Color(2)), "3") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::callisto, Color(3)), "4") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::callisto_2, Color(0)), "B") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::callisto_2, Color(1)), "W") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::callisto_3, Color(0)), "1") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::callisto_3, Color(1)), "2") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_color_id(Variant::callisto_3, Color(2)), "3") == 0);
}

LIBBOARDGAME_TEST_CASE(pentobi_base_get_setup_id)
{
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::classic, Color(0)), "A1") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::classic, Color(1)), "A2") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::classic, Color(2)), "A3") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::classic, Color(3)), "A4") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::classic_2, Color(0)), "A1") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::classic_2, Color(1)), "A2") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::classic_2, Color(2)), "A3") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::classic_2, Color(3)), "A4") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::classic_3, Color(0)), "A1") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::classic_3, Color(1)), "A2") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::classic_3, Color(2)), "A3") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::classic_3, Color(3)), "A4") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::duo, Color(0)), "AB") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::duo, Color(1)), "AW") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::junior, Color(0)), "AB") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::junior, Color(1)), "AW") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::trigon, Color(0)), "A1") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::trigon, Color(1)), "A2") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::trigon, Color(2)), "A3") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::trigon, Color(3)), "A4") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::trigon_2, Color(0)), "A1") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::trigon_2, Color(1)), "A2") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::trigon_2, Color(2)), "A3") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::trigon_2, Color(3)), "A4") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::trigon_3, Color(0)), "A1") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::trigon_3, Color(1)), "A2") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::trigon_3, Color(2)), "A3") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::nexos, Color(0)), "A1") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::nexos, Color(1)), "A2") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::nexos, Color(2)), "A3") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::nexos, Color(3)), "A4") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::nexos_2, Color(0)), "A1") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::nexos_2, Color(1)), "A2") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::nexos_2, Color(2)), "A3") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::nexos_2, Color(3)), "A4") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::callisto, Color(0)), "A1") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::callisto, Color(1)), "A2") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::callisto, Color(2)), "A3") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::callisto, Color(3)), "A4") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::callisto_2, Color(0)), "AB") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::callisto_2, Color(1)), "AW") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::callisto_3, Color(0)), "A1") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::callisto_3, Color(1)), "A2") == 0);
    LIBBOARDGAME_CHECK(strcmp(get_setup_id(Variant::callisto_3, Color(2)), "A3") == 0);
}

//-----------------------------------------------------------------------------
