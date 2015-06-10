/**
 * This test ensures that bit test query operators work.
 */
(function() {
    'use strict';

    load("jstests/libs/analyze_plan.js");

    var coll = db.jstests_bitwise;

    coll.drop();
    assert.writeOK(coll.insert({a: BinData(0, "AAAAAAAAAAAAAAAAAAAAAAAAAAAA")}));
    assert.writeOK(coll.insert({a: BinData(0, "AANgAAAAAAAAAAAAAAAAAAAAAAAA")}));
    assert.writeOK(coll.insert({a: BinData(0, "JANgqwetkqwklEWRbWERKKJREtbq")}));
    assert.writeOK(coll.insert({a: BinData(0, "////////////////////////////")}));
    assert.commandWorked(coll.createIndex({a: 1}));

    function assertQueryCorrect(query, count) {
        var explain = coll.find(query).explain("executionStats");
        assert(isCollscan, "bit test query plans must be COLLSCAN");
        assert.eq(count, explain.executionStats.nReturned,
                  "bit test query not returning correct documents");
    }

    // Tests with binary string bitmask.
    assertQueryCorrect({a: {$bitsAllSet: BinData(0, "AAAAAAAAAAAAAAAAAAAAAAAAAAAA")}}, 4);
    assertQueryCorrect({a: {$bitsAllSet: BinData(0, "AANgAAAAAAAAAAAAAAAAAAAAAAAA")}}, 3);
    assertQueryCorrect({a: {$bitsAllSet: BinData(0, "JANgqwetkqwklEWRbWERKKJREtbq")}}, 2);
    assertQueryCorrect({a: {$bitsAllSet: BinData(0, "////////////////////////////")}}, 1);
    assertQueryCorrect({a: {$bitsAllClear: BinData(0, "AAAAAAAAAAAAAAAAAAAAAAAAAAAA")}}, 4);
    assertQueryCorrect({a: {$bitsAllClear: BinData(0, "AAyfAAAAAAAAAAAAAAAAAAAAAAAA")}}, 3);
    assertQueryCorrect({a: {$bitsAllClear: BinData(0, "JAyfqwetkqwklEWRbWERKKJREtbq")}}, 2);
    assertQueryCorrect({a: {$bitsAllClear: BinData(0, "////////////////////////////")}}, 1);
    assertQueryCorrect({a: {$bitsAnySet: BinData(0, "AAAAAAAAAAAAAAAAAAAAAAAAAAAA")}}, 0);
    assertQueryCorrect({a: {$bitsAnySet: BinData(0, "AAyfAAAAAAAAAAAAAAAAAAAAAAAA")}}, 1);
    assertQueryCorrect({a: {$bitsAnySet: BinData(0, "JAyfqwetkqwklEWRbWERKKJREtbq")}}, 2);
    assertQueryCorrect({a: {$bitsAnySet: BinData(0, "////////////////////////////")}}, 3);
    assertQueryCorrect({a: {$bitsAnyClear: BinData(0, "AAAAAAAAAAAAAAAAAAAAAAAAAAAA")}}, 0);
    assertQueryCorrect({a: {$bitsAnyClear: BinData(0, "AANgAAAAAAAAAAAAAAAAAAAAAAAA")}}, 1);
    assertQueryCorrect({a: {$bitsAnyClear: BinData(0, "JANgqwetkqwklEWRbWERKKJREtbq")}}, 2);
    assertQueryCorrect({a: {$bitsAnyClear: BinData(0, "////////////////////////////")}}, 3);

    // Tests with multiple predicates.
    assertQueryCorrect({a: {$bitsAllSet: BinData(0, "AANgAAAAAAAAAAAAAAAAAAAAAAAA"),
                            $bitsAllClear: BinData(0, "//yf////////////////////////")}}, 1);

    coll.drop();
})();