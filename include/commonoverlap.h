/**
 *  CommonOverlap.h
 *
 *  Class to calculate the common mid-part of two texts
 *
 *  @author Emiel Bruijntjes <emiel.bruijntjes@copernica.com>
 *  @copyright 2018 Copernica BV
 */

/**
 *  Include guard
 */
#pragma once

/**
 *  Begin of namespace
 */
namespace DIFF {

/**
 *  Class definition
 */
template <typename text_t>
class CommonOverlap
{
private:
    /**
     *  Reference to the short text
     *  @var    text_t
     */
    const text_t &_shorttext;

    /**
     *  Reference to the long text
     *  @var    text_t
     */
    const text_t &_longtext;
    
    /**
     *  Number of characters to skip to get to the common part
     *  @var    ssize_t
     */
    ssize_t _skip;
    
    /**
     *  Was text1 longer than text2?
     *  @var    bool
     */
    bool _text1long;


public:
    /**
     *  Constructor
     *  @param  shorttext   the first text (must be the shorted one)
     *  @param  longtext    the second text (must be the longer one)
     */
    CommonOverlap(const text_t &text1, const text_t &text2) : 
        _shorttext(text1.characters() > text2.characters() ? text2 : text1),
        _longtext(&_shorttext == &text1 ? text2 : text1),
        _skip(_longtext.find(_shorttext)),
        _text1long(&_longtext == &text1) {}
        
    /**
     *  Destructor
     */
    virtual ~CommonOverlap() = default;
    
    /**
     *  Cast to boolean: is there a common center?
     *  @return bool
     */
    operator bool () const { return _skip >= 0; }
    bool operator! () const { return _skip < 0; }
    
    /**
     *  The prefix buffer (the non-overlapping part in front)
     *  @return Buffer
     */
    Buffer prefix() const { return _longtext.buffer(0, _skip); }
    
    /**
     *  The suffix buffer (the non-overlapping part behind)
     *  @return Buffer
     */
    Buffer suffix() const { return _longtext.buffer(_skip); }
    
    /**
     *  The part in middle that is overlapping
     *  @return Buffer
     */
    const Buffer &buffer() const { return _shorttext.buffer(); }
    
    /**
     *  The operation that should be applied to get from text1 to text2
     *  @return bool
     */
    Operation operation() const { return _text1long ? Operation::DELETE : Operation::INSERT; }
};

/**
 *  End of namespace
 */
}

