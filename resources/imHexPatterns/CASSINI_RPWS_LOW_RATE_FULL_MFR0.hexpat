#pragma author Alexis Jeandet
#pragma description Cassini RPWS_LOW_RATE_FULL Format
#pragma MIME application / x - dat

import std.core;
import std.mem;
import std.io;
import std.time;
std::core::set_endian(std::mem::Endian::Big);

fn format_time(auto seconds) {
    
	return "hello";
};

struct reccord {
    u8 data[912];
};

struct RPWS_SCLK {
    u32 SCLK_SECOND[[format("test_format")]];
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
};

struct time_table_reccord {
    RPWS_SCLK_SCET sclk_scet;
    u8 spare[4];
    float items[224];
};

struct frequency_table_reccord {
    RPWS_SCLK_SCET sclk_scet;
    u8 spare[4];
    float items[224];
};

enum DATA_VALIDITY : bool {
  good = 0,
  bad = 1
};

enum ACTIVITY_FLAG: bool {
  inactive = 0,
  active = 1
};

enum SENSOR_NUMBER_t: u8 {
  Ex=0,   
  Eu=1,
  Ev=2,
  Ew=3,
  Bx=4,
  By=5,
  Bz=6,
  Hf=8,
  Lp=11
};

bitfield LRFC_DATA_QUALITY {
    DATA_VALIDITY VALID_DATA_FLAG :1;
    ACTIVITY_FLAG HFR_SOUNDER_ACTIVE :1;
    ACTIVITY_FLAG LP_RAW_SWEEP_ACTIVE :1;
    bool GROUND_PRODUCED_DATA :1;
    _dummy :24;
    SENSOR_NUMBER_t SENSOR_NUMBER :4;
    
};

struct spectral_density_table_reccord {
    RPWS_SCLK_SCET sclk_scet;
    LRFC_DATA_QUALITY data_quality;
    float items[224];
};

struct file {
    reccord header;
    time_table_reccord tt;
    frequency_table_reccord ft;
    spectral_density_table_reccord values[while(!std::mem::eof())];
};

file f @ 0x0;