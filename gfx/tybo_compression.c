#include "tybo_compression.h"

typedef signed char  s8_t;
typedef signed short s16_t;
typedef signed int   s32_t;

typedef unsigned char  u8_t;
typedef unsigned short u16_t;
typedef unsigned int   u32_t;

u8_t copy_offset_lut[0xc0] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //0x00
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, //0x10
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, //0x20
    0x06, 0x06, 0x06, 0x06, 0x07, 0x07, 0x07, 0x07,
    0x08, 0x08, 0x09, 0x09, 0x0a, 0x0a, 0x0b, 0x0b, //0x30
    0x0c, 0x0c, 0x0d, 0x0d, 0x0e, 0x0e, 0x0f, 0x0f,
    0x10, 0x10, 0x11, 0x11, 0x12, 0x12, 0x13, 0x13, //0x40
    0x14, 0x14, 0x15, 0x15, 0x16, 0x16, 0x17, 0x17,
    0x18, 0x18, 0x19, 0x19, 0x1a, 0x1a, 0x1b, 0x1b, //0x50
    0x1c, 0x1c, 0x1d, 0x1d, 0x1e, 0x1e, 0x1f, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, //0x60
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, //0x70
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, //0x80
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, //0x90
    0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, //0xa0
    0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, //0xb0
    0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f
};

u8_t copy_size_lut[0x40] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //0x00
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
    0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, //0x10
    0x06, 0x06, 0x06, 0x06, 0x07, 0x07, 0x07, 0x07,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, //0x20
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, //0x30
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
};

u8_t token_bits_size_lut[0x40] = {
    0x0d, 0x0c, 0x0a, 0x0a, 0x08, 0x08, 0x08, 0x08, //0x00
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, //0x10
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, //0x20
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, //0x30
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02
};

u8_t token_bits_offs_lut[22] = {
    0x07, 0x07, 0x08, 0x09, 0x09, 0x09, 0x0a, 0x0a,
    0x0a, 0x0a, 0x0a, 0x0a, 0x05, 0x04, 0x03, 0x02,
    0x01, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};

s32_t tybo_decompress(u8_t  *cmp, 
                      u8_t  *dec,
                      s32_t *cmp_len,
                      s32_t *dec_len) {
    s32_t cmp_pos     = 0;
    s32_t dec_pos     = 0;
    u16_t value       = 0;
    u16_t token       = 0;
    u8_t  token_bits  = 16;
    u8_t  shift       = 0;
    u8_t  header      = 0;
    u8_t  header_mask = 0;

    if (*cmp_len == 0) {
        return TYBO_ERR_NO_INPUT_DATA;
    }

    #define TOKEN_DEC \
        if (token_bits > shift) { \
            token_bits -= shift; \
\
            token = value; \
\
            if (cmp_pos + 2 > *cmp_len) { \
                return TYBO_ERR_INPUT_OVERRUN; \
            } \
\
            value = ((u16_t)cmp[cmp_pos + 0] << 0 | \
                     (u16_t)cmp[cmp_pos + 1] << 8); \
\
            cmp_pos += 2; \
\
            shift = 16 - token_bits; \
        } else { \
            shift -= token_bits; \
        } \
\
        token <<= token_bits; \
        token |= (value >> shift) & ((1 << token_bits) - 1)

    TOKEN_DEC;

    do {
        header_mask >>= 1;

        if (header_mask == 0) {
            header_mask = 0x80;
            header = cmp[cmp_pos++];
        }

        if (header & header_mask) {
            //Literal
            if (cmp_pos >= *cmp_len) {
                return TYBO_ERR_INPUT_OVERRUN;
            }

            if (dec_pos >= *dec_len) {
                return TYBO_ERR_OUTPUT_OVERRUN;
            }

            dec[dec_pos++] = cmp[cmp_pos++];
        } else {
            //Copy
            s32_t copy_size;
            s32_t copy_offset;

            if (token & 0x8000) {
                //2 bytes copy
                copy_size = 1;
            } else {
                //Long copy
                u8_t lut_idx = token >> 9;

                if (lut_idx == 0) {
                    copy_size = (token >> 2) | 0x80;

                    if (copy_size == 0xff) {
                        //Compression end
                        break;
                    }
                } else if (lut_idx < 8) {
                    if (lut_idx < 2) {
                        copy_size = token >> 3;
                    } else if (lut_idx < 4) {
                        copy_size = token >> 5;
                    } else /* if (lut_idx < 8) */ {
                        copy_size = token >> 7;
                    }
                } else {
                    copy_size = copy_size_lut[lut_idx];
                }

                token_bits = token_bits_size_lut[lut_idx];

                TOKEN_DEC;
            }

            if ((u16_t)(token << 1) < 0x6000) {
                //Near copy
                token_bits = token_bits_offs_lut[(token >> 10) & 0xf];

                copy_offset = copy_offset_lut[(token >> 6) & 0xff];
            } else {
                //Far copy
                u8_t lut_idx = (token >> 12) & 7;

                token_bits = token_bits_offs_lut[14 + lut_idx];

                copy_offset = ((token & 0xfff) | 0x1000) >> token_bits_offs_lut[9 + lut_idx];
            }

            if (copy_offset - 1 >= dec_pos) {
                return TYBO_ERR_COPY_OVERRUN;
            }

            if (dec_pos + copy_size > *dec_len) {
                return TYBO_ERR_OUTPUT_OVERRUN;
            }

            while (copy_size-- >= 0) {
                dec[dec_pos] = dec[dec_pos - copy_offset - 1];

                dec_pos++;
            }

            TOKEN_DEC;
        }
    } while (cmp_pos < *cmp_len);

    *cmp_len = cmp_pos;
    *dec_len = dec_pos;

    return TYBO_ERR_SUCCESS;
}

