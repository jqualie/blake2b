// blake2b.c
// A simple BLAKE2b Reference Implementation.

#include "blake2b.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

// Cyclic right rotation.

#ifndef ROTR64
#define ROTR64(x, y) (((x) >> (y)) ^ ((x) << (64 - (y))))
#endif

// Little-endian byte access.

#define B2B_GET64(p)                           \
    (((uint64_t)((uint8_t *)(p))[0]) ^         \
     (((uint64_t)((uint8_t *)(p))[1]) << 8) ^  \
     (((uint64_t)((uint8_t *)(p))[2]) << 16) ^ \
     (((uint64_t)((uint8_t *)(p))[3]) << 24) ^ \
     (((uint64_t)((uint8_t *)(p))[4]) << 32) ^ \
     (((uint64_t)((uint8_t *)(p))[5]) << 40) ^ \
     (((uint64_t)((uint8_t *)(p))[6]) << 48) ^ \
     (((uint64_t)((uint8_t *)(p))[7]) << 56))

// G Mixing function.

#define B2B_G(a, b, c, d, x, y)         \
    {                                   \
        v[a] = v[a] + v[b] + x;         \
        v[d] = ROTR64(v[d] ^ v[a], 32); \
        v[c] = v[c] + v[d];             \
        v[b] = ROTR64(v[b] ^ v[c], 24); \
        v[a] = v[a] + v[b] + y;         \
        v[d] = ROTR64(v[d] ^ v[a], 16); \
        v[c] = v[c] + v[d];             \
        v[b] = ROTR64(v[b] ^ v[c], 63); \
    }

// Initialization Vector.

static const uint64_t blake2b_iv[8] = {
    0x6A09E667F3BCC908, 0xBB67AE8584CAA73B,
    0x3C6EF372FE94F82B, 0xA54FF53A5F1D36F1,
    0x510E527FADE682D1, 0x9B05688C2B3E6C1F,
    0x1F83D9ABFB41BD6B, 0x5BE0CD19137E2179};

// Compression function. "last" flag indicates last block.

static void blake2b_compress(blake2b_ctx *ctx, int last)
{
    const uint8_t sigma[12][16] = {
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
        {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3},
        {11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4},
        {7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8},
        {9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13},
        {2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9},
        {12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11},
        {13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10},
        {6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5},
        {10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13, 0},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
        {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3}};
    int i;
    uint64_t v[16], m[16];

    for (i = 0; i < 8; i++)
    { // init work variables
        v[i] = ctx->h[i];
        v[i + 8] = blake2b_iv[i];
    }

    v[12] ^= ctx->t[0]; // low 64 bits of offset
    v[13] ^= ctx->t[1]; // high 64 bits
    if (last)           // last block flag set ?
        v[14] = ~v[14];

    for (i = 0; i < 16; i++) // get little-endian words
        m[i] = B2B_GET64(&ctx->b[8 * i]);

    for (i = 0; i < 12; i++)
    { 
        //printf("v[%2d] =\n", i);
        //printf("        %0llx %0llx %0llx %0llx\n", v[0], v[1], v[2], v[3]);
        //printf("        %0llx %0llx %0llx %0llx\n", v[4], v[5], v[6], v[7]);
        //printf("        %0llx %0llx %0llx %0llx\n", v[8], v[9], v[10], v[11]);
        //printf("        %0llx %0llx %0llx %0llx\n", v[12], v[13], v[14], v[15]);
        //printf("\n");

        // twelve rounds
        B2B_G(0, 4, 8, 12, m[sigma[i][0]], m[sigma[i][1]]);
        B2B_G(1, 5, 9, 13, m[sigma[i][2]], m[sigma[i][3]]);
        B2B_G(2, 6, 10, 14, m[sigma[i][4]], m[sigma[i][5]]);
        B2B_G(3, 7, 11, 15, m[sigma[i][6]], m[sigma[i][7]]);
        B2B_G(0, 5, 10, 15, m[sigma[i][8]], m[sigma[i][9]]);
        B2B_G(1, 6, 11, 12, m[sigma[i][10]], m[sigma[i][11]]);
        B2B_G(2, 7, 8, 13, m[sigma[i][12]], m[sigma[i][13]]);
        B2B_G(3, 4, 9, 14, m[sigma[i][14]], m[sigma[i][15]]);
    }
 
    //printf("v[%2d] =\n", i);
    //printf("        %0llx %0llx %0llx %0llx\n", v[0], v[1], v[2], v[3]);
    //printf("        %0llx %0llx %0llx %0llx\n", v[4], v[5], v[6], v[7]);
    //printf("        %0llx %0llx %0llx %0llx\n", v[8], v[9], v[10], v[11]);
    //printf("        %0llx %0llx %0llx %0llx\n", v[12], v[13], v[14], v[15]);
    //printf("\n");

    for (i = 0; i < 8; ++i)
        ctx->h[i] ^= v[i] ^ v[i + 8];
}

