/**
 *  Patch.h
 *
 *  A patch is a list of diffs.
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
#include <list>
#include "ascii.h"
#include "diff.h"
#include "commonprefix.h"
#include "commonsuffix.h"
#include "commonoverlap.h"
#include "deadline.h"
#include "halfmatch.h"

/**
 *  Begin of namespace
 */
namespace DIFF {

/**
 *  Class definition
 */
template <typename text_t = Ascii, typename char_t = char>
class Patch
{
private:
    /**
     *  A patch consists of a serie of diffs
     *  @var std::list
     */
    std::list<Diff> _diffs;
    

private:
    /**
     *  Helper constructor that is used to calculate the diff with a certain deadline
     *  The algorithm stops once the deadline time is reached
     *  @param  limits      object with limits / settings for the algorithm
     *  @param  input1      the base string
     *  @param  input2      the string to compare
     *  @param  checklines  speedup flag
     *  @param  deadline    time when the algorithm should stop
     */
    Patch(const Limits &limits, const text_t &input1, const text_t &input2, bool checklines, const Deadline &deadline)
    {
        // are the two inputs identical?
        if (input1 == input2)
        {
            // if both texts are empty, than the diff can be empty
            if (input1.bytes() == 0) return;
            
            // there is one operation
            _diffs.emplace_back(Operation::EQUAL, input1.buffer());
        }
        else
        {
            // calculate the comming prefix and common suffix of the two texts
            CommonPrefix<text_t> prefix(input1, input2);
            CommonSuffix<text_t> suffix(input1.substr(prefix.characters()), input2.substr(prefix.characters()));
        
            // if there is a common prefix, it should be added to the  diffs
            if (prefix.characters() > 0) _diffs.emplace_back(Operation::EQUAL, prefix.buffer());
            
            // amount of common data
            size_t common = prefix.characters() + suffix.characters();
            
            // run the algorithm for the strings inside the common prefix and suffix
            compute(limits, input1.substr(prefix.characters(), common), input2.substr(prefix.characters(), common), checklines, deadline);
        
            // common suffix should also be added
            if (suffix.characters() > 0) _diffs.emplace_back(Operation::EQUAL, suffix.buffer());
            
            // optimize the _diffs member
            optimize();
        }
    }
    
public:
    /**
     *  Calculate the patch to transform one string into an other string
     * 
     *  If checklines is set to true, we run a slighly less optimal algorithm
     *  that is a little faster. If set to false, we first run a line-level
     *  diff to identify changed areas.
     * 
     *  @param  limits      object with limits / settings for the algorithm
     *  @param  input1      the base string
     *  @param  input2      the string to compare
     *  @param  checklines  tuning flag
     */
    Patch(const Limits &limits, const text_t &input1, const text_t &input2, bool checklines = true) :
        Patch(limits, input1, input2, checklines, limits.deadline()) {}
    
    /**
     *  Destructor
     */
    virtual ~Patch() = default;

private:
    /**
     *  Compute the algorithm for two strings
     *  This method is called by the constructor after the common prefix and common suffix
     *  have been removed from the input strings
     *  @param  limits      object with algorithm limits
     *  @param  text1       base input string
     *  @param  text2       the other input string
     *  @param  checklines  tuning flag
     *  @param  deadline    timestamp when to stop
     */
    void compute(const Limits &limits, const text_t &text1, const text_t &text2, bool checklines, const Deadline &deadline)
    {
        // if one of the texts is empty, the diff is really simple
        if (text1.characters() == 0) return _diffs.emplace_back(Operation::INSERT, text2.buffer());
        if (text2.characters() == 0) return _diffs.emplace_back(Operation::DELETE, text1.buffer());
        
        // get the long and short texts to see if the shorter one is completely included in the other
        if (overlap(text1, text2)) return;
        
        // if we have a deadline we are going to check first if we can find a fast, but non-optimal, solution
        if (deadline && halfmatch(limits, text1, text2, checklines, deadline)) return;
        
        // do the real diff
        if (checklines && text1.characters() > 100 && text2.characters() > 100) return linemode(text1, text2, deadline);

        // run the entire algorithm
        return bisect(text1, text2, deadline);
    }
    
