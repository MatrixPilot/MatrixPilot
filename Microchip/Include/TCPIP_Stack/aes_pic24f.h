#include "AESConfig.h"

#if defined(AES_KEY_128)
    #define AESCreateSessionKey(k,s,ks)     AES128CreateSessionKey(k,s)
    #define AESECBEncryptBlock(p,s,c,ks)    AES128EncryptBlock(p,s,c)
    #define AESECBDecryptBlock(c,s,p,ks)    AES128DecryptBlock(c,s,p)
#elif defined(AES_KEY_192)
    #define AESCreateSessionKey(k,s,ks)     AES192CreateSessionKey(k,s)     
    #define AESECBEncryptBlock(p,s,c,ks)    AES192EncryptBlock(p,s,c)
    #define AESECBDecryptBlock(c,s,p,ks)    AES192DecryptBlock(c,s,p)
#elif defined(AES_KEY_256)
    #define AESCreateSessionKey(k,s,ks)     AES256CreateSessionKey(k,s)
    #define AESECBEncryptBlock(p,s,c,ks)    AES256EncryptBlock(p,s,c)
    #define AESECBDecryptBlock(c,s,p,ks)    AES256DecryptBlock(c,s,p)
#else
    #if !defined(AES_KEY_DYNAMIC)
        #warning "No key selected.  Defaulting to dynamic key size.  This may have size and speed impacts."
    #endif

    #define AESCreateSessionKey(k,s,ks)                                         \
            switch(ks){                                                         \
                case AES_KEY_TYPE_128_BIT:AES128CreateSessionKey(k,s);break;    \
                case AES_KEY_TYPE_192_BIT:AES192CreateSessionKey(k,s);break;    \
                case AES_KEY_TYPE_256_BIT:AES256CreateSessionKey(k,s);break;    \
                default:break;                                                  \
            }

    #define AESECBEncryptBlock(p,s,c,ks)                                        \
            switch(ks){                                                         \
                case AES_KEY_TYPE_128_BIT:AES128EncryptBlock(p,s,c);break;      \
                case AES_KEY_TYPE_192_BIT:AES192EncryptBlock(p,s,c);break;      \
                case AES_KEY_TYPE_256_BIT:AES256EncryptBlock(p,s,c);break;      \
                default:break;                                                  \
            }
    #define AESECBDecryptBlock(c,s,p,ks)                                        \
            switch(ks){                                                         \
                case AES_KEY_TYPE_128_BIT:AES128DecryptBlock(c,s,p);break;      \
                case AES_KEY_TYPE_192_BIT:AES192DecryptBlock(c,s,p);break;      \
                case AES_KEY_TYPE_256_BIT:AES256DecryptBlock(c,s,p);break;      \
                default:break;                                                  \
            }
#endif
