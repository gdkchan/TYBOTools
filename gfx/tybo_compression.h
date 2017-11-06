#ifndef _TYBO_COMPRESSION_H_
#define _TYBO_COMPRESSION_H_

#define TYBO_ERR_SUCCESS         0
#define TYBO_ERR_COPY_OVERRUN   -1
#define TYBO_ERR_INPUT_OVERRUN  -2
#define TYBO_ERR_OUTPUT_OVERRUN -3
#define TYBO_ERR_NO_INPUT_DATA  -4

signed int tybo_decompress(unsigned char *cmp, 
                           unsigned char *dec,
                           signed   int  *cmp_len,
                           signed   int  *dec_len);

signed int tybo_compress(unsigned char *cmp,
                         unsigned char *dec,
                         signed   int  *cmp_len,
                         signed   int  *dec_len);

#endif /* _TYBO_COMPRESSION_H_ */