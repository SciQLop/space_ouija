#pragma author Alexis Jeandet
#pragma description Cassini RPWS_LOW_RATE_FULL Format
#pragma MIME application / x - dat

import std.core;
import std.mem;
import std.io;
import std.time;
std::core::set_endian(std::mem::Endian::Big);

fn format_time(auto sclk_scet) {
    u64 days = sclk_scet.scet.SCET_DAY -4383;
    double seconds = double(days) * 24 * 3600;
    return std::time::format(std::time::to_utc(seconds + double(sclk_scet.scet.SCET_MILLISECOND)/1000.), "%Y:%m:%d %H:%M:%S");
};

struct LRFULL_TABLE {
    char FILE_ID[8];
    u32 RECORD_LENGTH;
    u32 RECORDS;
    u32 RECEIVER_TYPE;
    u32 unused;
    char MINI_PACKET_HEADER[24];
    char SCET[16];
    char SCLK[16];
    u8 extra_space[RECORD_LENGTH-$];
};

struct RPWS_SCLK {
    u32 SCLK_SECOND;
    u8 SCLK_PARTITION;
    u8 SCLK_FINE;
};

struct  RPWS_SCET {
    u16 SCET_DAY;
    u32  SCET_MILLISECOND;
};

struct RPWS_SCLK_SCET {
    RPWS_SCLK sclk;
    RPWS_SCET scet;
}[[format("format_time")]];

enum DATA_VALIDITY : bool {
  good = 0,
  bad = 1
};

enum ACTIVITY_FLAG: bool {
  inactive = 0,
  active = 1
};

enum ANTENNA_t: u8 {
  Ex=0,
  Eu=1,
  Ev=2,
  Ew=3,
  Bx=4,
  By=5,
  Bz=6,
  Hf=8,
  Lp=11,
  Unknown=15
};

struct RPWS_WBR_WFR_ROW_PREFIX
{
    RPWS_SCLK_SCET sclk_scet;
    u16 RECORD_BYTES;
    u16 SAMPLES;
    u16 DATA_RTI;
    u8 VALIDITY_FLAG;
    u8 STATUS_FLAG;
    u8 FREQUENCY_BAND;
    u8 GAIN;
    u8 ANTENNA;
    u8 AGC;
    u8 HFR_XLATE;
    u8 SUB_RTI;
    u8 LP_DAC_0;
    u8 LP_DAC_1;
    u8 FSW_VER;
    u8 SPARE[3];
};

struct RPWS_WBR_WFR_ROW {
  RPWS_WBR_WFR_ROW_PREFIX prefix;
  u8 TIME_SERIES[1536];
  u8 _dummy[2048 - $];
};

struct RPWS_WBR_WFR {
    RPWS_WBR_WFR_ROW rows[while(!std::mem::eof())];
};

RPWS_WBR_WFR f @ 0x0;