    /**
     *  Run the half-match algorithm to find a non-optimal result
     *  @param  limits      object with algorithm limits
     *  @param  text1       first input text
     *  @param  text2       text to reach
     *  @param  checklines  tuning flag
     *  @param  deadline    timestamp when to stop
     *  @return bool        was this a success?
     */
    bool halfmatch(const Limits &limits, const text_t &text1, const text_t &text2, bool checklines, const Deadline &deadline)
    {
        // find the long and short texts
        const text_t &longtext = text1.characters() > text2.characters() ? text1 : text2;
        const text_t &shorttext = &longtext == &text1 ? text2 : text1;

        // check if this algorithm can succeed in the first place
        if (longtext.characters() < 4 || shorttext.characters() * 2 < longtext.characters()) return false;
        
        // construct the half-match result
        HalfMatch<text_t> result(longtext, shorttext);
        
        // stop if no match
        if (!result) return false;
        
        // check
        if (&longtext == &text1)
        {
            // find the diffs inside the non-common stuff
            Patch<text_t> part1(limits, result.longPrefix(), result.shortPrefix(), checklines, deadline);
            Patch<text_t> part2(limits, result.longSuffix(), result.shortSuffix(), checklines, deadline);
            
            // add to the result
            _diffs.splice(_diffs.end(), part1._diffs, part1._diffs.begin(), part1._diffs.end());
            _diffs.emplace_back(Operation::EQUAL, result.common().buffer());
            _diffs.splice(_diffs.end(), part2._diffs, part2._diffs.begin(), part2._diffs.end());
        }
        else
        {
            // find the diffs inside the non-common-stuff
            Patch<text_t> part1(limits, result.shortPrefix(), result.longPrefix(), checklines, deadline);
            Patch<text_t> part2(limits, result.shortSuffix(), result.longSuffix(), checklines, deadline);
            
            // add to the result
            _diffs.splice(_diffs.end(), part1._diffs, part1._diffs.begin(), part1._diffs.end());
            _diffs.emplace_back(Operation::EQUAL, result.common().buffer());
            _diffs.splice(_diffs.end(), part2._diffs, part2._diffs.begin(), part2._diffs.end());
        }
        
        // done
        return true;
    }
    
    /**
     *  Run the linemode algorithm, this first does a line-based diff to locate the areas
     *  that are most effective to spend time on to look for an efficient diff
     *  @param  text1       first input text
     *  @param  text2       text to reach
     *  @param  deadline    timestamp when to stop
     */
    void linemode(const text_t &text1, const text_t &text2, const Deadline &deadline)
    {
        // @todo implement this algorithm
    }
    
    /**
     *  The entire algorithm (without the linemode stuff)
     *  @param  text1       first input text
     *  @param  text2       text to reach
     *  @param  deadline    timestamp when to stop
     */
    void bisect(const text_t &text1, const text_t &text2, const Deadline &deadline)
    {
        // @todo implement this algorithm
    }
    
    /**
     *  Algorithm that checks if one text is completely covered by the other
     *  @param  text1       the first text to check
     *  @param  text2       the other text
     *  @return bool        was there an overlap (algorithm is ready if true)
     */
    bool overlap(const text_t &text1, const text_t &text2)
    {
        // check if there is an overlap
        CommonOverlap<text_t> overlap(text1, text2);
        
        // is there indeed this overlap?
        if (overlap)
        {
            // this is a simple optimization
            _diffs.emplace_back(overlap.operation(), overlap.prefix());
            _diffs.emplace_back(Operation::EQUAL, overlap.buffer());
            _diffs.emplace_back(overlap.operation(), overlap.suffix());
            
            // we have a match indeed
            return true;
        }
        
        // no match, but we can still optimize if the short-string is only one char wide
        if (text1.characters() == 1 || text2.characters() == 1)
        {
            // after the previous check, we are sure that there is no EQUALS
            _diffs.emplace_back(Operation::DELETE, text1.buffer());
            _diffs.emplace_back(Operation::INSERT, text2.buffer());
            
            // we also have a match
            return true;
        }
        
        // no overlap between the texts
        return false;
    }
    
    /**
     *  Helper method to optimize the _diffs member: subsequent operations that can be
     *  combined or merged are grouped
     */
    void optimize()
    {
        // keep optimizing until there is nothing left to optimize
        do
        {
            // combined insert and remove operations
            mergeUpdates();
            
            // combined equal operations
            mergeEquals();
        }
        while (shift() > 0);
    }
    
