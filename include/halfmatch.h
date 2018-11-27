/**
 *  HalfMatch.h
 *
 *  Class that checks if there is a common substring that is at least
 *  half of the size of the longer text. This is an optimization.
 *
 *  @author Emiel Bruijntjes <emiel.bruijntjes@copernica.com>
 *  @copyright 2018 Copernica BV
 */

/**
 *  Include guard
 */
#pragma once

/**
 *  Dependencies
 */
#include "commonhalf.h"

/**
 *  Begin of namespace
 */
namespace DIFF {

/**
 *  Class definition
 */
template <typename text_t>
class HalfMatch
{
private:
    /**
     *  The common half based on the second quarter
     *  @var CommonHalf
     */
    CommonHalf<text_t> _q2;
    
    /**
     *  The common half based on the third quarter
     *  @var CommonHalf
     */
    CommonHalf<text_t> _q3;
    
    /**
     *  The winning result
     *  @var CommonHald
     */
    CommonHalf<text_t> *_winner = nullptr;
    
    
public:
    /**
     *  Constructor
     *  @param  text1
     *  @param  text2
     */
    HalfMatch(const text_t &longtext, const text_t &shorttext) :
        _q2(longtext, shorttext, (longtext.characters() + 3) / 4),
        _q3(longtext, shorttext, (longtext.characters() + 1) / 2)
    {
        // do we have two matches?
        if (_q2 && _q3) _winner = _q2.characters() > _q3.characters() ? &_q2 : &_q3;
    
        // or only one?
        else if (_q2) _winner = &_q2;
        else if (_q3) _winner = &_q3;
    }
    
    /**
     *  Destructor
     */
    virtual ~HalfMatch() = default;
    
    /**
     *  Is the object in a valid state and is it useful?
     *  @return bool
     */
    bool valid() const { return _winner != nullptr; } 
    
    /**
     *  Cast to boolean
     *  @return bool
     */
    operator bool () const { return valid(); }
    bool operator! () const { return !valid(); }
    
    /**
     *  The prefix and suffix of the long text and the shorttext
     *  @return text_t
     */
    text_t longPrefix() const  { return _winner->longPrefix();  }
    text_t longSuffix() const  { return _winner->longSuffix();  }
    text_t shortPrefix() const { return _winner->shortPrefix(); }
    text_t shortSuffix() const { return _winner->shortSuffix(); }
    
    /**
     *  The common text in between
     *  @return text_t
     */
    text_t common() const { return _winner->common(); }
};

/**
 *  End of namespace
 */
}

