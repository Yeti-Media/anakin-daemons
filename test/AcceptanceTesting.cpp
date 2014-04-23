#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_log.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test_suite_impl.hpp>
#include <boost/test/utils/basic_cstring/basic_cstring.hpp>
#include <boost/test/utils/basic_cstring/basic_cstring_fwd.hpp>
#include <boost/test/utils/lazy_ostream.hpp>
#include <cstdlib>


#define BOOST_TEST_MODULE AcceptanceTesting
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE( acceptance_testing )
{
	system("dropdb anakinAcceptanceTesting");

	BOOST_REQUIRE(system("createdb anakinAcceptanceTesting"));
}
