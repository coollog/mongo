/**
 * This test ensures that bit test query operators work.
 */
(function() {
    'use strict';

    load("jstests/libs/analyze_plan.js");

    var coll = db.jstests_bitwise;

    coll.drop();
    assert.writeOK(coll.insert({a: 0}));
    assert.writeOK(coll.insert({a: 1}));
    assert.writeOK(coll.insert({a: 54}));
    assert.writeOK(coll.insert({a: 88}));
    assert.writeOK(coll.insert({a: 255}));
    assert.commandWorked(coll.createIndex({a: 1}));

    function assertQueryCorrect(query, count) {
        var explain = coll.find(query).explain("executionStats");
        assert(isCollscan, "bit test query plans must be COLLSCAN");
        assert.eq(count, explain.executionStats.nReturned,
                  "bit test query not returning correct documents");
    }

    // Tests with bitmask.
    assertQueryCorrect({a: {$bitsAllSet: 0}}, 5);
    assertQueryCorrect({a: {$bitsAllSet: 1}}, 2);
    assertQueryCorrect({a: {$bitsAllSet: 16}}, 3);
    assertQueryCorrect({a: {$bitsAllSet: 54}}, 2);
    assertQueryCorrect({a: {$bitsAllSet: 55}}, 1);
    assertQueryCorrect({a: {$bitsAllSet: 88}}, 2);
    assertQueryCorrect({a: {$bitsAllSet: 255}}, 1);
    assertQueryCorrect({a: {$bitsAllClear: 0}}, 5);
    assertQueryCorrect({a: {$bitsAllClear: 1}}, 3);
    assertQueryCorrect({a: {$bitsAllClear: 16}}, 2);
    assertQueryCorrect({a: {$bitsAllClear: 129}}, 3);
    assertQueryCorrect({a: {$bitsAllClear: 255}}, 1);
    assertQueryCorrect({a: {$bitsAnySet: 0}}, 0);
    assertQueryCorrect({a: {$bitsAnySet: 9}}, 3);
    assertQueryCorrect({a: {$bitsAnySet: 255}}, 4);
    assertQueryCorrect({a: {$bitsAnyClear: 0}}, 0);
    assertQueryCorrect({a: {$bitsAnyClear: 18}}, 3);
    assertQueryCorrect({a: {$bitsAnyClear: 24}}, 3);
    assertQueryCorrect({a: {$bitsAnyClear: 255}}, 4);

    // Tests with array of bit positions.
    assertQueryCorrect({a: {$bitsAllSet: []}}, 5);
    assertQueryCorrect({a: {$bitsAllSet: [0]}}, 2);
    assertQueryCorrect({a: {$bitsAllSet: [4]}}, 3);
    assertQueryCorrect({a: {$bitsAllSet: [1, 2, 4, 5]}}, 2);
    assertQueryCorrect({a: {$bitsAllSet: [0, 1, 2, 4, 5]}}, 1);
    assertQueryCorrect({a: {$bitsAllSet: [3, 4, 6]}}, 2);
    assertQueryCorrect({a: {$bitsAllSet: [0, 1, 2, 3, 4, 5, 6, 7]}}, 1);
    assertQueryCorrect({a: {$bitsAllClear: []}}, 5);
    assertQueryCorrect({a: {$bitsAllClear: [0]}}, 3);
    assertQueryCorrect({a: {$bitsAllClear: [4]}}, 2);
    assertQueryCorrect({a: {$bitsAllClear: [1, 7]}}, 3);
    assertQueryCorrect({a: {$bitsAllClear: [0, 1, 2, 3, 4, 5, 6, 7]}}, 1);
    assertQueryCorrect({a: {$bitsAnySet: []}}, 0);
    assertQueryCorrect({a: {$bitsAnySet: [1, 3]}}, 3);
    assertQueryCorrect({a: {$bitsAnySet: [0, 1, 2, 3, 4, 5, 6, 7]}}, 4);
    assertQueryCorrect({a: {$bitsAnyClear: []}}, 0);
    assertQueryCorrect({a: {$bitsAnyClear: [1, 4]}}, 3);
    assertQueryCorrect({a: {$bitsAnyClear: [3, 4]}}, 3);
    assertQueryCorrect({a: {$bitsAnyClear: [0, 1, 2, 3, 4, 5, 6, 7]}}, 4);

    // Tests with multiple predicates.
    assertQueryCorrect({a: {$bitsAllSet: 54, $bitsAllClear: 201}}, 1);

    coll.drop();
})();