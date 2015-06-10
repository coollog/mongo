// expression_leaf.h

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

#pragma once


#include "mongo/bson/bsonobj.h"
#include "mongo/bson/bsonmisc.h"
#include "mongo/db/matcher/expression.h"
#include "mongo/stdx/memory.h"

namespace pcrecpp {
class RE;
}  // namespace pcrecpp;

namespace mongo {

/**
 * This file contains leaves in the parse tree that are not array-based.
 *
 * LeafMatchExpression: REGEX MOD EXISTS MATCH_IN
 * ComparisonMatchExpression: EQ LTE LT GT GTE
 * MatchExpression: TYPE_OPERATOR
 */

/**
 * Many operators subclass from this:
 * REGEX, MOD, EXISTS, IN
 * Everything that inherits from ComparisonMatchExpression.
 */
class LeafMatchExpression : public MatchExpression {
public:
    LeafMatchExpression(MatchType matchType) : MatchExpression(matchType) {}

    virtual ~LeafMatchExpression() {}

    virtual bool matches(const MatchableDocument* doc, MatchDetails* details = 0) const;

    virtual bool matchesSingleElement(const BSONElement& e) const = 0;

    virtual const StringData path() const {
        return _path;
    }

protected:
    Status initPath(StringData path);

private:
    StringData _path;
    ElementPath _elementPath;
};

/**
 * EQ, LTE, LT, GT, GTE subclass from ComparisonMatchExpression.
 */
class ComparisonMatchExpression : public LeafMatchExpression {
public:
    ComparisonMatchExpression(MatchType type) : LeafMatchExpression(type) {}

    Status init(StringData path, const BSONElement& rhs);

    virtual ~ComparisonMatchExpression() {}

    virtual bool matchesSingleElement(const BSONElement& e) const;

    virtual const BSONElement& getRHS() const {
        return _rhs;
    }

    virtual void debugString(StringBuilder& debug, int level = 0) const;

    virtual void toBSON(BSONObjBuilder* out) const;

    virtual bool equivalent(const MatchExpression* other) const;

    const BSONElement& getData() const {
        return _rhs;
    }

protected:
    BSONElement _rhs;
};

//
// ComparisonMatchExpression inheritors
//

class EqualityMatchExpression : public ComparisonMatchExpression {
public:
    EqualityMatchExpression() : ComparisonMatchExpression(EQ) {}
    virtual std::unique_ptr<MatchExpression> shallowClone() const {
        std::unique_ptr<ComparisonMatchExpression> e = stdx::make_unique<EqualityMatchExpression>();
        e->init(path(), _rhs);
        if (getTag()) {
            e->setTag(getTag()->clone());
        }
        return std::move(e);
    }
};

class LTEMatchExpression : public ComparisonMatchExpression {
public:
    LTEMatchExpression() : ComparisonMatchExpression(LTE) {}
    virtual std::unique_ptr<MatchExpression> shallowClone() const {
        std::unique_ptr<ComparisonMatchExpression> e = stdx::make_unique<LTEMatchExpression>();
        e->init(path(), _rhs);
        if (getTag()) {
            e->setTag(getTag()->clone());
        }
        return std::move(e);
    }
};

class LTMatchExpression : public ComparisonMatchExpression {
public:
    LTMatchExpression() : ComparisonMatchExpression(LT) {}
    virtual std::unique_ptr<MatchExpression> shallowClone() const {
        std::unique_ptr<ComparisonMatchExpression> e = stdx::make_unique<LTMatchExpression>();
        e->init(path(), _rhs);
        if (getTag()) {
            e->setTag(getTag()->clone());
        }
        return std::move(e);
    }
};

class GTMatchExpression : public ComparisonMatchExpression {
public:
    GTMatchExpression() : ComparisonMatchExpression(GT) {}
    virtual std::unique_ptr<MatchExpression> shallowClone() const {
        std::unique_ptr<ComparisonMatchExpression> e = stdx::make_unique<GTMatchExpression>();
        e->init(path(), _rhs);
        if (getTag()) {
            e->setTag(getTag()->clone());
        }
        return std::move(e);
    }
};

class GTEMatchExpression : public ComparisonMatchExpression {
public:
    GTEMatchExpression() : ComparisonMatchExpression(GTE) {}
    virtual std::unique_ptr<MatchExpression> shallowClone() const {
        std::unique_ptr<ComparisonMatchExpression> e = stdx::make_unique<GTEMatchExpression>();
        e->init(path(), _rhs);
        if (getTag()) {
            e->setTag(getTag()->clone());
        }
        return std::move(e);
    }
};

//
// LeafMatchExpression inheritors
//

class RegexMatchExpression : public LeafMatchExpression {
public:
    /**
     * Maximum pattern size which pcre v8.3 can do matches correctly with
     * LINK_SIZE define macro set to 2 @ pcre's config.h (based on
     * experiments)
     */
    static const size_t MaxPatternSize = 32764;

