/**
 * This test ensures that bit test query operators work.
 */
(function() {
    'use strict';

    load("jstests/libs/analyze_plan.js");

    var coll = db.jstests_bitwise;

    coll.drop();
    assert.writeOK(coll.insert({a: -0}));
    assert.writeOK(coll.insert({a: -1}));
    assert.writeOK(coll.insert({a: -54}));
    assert.commandWorked(coll.createIndex({a: 1}));

    function assertQueryCorrect(query, count) {
        var explain = coll.find(query).explain("executionStats");
        assert(isCollscan, "bit test query plans must be COLLSCAN");
        assert.eq(count, explain.executionStats.nReturned,
                  "bit test query not returning correct documents");
    }

    // Tests with bitmask.
    assertQueryCorrect({a: {$bitsAllSet: 0}}, 3);
    assertQueryCorrect({a: {$bitsAllSet: 2}}, 2);
    assertQueryCorrect({a: {$bitsAllSet: -1}}, 1);
    assertQueryCorrect({a: {$bitsAllSet: -54}}, 2);
    assertQueryCorrect({a: {$bitsAllClear: 0}}, 3);
    assertQueryCorrect({a: {$bitsAllClear: 53}}, 2);
    assertQueryCorrect({a: {$bitsAllClear: -1}}, 1);
    assertQueryCorrect({a: {$bitsAnySet: 0}}, 0);
    assertQueryCorrect({a: {$bitsAnySet: 2}}, 2);
    assertQueryCorrect({a: {$bitsAnySet: -1}}, 2);
    assertQueryCorrect({a: {$bitsAnyClear: 0}}, 0);
    assertQueryCorrect({a: {$bitsAnyClear: 53}}, 2);
    assertQueryCorrect({a: {$bitsAnyClear: -1}}, 2);

    // Tests with array of bit positions.
    var allPositions = [];
    for (var i = 0; i < 64; i ++) {
        allPositions.push(i);
    }
    assertQueryCorrect({a: {$bitsAllSet: []}}, 3);
    assertQueryCorrect({a: {$bitsAllSet: [1]}}, 2);
    assertQueryCorrect({a: {$bitsAllSet: allPositions}}, 1);
    assertQueryCorrect({a: {$bitsAllSet: [1, 3, 6, 7]}}, 2);
    assertQueryCorrect({a: {$bitsAllClear: []}}, 3);
    assertQueryCorrect({a: {$bitsAllClear: [0, 2, 4, 5]}}, 2);
    assertQueryCorrect({a: {$bitsAllClear: allPositions}}, 1);
    assertQueryCorrect({a: {$bitsAnySet: []}}, 0);
    assertQueryCorrect({a: {$bitsAnySet: [1]}}, 2);
    assertQueryCorrect({a: {$bitsAnySet: allPositions}}, 2);
    assertQueryCorrect({a: {$bitsAnyClear: []}}, 0);
    assertQueryCorrect({a: {$bitsAnyClear: [0, 2, 4, 5]}}, 2);
    assertQueryCorrect({a: {$bitsAnyClear: allPositions}}, 2);

    // Tests with multiple predicates.
    assertQueryCorrect({a: {$bitsAllSet: -54, $bitsAllClear: 53}}, 1);

    coll.drop();
})();