    /**
     *  Helper method to merge multiple INSERT and REMOVE operations in the _diffs member
     *  into a single array
     */
    void mergeUpdates()
    {
        // merged INSERT and DELETE operations
        Diff mergedInsert(Operation::INSERT);
        Diff mergedDelete(Operation::DELETE);
        
        // iterator to the begin of a serie of edit-operations
        auto updates = _diffs.end();
        
        // go iterate
        for (auto iter = _diffs.begin(); iter != _diffs.end(); ++iter)
        {
            // remember if this is the begin of a set of update operations
            if (updates == _diffs.end() && iter->operation() != Operation::EQUAL) updates = iter;
            
            // check the operation at this position
            switch (iter->operation()) {
            case Operation::INSERT:
                // track all inserted data
                mergedInsert.append(*iter);
                break;

            case Operation::DELETE:
                // track all deleted data
                mergedDelete.append(*iter);
                break;

            case Operation::EQUAL:
                // have we just passed a set of update operations?
                if (updates != _diffs.end()) break;

                // remove these operations from the list
                _diffs.erase(updates, iter);
                
                // forget that we have seen updates now that they have been removed
                updates = _diffs.end();
                
                // check if the inserting and deleting start with the same text?
                CommonPrefix<text_t> commonprefix(mergedInsert, mergedDelete);
                
                // is there indeed a common prefix?
                if (commonprefix)
                {
                    // insert a new EQUAL operation for this prefix
                    _diffs.emplace(iter, Operation::EQUAL, commonprefix.buffer());
                    
                    // remove the leading bytes
                    mergedInsert.assign(mergedInsert.part(commonprefix.bytes()));
                    mergedDelete.assign(mergedDelete.part(commonprefix.bytes()));
                }
                
                // and is there a common suffic too?
                CommonSuffix<text_t> commonsuffix(mergedInsert, mergedDelete);
                
                // check if it exists
                if (commonsuffix)
                {
                    // the actual suffix as byte-array is inserted as EQUAL operation right 
                    // in front of the current operation as EQUAL operation, and we update the
                    // iterator so that the subsequent DELETE and INSERT operations will be 
                    // placed in front
                    iter = _diffs.emplace(iter, Operation::EQUAL, std::move(commonsuffix.buffer()));
                     
                    // remove the bytes that were in the common suffix
                    mergedInsert.shrink(commonsuffix.bytes());
                    mergedDelete.shrink(commonsuffix.bytes());
                }

                // the merge update operations are no added to the diffs
                if (mergedDelete.bytes() > 0) _diffs.emplace(iter, Operation::DELETE, std::move(mergedDelete));
                if (mergedInsert.bytes() > 0) _diffs.emplace(iter, Operation::INSERT, std::move(mergedInsert));
            
                // reset the merged texts
                mergedInsert.clear();
                mergedDelete.clear();
                break;
            }
        }
    }
    
    /**
     *  Method to merge subsequent EQUAL operations into a single EQUAL operation
     */
    void mergeEquals()
    {
        // iterator to the begin of a serie of EQUAL-operations
        auto begin = _diffs.end();
        
        // total size of all EQUAL operations
        size_t size = 0;
        
        // number of operations
        size_t count = 0;
        
        // go iterate
        for (auto iter = _diffs.begin(); iter != _diffs.end(); ++iter)
        {
            // check the operation at this position
            if (iter->operation() == Operation::EQUAL)
            {
                // is this the begin?
                if (count == 0) begin = iter;
                
                // update counters
                ++count; size += iter->bytes();
            }
            else if (count > 1)
            {
                // we are going to insert one big EQUAL operation that combines all others
                _diffs.emplace(begin, Operation::EQUAL, size, begin, iter);
                
                // and remove the discovered set of operations
                _diffs.erase(begin, iter);
                
                // erase the administration
                begin = _diffs.end(); size = 0; count = 0;
            }
            else
            {
                // this was a range of only one operation, so can be kept
                begin = _diffs.end(); size = 0; count = 0;
            }
        }
        
        // done if this was all
        if (count < 2) return;
        
        // merge the final EQUAL's
        _diffs.emplace(begin, Operation::EQUAL, size, begin, _diffs.end());
        
        // remove the range of EQUAL operations
        _diffs.erase(begin, _diffs.end());
    }
    
    /**
     *  Shift single edits that are surrounded by EQUAL operations
     *  e.g: A<ins>BA</ins>C -> <ins>AB</ins>AC
     *  @return size_t      number of changes
     */
    size_t shift()
    {
        // number of changes
        size_t changes = 0;
        
        // we need a couple of iterators
        auto prev    = _diffs.begin();
        auto current = std::next(prev);
        auto next    = std::next(current);
        
        // iterate over all the diffs
        while (next != _diffs.end())
        {
            // we are looking for operations that are surrounded by equalities,
            // and we do this by looking at the current iter position, and 
            // position - 1 and position - 2, if current diff is not EQUALS,
            // then checking the previous ones is pointless anywat
            if (next->operation() != Operation::EQUAL) continue;
            
            // things are pointless if the operation is not an update, or when
            // the start EQUALS is not even a real EQUAL operation
            if (current->operation() == Operation::EQUAL) continue;
            if (prev->operation() != Operation::EQUAL) continue;
            
            // do the comparison
            if (current->bytes() >= prev->bytes() && current->compare(current->bytes() - prev->bytes(), prev->bytes(), *prev) == 0)
            {
                // shift the edit over the previous equals operation
                current->shrink(prev->bytes());
                current->prepend(*prev);
                next->prepend(*prev);
                
                // remove the first EQUALS operation
                _diffs.erase(prev);
                
                // remember that something changed
                ++changes;
            }
            else if (current->bytes() >= next->bytes() && current->compare(0, next->bytes(), *next) == 0)
            {
                // shift the edit over the next equals operation
                prev->append(*next);
                current->skip(next->bytes());
                current->append(*next);
                
                // remove the second EQUALS operation
                next = _diffs.erase(next);
                
                // remember that something changed
                ++changes;
            }
            
            // move the iterators for the next iteration
            prev = current; current = next; ++next;
        }
        
        // done
        return changes > 0;
    }
};


/**
 *  End of namespace
 */
}

