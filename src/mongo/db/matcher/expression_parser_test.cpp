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

    TEST( MatchExpressionParserTest, SimpleEQ1 ) {
        BSONObj query = BSON( "x" << 2 );
        StatusWithMatchExpression result = MatchExpressionParser::parse( query );
        ASSERT_TRUE( result.isOK() );

        ASSERT( result.getValue()->matchesBSON( BSON( "x" << 2 ) ) );
        ASSERT( !result.getValue()->matchesBSON( BSON( "x" << 3 ) ) );

        delete result.getValue();
    }

    TEST( MatchExpressionParserTest, Multiple1 ) {
        BSONObj query = BSON( "x" << 5 << "y" << BSON( "$gt" << 5 << "$lt" << 8 ) );
        StatusWithMatchExpression result = MatchExpressionParser::parse( query );
        ASSERT_TRUE( result.isOK() );

        ASSERT( result.getValue()->matchesBSON( BSON( "x" << 5 << "y" << 7 ) ) );
        ASSERT( result.getValue()->matchesBSON( BSON( "x" << 5 << "y" << 6 ) ) );
        ASSERT( !result.getValue()->matchesBSON( BSON( "x" << 6 << "y" << 7 ) ) );
        ASSERT( !result.getValue()->matchesBSON( BSON( "x" << 5 << "y" << 9 ) ) );
        ASSERT( !result.getValue()->matchesBSON( BSON( "x" << 5 << "y" << 4 ) ) );

        delete result.getValue();
    }

    TEST(MatchExpressionParserTest, BitwiseMatchExpressionValid) {
        BSONObj query1_1 = BSON("a" << BSON("$bitsSet" << LLONG_MIN));
        BSONObj query1_2 = BSON("a" << BSON("$bitsSet" << 54));
        BSONObj query1_3 = BSON("a" << BSON("$bitsSet" << LLONG_MAX));
        BSONObj query1_4 = BSON("a" << BSON("$bitsSet" << BSON_ARRAY(0)));
        BSONObj query1_5 = BSON("a" << BSON("$bitsSet" << BSON_ARRAY(0 << 1 << 2 << 3)));
        BSONObj query1_6 = BSON("a" << BSON("$bitsSet" << BSON_ARRAY(INT_MAX)));
        BSONObj query2_1 = BSON("a" << BSON("$bitsAllSet" << LLONG_MIN));
        BSONObj query2_2 = BSON("a" << BSON("$bitsAllSet" << 54));
        BSONObj query2_3 = BSON("a" << BSON("$bitsAllSet" << LLONG_MAX));
        BSONObj query2_4 = BSON("a" << BSON("$bitsAllSet" << BSON_ARRAY(0)));
        BSONObj query2_5 = BSON("a" << BSON("$bitsAllSet" << BSON_ARRAY(0 << 1 << 2 << 3)));
        BSONObj query2_6 = BSON("a" << BSON("$bitsAllSet" << BSON_ARRAY(INT_MAX)));
        BSONObj query3_1 = BSON("a" << BSON("$bitsAllClear" << LLONG_MIN));
        BSONObj query3_2 = BSON("a" << BSON("$bitsAllClear" << 54));
        BSONObj query3_3 = BSON("a" << BSON("$bitsAllClear" << LLONG_MAX));
        BSONObj query3_4 = BSON("a" << BSON("$bitsAllClear" << BSON_ARRAY(0)));
        BSONObj query3_5 = BSON("a" << BSON("$bitsAllClear" << BSON_ARRAY(0 << 1 << 2 << 3)));
        BSONObj query3_6 = BSON("a" << BSON("$bitsAllClear" << BSON_ARRAY(INT_MAX)));
        BSONObj query4_1 = BSON("a" << BSON("$bitsAnySet" << LLONG_MIN));
        BSONObj query4_2 = BSON("a" << BSON("$bitsAnySet" << 54));
        BSONObj query4_3 = BSON("a" << BSON("$bitsAnySet" << LLONG_MAX));
        BSONObj query4_4 = BSON("a" << BSON("$bitsAnySet" << BSON_ARRAY(0)));
        BSONObj query4_5 = BSON("a" << BSON("$bitsAnySet" << BSON_ARRAY(0 << 1 << 2 << 3)));
        BSONObj query4_6 = BSON("a" << BSON("$bitsAnySet" << BSON_ARRAY(INT_MAX)));
        BSONObj query5_1 = BSON("a" << BSON("$bitsAnyClear" << LLONG_MIN));
        BSONObj query5_2 = BSON("a" << BSON("$bitsAnyClear" << 54));
        BSONObj query5_3 = BSON("a" << BSON("$bitsAnyClear" << LLONG_MAX));
        BSONObj query5_4 = BSON("a" << BSON("$bitsAnyClear" << BSON_ARRAY(0)));
        BSONObj query5_5 = BSON("a" << BSON("$bitsAnyClear" << BSON_ARRAY(0 << 1 << 2 << 3)));
        BSONObj query5_6 = BSON("a" << BSON("$bitsAnyClear" << BSON_ARRAY(INT_MAX)));

        ASSERT_OK(MatchExpressionParser::parse(query1_1));
        ASSERT_OK(MatchExpressionParser::parse(query1_2));
        ASSERT_OK(MatchExpressionParser::parse(query1_3));
        ASSERT_OK(MatchExpressionParser::parse(query1_4));
        ASSERT_OK(MatchExpressionParser::parse(query1_5));
        ASSERT_OK(MatchExpressionParser::parse(query1_6));
        ASSERT_OK(MatchExpressionParser::parse(query2_1));
        ASSERT_OK(MatchExpressionParser::parse(query2_2));
        ASSERT_OK(MatchExpressionParser::parse(query2_3));
        ASSERT_OK(MatchExpressionParser::parse(query2_4));
        ASSERT_OK(MatchExpressionParser::parse(query2_5));
        ASSERT_OK(MatchExpressionParser::parse(query2_6));
        ASSERT_OK(MatchExpressionParser::parse(query3_1));
        ASSERT_OK(MatchExpressionParser::parse(query3_2));
        ASSERT_OK(MatchExpressionParser::parse(query3_3));
        ASSERT_OK(MatchExpressionParser::parse(query3_4));
        ASSERT_OK(MatchExpressionParser::parse(query3_5));
        ASSERT_OK(MatchExpressionParser::parse(query3_6));
        ASSERT_OK(MatchExpressionParser::parse(query4_1));
        ASSERT_OK(MatchExpressionParser::parse(query4_2));
        ASSERT_OK(MatchExpressionParser::parse(query4_3));
        ASSERT_OK(MatchExpressionParser::parse(query4_4));
        ASSERT_OK(MatchExpressionParser::parse(query4_5));
        ASSERT_OK(MatchExpressionParser::parse(query4_6));
        ASSERT_OK(MatchExpressionParser::parse(query5_1));
        ASSERT_OK(MatchExpressionParser::parse(query5_2));
        ASSERT_OK(MatchExpressionParser::parse(query5_3));
        ASSERT_OK(MatchExpressionParser::parse(query5_4));
        ASSERT_OK(MatchExpressionParser::parse(query5_5));
        ASSERT_OK(MatchExpressionParser::parse(query5_6));
    }

    TEST(MatchExpressionParserTest, BitwiseMatchExpressionInvalid) {
        BSONObj query1_0 = fromjson("{a: {$bitsSet: null}}");
        BSONObj query1_1 = fromjson("{a: {$bitsSet: true}}");
        BSONObj query1_2 = fromjson("{a: {$bitsSet: {}}}");
        BSONObj query1_3 = fromjson("{a: {$bitsSet: ''}}");
        BSONObj query1_4 = fromjson("{a: {$bitsSet: ObjectId('000000000000000000000000')}}");
        BSONObj query1_5 = fromjson("{a: {$bitsSet: [null]}}");
        BSONObj query1_6 = fromjson("{a: {$bitsSet: [true]}}");
        BSONObj query1_7 = fromjson("{a: {$bitsSet: ['']}}");
        BSONObj query1_8 = fromjson("{a: {$bitsSet: [{}]}}");
        BSONObj query1_9 = fromjson("{a: {$bitsSet: [[]]}}");
        BSONObj query1_10 = fromjson("{a: {$bitsSet: [-1]}}");
        BSONObj query2_0 = fromjson("{a: {$bitsAllSet: null}}");
        BSONObj query2_1 = fromjson("{a: {$bitsAllSet: true}}");
        BSONObj query2_2 = fromjson("{a: {$bitsAllSet: {}}}");
        BSONObj query2_3 = fromjson("{a: {$bitsAllSet: ''}}");
        BSONObj query2_4 = fromjson("{a: {$bitsAllSet: ObjectId('000000000000000000000000')}}");
        BSONObj query2_5 = fromjson("{a: {$bitsAllSet: [null]}}");
        BSONObj query2_6 = fromjson("{a: {$bitsAllSet: [true]}}");
        BSONObj query2_7 = fromjson("{a: {$bitsAllSet: ['']}}");
        BSONObj query2_8 = fromjson("{a: {$bitsAllSet: [{}]}}");
        BSONObj query2_9 = fromjson("{a: {$bitsAllSet: [[]]}}");
        BSONObj query2_10 = fromjson("{a: {$bitsAllSet: [-1]}}");
        BSONObj query3_0 = fromjson("{a: {$bitsAllClear: null}}");
        BSONObj query3_1 = fromjson("{a: {$bitsAllClear: true}}");
        BSONObj query3_2 = fromjson("{a: {$bitsAllClear: {}}}");
        BSONObj query3_3 = fromjson("{a: {$bitsAllClear: ''}}");
        BSONObj query3_4 = fromjson("{a: {$bitsAllClear: ObjectId('000000000000000000000000')}}");
        BSONObj query3_5 = fromjson("{a: {$bitsAllClear: [null]}}");
        BSONObj query3_6 = fromjson("{a: {$bitsAllClear: [true]}}");
        BSONObj query3_7 = fromjson("{a: {$bitsAllClear: ['']}}");
        BSONObj query3_8 = fromjson("{a: {$bitsAllClear: [{}]}}");
        BSONObj query3_9 = fromjson("{a: {$bitsAllClear: [[]]}}");
        BSONObj query3_10 = fromjson("{a: {$bitsAllClear: [-1]}}");
        BSONObj query4_0 = fromjson("{a: {$bitsAnySet: null}}");
        BSONObj query4_1 = fromjson("{a: {$bitsAnySet: true}}");
        BSONObj query4_2 = fromjson("{a: {$bitsAnySet: {}}}");
        BSONObj query4_3 = fromjson("{a: {$bitsAnySet: ''}}");
        BSONObj query4_4 = fromjson("{a: {$bitsAnySet: ObjectId('000000000000000000000000')}}");
        BSONObj query4_5 = fromjson("{a: {$bitsAnySet: [null]}}");
        BSONObj query4_6 = fromjson("{a: {$bitsAnySet: [true]}}");
        BSONObj query4_7 = fromjson("{a: {$bitsAnySet: ['']}}");
        BSONObj query4_8 = fromjson("{a: {$bitsAnySet: [{}]}}");
        BSONObj query4_9 = fromjson("{a: {$bitsAnySet: [[]]}}");
        BSONObj query4_10 = fromjson("{a: {$bitsAnySet: [-1]}}");
        BSONObj query5_0 = fromjson("{a: {$bitsAnyClear: null}}");
        BSONObj query5_1 = fromjson("{a: {$bitsAnyClear: true}}");
        BSONObj query5_2 = fromjson("{a: {$bitsAnyClear: {}}}");
        BSONObj query5_3 = fromjson("{a: {$bitsAnyClear: ''}}");
        BSONObj query5_4 = fromjson("{a: {$bitsAnyClear: ObjectId('000000000000000000000000')}}");
        BSONObj query5_5 = fromjson("{a: {$bitsAnyClear: [null]}}");
        BSONObj query5_6 = fromjson("{a: {$bitsAnyClear: [true]}}");
        BSONObj query5_7 = fromjson("{a: {$bitsAnyClear: ['']}}");
        BSONObj query5_8 = fromjson("{a: {$bitsAnyClear: [{}]}}");
        BSONObj query5_9 = fromjson("{a: {$bitsAnyClear: [[]]}}");
        BSONObj query5_10 = fromjson("{a: {$bitsAnyClear: [-1]}}");

        ASSERT_NOT_OK(MatchExpressionParser::parse(query1_0));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query1_1));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query1_2));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query1_3));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query1_4));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query1_5));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query1_6));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query1_7));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query1_8));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query1_9));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query2_0));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query2_1));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query2_2));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query2_3));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query2_4));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query2_5));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query2_6));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query2_7));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query2_8));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query2_9));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query3_0));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query3_1));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query3_2));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query3_3));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query3_4));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query3_5));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query3_6));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query3_7));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query3_8));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query3_9));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query4_0));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query4_1));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query4_2));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query4_3));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query4_4));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query4_5));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query4_6));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query4_7));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query4_8));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query4_9));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query5_0));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query5_1));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query5_2));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query5_3));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query5_4));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query5_5));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query5_6));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query5_7));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query5_8));
        ASSERT_NOT_OK(MatchExpressionParser::parse(query5_9));
    }

    TEST( AtomicMatchExpressionTest, Simple1 ) {
        BSONObj query = BSON( "x" << 5 << "$atomic" << BSON( "$gt" << 5 << "$lt" << 8 ) );
        StatusWithMatchExpression result = MatchExpressionParser::parse( query );
        ASSERT_TRUE( result.isOK() );
        delete result.getValue();

        query = BSON( "x" << 5 << "$isolated" << 1 );
        result = MatchExpressionParser::parse( query );
        ASSERT_TRUE( result.isOK() );
        delete result.getValue();

        query = BSON( "x" << 5 << "y" << BSON( "$isolated" << 1 ) );
        result = MatchExpressionParser::parse( query );
        ASSERT_FALSE( result.isOK() );
    }

    StatusWith<int> fib( int n ) {
        if ( n < 0 ) return StatusWith<int>( ErrorCodes::BadValue, "paramter to fib has to be >= 0" );
        if ( n <= 1 ) return StatusWith<int>( 1 );
        StatusWith<int> a = fib( n - 1 );
        StatusWith<int> b = fib( n - 2 );
        if ( !a.isOK() ) return a;
        if ( !b.isOK() ) return b;
        return StatusWith<int>( a.getValue() + b.getValue() );
    }

    TEST( StatusWithTest, Fib1 ) {
        StatusWith<int> x = fib( -2 );
        ASSERT( !x.isOK() );

        x = fib(0);
        ASSERT( x.isOK() );
        ASSERT( 1 == x.getValue() );

        x = fib(1);
        ASSERT( x.isOK() );
        ASSERT( 1 == x.getValue() );

        x = fib(2);
        ASSERT( x.isOK() );
        ASSERT( 2 == x.getValue() );

        x = fib(3);
        ASSERT( x.isOK() );
        ASSERT( 3 == x.getValue() );


    }
}
