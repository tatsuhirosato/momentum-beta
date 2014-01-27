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
    ResetArgs("-PMM");
    BOOST_CHECK(GetBoolArg("-PMM"));
    BOOST_CHECK(GetBoolArg("-PMM", false));
    BOOST_CHECK(GetBoolArg("-PMM", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-PMMo"));
    BOOST_CHECK(!GetBoolArg("-PMMo", false));
    BOOST_CHECK(GetBoolArg("-PMMo", true));

    ResetArgs("-PMM=0");
    BOOST_CHECK(!GetBoolArg("-PMM"));
    BOOST_CHECK(!GetBoolArg("-PMM", false));
    BOOST_CHECK(!GetBoolArg("-PMM", true));

    ResetArgs("-PMM=1");
    BOOST_CHECK(GetBoolArg("-PMM"));
    BOOST_CHECK(GetBoolArg("-PMM", false));
    BOOST_CHECK(GetBoolArg("-PMM", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noPMM");
    BOOST_CHECK(!GetBoolArg("-PMM"));
    BOOST_CHECK(!GetBoolArg("-PMM", false));
    BOOST_CHECK(!GetBoolArg("-PMM", true));

    ResetArgs("-noPMM=1");
    BOOST_CHECK(!GetBoolArg("-PMM"));
    BOOST_CHECK(!GetBoolArg("-PMM", false));
    BOOST_CHECK(!GetBoolArg("-PMM", true));

    ResetArgs("-PMM -noPMM");  // -PMM should win
    BOOST_CHECK(GetBoolArg("-PMM"));
    BOOST_CHECK(GetBoolArg("-PMM", false));
    BOOST_CHECK(GetBoolArg("-PMM", true));

    ResetArgs("-PMM=1 -noPMM=1");  // -PMM should win
    BOOST_CHECK(GetBoolArg("-PMM"));
    BOOST_CHECK(GetBoolArg("-PMM", false));
    BOOST_CHECK(GetBoolArg("-PMM", true));

    ResetArgs("-PMM=0 -noPMM=0");  // -PMM should win
    BOOST_CHECK(!GetBoolArg("-PMM"));
    BOOST_CHECK(!GetBoolArg("-PMM", false));
    BOOST_CHECK(!GetBoolArg("-PMM", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--PMM=1");
    BOOST_CHECK(GetBoolArg("-PMM"));
    BOOST_CHECK(GetBoolArg("-PMM", false));
    BOOST_CHECK(GetBoolArg("-PMM", true));

    ResetArgs("--noPMM=1");
    BOOST_CHECK(!GetBoolArg("-PMM"));
    BOOST_CHECK(!GetBoolArg("-PMM", false));
    BOOST_CHECK(!GetBoolArg("-PMM", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-PMM", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-PMM", "eleven"), "eleven");

    ResetArgs("-PMM -bar");
    BOOST_CHECK_EQUAL(GetArg("-PMM", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-PMM", "eleven"), "");

    ResetArgs("-PMM=");
    BOOST_CHECK_EQUAL(GetArg("-PMM", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-PMM", "eleven"), "");

    ResetArgs("-PMM=11");
    BOOST_CHECK_EQUAL(GetArg("-PMM", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-PMM", "eleven"), "11");

    ResetArgs("-PMM=eleven");
    BOOST_CHECK_EQUAL(GetArg("-PMM", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-PMM", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-PMM", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-PMM", 0), 0);

    ResetArgs("-PMM -bar");
    BOOST_CHECK_EQUAL(GetArg("-PMM", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-PMM=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-PMM", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-PMM=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-PMM", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--PMM");
    BOOST_CHECK_EQUAL(GetBoolArg("-PMM"), true);

    ResetArgs("--PMM=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-PMM", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noPMM");
    BOOST_CHECK(!GetBoolArg("-PMM"));
    BOOST_CHECK(!GetBoolArg("-PMM", true));
    BOOST_CHECK(!GetBoolArg("-PMM", false));

    ResetArgs("-noPMM=1");
    BOOST_CHECK(!GetBoolArg("-PMM"));
    BOOST_CHECK(!GetBoolArg("-PMM", true));
    BOOST_CHECK(!GetBoolArg("-PMM", false));

    ResetArgs("-noPMM=0");
    BOOST_CHECK(GetBoolArg("-PMM"));
    BOOST_CHECK(GetBoolArg("-PMM", true));
    BOOST_CHECK(GetBoolArg("-PMM", false));

    ResetArgs("-PMM --noPMM");
    BOOST_CHECK(GetBoolArg("-PMM"));

    ResetArgs("-noPMM -PMM"); // PMM always wins:
    BOOST_CHECK(GetBoolArg("-PMM"));
}

BOOST_AUTO_TEST_SUITE_END()
