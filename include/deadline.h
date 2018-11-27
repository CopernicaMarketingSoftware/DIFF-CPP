/**
 *  Deadline.h
 *
 *  Class that checks if the deadline for the algorithm has already been reached
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
class Deadline
{
private:
    /**
     *  Start time of the operation
     *  @var clock_t
     */
    clock_t _start;
    
    /**
     *  Number of seconds that the operation may take (0 for infinity)
     *  @var time_t
     */
    time_t _max;


public:
    /**
     *  Default constructor creates an infinite deadline
     */
    Deadline() : _start(clock()), _max(0) {}
    
    /**
     *  Constructor to set a max
     *  @param  seconds
     */
    Deadline(time_t seconds) : _start(clock()), _max(seconds) {}
    
    /**
     *  Destructor
     */
    virtual ~Deadline() = default;
    
    /**
     *  Is the deadline set? This is a cast-to-boolean operation
     *  @return bool
     */
    operator bool () const { return _max > 0; }
    bool operator! () const { return _max == 0; }
    
    /**
     *  What is the value of the deadline? Could be a wrapped value.
     *  @return value
     */
    clock_t expiration() const { return _start + _max * CLOCKS_PER_SEC; }
    
    /**
     *  Have we reached the deadline?
     *  @return bool
     */
    bool reached() const 
    {
        // never reached if there is no max
        if (_max == 0) return false;
        
        // get the current cpu time and the expire time
        clock_t expire = expiration();
        clock_t now = clock();
        
        // if the expire time has not wrapped, we expired if time is after
        // the expire time, or when the time did wrap
        if (expire > _start) return now >= expire || now < _start;
        
        // the expire time did wrap, so the start time must have wrapped too
        return now > _start && now >= expire;
    }

};

/**
 *  End of namespace
 */
}

