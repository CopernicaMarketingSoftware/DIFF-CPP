/**
 *  Common suffix
 *
 *  Utility class to calculate the common suffix of two strings.
 *  This class is used internally by the library, but may be useful
 *  for external applications too.
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
template <typename text_t = Ascii, typename char_t = char>
class CommonSuffix
{
private:
    /**
     *  The original input
     *  @var text_t
     */
    const text_t &_input;

    /**
     *  Number of elements that the inputs have in common
     *  @var size_t
     */
    size_t _size = 0;
    
public:
    /**
     *  Constructor
     *  @param  input1
     *  @param  input2
     */
    CommonSuffix(const text_t &input1, const text_t &input2) : _input(input1)
    {
        // the max-size
        size_t maxsize = std::min(input1.characters(), input2.characters());
        
        // get iterators for the two inputs
        auto iter1 = input1.rbegin();
        auto iter2 = input2.rbegin();
        
        // check how many characters the strings have in common
        while (_size < maxsize)
        {
            // leap out if there is a difference
            if (*iter1 != *iter2) return;
            
            // we found one more identical suffix character
            ++_size;
            
            // proceed the iterators
            ++iter1;
            ++iter2;
        }
    }

    /**
     *  Constructor to access raw byte buffers
     *  @param  input1
     *  @param  input2
     */
    CommonSuffix(const Diff &input1, const Diff &input2) : 
        CommonSuffix(text_t(input1.data(), input1.bytes()), text_t(input2.data(), input2.bytes())) {}

    /**
     *  Destructor
     */
    virtual ~CommonSuffix() = default;

    /**
     *  Get the actual text
     *  @return text_t
     */
    text_t text() const { return _input.substr(_input.characters() - _size, _size); }
    
    /**
     *  Get the underlying data buffer
     *  @return Buffer
     */
    Buffer buffer() const { return _input.buffer(_input.characters() - _size, _size); }
    
    /**
     *  Size of the suffix in characters
     *  @return size_t
     */
    size_t characters() const { return _size; }
    
    /**
     *  Size of the suffix in bytes
     *  @return size_t
     */
    size_t bytes() const { return buffer().bytes(); }

    /**
     *  Cast to bool: is there a common suffix?
     *  @return bool
     */
    operator bool () const { return _size > 0; }
};

/**
 *  End of namespace
 */
}

