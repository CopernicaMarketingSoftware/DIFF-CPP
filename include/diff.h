/**
 *  Diff.h
 *
 *  A patch is implemented as a list of diffs. Each diff holds a 
 *  INSERT, DELETE or EQUAL operation
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
#include "operation.h"
#include "buffer.h"

/**
 *  Begin of namespace
 */
namespace DIFF {

/**
 *  Class definition
 */
class Diff : public Buffer
{
private:
    /**
     *  The operation
     *  @var Operation
     */
    Operation _operation;
    

public:
    /**
     *  Constructor for an empty buffer
     *  @param  operation   the operation
     */
    Diff(const Operation &operation) : _operation(operation) {}

    /**
     *  Constructor to wrap around a buffer
     *  @param  operation   the operation
     *  @param  buffer      buffer to copy
     */
    Diff(const Operation &operation, const Buffer &buffer) :
        Buffer(buffer), _operation(operation) {}

    /**
     *  Constructor to wrap around a buffer by moving the buffer
     *  @param  operation   the operation
     *  @param  buffer      buffer to move
     */
    Diff(const Operation &operation, Buffer &&buffer) :
        Buffer(std::move(buffer)), _operation(operation) {}

    /**
     *  Constructor to construct based on a list of other buffers
     *  @param  operation   the operation
     *  @param  size        total size
     *  @param  begin       start iterator
     *  @param  end         end iterator
     */
    template <typename iter_t>
    Diff(const Operation &operation, size_t size, const iter_t &begin, const iter_t &end) :
        Buffer(size, begin, end), _operation(operation) {}
        
    /**
     *  Copy constructor
     *  @param  that
     */
    Diff(const Diff &that) :
        Buffer(that), _operation(that._operation) {}
    
    /**
     *  Move constructor
     *  @param  that
     */
    Diff(Diff &&that) :
        Buffer(std::move(that)), _operation(that._operation) {}
        
    /**
     *  Destructor
     */
    virtual ~Diff() = default;
    
    /**
     *  Expose the operation
     *  @return Operation
     */
    const Operation &operation() const { return _operation; }

    /**
     *  Get a partial substring
     *  @param  start       start position
     */
    Diff part(size_t start) const
    {
        // call base operation
        return Diff(_operation, std::move(Buffer::part(start)));
    }
    
    /**
     *  Get a partial substring
     *  @param  start       start position  
     *  @param  size        size of the buffre
     */
    Diff part(size_t start, size_t size) const
    {
        // call base operation
        return Diff(_operation, std::move(Buffer::part(start, size)));
    }
};

/**
 *  End of namespace
 */
}

