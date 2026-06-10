#pragma once

#include "zftools_api.h"

// STD
#include <vector>
#include <new>

//===================================== CLASS MemoryEater ==================================================
/**
\class MemoryEater
\author Ulrich Hierl
\brief  This class provides functions that occupy a certain ammount of memory. This can be used before testing 
        how other functions react on low memory.
*/
class ZFTOOLS_API MemoryEater
{
    // ---------------------- Variables -------------------------------
private:

    int chunkSize;               //!< Size of the memory chunks that are allocated (in byte).
    std::vector<char*> chunkList;  //!< Vector that holds the pointers to the allocated areas.
    std::vector< char* >::iterator it;

    // ----------------------- Methods -------------------------------
public:
    MemoryEater();
    MemoryEater( int memoryInMB );               //!< Allocates memoryInMB megabyte on creation.
    ~MemoryEater();

    bool reservePieceWise( int memoryInMB );    //!< Tries to allocate memoryInMB megabyte of memory. It allocates small chunks of memory to allocate as much as possible.
    bool reserveSingleChunk( int memoryInMB );  //!< Tries to allocate memoryInMB megabyte of memory in one big piece.
    bool reserveAllBut( int memoryInMB = 0 );   //!< Allocates as much memory as it can and then dealocates memoryInMB megabyte.
    bool burnAllBut( int memoryInMB = 0 );      //!< The same as reserveAllBut() but the memory can not be freed after the call of this function.
    void freeAll();                             //!< Frees all allocated memory.

private:
    void initMember();
};

