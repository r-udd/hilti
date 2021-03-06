///
/// User-visible top-level include file.
///

#ifndef BINPACXX_H
#define BINPACXX_H

#include <fstream>

#include "common.h"
#include "context.h"
#include "options.h"
#include "operator.h"

namespace binpac {

/// Initializes the BinPAC++ compiler infrastructure. Must be called before
/// any other method.
extern void init();

/// Returns the current BinPAC++ version.
extern string version();

/// Returns a list of all BinPAC++ operators.
operator_list operators();

}

#endif
