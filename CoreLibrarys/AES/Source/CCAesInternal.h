
#ifndef CCAesInternal_h
#define CCAesInternal_h

#include "CCAes.h"

void AES_encrypt(const unsigned char *in, unsigned char *out,
                 const CCAesContext_s *key);
void AES_decrypt(const unsigned char *in, unsigned char *out,
                 const CCAesContext_s *key);

#endif /* CCAesInternal_h */
