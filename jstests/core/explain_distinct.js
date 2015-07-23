/**
 * This test ensures that explain on the distinct command works.
 */
(function() {
    'use strict';

    load("jstests/libs/analyze_plan.js");

    var collName = "jstests_explain_distinct";
    var coll = db[collName];

    function runDistinctExplain(collection, keyString, query) {
        if (typeof query == 'undefined') {
            query = {};
        }
        var distinctCmd = {
            distinct: collection.getName(),
            key: keyString,
            query: query
        };
        return coll.runCommand({
            explain: distinctCmd,
            verbosity: 'executionStats'
        });
    }

    coll.drop();

    assert.commandWorked(runDistinctExplain(coll, 'a')); // Collection doesn't exist.

    // Insert the data to perform distinct() on.
    for (var i = 0; i < 10; i ++) {
        assert.writeOK(coll.insert({a: 1, b: 1}));
        assert.writeOK(coll.insert({a: 2, c: 1}));
    }
    assert.commandWorked(coll.createIndex({a: 1}));

    assert.commandFailed(runDistinctExplain(coll, {})); // Bad keyString.
    assert.commandFailed(runDistinctExplain(coll, 'a', 'a')); // Bad query.
    assert.commandFailed(runDistinctExplain(coll, 'b', {$not: 1})); // Bad query.
    assert.commandFailed(runDistinctExplain(coll, 'a', {$not: 1})); // Bad query.
    assert.commandFailed(runDistinctExplain(coll, '_id', {$not: 1})); // Bad query.

    assert.commandWorked(runDistinctExplain(coll, '', null)); // Valid query.

    assert.eq([1, 2], coll.distinct('a'));
    var explain = runDistinctExplain(coll, 'a');
    assert.commandWorked(explain);
    assert.eq(2, explain.executionStats.nReturned);
    assert(planHasStage(explain.executionStats.executionStages, "PROJECTION"));
    assert(planHasStage(explain.executionStats.executionStages.inputStage, "DISTINCT_SCAN"));

    assert.eq([1], coll.distinct('b'));
    var explain = runDistinctExplain(coll, 'b');
    assert.commandWorked(explain);
    assert.eq(20, explain.executionStats.nReturned);
    assert(isCollscan(explain.executionStats.executionStages));

    assert.commandWorked(coll.createIndex({a: 1, b: 1}));

    assert.eq([1], coll.distinct('a', {a: 1}));
    var explain = runDistinctExplain(coll, 'a', {a: 1});
    assert.commandWorked(explain);
    assert.eq(1, explain.executionStats.nReturned);
    assert(planHasStage(explain.executionStats.executionStages, "PROJECTION"));
    assert(planHasStage(explain.executionStats.executionStages.inputStage, "DISTINCT_SCAN"));

    assert.eq([1], coll.distinct('b', {a: 1}));
    var explain = runDistinctExplain(coll, 'b', {a: 1});
    assert.commandWorked(explain);
    assert.eq(10, explain.executionStats.nReturned);
    assert(planHasStage(explain.executionStats.executionStages, "FETCH"));
    assert(isIxscan(explain.executionStats.executionStages.inputStage));
})();