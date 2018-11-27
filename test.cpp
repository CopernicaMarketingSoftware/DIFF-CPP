/**
 *  Test.cpp
 * 
 *  Test file for the library
 *  
 *  @author Emiel Bruijntjes <emiel.bruijntjes@copernica.com>
 *  @copyright 2018 Copernica BV
 */

/**
 *  Dependencies
 */
#include <string>
#include "include/limits.h"
#include "include/patch.h"

/**
 *  Main procedure
 *  @return int
 */
int main()
{
    // create two string
    DIFF::Ascii input1("hallo daar");
    DIFF::Ascii input2("hallo hier");

    // limits for calculating
    DIFF::Limits limits;
    
    // get the patch
    DIFF::Patch<> patch(limits, input1, input2);
    
    // done
    return 0;
}
