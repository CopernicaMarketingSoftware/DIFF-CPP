/**
 *  Limits.h
 *
 *  Class that sets the limits for an operation
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
#include <limits>

/**
 *  Begin of namespace
 */
namespace DIFF {

/**
 *  Class definition
 */
class Limits
{
public:
    /**
     *  Timeout for the "diff" operation
     *  @var float
     */
    float timeout = 1.0f;
    
    /**
     *  Cost for an empty operation in terms of characters
     *  @var short
     */
    short editcost = 4;



  // At what point is no match declared (0.0 = perfection, 1.0 = very loose).
  float Match_Threshold = 0.5f;
  // How far to search for a match (0 = exact location, 1000+ = broad match).
  // A match this many characters away from the expected location will add
  // 1.0 to the score (0.0 is a perfect match).
  int Match_Distance = 1000;
  // When deleting a large block of text (over ~64 characters), how close does
  // the contents have to match the expected contents. (0.0 = perfection,
  // 1.0 = very loose).  Note that Match_Threshold controls how closely the
  // end points of a delete need to match.
  float Patch_DeleteThreshold = 0.5f;
  // Chunk size for context length.
  short Patch_Margin = 4;

  // The number of bits in an int.
  short Match_MaxBits = 32;


public:
    /**
     *  Constructor
     */
    Limits() = default;
    
    /**
     *  Destructor
     */
    virtual ~Limits() = default;
    
    /**
     *  Get the deadline for the algorithm, given the current time
     *  @return clock_t
     */
    clock_t deadline() const
    {
        // if there is no limit
        if (timeout <= 0) return std::numeric_limits<clock_t>::max();
        
        // get current time
        return clock() + (clock_t)(timeout * CLOCKS_PER_SEC);
    }
};

/**
 *  End of namespace
 */
}

