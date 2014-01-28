#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-CBRT");
    BOOST_CHECK(GetBoolArg("-CBRT"));
    BOOST_CHECK(GetBoolArg("-CBRT", false));
    BOOST_CHECK(GetBoolArg("-CBRT", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-CBRTo"));
    BOOST_CHECK(!GetBoolArg("-CBRTo", false));
    BOOST_CHECK(GetBoolArg("-CBRTo", true));

    ResetArgs("-CBRT=0");
    BOOST_CHECK(!GetBoolArg("-CBRT"));
    BOOST_CHECK(!GetBoolArg("-CBRT", false));
    BOOST_CHECK(!GetBoolArg("-CBRT", true));

    ResetArgs("-CBRT=1");
    BOOST_CHECK(GetBoolArg("-CBRT"));
    BOOST_CHECK(GetBoolArg("-CBRT", false));
    BOOST_CHECK(GetBoolArg("-CBRT", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noCBRT");
    BOOST_CHECK(!GetBoolArg("-CBRT"));
    BOOST_CHECK(!GetBoolArg("-CBRT", false));
    BOOST_CHECK(!GetBoolArg("-CBRT", true));

    ResetArgs("-noCBRT=1");
    BOOST_CHECK(!GetBoolArg("-CBRT"));
    BOOST_CHECK(!GetBoolArg("-CBRT", false));
    BOOST_CHECK(!GetBoolArg("-CBRT", true));

    ResetArgs("-CBRT -noCBRT");  // -CBRT should win
    BOOST_CHECK(GetBoolArg("-CBRT"));
    BOOST_CHECK(GetBoolArg("-CBRT", false));
    BOOST_CHECK(GetBoolArg("-CBRT", true));

    ResetArgs("-CBRT=1 -noCBRT=1");  // -CBRT should win
    BOOST_CHECK(GetBoolArg("-CBRT"));
    BOOST_CHECK(GetBoolArg("-CBRT", false));
    BOOST_CHECK(GetBoolArg("-CBRT", true));

    ResetArgs("-CBRT=0 -noCBRT=0");  // -CBRT should win
    BOOST_CHECK(!GetBoolArg("-CBRT"));
    BOOST_CHECK(!GetBoolArg("-CBRT", false));
    BOOST_CHECK(!GetBoolArg("-CBRT", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--CBRT=1");
    BOOST_CHECK(GetBoolArg("-CBRT"));
    BOOST_CHECK(GetBoolArg("-CBRT", false));
    BOOST_CHECK(GetBoolArg("-CBRT", true));

    ResetArgs("--noCBRT=1");
    BOOST_CHECK(!GetBoolArg("-CBRT"));
    BOOST_CHECK(!GetBoolArg("-CBRT", false));
    BOOST_CHECK(!GetBoolArg("-CBRT", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CBRT", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CBRT", "eleven"), "eleven");

    ResetArgs("-CBRT -bar");
    BOOST_CHECK_EQUAL(GetArg("-CBRT", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CBRT", "eleven"), "");

    ResetArgs("-CBRT=");
    BOOST_CHECK_EQUAL(GetArg("-CBRT", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CBRT", "eleven"), "");

    ResetArgs("-CBRT=11");
    BOOST_CHECK_EQUAL(GetArg("-CBRT", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-CBRT", "eleven"), "11");

    ResetArgs("-CBRT=eleven");
    BOOST_CHECK_EQUAL(GetArg("-CBRT", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-CBRT", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CBRT", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-CBRT", 0), 0);

    ResetArgs("-CBRT -bar");
    BOOST_CHECK_EQUAL(GetArg("-CBRT", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-CBRT=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-CBRT", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-CBRT=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-CBRT", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--CBRT");
    BOOST_CHECK_EQUAL(GetBoolArg("-CBRT"), true);

    ResetArgs("--CBRT=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-CBRT", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noCBRT");
    BOOST_CHECK(!GetBoolArg("-CBRT"));
    BOOST_CHECK(!GetBoolArg("-CBRT", true));
    BOOST_CHECK(!GetBoolArg("-CBRT", false));

    ResetArgs("-noCBRT=1");
    BOOST_CHECK(!GetBoolArg("-CBRT"));
    BOOST_CHECK(!GetBoolArg("-CBRT", true));
    BOOST_CHECK(!GetBoolArg("-CBRT", false));

    ResetArgs("-noCBRT=0");
    BOOST_CHECK(GetBoolArg("-CBRT"));
    BOOST_CHECK(GetBoolArg("-CBRT", true));
    BOOST_CHECK(GetBoolArg("-CBRT", false));

    ResetArgs("-CBRT --noCBRT");
    BOOST_CHECK(GetBoolArg("-CBRT"));

    ResetArgs("-noCBRT -CBRT"); // CBRT always wins:
    BOOST_CHECK(GetBoolArg("-CBRT"));
}

BOOST_AUTO_TEST_SUITE_END()
