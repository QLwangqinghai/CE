
#ifndef CCAes_h
#define CCAes_h


/* Because array size can't be a const in C, the following two are macros.
   Both sizes are in bytes. */
#define AES_MAXNR 14
#define AES_BLOCK_SIZE 16


//#ifdef  __cplusplus
//extern "C" {
//#endif
#include <stdint.h>
#include <limits.h>

/* This should be a hidden type, but EVP requires that the size be known */
typedef struct __CCAesContext {
    uint32_t rd_key[4 *(AES_MAXNR + 1)];
    uint8_t iv[AES_BLOCK_SIZE];
    uint32_t rounds: 8;
    uint32_t op: 1;// CCAesOperationEncrypt or CCAesOperationDecrypt
    uint32_t mode: 6;
    uint32_t paddingMode: 9;
    uint32_t xxx: 8;
    uint32_t xxxx;
} CCAesContext_s;
typedef CCAesContext_s * CCAesContextPtr;


typedef uint32_t CCAesOperation_t;
static const CCAesOperation_t CCAesOperationEncrypt = 1;
static const CCAesOperation_t CCAesOperationDecrypt = 0;

typedef uint32_t CCAesCryptMode_t;
static const CCAesCryptMode_t CCAesCryptModeCbc = 0;
static const CCAesCryptMode_t CCAesCryptModeEcb = 1;

typedef uint32_t CCAesPaddingMode_t;
static const CCAesPaddingMode_t CCAesPaddingModeNoPadding = 0;
static const CCAesPaddingMode_t CCAesPaddingModePkcs5 = 1;

typedef int CCResult_t;
static const CCResult_t CCResultSuccess = 0;

static const CCResult_t CCResultErrorNonnullPtr = -1;
static const CCResult_t CCResultErrorUserKey = -2;
static const CCResult_t CCResultErrorOp = -3;
static const CCResult_t CCResultErrorMode = -4;
static const CCResult_t CCResultErrorPadding = -5;

static const CCResult_t CCResultErrorInput = -6;
static const CCResult_t CCResultErrorOutputSize = -7;

static const CCResult_t CCResultErrorDecrypt = -8;


static const CCResult_t CCResultErrorParma = INT_MIN;


/*!
    @function   CCAesContextInit
    @abstract   aes context init
 
    @param      context             context.
 
 
    @param      userKey             Raw key material, length bits/8 bytes.
 
    @param      bits                Length of key material. Must be appropriate (128, 192 or 256)
 
    @param      vi                  Initialization vector, optional. Used for Cipher Block Chaining (CBC) mode. If present,
                                    must be AES_BLOCK_SIZE. If CBC mode is selected (by the absence of any mode bits in the options flags) and no IV is present, a NULL (all zeroes) IV will be used.
 
    @param      op                  Defines the basic operation: CCAesOperationEncrypt or CCAesOperationDecrypt
 
    @param      mode                Defines the basic operation: CCAesCryptModeCbc or CCAesCryptModeEcb

    @param      padding             Defines the basic operation: CCAesPaddingModeNoPadding or CCAesPaddingModePkcs5

    @result     CCResultSuccess, CCResultErrorNonnullPtr, CCResultErrorUserKey, CCResultErrorOp, CCResultErrorMode, CCResultErrorPadding
 */
CCResult_t CCAesContextInit(CCAesContextPtr _Nonnull context,
                            const uint8_t * _Nonnull userKey,
                            const int bits,
                            const uint8_t * _Nullable vi,
                            const CCAesOperation_t op,
                            CCAesCryptMode_t mode,
                            CCAesPaddingMode_t padding);


CCResult_t CCAesCryptBlock(CCAesContextPtr _Nonnull context,
                           const uint8_t * _Nonnull in,
                           uint8_t * _Nonnull out,
                           size_t blockCount);

CCResult_t CCAesCrypt(CCAesContextPtr _Nonnull context,
                      const uint8_t * _Nonnull in,
                      const size_t inSize,
                      uint8_t * _Nonnull out,
                      const size_t outSize,
                      size_t * _Nonnull outMoved);






//#ifdef  __cplusplus
//}
//#endif

#endif /* !HEADER_AES_H */