// Initialize the hashing context "ctx" with optional key "key".
//      1 <= outlen <= 64 gives the digest size in bytes.
//      Secret key (also <= 64 bytes) is optional (keylen = 0).

int blake2b_init(blake2b_ctx *ctx, size_t outlen,
                 const void *key, size_t keylen) // (keylen=0: no key)
{
    size_t i;

    if (outlen == 0 || outlen > 64 || keylen > 64)
        return -1; // illegal parameters

    for (i = 0; i < 8; i++) // state, "param block"
        ctx->h[i] = blake2b_iv[i];
    ctx->h[0] ^= 0x01010000 ^ (keylen << 8) ^ outlen;

    ctx->t[0] = 0; // input count low word
    ctx->t[1] = 0; // input count high word
    ctx->c = 0;    // pointer within buffer
    ctx->outlen = outlen;

    for (i = keylen; i < 128; i++) // zero input block
        ctx->b[i] = 0;
    if (keylen > 0)
    {
        blake2b_update(ctx, key, keylen);
        ctx->c = 128; // at the end
    }

    return 0;
}

// Add "inlen" bytes from "in" into the hash.

void blake2b_update(blake2b_ctx *ctx,
                    const void *in, size_t inlen) // data bytes
{
    size_t i;

    for (i = 0; i < inlen; i++)
    {
        if (ctx->c == 128)
        {                             // buffer full ?
            ctx->t[0] += ctx->c;      // add counters
            if (ctx->t[0] < ctx->c)   // carry overflow ?
                ctx->t[1]++;          // high word
            blake2b_compress(ctx, 0); // compress (not last)
            ctx->c = 0;               // counter to zero
        }
        ctx->b[ctx->c++] = ((const uint8_t *)in)[i];
    }
}

// Generate the message digest (size given in init).
//      Result placed in "out".

void blake2b_final(blake2b_ctx *ctx, void *out)
{
    size_t i;

    ctx->t[0] += ctx->c;    // mark last block offset
    if (ctx->t[0] < ctx->c) // carry overflow
        ctx->t[1]++;        // high word

    while (ctx->c < 128) // fill up with zeros
        ctx->b[ctx->c++] = 0;
    blake2b_compress(ctx, 1); // final block flag = 1

    // little endian convert and store
    for (i = 0; i < ctx->outlen; i++)
    {
        ((uint8_t *)out)[i] =
            (ctx->h[i >> 3] >> (8 * (i & 7))) & 0xFF;
    }
}

// Convenience function for all-in-one computation.

int blake2b(void *out, size_t outlen,
            const void *key, size_t keylen,
            const void *in, size_t inlen)
{
    blake2b_ctx ctx;

    if (blake2b_init(&ctx, outlen, key, keylen))
        return -1;
    blake2b_update(&ctx, in, inlen);
    blake2b_final(&ctx, out);

    return 0;
}

