/**
 *  Common prefix
 *
 *  Utility class to calculate the common prefix of two strings.
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
template <typename text_t = Ascii>
class CommonPrefix
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
     *  Constructor to access inputs that map bytes to characters
     *  @param  input1
     *  @param  input2
     */
    CommonPrefix(const text_t &input1, const text_t &input2) : _input(input1)
    {
        // the max-size
        size_t maxsize = std::min(input1.characters(), input2.characters());
        
        // get iterators for the two inputs
        auto iter1 = input1.begin();
        auto iter2 = input2.begin();
        
        // check how many characters the strings have in common
        while (_size < maxsize)
        {
            // leap out if there is a difference
            if (*iter1 != *iter2) return;
            
            // we found one more identical prefix character
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
    CommonPrefix(const Diff &input1, const Diff &input2) : 
        CommonPrefix(text_t(input1.data(), input1.bytes()), text_t(input2.data(), input2.bytes())) {}

    /**
     *  Destructor
     */
    virtual ~CommonPrefix() = default;

    /**
     *  Get the actual text
     *  @return text_t
     */
    text_t text() const { return _input.substr(0, _size); }
    
    /**
     *  Get the underlying data buffer
     *  @return Buffer
     */
    Buffer buffer() const { return _input.buffer(0, _size); }
    
    /**
     *  Size of the prefix characters
     *  @return size_t
     */
    size_t characters() const { return _size; }
    
    /**
     *  Size of the prefix in bytes
     *  @return size_t
     */
    size_t bytes() const { return buffer().bytes(); }
    
    /**
     *  Cast to bool: is there a common prefix?
     *  @return bool
     */
    operator bool () const { return _size > 0; }
    
};

/**
 *  End of namespace
 */
}

