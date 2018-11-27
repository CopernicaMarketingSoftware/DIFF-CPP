/**
 *  Operation.h
 *
 *  Enumeration type for operations. A patch contains a list of diffs,
 *  where each diff holds a INSERT, DELETE or EQUAL operation.
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
 *  The enumeration class
 */
enum class Operation {
    INSERT,
    DELETE,
    EQUAL
};

/**
 *  End of namespace
 */
}