/*
Appendix A.  Example of BLAKE2b Computation

   We compute the unkeyed hash of three ASCII bytes "abc" with
   BLAKE2b-512 and show internal values during computation.

          m[16] = 0000000000636261 0000000000000000 0000000000000000
                  0000000000000000 0000000000000000 0000000000000000
                  0000000000000000 0000000000000000 0000000000000000
                  0000000000000000 0000000000000000 0000000000000000
                  0000000000000000 0000000000000000 0000000000000000
                  0000000000000000

   (i= 0) v[16] = 6A09E667F2BDC948 BB67AE8584CAA73B 3C6EF372FE94F82B
                  A54FF53A5F1D36F1 510E527FADE682D1 9B05688C2B3E6C1F
                  1F83D9ABFB41BD6B 5BE0CD19137E2179 6A09E667F3BCC908
                  BB67AE8584CAA73B 3C6EF372FE94F82B A54FF53A5F1D36F1
                  510E527FADE682D2 9B05688C2B3E6C1F E07C265404BE4294
                  5BE0CD19137E2179

   (i= 1) v[16] = 86B7C1568029BB79 C12CBCC809FF59F3 C6A5214CC0EACA8E
                  0C87CD524C14CC5D 44EE6039BD86A9F7 A447C850AA694A7E
                  DE080F1BB1C0F84B 595CB8A9A1ACA66C BEC3AE837EAC4887
                  6267FC79DF9D6AD1 FA87B01273FA6DBE 521A715C63E08D8A
                  E02D0975B8D37A83 1C7B754F08B7D193 8F885A76B6E578FE
                  2318A24E2140FC64

   (i= 2) v[16] = 53281E83806010F2 3594B403F81B4393 8CD63C7462DE0DFF
                  85F693F3DA53F974 BAABDBB2F386D9AE CA5425AEC65A10A8
                  C6A22E2FF0F7AA48 C6A56A51CB89C595 224E6A3369224F96
                  500E125E58A92923 E9E4AD0D0E1A0D48 85DF9DC143C59A74
                  92A3AAAA6D952B7F C5FDF71090FAE853 2A8A40F15A462DD0
                  572D17EFFDD37358

   (i= 3) v[16] = 60ED96AA7AD41725 E46A743C71800B9D 1A04B543A01F156B
                  A2F8716E775C4877 DA0A61BCDE4267EA B1DD230754D7BDEE
                  25A1422779E06D14 E6823AE4C3FF58A5 A1677E19F37FD5DA
                  22BDCE6976B08C51 F1DE8696BEC11BF1 A0EBD586A4A1D2C8
                  C804EBAB11C99FA9 8E0CEC959C715793 7C45557FAE0D4D89
                  716343F52FDD265E

   (i= 4) v[16] = BB2A77D3A8382351 45EB47971F23B103 98BE297F6E45C684
                  A36077DEE3370B89 8A03C4CB7E97590A 24192E49EBF54EA0
                  4F82C9401CB32D7A 8CCD013726420DC4 A9C9A8F17B1FC614
                  55908187977514A0 5B44273E66B19D27 B6D5C9FCA2579327
                  086092CFB858437E 5C4BE2156DBEECF9 2EFEDE99ED4EFF16
                  3E7B5F234CD1F804

   (i= 5) v[16] = C79C15B3D423B099 2DA2224E8DA97556 77D2B26DF1C45C55
                  8934EB09A3456052 0F6D9EEED157DA2A 6FE66467AF88C0A9
                  4EB0B76284C7AAFB 299C8E725D954697 B2240B59E6D567D3
                  2643C2370E49EBFD 79E02EEF20CDB1AE 64B3EED7BB602F39
                  B97D2D439E4DF63D C718E755294C9111 1F0893F2772BB373
                  1205EA4A7859807D

   (i= 6) v[16] = E58F97D6385BAEE4 7640AA9764DA137A DEB4C7C23EFE287E
                  70F6F41C8783C9F6 7127CD48C76A7708 9E472AF0BE3DB3F6
                  0F244C62DDF71788 219828AA83880842 41CCA9073C8C4D0D
                  5C7912BC10DF3B4B A2C3ABBD37510EE2 CB5668CC2A9F7859
                  8733794F07AC1500 C67A6BE42335AA6F ACB22B28681E4C82
                  DB2161604CBC9828

   (i= 7) v[16] = 6E2D286EEADEDC81 BCF02C0787E86358 57D56A56DD015EDF
                  55D899D40A5D0D0A 819415B56220C459 B63C479A6A769F02
                  258E55E0EC1F362A 3A3B4EC60E19DFDC 04D769B3FCB048DB
                  B78A9A33E9BFF4DD 5777272AE1E930C0 5A387849E578DBF6
                  92AAC307CF2C0AFC 30AACCC4F06DAFAA 483893CC094F8863
                  E03C6CC89C26BF92

   (i= 8) v[16] = FFC83ECE76024D01 1BE7BFFB8C5CC5F9 A35A18CBAC4C65B7
                  B7C2C7E6D88C285F 81937DA314A50838 E1179523A2541963
                  3A1FAD7106232B8F 1C7EDE92AB8B9C46 A3C2D35E4F685C10
                  A53D3F73AA619624 30BBCC0285A22F65 BCEFBB6A81539E5D
                  3841DEF6F4C9848A 98662C85FBA726D4 7762439BD5A851BD
                  B0B9F0D443D1A889

   (i= 9) v[16] = 753A70A1E8FAEADD 6B0D43CA2C25D629 F8343BA8B94F8C0B
                  BC7D062B0DB5CF35 58540EE1B1AEBC47 63C5B9B80D294CB9
                  490870ECAD27DEBD B2A90DDF667287FE 316CC9EBEEFAD8FC
                  4A466BCD021526A4 5DA7F7638CEC5669 D9C8826727D306FC
                  88ED6C4F3BD7A537 19AE688DDF67F026 4D8707AAB40F7E6D
                  FD3F572687FEA4F1

   (i=10) v[16] = E630C747CCD59C4F BC713D41127571CA 46DB183025025078
                  6727E81260610140 2D04185EAC2A8CBA 5F311B88904056EC
                  40BD313009201AAB 0099D4F82A2A1EAB 6DD4FBC1DE60165D
                  B3B0B51DE3C86270 900AEE2F233B08E5 A07199D87AD058D8
                  2C6B25593D717852 37E8CA471BEAA5F8 2CFC1BAC10EF4457
                  01369EC18746E775

   (i=11) v[16] = E801F73B9768C760 35C6D22320BE511D 306F27584F65495E
                  B51776ADF569A77B F4F1BE86690B3C34 3CC88735D1475E4B
                  5DAC67921FF76949 1CDB9D31AD70CC4E 35BA354A9C7DF448
                  4929CBE45679D73E 733D1A17248F39DB 92D57B736F5F170A
                  61B5C0A41D491399 B5C333457E12844A BD696BE010D0D889
                  02231E1A917FE0BD

   (i=12) v[16] = 12EF8A641EC4F6D6 BCED5DE977C9FAF5 733CA476C5148639
                  97DF596B0610F6FC F42C16519AD5AFA7 AA5AC1888E10467E
                  217D930AA51787F3 906A6FF19E573942 75AB709BD3DCBF24
                  EE7CE1F345947AA4 F8960D6C2FAF5F5E E332538A36B6D246
                  885BEF040EF6AA0B A4939A417BFB78A3 646CBB7AF6DCE980
                  E813A23C60AF3B82

           h[8] = 0D4D1C983FA580BA E9F6129FB697276A B7C45A68142F214C
                  D1A2FFDB6FBB124B 2D79AB2A39C5877D 95CC3345DED552C2
                  5A92F1DBA88AD318 239900D4ED8623B9

   BLAKE2b-512("abc") = BA 80 A5 3F 98 1C 4D 0D 6A 27 97 B6 9F 12 F6 E9
                        4C 21 2F 14 68 5A C4 B7 4B 12 BB 6F DB FF A2 D1
                        7D 87 C5 39 2A AB 79 2D C2 52 D5 DE 45 33 CC 95
                        18 D3 8A A8 DB F1 92 5A B9 23 86 ED D4 00 99 23

*/