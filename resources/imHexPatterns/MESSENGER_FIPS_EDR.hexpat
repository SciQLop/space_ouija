#pragma author Alexis Jeandet
#pragma description Messenger FIPS EDR Format
#pragma MIME application / x - dat

import std.core;
import std.mem;
import std.io;
import std.time;
std::core::set_endian(std::mem::Endian::Big);


struct FIPS_EDR {
    u32 met;
    u16 scan;
    u32 start[64];
    u32 stop[64];
    u32 valid[64];
    u32 prog[64];
    u32 met2[64];
};



struct file {
    FIPS_EDR values[while(!std::mem::eof())];
};

file f @ 0x0;