#define MAX_DIST 0x1000

s32_t tybo_compress(u8_t  *cmp,
                    u8_t  *dec,
                    s32_t *cmp_len,
                    s32_t *dec_len) {
    u32_t queue       = 0;
    s32_t queue_cnt   = 0;
    s32_t cmp_pos     = 2;
    s32_t dec_pos     = 0;
    s32_t header_pos  = 0;
    s32_t token_pos0  = 0;
    s32_t token_pos1  = 0;
    u16_t token       = 0;
    u8_t  token_bits  = 16;
    u8_t  shift       = 0;
    u8_t  header      = 0;
    u8_t  header_mask = 0;

    #define TOKEN_ENC \
        queue |= ((u32_t)token << 16) >> queue_cnt; \
\
        if ((queue_cnt += token_bits) > 16) { \
            queue_cnt -= 16; \
\
            if (token_pos0 >= *cmp_len) \
                return TYBO_ERR_OUTPUT_OVERRUN; \
\
            cmp[token_pos0 + 0] = (u8_t)(queue >> 16); \
            cmp[token_pos0 + 1] = (u8_t)(queue >> 24); \
\
            queue <<= 16; \
        } \
\
        if (token_bits > shift) { \
            token_bits -= shift; \
\
            token_pos0 = token_pos1; \
            token_pos1 = cmp_pos; \
\
            cmp_pos += 2; \
\
            shift = 16 - token_bits; \
        } else \
            shift -= token_bits

    while (dec_pos < *dec_len) {
        header_mask >>= 1;

        if (header_mask == 0) {
            if (header_pos >= *cmp_len) {
                return TYBO_ERR_OUTPUT_OVERRUN;
            }

            cmp[header_pos] = header;

            header_mask = 0x80;
            header_pos  = cmp_pos++;
            header      = 0;
        }

        s32_t copy_size   = 0;
        s32_t copy_offset = 0;

        if (dec_pos > 0) {
            //Find best match on sliding window.
            s32_t m_size  = 0;
            s32_t m_start = dec_pos > MAX_DIST ?
                            dec_pos - MAX_DIST : 0;

            #define UPDATE_COPY \
                do { \
                    if (m_size - 1 >= copy_size) { \
                        copy_offset  = dec_pos; \
                        copy_offset -= m_start + 1; \
                        copy_size    = m_size  - 1; \
                    } \
                } while (0)

            while (m_start < dec_pos && dec_pos + m_size < *dec_len) {
                 u8_t copy_byte = m_start + m_size >= dec_pos
                    ? dec[m_start + (m_size % (dec_pos - m_start))]
                    : dec[m_start + m_size];

                if (dec[dec_pos + m_size] == copy_byte) {
                    m_size++;
                } else {
                    UPDATE_COPY;

                    m_start++;
                    m_size = 0;
                }
            }

            UPDATE_COPY;
        }

        if (copy_size > 0) {
            if (copy_size > 1) {
                if (copy_size > 0xfe) {
                    copy_size = 0xfe;
                }

                if (copy_size < 4) {
                    token = copy_size << 13;
                } else if (copy_size < 8) {
                    token = (0x10 << 9) + ((copy_size - 4) << 11);
                } else if (copy_size < 16) {
                    token = (0x08 << 9) + ((copy_size - 8) << 9);
                } else if (copy_size < 32) {
                    token = copy_size << 7;
                } else if (copy_size < 64) {
                    token = copy_size << 5;
                } else if (copy_size < 128) {
                    token = copy_size << 3;
                } else /* if (copy_size < 255) */ {
                    token = (copy_size & 0x7f) << 2;
                }

                token_bits = token_bits_size_lut[token >> 9];

                TOKEN_ENC;

                token = 0;
            } else {
                token = 0x8000;
            }

            if (copy_offset < 128) {
                if (copy_offset < 4) {
                    token |= copy_offset << 9;
                } else if (copy_offset < 8) {
                    token |= (0x20 << 6) + ((copy_offset - 4) << 8);
                } else if (copy_offset < 32) {
                    token |= (0x30 << 6) + ((copy_offset - 8) << 7);
                } else {
                    token |= (0x40 + copy_offset) << 6;
                }

                token_bits = token_bits_offs_lut[(token >> 10) & 0xf];
            } else {
                if (copy_offset < 256) {
                    token |= (3 << 12) | ((copy_offset & 0x07f) << 5);
                } else if (copy_offset < 512) {
                    token |= (4 << 12) | ((copy_offset & 0x0ff) << 4);
                } else if (copy_offset < 1024) {
                    token |= (5 << 12) | ((copy_offset & 0x1ff) << 3);
                } else if (copy_offset < 2048) {
                    token |= (6 << 12) | ((copy_offset & 0x3ff) << 2);
                } else /* if (copy_offset < 4096) */ {
                    token |= (7 << 12) | ((copy_offset & 0x7ff) << 1);
                }

                token_bits = token_bits_offs_lut[14 + ((token >> 12) & 7)];
            }

            dec_pos += copy_size + 1;

            TOKEN_ENC;
        } else {
            header |= header_mask;

            if (cmp_pos >= *cmp_len) {
                return TYBO_ERR_OUTPUT_OVERRUN;
            }

            cmp[cmp_pos++] = dec[dec_pos++];
        }
    }

    //Ensure header is written to stream.
    cmp[header_pos] = header;

    //Write end of compression marker.
    token      = 0x1fc;
    token_bits = 14 + 16;

    *cmp_len = cmp_pos;
    *dec_len = dec_pos;

    if (cmp_pos == token_pos1 + 2) {
        *cmp_len = token_pos1;
    }

    TOKEN_ENC;

    return TYBO_ERR_SUCCESS;
}