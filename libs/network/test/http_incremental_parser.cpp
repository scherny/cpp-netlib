
//          Copyright Dean Michael Berris 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MODULE HTTP Incremental Parser Test
#include <boost/config/warning_disable.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/network/protocol/http/parser/incremental.hpp>
#include <boost/range.hpp>
#include <boost/logic/tribool.hpp>
#include <string>
#include <iostream>

/** Synopsis
 *
 * Test for HTTP Response Incremental Parser
 * -----------------------------------------
 *
 * In this test we fully intend to specify how an incremental
 * HTTP Response parser should be used. This defines the bare
 * minimum implementation for an Incremental Parser concept,
 * and shall follow an interface that puts a premium on simplicity.
 *
 * The motivation for coming up with a re-startable stateful
 * incremental parser comes from the requirement in the asynchronous
 * HTTP client implementation that allows for parsing an HTTP
 * response as the data comes in. By being able to process some
 * parts of the message ahead of others, we are allowed to set
 * the promise values bound to futures that the users of the client
 * would be waiting on.
 *
 * The basic interface that we're looking for is a means of providing:
 *   - a range of input
 *   - a completion function once a certain state is reached
 *   - a means of resetting the parser's state
 *   
 * One of the possible implementations can use the Boost.MSM library
 * to create the state machine. The test however does not specify what
 * implementation should be used, but rather that the interface and the
 * semantics are according to expectations.
 *
 * Date: September 9, 2010
 * Author: Dean Michael Berris <mikhailberis@gmail.com>
 */

namespace tags = boost::network::tags;
namespace logic = boost::logic;
namespace fusion = boost::fusion;
using namespace boost::network::http;

BOOST_AUTO_TEST_CASE(incremental_parser_constructor) {
    response_parser<tags::default_string> p; // default constructible
}

/** In this test we want to be able to parse incrementally a
 *  range passed in as input, and specify to the parser that
 *  it should stop when we reach a certain state. In this case
 *  we want it to parse until it either finds the HTTP version
 *  or there is an error encountered.
 */
BOOST_AUTO_TEST_CASE(incremental_parser_parse_http_version) {
    response_parser<tags::default_string> p; // default constructible
    logic::tribool parsed_ok = false;
    typedef response_parser<tags::default_string>::range_type range_type;
    range_type result_range;

    std::string valid_http_version = "HTTP/1.0 ";
    fusion::tie(parsed_ok, result_range) = p.parse_until(
        response_parser<tags::default_string>::http_version_done,
        valid_http_version);
    BOOST_CHECK_EQUAL(parsed_ok, true);
    BOOST_CHECK(!boost::empty(result_range));
    std::string parsed(boost::begin(result_range), boost::end(result_range));
    std::cout << "PARSED: " << parsed << " state=" << p.state() << std::endl;
    p.reset();
    valid_http_version = "HTTP/1.1 ";
    fusion::tie(parsed_ok, result_range) = p.parse_until(
        response_parser<tags::default_string>::http_version_done,
        valid_http_version);
    BOOST_CHECK_EQUAL(parsed_ok, true);
    BOOST_CHECK(!boost::empty(result_range));
    parsed = std::string(boost::begin(result_range), boost::end(result_range));
    std::cout << "PARSED: " << parsed << " state=" << p.state() << std::endl;
}