/**
 *  Ascii.h
 *
 *  Wrapper around an an ascii buffer. The buffer is not managed: it does
 *  not do any allocation and expects the caller to supply a valid buffer,
 *  and that this buffer is kept in scope during the lifetime of the object. 
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
#include <string.h>
#include "buffer.h"

/**
 *  Begin of namespace
 */
namespace DIFF {

/**
 *  Class definition
 */
class Ascii
{
private:
    /**
     *  The buffer of bytes
     *  @var Buffer
     */
    Buffer _buffer;
    
    /**
     *  Iterator to go over the buffer
     */
    class iterator
    {
    private:
        /**
         *  The current position in the buffer
         *  @var const char *
         */
        const char *_buffer;
    
    public:
        /**
         *  Constructor
         *  @param  buffer
         */
        iterator(const char *buffer) : _buffer(buffer) {}
        
        /**
         *  Destructor
         */
        virtual ~iterator() = default;
        
        /**
         *  Compare with a different iterator
         *  @param  that
         */
        bool operator==(const iterator &that) const { return _buffer == that._buffer; }
        bool operator!=(const iterator &that) const { return _buffer != that._buffer; }
        
        /**
         *  Dereference, get the character
         *  @return char
         */
        char operator*() const { return *_buffer; }
        
        /**
         *  Move the iterator (implements ++iter)
         *  @return iterator
         */
        const iterator &operator++()
        {
            // move the iterator
            ++_buffer;
            
            // allow chaining
            return *this;
        }
        
        /**
         *  Move the iterator (implements iter++)
         *  @return iterator
         */
        iterator operator++(int)
        {
            // move the iterator, but return the prev
            return iterator(_buffer++);
        }
    };

    /**
     *  Iterator to go over the buffer in reverse order
     */
    class reverse_iterator
    {
    private:
        /**
         *  The buffer that is being iterated, this points to the position right after the current character
         *  @var const char *
         */
        const char *_buffer;
    
    public:
        /**
         *  Constructor
         *  @param  buffer
         */
        reverse_iterator(const char *buffer) : _buffer(buffer) {}
        
        /**
         *  Destructor
         */
        virtual ~reverse_iterator() = default;
        
        /**
         *  Compare with a different iterator
         *  @param  that
         */
        bool operator==(const reverse_iterator &that) const { return _buffer == that._buffer; }
        bool operator!=(const reverse_iterator &that) const { return _buffer != that._buffer; }
        
        /**
         *  Dereference, get the character
         *  @return char
         */
        char operator*() const { return _buffer[-1]; }
        
        /**
         *  Move the iterator (implements ++iter)
         *  @return reverse_iterator
         */
        const reverse_iterator &operator++()
        {
            // move the iterator
            --_buffer;
            
            // allow chaining
            return *this;
        }
        
        /**
         *  Move the iterator (implements iter++)
         *  @return iterator
         */
        reverse_iterator operator++(int)
        {
            // move the iterator, but return the prev
            return reverse_iterator(_buffer--);
        }
    };
    

public:
    /**
     *  Default constructor
     */
    Ascii() = default;

    /**
     *  Constructor to wrap around a user-supplied buffer
     *  @param  buffer
     *  @param  size
     */
    Ascii(const char *buffer, size_t size) : _buffer(buffer, size, false) {}

    /**
     *  Constructor
     *  @param  buffer
     */
    explicit Ascii(const char *buffer) : Ascii(buffer, strlen(buffer)) {}
    
    /**
     *  Copy constructor
     *  @param  that
     */
    Ascii(const Ascii &that) : _buffer(that._buffer) {}

    /**
     *  Move constructor
     *  @param  that
     */
    Ascii(Ascii &&that) : _buffer(std::move(that._buffer)) {}
    
    /**
     *  Wrap around a buffer
     *  @param  buffer
     *  @param  deepcopy
     */
    Ascii(const Buffer &buffer, bool deepcopy) : _buffer(buffer, deepcopy) {}

    /**
     *  Move a buffer inside the ascii object
     *  @param  buffer
     */
    explicit Ascii(Buffer &&buffer) : _buffer(std::move(buffer)) {}
    
    /**
     *  Destructor
     */
    virtual ~Ascii() = default;
    
    /**
     *  The underlying raw data (for ascii this is the same as the characters,
     *  but the library does not treat this as characted, but as raw data)
     *  @return const Buffer
     */
    const Buffer &buffer() const { return _buffer; }
    
    /**
     *  Get the underlying partial raw data buffer
     *  @param  start       start position in characters
     *  @param  size        size in characters
     *  @return Buffer
     */
    Buffer buffer(size_t start) const { return _buffer.part(start); }
    Buffer buffer(size_t start, size_t size) const { return _buffer.part(start, size); }
    
    /**
     *  Size of the raw data buffer in bytes
     *  @return size_t
     */
    size_t bytes() const { return _buffer.bytes(); }
    
    /**
     *  Number of characters in the string
     *  @return size_t
     */
    size_t characters() const { return _buffer.bytes(); }
    
    /**
     *  Find the substring
     *  @param  that        text to compare
     *  @return ssize_t     start position, -1 on no-match
     */
    ssize_t find(const Ascii &that) const
    {
        // do a memory-compare
        return _buffer.find(that._buffer);
    }

    /**
     *  Find the substring
     *  @param  that        text to compare
     *  @param  index       start position
     *  @return ssize_t     start position, -1 on no-match
     */
    ssize_t find(const Ascii &that, size_t index) const
    {
        // do a memory-compare
        return _buffer.find(that._buffer, index);
    }
    
    /**
     *  Comparison operation
     *  @param  that
     *  @return int
     */
    int compare(const Ascii &that) const
    {
        // compare the buffer
        return _buffer.compare(that._buffer);
    }
    
    /**
     *  Comparison operators
     *  @param  that
     *  @return bool
     */
    bool operator==(const Ascii &that) const { return compare(that) == 0; }
    bool operator!=(const Ascii &that) const { return compare(that) != 0; }
    
    /**
     *  Get a substring
     *  Is is up to the called to ensure that valid parameters are supplied (inside the right range)
     *  @param  start       start position in the current buffer
     *  @param  size        size of the substring
     *  @return Ascii
     */
    Ascii substr(size_t start, size_t size) const
    {
        return Ascii(std::move(_buffer.part(start, size)));
    }
    
    /**
     *  Get a substring
     *  Is is up to the called to ensure that a valid parameter is supplied (inside the right range)
     *  @param  start       start position of the substring
     *  @return Ascii
     */
    Ascii substr(size_t start) const
    {
        return Ascii(std::move(_buffer.part(start)));
    }
    
    /**
     *  Iterator over the buffer
     *  @return iterator
     */
    iterator begin() const { return iterator(_buffer.data()); }
    iterator end() const { return iterator(_buffer.data() + _buffer.bytes()); }
    
    /**
     *  Reverse iterator
     *  @return iterator
     */
    reverse_iterator rbegin() const { return reverse_iterator(_buffer.data() + _buffer.bytes()); }
    reverse_iterator rend() const { return reverse_iterator(_buffer.data()); }
};

/**
 *  End of namespace
 */
}

