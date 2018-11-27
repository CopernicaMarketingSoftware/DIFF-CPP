/**
 *  CommonHalf.h
 *
 *  Class that checks if one string is for the half equal to the other string.
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
class CommonHalf
{
private:
    /**
     *  The long text
     *  @var text_t
     */
    const text_t &_longtext;
    
    /**
     *  The short text
     *  @var text_t
     */
    const text_t &_shorttext;

    /**
     *  Index in the long-string where we start to check
     *  @var size_t
     */
    size_t _start = 0;

    /**
     *  Position where the common substring starts
     *  @var size_t
     */
    size_t _substr = 0;

    /**
     *  Size of the common prefix
     *  @var size_t
     */
    size_t _prefix = 0;
    
    /**
     *  Size of the common suffix
     *  @var size_t
     */
    size_t _suffix = 0;

public:
    /**
     *  Constructor that checks 
     *  @param  longtext        the long text
     *  @param  shorttext       the short text
     *  @param  index           index of the quarter that is checked (this should be the 2nd or 3th quarter)
     */
    CommonHalf(const text_t &longtext, const text_t &shorttext, size_t index) : 
        _longtext(longtext), 
        _shorttext(shorttext),
        _start(index)
    {
        // start with a 1/4 length substring at position i as a seed.
        auto seed = longtext.substr(index, longtext.characters() / 4);
        
        // look for the substring size
        int pos = -1;

        // look for the substring size
        while ((pos = shorttext.find(seed, pos + 1)) >= 0)
        {
            // get the size of the shared prefix and suffix
            CommonPrefix<text_t> prefix(longtext.substr(index), shorttext.substr(pos));
            CommonSuffix<text_t> suffix(longtext.substr(0, index), shorttext.substr(0, pos));
            
            // proceed if we already have a better score
            if (characters() > prefix.characters() + suffix.characters()) continue;
            
            // we have a new best match
            _substr = pos;
        }
    }

    /**
     *  Destructor
     */
    virtual ~CommonHalf() = default;
    
    /**
     *  Is the object in a valid state and is it useful?
     *  @return bool
     */
    bool valid() const { return characters() * 2 >= _longtext.characters(); } 
    
    /**
     *  Number of matching characters
     *  @return size_t
     */
    size_t characters() const { return _prefix + _suffix; }
    
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
    text_t longPrefix() const { return _longtext.substr(0, _start - _suffix); }
    text_t longSuffix() const { return _longtext.substr(_start + _prefix); }
    text_t shortPrefix() const { return _shorttext.substr(0, _substr - _suffix); }
    text_t shortSuffix() const { return _shorttext.substr(_substr + _prefix); }
    
    /**
     *  The common text in the two texts
     *  @return text_t
     */
    text_t common() const { return _shorttext.substr(_substr - _suffix, _suffix + _prefix); }
};

/**
 *  End of namespace
 */
}

