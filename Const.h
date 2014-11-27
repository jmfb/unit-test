#pragma once

namespace UnitTest
{

//16384 Recommended Standard C++ Value, but limited to 17 Recommended Recursive Template Instantiations
//Must be limited because the actual number of virtual functions for any given interface is not detectable
static constexpr unsigned long MAX_VIRTUAL_FUNCTIONS = 50;

}