    RegexMatchExpression();
    ~RegexMatchExpression();

    Status init(StringData path, StringData regex, StringData options);
    Status init(StringData path, const BSONElement& e);

    virtual std::unique_ptr<MatchExpression> shallowClone() const {
        std::unique_ptr<RegexMatchExpression> e = stdx::make_unique<RegexMatchExpression>();
        e->init(path(), _regex, _flags);
        if (getTag()) {
            e->setTag(getTag()->clone());
        }
        return std::move(e);
    }

    virtual bool matchesSingleElement(const BSONElement& e) const;

    virtual void debugString(StringBuilder& debug, int level) const;

    virtual void toBSON(BSONObjBuilder* out) const;

    void shortDebugString(StringBuilder& debug) const;

    virtual bool equivalent(const MatchExpression* other) const;

    const std::string& getString() const {
        return _regex;
    }
    const std::string& getFlags() const {
        return _flags;
    }

private:
    std::string _regex;
    std::string _flags;
    std::unique_ptr<pcrecpp::RE> _re;
};

class ModMatchExpression : public LeafMatchExpression {
public:
    ModMatchExpression() : LeafMatchExpression(MOD) {}

    Status init(StringData path, int divisor, int remainder);

    virtual std::unique_ptr<MatchExpression> shallowClone() const {
        std::unique_ptr<ModMatchExpression> m = stdx::make_unique<ModMatchExpression>();
        m->init(path(), _divisor, _remainder);
        if (getTag()) {
            m->setTag(getTag()->clone());
        }
        return std::move(m);
    }

    virtual bool matchesSingleElement(const BSONElement& e) const;

    virtual void debugString(StringBuilder& debug, int level) const;

    virtual void toBSON(BSONObjBuilder* out) const;

    virtual bool equivalent(const MatchExpression* other) const;

    int getDivisor() const {
        return _divisor;
    }
    int getRemainder() const {
        return _remainder;
    }

private:
    int _divisor;
    int _remainder;
};

class ExistsMatchExpression : public LeafMatchExpression {
public:
    ExistsMatchExpression() : LeafMatchExpression(EXISTS) {}

    Status init(StringData path);

    virtual std::unique_ptr<MatchExpression> shallowClone() const {
        std::unique_ptr<ExistsMatchExpression> e = stdx::make_unique<ExistsMatchExpression>();
        e->init(path());
        if (getTag()) {
            e->setTag(getTag()->clone());
        }
        return std::move(e);
    }

    virtual bool matchesSingleElement(const BSONElement& e) const;

    virtual void debugString(StringBuilder& debug, int level) const;

    virtual void toBSON(BSONObjBuilder* out) const;

    virtual bool equivalent(const MatchExpression* other) const;
};

/**
 * INTERNAL
 * terrible name
 * holds the entries of an $in or $all
 * either scalars or regex
 */
class ArrayFilterEntries {
    MONGO_DISALLOW_COPYING(ArrayFilterEntries);

public:
    ArrayFilterEntries();
    ~ArrayFilterEntries();

    Status addEquality(const BSONElement& e);
    Status addRegex(RegexMatchExpression* expr);

    const BSONElementSet& equalities() const {
        return _equalities;
    }
    bool contains(const BSONElement& elem) const {
        return _equalities.count(elem) > 0;
    }

