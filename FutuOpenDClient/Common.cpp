﻿#include "Common.h"

#include <memory.h>

using namespace ftq;
using namespace std;

namespace ftq
{
void APIProtoHeader::init()
{
    memset(this, 0, sizeof(*this));
    header_flag_[0] = 'F';
    header_flag_[1] = 'T';
}
}

