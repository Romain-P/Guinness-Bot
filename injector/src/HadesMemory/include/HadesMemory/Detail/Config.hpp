// Copyright Joshua Boyce 2011.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// This file is part of HadesMem.
// <http://www.raptorfactor.com/> <raptorfactor@raptorfactor.com>

#pragma once

#include <boost/config.hpp>

// Version numbers
#define HADES_VERSION_MAJOR 1
#define HADES_VERSION_MINOR 6
#define HADES_VERSION_PATCH 0

// Full version number
#define HADES_VERSION_FULL ((HADES_VERSION_MAJOR * 10000) + \
(HADES_VERSION_MINOR * 100) + HADES_VERSION_PATCH)

// Full version number string generator helper
#define HADES_VERSION_FULL_STRING_GEN_EXP(x, y, z) "v" #x "." #y "." #z

// Full version number string generator
#define HADES_VERSION_FULL_STRING_GEN(x, y, z) \
HADES_VERSION_FULL_STRING_GEN_EXP(x, y, z)

// Full version number string
#define HADES_VERSION_FULL_STRING HADES_VERSION_FULL_STRING_GEN(\
HADES_VERSION_MAJOR, HADES_VERSION_MINOR, HADES_VERSION_PATCH)