    size_t numRegexes() const {
        return _regexes.size();
    }
    RegexMatchExpression* regex(int idx) const {
        return _regexes[idx];
    }

    bool hasNull() const {
        return _hasNull;
    }
    bool singleNull() const {
        return size() == 1 && _hasNull;
    }
    bool hasEmptyArray() const {
        return _hasEmptyArray;
    }
    int size() const {
        return _equalities.size() + _regexes.size();
    }

    bool equivalent(const ArrayFilterEntries& other) const;

    void copyTo(ArrayFilterEntries& toFillIn) const;

    void debugString(StringBuilder& debug) const;

    void toBSON(BSONArrayBuilder* out) const;

private:
    bool _hasNull;  // if _equalities has a jstNULL element in it
    bool _hasEmptyArray;
    BSONElementSet _equalities;
    std::vector<RegexMatchExpression*> _regexes;
};

/**
 * query operator: $in
 */
class InMatchExpression : public LeafMatchExpression {
public:
    InMatchExpression() : LeafMatchExpression(MATCH_IN) {}
    Status init(StringData path);

    virtual std::unique_ptr<MatchExpression> shallowClone() const;

    ArrayFilterEntries* getArrayFilterEntries() {
        return &_arrayEntries;
    }

    virtual bool matchesSingleElement(const BSONElement& e) const;

    virtual void debugString(StringBuilder& debug, int level) const;

    virtual void toBSON(BSONObjBuilder* out) const;

    virtual bool equivalent(const MatchExpression* other) const;

    void copyTo(InMatchExpression* toFillIn) const;

    const ArrayFilterEntries& getData() const {
        return _arrayEntries;
    }

private:
    bool _matchesRealElement(const BSONElement& e) const;
    ArrayFilterEntries _arrayEntries;
};

//
// The odd duck out, TYPE_OPERATOR.
//

/**
 * Type has some odd semantics with arrays and as such it can't inherit from
 * LeafMatchExpression.
 */
class TypeMatchExpression : public MatchExpression {
public:
    TypeMatchExpression() : MatchExpression(TYPE_OPERATOR) {}

    Status init(StringData path, int type);

    virtual std::unique_ptr<MatchExpression> shallowClone() const {
        std::unique_ptr<TypeMatchExpression> e = stdx::make_unique<TypeMatchExpression>();
        e->init(_path, _type);
        if (getTag()) {
            e->setTag(getTag()->clone());
        }
        return std::move(e);
    }

    virtual bool matchesSingleElement(const BSONElement& e) const;

    virtual bool matches(const MatchableDocument* doc, MatchDetails* details = 0) const;

    virtual void debugString(StringBuilder& debug, int level) const;

    virtual void toBSON(BSONObjBuilder* out) const;

    virtual bool equivalent(const MatchExpression* other) const;

    /**
     * What is the type we're matching against?
     */
    int getData() const {
        return _type;
    }

    virtual const StringData path() const {
        return _path;
    }

private:
    bool _matches(StringData path, const MatchableDocument* doc, MatchDetails* details = 0) const;

    StringData _path;
    ElementPath _elementPath;
    int _type;
};

/**
 * Bit test query operators include $bitsAllSet, $bitsAllClear, $bitsAnySet, and $bitsAnyClear.
 */
class BitTestMatchExpression : public LeafMatchExpression {
public:
    BitTestMatchExpression(MatchType type) : LeafMatchExpression(type) {}

    /**
     * Initialize with either bit positions, a 64-bit numeric bitmask, or a binary string
     * bitmask.
     */
    Status init(StringData path, const std::vector<int>& bitPositions);
    Status init(StringData path, long long bitMask);
    Status init(StringData path, const char* bitMaskBinary, int bitMaskLen);

    virtual ~BitTestMatchExpression() {}

