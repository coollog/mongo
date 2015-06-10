// expression_parser_test.cpp

/**
 *    Copyright (C) 2013 10gen Inc.
 *
 *    This program is free software: you can redistribute it and/or  modify
 *    it under the terms of the GNU Affero General Public License, version 3,
 *    as published by the Free Software Foundation.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the GNU Affero General Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */

#include "mongo/unittest/unittest.h"

#include "mongo/db/matcher/expression_parser.h"

#include "mongo/db/jsobj.h"
#include "mongo/db/json.h"
#include "mongo/db/matcher/expression.h"
#include "mongo/db/matcher/expression_leaf.h"

namespace mongo {

TEST(MatchExpressionParserTest, SimpleEQ1) {
    BSONObj query = BSON("x" << 2);
    StatusWithMatchExpression result = MatchExpressionParser::parse(query);
    ASSERT_TRUE(result.isOK());

    ASSERT(result.getValue()->matchesBSON(BSON("x" << 2)));
    ASSERT(!result.getValue()->matchesBSON(BSON("x" << 3)));

    delete result.getValue();
}

TEST(MatchExpressionParserTest, Multiple1) {
    BSONObj query = BSON("x" << 5 << "y" << BSON("$gt" << 5 << "$lt" << 8));
    StatusWithMatchExpression result = MatchExpressionParser::parse(query);
    ASSERT_TRUE(result.isOK());

    ASSERT(result.getValue()->matchesBSON(BSON("x" << 5 << "y" << 7)));
    ASSERT(result.getValue()->matchesBSON(BSON("x" << 5 << "y" << 6)));
    ASSERT(!result.getValue()->matchesBSON(BSON("x" << 6 << "y" << 7)));
    ASSERT(!result.getValue()->matchesBSON(BSON("x" << 5 << "y" << 9)));
    ASSERT(!result.getValue()->matchesBSON(BSON("x" << 5 << "y" << 4)));

    delete result.getValue();
}

TEST(MatchExpressionParserTest, BitTestMatchExpressionValid) {
    ASSERT_OK(MatchExpressionParser::parse(BSON("a" << BSON("$bitsAllSet" << LLONG_MIN))));
    ASSERT_OK(MatchExpressionParser::parse(BSON("a" << BSON("$bitsAllSet" << 54))));
    ASSERT_OK(MatchExpressionParser::parse(BSON("a" << BSON("$bitsAllSet" << LLONG_MAX))));
    ASSERT_OK(MatchExpressionParser::parse(BSON("a" << BSON("$bitsAllSet" << BSON_ARRAY(0)))));
    ASSERT_OK(MatchExpressionParser::parse(
        BSON("a" << BSON("$bitsAllSet" << BSON_ARRAY(0 << 1 << 2 << 3)))));
    ASSERT_OK(
        MatchExpressionParser::parse(BSON("a" << BSON("$bitsAllSet" << BSON_ARRAY(INT_MAX)))));
    ASSERT_OK(MatchExpressionParser::parse(
        fromjson("{a: {$bitsAllSet: {$binary: 'AAAAAAAAAAAAAAAAAAAAAAAAAAAA', $type: '00'}}}")));
    ASSERT_OK(MatchExpressionParser::parse(BSON("a" << BSON("$bitsAllClear" << LLONG_MIN))));
    ASSERT_OK(MatchExpressionParser::parse(BSON("a" << BSON("$bitsAllClear" << 54))));
    ASSERT_OK(MatchExpressionParser::parse(BSON("a" << BSON("$bitsAllClear" << LLONG_MAX))));
    ASSERT_OK(MatchExpressionParser::parse(BSON("a" << BSON("$bitsAllClear" << BSON_ARRAY(0)))));
    ASSERT_OK(MatchExpressionParser::parse(
        BSON("a" << BSON("$bitsAllClear" << BSON_ARRAY(0 << 1 << 2 << 3)))));
    ASSERT_OK(
        MatchExpressionParser::parse(BSON("a" << BSON("$bitsAllClear" << BSON_ARRAY(INT_MAX)))));
    ASSERT_OK(MatchExpressionParser::parse(
        fromjson("{a: {$bitsAllClear: {$binary: 'AAAAAAAAAAAAAAAAAAAAAAAAAAAA', $type: '00'}}}")));
    ASSERT_OK(MatchExpressionParser::parse(BSON("a" << BSON("$bitsAnySet" << LLONG_MIN))));
    ASSERT_OK(MatchExpressionParser::parse(BSON("a" << BSON("$bitsAnySet" << 54))));
    ASSERT_OK(MatchExpressionParser::parse(BSON("a" << BSON("$bitsAnySet" << LLONG_MAX))));
    ASSERT_OK(MatchExpressionParser::parse(BSON("a" << BSON("$bitsAnySet" << BSON_ARRAY(0)))));
    ASSERT_OK(MatchExpressionParser::parse(
        BSON("a" << BSON("$bitsAnySet" << BSON_ARRAY(0 << 1 << 2 << 3)))));
    ASSERT_OK(
        MatchExpressionParser::parse(BSON("a" << BSON("$bitsAnySet" << BSON_ARRAY(INT_MAX)))));
    ASSERT_OK(MatchExpressionParser::parse(
        fromjson("{a: {$bitsAnySet: {$binary: 'AAAAAAAAAAAAAAAAAAAAAAAAAAAA', $type: '00'}}}")));
    ASSERT_OK(MatchExpressionParser::parse(BSON("a" << BSON("$bitsAnyClear" << LLONG_MIN))));
    ASSERT_OK(MatchExpressionParser::parse(BSON("a" << BSON("$bitsAnyClear" << 54))));
    ASSERT_OK(MatchExpressionParser::parse(BSON("a" << BSON("$bitsAnyClear" << LLONG_MAX))));
    ASSERT_OK(MatchExpressionParser::parse(BSON("a" << BSON("$bitsAnyClear" << BSON_ARRAY(0)))));
    ASSERT_OK(MatchExpressionParser::parse(
        BSON("a" << BSON("$bitsAnyClear" << BSON_ARRAY(0 << 1 << 2 << 3)))));
    ASSERT_OK(
        MatchExpressionParser::parse(BSON("a" << BSON("$bitsAnyClear" << BSON_ARRAY(INT_MAX)))));
    ASSERT_OK(MatchExpressionParser::parse(
        fromjson("{a: {$bitsAnyClear: {$binary: 'AAAAAAAAAAAAAAAAAAAAAAAAAAAA', $type: '00'}}}")));
}

TEST(MatchExpressionParserTest, BitTestMatchExpressionInvalid) {
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAllSet: null}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAllSet: true}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAllSet: {}}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAllSet: ''}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(
        fromjson("{a: {$bitsAllSet: ObjectId('000000000000000000000000')}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAllSet: [null]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAllSet: [true]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAllSet: ['']}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAllSet: [{}]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAllSet: [[]]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAllSet: [-1]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAllSet: [2.5]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(
        fromjson("{a: {$bitsAllSet: [{$binary: 'AAAAAAAAAAAAAAAAAAAAAAAAAAAA', $type: '00'}]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAllClear: null}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAllClear: true}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAllClear: {}}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAllClear: ''}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(
        fromjson("{a: {$bitsAllClear: ObjectId('000000000000000000000000')}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAllClear: [null]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAllClear: [true]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAllClear: ['']}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAllClear: [{}]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAllClear: [[]]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAllClear: [-1]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAllClear: [2.5]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson(
        "{a: {$bitsAllClear: [{$binary: 'AAAAAAAAAAAAAAAAAAAAAAAAAAAA', $type: '00'}]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAnySet: null}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAnySet: true}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAnySet: {}}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAnySet: ''}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(
        fromjson("{a: {$bitsAnySet: ObjectId('000000000000000000000000')}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAnySet: [null]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAnySet: [true]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAnySet: ['']}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAnySet: [{}]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAnySet: [[]]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAnySet: [-1]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAnySet: [2.5]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(
        fromjson("{a: {$bitsAnySet: [{$binary: 'AAAAAAAAAAAAAAAAAAAAAAAAAAAA', $type: '00'}]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAnyClear: null}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAnyClear: true}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAnyClear: {}}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAnyClear: ''}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(
        fromjson("{a: {$bitsAnyClear: ObjectId('000000000000000000000000')}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAnyClear: [null]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAnyClear: [true]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAnyClear: ['']}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAnyClear: [{}]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAnyClear: [[]]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAnyClear: [-1]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson("{a: {$bitsAnyClear: [2.5]}}")));
    ASSERT_NOT_OK(MatchExpressionParser::parse(fromjson(
        "{a: {$bitsAnyClear: [{$binary: 'AAAAAAAAAAAAAAAAAAAAAAAAAAAA', $type: '00'}]}}")));
}

TEST(AtomicMatchExpressionTest, Simple1) {
    BSONObj query = BSON("x" << 5 << "$atomic" << BSON("$gt" << 5 << "$lt" << 8));
    StatusWithMatchExpression result = MatchExpressionParser::parse(query);
    ASSERT_TRUE(result.isOK());
    delete result.getValue();

    query = BSON("x" << 5 << "$isolated" << 1);
    result = MatchExpressionParser::parse(query);
    ASSERT_TRUE(result.isOK());
    delete result.getValue();

    query = BSON("x" << 5 << "y" << BSON("$isolated" << 1));
    result = MatchExpressionParser::parse(query);
    ASSERT_FALSE(result.isOK());
}

StatusWith<int> fib(int n) {
    if (n < 0)
        return StatusWith<int>(ErrorCodes::BadValue, "paramter to fib has to be >= 0");
    if (n <= 1)
        return StatusWith<int>(1);
    StatusWith<int> a = fib(n - 1);
    StatusWith<int> b = fib(n - 2);
    if (!a.isOK())
        return a;
    if (!b.isOK())
        return b;
    return StatusWith<int>(a.getValue() + b.getValue());
}

TEST(StatusWithTest, Fib1) {
    StatusWith<int> x = fib(-2);
    ASSERT(!x.isOK());

    x = fib(0);
    ASSERT(x.isOK());
    ASSERT(1 == x.getValue());

    x = fib(1);
    ASSERT(x.isOK());
    ASSERT(1 == x.getValue());

    x = fib(2);
    ASSERT(x.isOK());
    ASSERT(2 == x.getValue());

    x = fib(3);
    ASSERT(x.isOK());
    ASSERT(3 == x.getValue());
}
}
