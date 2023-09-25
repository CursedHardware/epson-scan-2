#ifndef sane_version_hpp_
#define sane_version_hpp_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//TBD env
#define PACKAGE_STRING 1
#define SANE_MAJOR 1
#define SANE_MINOR 0

/*! \file
 *  \brief  Backend version information and checking
 *
 *  Not only does this file define the various bits of backend version
 *  information, it also contains code that performs sanity checking
 *  on these definitions.
 */

extern "C" {                    // needed until sane-backends-1.0.14
#include "epsonsane.h"
}

#if !((SANE_MAJOR == SANE_CURRENT_MAJOR) && (SANE_MINOR == 0))
#error "SANE installation violates versioning portability constraints."
#endif

//! Version of the SANE C API that the backend implements
/*! \hideinitializer
 */
#define BACKEND_MAJOR  1

#if (BACKEND_MAJOR != SANE_CURRENT_MAJOR)
#error "Backend assumptions do not match current SANE C API version."
#endif

#if (0 > BACKEND_MAJOR || 255 < BACKEND_MAJOR)
#error "BACKEND_MAJOR value out of range."
#endif

//! Version of the backend implementation
/*! This number is typically increased with every external release of
 *  the backend when there have been changes to the implementation.
 *
 *  \hideinitializer
 */
#define BACKEND_MINOR  1

#if (0 > BACKEND_MINOR || 255 < BACKEND_MINOR)
#error "BACKEND_MINOR value out of range."
#endif

//! Build revision of the backend implementation
/*! This number can be used to track the version of internal releases
 *  or to differentiate between different builds of the backend.
 *
 *  It takes a default value of zero and should probably be reset to
 *  that value when the ::BACKEND_MINOR value is increased.
 *
 *  \hideinitializer
 */
#define BACKEND_BUILD  0

#if (0 > BACKEND_BUILD || 65535 < BACKEND_BUILD)
#error "BACKEND_BUILD value out of range."
#endif

//! Indicates where the backend originates from
/*! This information includes both the name and the version of the
 *  originating software package.
 *
 *  \hideinitializer
 */
#define BACKEND_SOURCE PACKAGE_STRING

#ifndef PACKAGE_STRING
#error "PACKAGE_STRING is not defined at this point."
#endif

#endif  /* sane_version_hpp_ */