    /**
     * Checks the type of 'e' and processes its value into the out-parameters.
     * Returns false if 'e' cannot pass the filter.
     * 'isNumber' is if e is a number (vs is binary data).
     * 'eValue' is the 64-bit integer representation of e (only set if e is a number).
     * 'eBinary' is the binary char array representation of e (only set if e is not a number).
     * 'eBinaryLen' is the length of 'eBinary' (only set if 'eBinary' is set).
     */
    virtual bool processSingleElement(const BSONElement& e,
                                      bool* isNumber,
                                      long long* eValue,
                                      const char** eBinary,
                                      int* eBinaryLen) const;

    /**
     * Performs bit check on 'eValue' if 'isNumber' or 'eBinary' if not 'isNumber' using bit
     * positions.
     * Returns whether or not the bit check passes.
     */
    bool performBitCheck(const bool& isNumber,
                         const long long& eValue,
                         const char*& eBinary,
                         const int& eBinaryLen) const;

    virtual bool matchesSingleElement(const BSONElement& e) const;

    virtual void debugString(StringBuilder& debug, int level) const;

    virtual void toBSON(BSONObjBuilder* out) const;

    virtual bool equivalent(const MatchExpression* other) const;

    size_t numBitPositions() const {
        return _bitPositions.size();
    }

    void bitPositionsBSONArray(BSONArrayBuilder* out) const {
        for (unsigned i = 0; i < _bitPositions.size(); i++) {
            out->append(_bitPositions[i]);
        }
        out->doneFast();
    };

    std::vector<int> copyBitPositions() const {
        return _bitPositions;
    }

protected:
    // Vector of bit positions to check, with bit position 0 being the least significant bit.
    std::vector<int> _bitPositions;

    /**
     * Used to copy this match expression to another BitTestMatchExpression. Does not take
     * ownership.
     */
    void copyToBitTestMatchExpression(BitTestMatchExpression* bitTestMatchExpression) const {
        bitTestMatchExpression->init(path(), _bitPositions);
        if (getTag()) {
            bitTestMatchExpression->setTag(getTag()->clone());
        }
    }
};

/**
 * BitTestMatchExpression inheritors for the 4 bit test query operators.
 */

class BitsAllSetMatchExpression : public BitTestMatchExpression {
public:
    BitsAllSetMatchExpression() : BitTestMatchExpression(BITS_ALL_SET) {}
    virtual std::unique_ptr<MatchExpression> shallowClone() const {
        std::unique_ptr<BitTestMatchExpression> bitTestMatchExpression =
            stdx::make_unique<BitsAllSetMatchExpression>();
        copyToBitTestMatchExpression(bitTestMatchExpression.get());
        return std::move(bitTestMatchExpression);
    }
};

class BitsAllClearMatchExpression : public BitTestMatchExpression {
public:
    BitsAllClearMatchExpression() : BitTestMatchExpression(BITS_ALL_CLEAR) {}
    virtual std::unique_ptr<MatchExpression> shallowClone() const {
        std::unique_ptr<BitTestMatchExpression> bitTestMatchExpression =
            stdx::make_unique<BitsAllClearMatchExpression>();
        copyToBitTestMatchExpression(bitTestMatchExpression.get());
        return std::move(bitTestMatchExpression);
    }
};

class BitsAnySetMatchExpression : public BitTestMatchExpression {
public:
    BitsAnySetMatchExpression() : BitTestMatchExpression(BITS_ANY_SET) {}
    virtual std::unique_ptr<MatchExpression> shallowClone() const {
        std::unique_ptr<BitTestMatchExpression> bitTestMatchExpression =
            stdx::make_unique<BitsAnySetMatchExpression>();
        copyToBitTestMatchExpression(bitTestMatchExpression.get());
        return std::move(bitTestMatchExpression);
    }
};

class BitsAnyClearMatchExpression : public BitTestMatchExpression {
public:
    BitsAnyClearMatchExpression() : BitTestMatchExpression(BITS_ANY_CLEAR) {}
    virtual std::unique_ptr<MatchExpression> shallowClone() const {
        std::unique_ptr<BitTestMatchExpression> bitTestMatchExpression =
            stdx::make_unique<BitsAnyClearMatchExpression>();
        copyToBitTestMatchExpression(bitTestMatchExpression.get());
        return std::move(bitTestMatchExpression);
    }
};

}  // namespace mongo
