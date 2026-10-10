/*
 * Transitional DOSBox-only compatibility boundary for the legacy Coalface
 * declarations and global remaps. New DOSBox code should use the typed domain
 * adapters instead. This include remains while the legacy declarations are
 * migrated out incrementally.
 */

#ifndef BXDOSBoxCoalfaceCompatibility_h
#define BXDOSBoxCoalfaceCompatibility_h

#include "BXCoalface.h"

// Keep this legacy fatal remap inside the DOSBox-only compatibility boundary.
#define E_Exit(format,...) boxer_die(__PRETTY_FUNCTION__, __FILE__, __LINE__, format, ##__VA_ARGS__)

#endif /* BXDOSBoxCoalfaceCompatibility_h */
