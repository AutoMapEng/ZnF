#pragma once

#include "ZfsPrjBasic.h"

#include "target.h"
#include <vector>


/// targets_Common returns the number of identical target-names in the two lists
ZFSPRJ_API int targets_Common(targetlist_t&, targetlist_t&, bool casesens = false);
