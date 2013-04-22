#ifndef _CONF_INC_
#define _CONF_INC_

enum MaxStreamCount : unsigned {
  MAX_CAMS_8    = 8,
  MAX_CAMS_16   = 16,
  MAX_CAMS_32   = 32,
  MAX_CAMS_64   = 64,
  MAX_CAMS_128  = 128,
  MAX_CAMS_256  = 256,
  MAX_CAMS_512  = 512,
  MAX_CAMS_1024 = 1024
};

#define STREAM_ZBUFFER_SIZE 32

/// @todo generate on license check
#define MAX_CAMS_CONFIG MaxStreamCount::MAX_CAMS_64

/// @todo should get is from runtime
#define SYS_PAGE_SIZE (1<<12)

#define BS_BUFFER_LOW_WATERMARK 16

/// @define IO_CHUNK 
/// @brief  should be reasonable value to not to call handlers too often
#define IO_CHUNK 512

#define DEFAULT_IO_THREADS_NUMBER 2

#define DEFAULT_HTTP_PORT 80

#define JPEGMARK1 0xFF
#define JPEGMARK2 0xD8

#define JESMARK1 0xFF
#define JESMARK2 0xFE

#define JES_HEADER_SIZE 60 /* bytes */

/// @brief without leading marks (4 bytes)
#define JES_HDR_SIZE_OFFSET 44 /* bytes */

#endif
