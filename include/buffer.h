/**
 *  Buffer.h
 *
 *  Class that wraps around a buffer of bytes. This is not necessarily
 *  the same as the buffer of characters (for utf8 for example, a char
 *  can be 1, 2, 3 or 4 bytes wide.
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
class Buffer
{
private:
    /**
     *  The byte-string (not characters!)
     *  @var void *
     */
    char *_data;
    
    /**
     *  Size of the byte-string
     *  @var size_t
     */
    size_t _size;
    
    /**
     *  Is the byte-buffer self-allocated?
     *  @var bool
     */
    bool _allocated;

public:
    /**
     *  Default constructor
     */
    Buffer() : _data(nullptr), _size(0), _allocated(false) {}
    
    /**
     *  Constructor
     *  @param  data        the buffer to wrap
     *  @param  size        size of the buffer
     *  @param  deepcopy    should we make a deep-copy?
     */
    Buffer(const char *data, size_t size, bool deepcopy) :
        _data(deepcopy ? (char *)malloc(size) : (char *)data),
        _size(size),
        _allocated(deepcopy)
    {
        // make a deepcopy if necessary
        if (deepcopy) memcpy(_data, data, size);
    }

    /**
     *  Constructor based on other object
     *  @param  that        object to copy
     *  @param  deepcopy    make a deep copy?
     */
    Buffer(const Buffer &that, bool deepcopy) :
        Buffer(that._data, that._size, deepcopy) {}

    /**
     *  Copy constructor
     *  @param  that
     */
    Buffer(const Buffer &that) : Buffer(that, that._allocated) {}
    
    /**
     *  Move constructor
     *  @param  that        object to move
     */
    Buffer(Buffer &&that) : 
        _data(that._data),
        _size(that._size),
        _allocated(that._allocated)
    {
        // invalidate the other object
        that._size = 0;
        that._allocated = false;
    }
    
    /**
     *  Constructor to construct based on a list of other buffers
     *  @param  size        total size
     *  @param  begin       start iterator
     *  @param  end         end iterator
     */
    template <typename iter_t>
    Buffer(size_t size, const iter_t &begin, const iter_t &end) :
        _data(size > 0 ? (char *)malloc(size) : nullptr),
        _size(0),
        _allocated(size > 0)
    {
        // iterate over the data
        for (auto iter = begin; iter != end; ++iter)
        {
            // append data
            memcpy(_data + _size, iter->_data, iter->_size);
            
            // update size
            _size += iter->_size;
        }
    }
    
    /**
     *  Destructor
     */
    virtual ~Buffer()
    {
        // deallocate
        if (_allocated) free(_data);
    }

    /**
     *  Get access to the underlying raw data
     *  @return const char *
     */
    const char *data() const { return _data; }
    
    /**
     *  Size of the data: number of bytes
     *  @return size_t
     */
    size_t bytes() const { return _size; }
    
    /**
     *  Append an extra buffer
     *  @param  data        bytes to append
     *  @param  size        number of bytes
     */
    void append(const char *data, size_t size)
    {
        // are we already allocated?
        if (_allocated)
        {
            // reallocate
            _data = (char *)realloc(_data, _size + size);
        }
        else
        {
            // remember the old data
            void *olddata = _data;
            
            // now allocate it
            _data = (char *)malloc(_size + size);
            
            // copy the old data
            if (_size > 0) memcpy(_data, olddata, _size);
            
            // data is now allocated
            _allocated = true;
        }
        
        // append the other data
        memcpy(_data + _size, data, size);
        
        // update total size
        _size += size;
    }

    /**
     *  Append a normal buffer
     *  @param  buffer      buffer to append
     */
    void append(const Buffer &that) 
    {
        // pass on
        append(that._data, that._size);
    }
    
    /**
     *  Prepend data
     *  @param  buffer
     *  @param  size
     */
    void prepend(const char *data, size_t size)
    {
        // allocate a brand new buffer
        char *buffer = (char *)malloc(_size + size);
        
        // copy all data
        memcpy(buffer, data, size);
        memcpy(buffer + size, _data, _size);
        
        // get rid of the old buffer
        if (_allocated) free(_data);
        
        // update status
        _data = buffer;
        _size += size;
        _allocated = true;
    }
        
    /**
     *  Prepend data
     *  @param  that
     */
    void prepend(const Buffer &that)
    {
        // pass on
        prepend(that._data, that._size);
    }
    
    /**
     *  Assign a different buffer
     *  @param  data        bytes to assign
     *  @param  size        size of the data
     *  @param  deepcopy    make a deep copy
     */
    void assign(const char *data, size_t size, bool deepcopy)
    {
        // should we allocate or reallocate?
        if (_allocated)
        {
            // should we stay allocated?
            if (deepcopy)
            {
                // we may have to reallocate
                if (_size != size) _data = (char *)realloc(_data, size);

                // copy the data
                memcpy(_data, data, size);
            }
            else
            {
                // data no longer has to be allocated
                free(_data);
                
                // assign data
                _allocated = false;
                _data = (char *)data;
            }
        }
        else
        {
            // do we have to allocate?
            if (deepcopy)
            {
                // allocate enough data
                _allocated = true;
                _data = (char *)malloc(size);
                
                // copy the data
                memcpy(_data, data, size);
            }
            else
            {
                // we just have to assign data
                _data = (char *)data;
            }
        }

        // there is a new size
        _size = size;
    }
    
    /**
     *  Assign a different buffer
     *  @param  buffer      buffer to copy
     *  @param  deepcopy    make a deep copy
     */
    void assign(const Buffer &that, bool deepcopy)
    {
        // pass on
        assign(that._data, that._size, deepcopy);
    }
    
    /**
     *  Make a deep copy
     *  @param  buffer
     */
    void assign(const Buffer &that)
    {
        // pass on
        assign(that, that._allocated);
    }
    
    /**
     *  Shrink a buffer from the end (make it smaller with a number of bytes)
     *  @param  size        number of bytes to shrink
     */
    void shrink(size_t size)
    {
        // if the buffer should be emptied
        if (size >= _size) return clear();
        
        // if nothing changes
        if (size == 0) return;
    
        // do we have an allocated buffer?
        if (_allocated) _data = (char *)realloc(_data, _size - size);
        
        // store the new size
        _size -= size;
    }
    
    /**
     *  Shrink the buffer from the beginning
     *  @param  size        number of bytes to shrink
     */
    void skip(size_t size)
    {
        // if everything is removed
        if (size >= _size) return clear();

        // if nothing changes
        if (size == 0) return;
        
        // create a new buffer
        char *buffer = (char *)malloc(_size - size);
        
        // copy the old data
        memcpy(buffer, _data + size, _size - size);
        
        // get rid of the old buffer
        if (_allocated) free(_data);
        
        // update status
        _data = buffer;
        _size -= size;
        _allocated = true;
    }
    
    /**
     *  Make the buffer empty
     */
    void clear()
    {
        // deallocate
        if (_allocated) free(_data);
        
        // reset members
        _allocated = false;
        _data = nullptr;
        _size = 0;
    }
    
    /**
     *  Comparison operation
     *  @param  start       start position
     *  @param  size        number of bytes to compare
     *  @param  that        buffer to compare
     *  @return int
     */
    int compare(size_t start, size_t size, const Buffer &that) const
    {
        // if user wants to compare more data than fits in buffer
        if (size > that._size) size = that._size;
        
        // compare the data
        return memcmp(_data + start, that._data, std::min(size, _size - start));
    }
    
    /**
     *  Comparison operation
     *  @param  that
     *  @return int
     */
    int compare(const Buffer &that) const
    {
        // compare the prefix
        int result = memcmp(_data, that._data, std::min(_size, that._size));
        
        // the result is final if both strings have the same size, or if we 
        // already discovered that there is a difference between the strings
        if (result != 0 || _size == that._size) return result;
        
        // the shortest string go first
        return _size - that._size;
    }
    
    /**
     *  Find a sub-buffer
     *  @param  that
     *  @return ssize_t
     */
    ssize_t find(const Buffer &that) const
    {
        // do the operation
        char *result = (char *)memmem(_data, _size, that._data, that._size);
        
        // if there is no other buffer
        if (result == nullptr) return -1;
        
        // the parameter appears in this buffer
        return result - _data;
    }

    /**
     *  Find a sub-buffer
     *  @param  that
     *  @param  pos
     *  @return ssize_t
     */
    ssize_t find(const Buffer &that, size_t pos) const
    {
        // do the operation
        char *result = (char *)memmem(_data + pos, _size - pos, that._data, that._size);
        
        // if there is no other buffer
        if (result == nullptr) return -1;
        
        // the parameter appears in this buffer
        return result - _data;
    }
    
    /**
     *  Comparison operators
     *  @param  that
     *  @return bool
     */
    bool operator==(const Buffer &that) const { return compare(that) == 0; }
    bool operator!=(const Buffer &that) const { return compare(that) != 0; }

    /**
     *  Get a partial substring
     *  @param  start       start position
     */
    Buffer part(size_t start) const
    {
        // prevent out-of-range errors
        if (start >= _size) return Buffer();
        
        // get the partial buffer
        return Buffer(_data + start, _size - start, _allocated);
    }
    
    /**
     *  Get a partial substring
     *  @param  start       start position  
     *  @param  size        size of the buffre
     */
    Buffer part(size_t start, size_t size) const
    {
        // prevent out-of-range errors
        if (start >= _size || size == 0) return Buffer();
        
        // get the partial buffer
        return Buffer(_data + start, std::min(_size - start, size), _allocated);
    }
};

    
/**
 *  End of namespace
 */
}
