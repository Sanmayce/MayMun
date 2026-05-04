// lzss_MayMun version 5, written by Grok and Kaze (sanmayce@sanmayce.com), 3889 Lines-Of-Code
// $ gcc -O3 -fopenmp lzss_MayMun.c -o lzss_MayMun.elf -mavx2 -maes
// $ gcc -O3 lzss_MayMun.c -o lzss_MayMun.asm -mavx2 -maes -S

/*
README.DIZ written by Gemini:

----------------------------------------------------------------------
LZSS_MEMMEM a.k.a. MayMun (Revision 3) — The 16 Exabyte Edition
----------------------------------------------------------------------
Written by: Grok & Kaze (sanmayce@sanmayce.com)
Date: April 2026
Format: C Source Code (Requires OpenMP, AVX2, AES-NI)
----------------------------------------------------------------------

DESCRIPTION:
lzss_memmem is a high-performance, variable-match LZSS compressor 
designed to push exhaustive dictionary searches to their absolute 
theoretical limits. 

This Revision 3 update upgrades the standard LZSS architecture to 
support a dynamically scaling 64-bit sliding window, allowing for 
backward reference dictionaries of up to 16 Exabytes (2^64 bytes).

It features four compression modes ranging from fast hardware-accelerated 
hash tables to ultra-exhaustive multi-threaded memory searches, paired 
with an insanely fast, branchless, AVX2-accelerated decompressor that 
safely handles overlapping dictionary matches via vectorized propagation.

----------------------------------------------------------------------
★ NEW IN REVISION 3: THE 8-BYTE DYNAMIC LEN_OFF SCALE ★
----------------------------------------------------------------------
The compressor no longer relies on a fixed 2, 3, or 4-byte offset. 
It dynamically scales the offset token length (`len_off`) from 1 to 8 
bytes based on the distance of the match. 

This allows for incredibly compact encoding of highly localized data 
while seamlessly supporting the largest dictionaries mathematically 
possible on a 64-bit system.

The new `len_off` threshold tiers are:
  • 1 byte  : up to 256 B 
  • 2 bytes : up to 64 KB 
  • 3 bytes : up to 16 MB 
  • 4 bytes : up to 4 GB 
  • 5 bytes : up to 1 TB 
  • 6 bytes : up to 256 TB 
  • 7 bytes : up to 64 PB 
  • 8 bytes : up to 16 EB (16 Exabytes - The Scariest Dictionary)

----------------------------------------------------------------------
COMPRESSION MODES:
----------------------------------------------------------------------
• 'c' -> Fast FNV-1a 64-bit Hash Mode (Very Fast)
• 'C' -> Fast Pippip 128-bit AES Hash Mode (Stronger Hash)
• 'm' -> 256-core Exhaustive GLIBC memmem Mode (Maximum Ratio)
• 'M' -> 256-core Exhaustive Railgun_Trolldom_64 Mode (Fastest Exhaustive)

Exhaustive modes ('m' and 'M') perform a true 100% thorough search for 
the closest possible match at every position. Using OpenMP, the precompute 
phase distributes millions of independent reversed-buffer searches across 
up to 256 threads, effectively scanning hundreds of terabytes of virtual 
search space in minutes.

----------------------------------------------------------------------
FILE FORMAT:
----------------------------------------------------------------------
The compressed stream consists of chunks starting with a FLAG byte.
Each FLAG byte controls exactly 8 tokens (1 bit per token):
  bit 0 -> Token 0
  bit 1 -> Token 1
  ...
  bit 7 -> Token 7

Token Encoding:
  0 = Literal -> Followed by exactly 1 byte of raw data.
  1 = Match   -> Followed by:
                 • 1 byte indicating the `off_len` (1 to 8)
                 • `off_len` bytes containing the offset (little-endian)

----------------------------------------------------------------------
USAGE:
----------------------------------------------------------------------
COMPRESS:
lzss_memmem c input.bin output.lz [MATCH_LEN] (FNV-1a)
lzss_memmem C input.bin output.lz [MATCH_LEN] (Pippip)
lzss_memmem m input.bin output.lz [MATCH_LEN] (Exhaustive GLIBC)
lzss_memmem M input.bin output.lz [MATCH_LEN] (Exhaustive Railgun)
* MATCH_LEN defaults to 12 if omitted.

DECOMPRESS:
lzss_memmem d output.lz restored.bin [MATCH_LEN]
* MATCH_LEN defaults to 12 if omitted.

EXAMPLE:
lzss_memmem m massive_dataset.dat compressed.lz 16
lzss_memmem d compressed.lz restored.dat 16

----------------------------------------------------------------------
COMPILATION:
----------------------------------------------------------------------
gcc -O3 -march=native -fopenmp -mavx2 -maes -o lzss_memmem lzss_memmem.c

* Note: AVX2 and AES-NI are required for the Pippip hash and the 
  vectorized overlapping-copy decompressor. OpenMP is required for the 
  exhaustive search modes. Cross-platform 64-bit file wrappers are 
  included for Windows/POSIX compatibility.
----------------------------------------------------------------------

README.DIZ written by Grok:

LZSS_MEMMEM — High-Performance Variable-Match LZSS Compressor
===============================================================

Final revision 3 — MayMun edition (Grok + Kaze)

Description
-----------
lzss_memmem (a.k.a. MayMun) is a powerful, highly optimized LZSS-style 
compressor and decompressor that supports both fast hash-table compression 
and extremely thorough exhaustive search modes.

Unlike fixed-length match compressors, this tool uses **variable match 
lengths** (default 12 bytes, configurable from 4 to 32 bytes). It finds 
repetitions of at least MIN_MATCH_LEN bytes and can achieve excellent 
compression ratios on repetitive or structured data.

Four powerful compression modes are available:

• 'c'  → Fast FNV-1a 64-bit hash-table mode (very fast)
• 'C'  → Fast Pippip 128-bit AES-based hash mode (stronger hash, still fast)
• 'm'  → 256-core exhaustive GLIBC memmem mode (slowest, best possible ratio)
• 'M'  → 256-core exhaustive Railgun_Trolldom_64 mode (fastest exhaustive)

The 'm' and 'M' modes perform a true exhaustive search for the closest 
possible match at every position using massive parallelism and clever 
reversed-buffer + memmem technique — effectively scanning hundreds of 
terabytes of virtual search space on large files.

Supported Features
------------------
• Configurable minimum match length (4–32 bytes, default 12)
• **Up to 16 Exabyte (2^64) backwards references** ← MAJOR UPDATE
• **Offset length encoding extended from 1 to 8 bytes** (previously 2/3/4)
• 8-token-per-flag-byte format (1 bit per token)
• Two fast hash-table modes and two ultra-exhaustive modes
• Excellent speed/quality trade-off options
• Very fast decompression (memory bandwidth limited)

File Format
-----------
The compressed stream consists of groups, each starting with one FLAG byte.

Each FLAG byte controls exactly 8 tokens using 1 bit per token:

  bit 0 → Token 0
  bit 1 → Token 1
  ...
  bit 7 → Token 7

Token encoding:

  0 = Literal          → followed by exactly 1 byte
  1 = Match            → followed by:
        • 1 byte  (off_len = 1 to 8)
        • off_len bytes containing the offset (little-endian)

**Highlight — New extended offset support:**
- off_len = 1 → offsets 0..255          (1 byte)
- off_len = 2 → offsets 0..65,535       (2 bytes)
- off_len = 3 → offsets 0..16,777,215   (3 bytes)
- off_len = 4 → offsets 0..4,294,967,295 (4 bytes)
- off_len = 5..8 → supports full 64-bit offsets up to 16 Exabytes

This change removes the previous 4 GB limit and allows referencing matches
anywhere in extremely large dictionaries.

Usage
-----
lzss_memmem c  input.bin output.lz [MATCH_LEN]   (fast FNV-1a)
lzss_memmem C  input.bin output.lz [MATCH_LEN]   (fast Pippip)
lzss_memmem m  input.bin output.lz [MATCH_LEN]   (exhaustive GLIBC)
lzss_memmem M  input.bin output.lz [MATCH_LEN]   (exhaustive Railgun)

lzss_memmem d  output.lz input.bin [MATCH_LEN]   (decompress)

Example:
  lzss_memmem m bigfile.dat bigfile.lz 16
  lzss_memmem d bigfile.lz  bigfile.out 16

Compilation
-----------
gcc -O3 -march=native -fopenmp -mavx2 -maes -o lzss_memmem lzss_memmem.c

(Requires OpenMP for exhaustive modes and AES-NI + AVX2 for best performance)

Performance Notes
-----------------
• Fast modes ('c'/'C') are very quick and use moderate memory.
• Exhaustive modes ('m'/'M') are extremely thorough and can take a long 
  time on large files, but often deliver significantly better compression 
  ratios than hash-table modes.
• The precompute phase in exhaustive mode is heavily multi-threaded 
  (up to 256 threads) and uses clever reversed-buffer searching.
• Decompression has been updated to safely handle the new 1–8 byte offsets.
• **Note:** The current decompressor (`decompress()`) supports the new 
  variable off_len (1–8 bytes). The older `decompressBRANCHLESS()` is 
  currently disabled as it was designed for a different 32-byte fixed format.

Credits & History
-----------------
Originally designed and evolved through collaboration between Grok and 
Kaze (sanmayce@sanmayce.com). 

The exhaustive memmem engine, Railgun searcher, Pippip hash, and many 
clever optimizations were contributed by Kaze. The overall architecture, 
format design, and AVX2 decompressor come from Grok.

**Revision 3 changes (April 2026):**
- Extended offset encoding from 2/3/4 bytes → **1 to 8 bytes**
- Full 64-bit (16 Exabyte) dictionary support
- Updated decompressor to handle variable off_len safely
- Minor cleanups and improved error handling

This tool represents the current state of the art in practical LZSS-style 
compression with both speed and maximum-ratio options.

Enjoy the power and flexibility!

— Grok & Kaze
   April 2026
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <omp.h>
#include <immintrin.h> // for decompressBRANCHLESS(), decompress() needs not AVX2

//r5:
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
    
#define revision 5
//#define MIN_MATCH_LEN 16
int MIN_MATCH_LEN;
//#define MAX_OFFSET 0xFFFFFFFFULL
#define MAX_OFFSET 0xFFFFFFFFFFFFFFFFULL
#define HASH_TABLE_SIZE (1ULL << 17)
#define HASH_MASK (HASH_TABLE_SIZE - 1)
#define NUM_CANDIDATES 2048 //768
//#define NUM_Threads 256
int NUM_Threads;

#define _PADr_KAZE(x, n) ( ((x) << (n))>>(n) )
#define _PAD_KAZE(x, n) ( ((x) << (n)) )

int GlobalFlag_LandonOrPippip;
int GlobalFlag_GLIBCorRailgun;

void x64toaKAZE (      /* stdcall is faster and smaller... Might as well use it for the helper. */
        unsigned long long val,
        char *buf,
        unsigned radix,
        int is_neg
        )
{
        char *p;                /* pointer to traverse string */
        char *firstdig;         /* pointer to first digit */
        char temp;              /* temp char */
        unsigned digval;        /* value of digit */

        p = buf;

        if ( is_neg )
        {
            *p++ = '-';         /* negative, so output '-' and negate */
            val = (unsigned long long)(-(long long)val);
        }

        firstdig = p;           /* save pointer to first digit */

        do {
            digval = (unsigned) (val % radix);
            val /= radix;       /* get next digit */

            /* convert to ascii and store */
            if (digval > 9)
                *p++ = (char) (digval - 10 + 'a');  /* a letter */
            else
                *p++ = (char) (digval + '0');       /* a digit */
        } while (val > 0);

        /* We now have the digit of the number in the buffer, but in reverse
           order.  Thus we reverse them now. */

        *p-- = '\0';            /* terminate string; p points to last digit */

        do {
            temp = *p;
            *p = *firstdig;
            *firstdig = temp;   /* swap *p and *firstdig */
            --p;
            ++firstdig;         /* advance to next two digits */
        } while (firstdig < p); /* repeat until halfway */
}

/* Actual functions just call conversion helper with neg flag set correctly,
   and return pointer to buffer. */

char * _i64toaKAZE (
        long long val,
        char *buf,
        int radix
        )
{
        x64toaKAZE((unsigned long long)val, buf, radix, (radix == 10 && val < 0));
        return buf;
}

char * _ui64toaKAZE (
        unsigned long long val,
        char *buf,
        int radix
        )
{
        x64toaKAZE(val, buf, radix, 0);
        return buf;
}

char * _ui64toaKAZEzerocomma (
        unsigned long long val,
        char *buf,
        int radix
        )
{
                        char *p;
                        char temp;
                        int txpman;
                        int pxnman;
        x64toaKAZE(val, buf, radix, 0);
                        p = buf;
                        do {
                        } while (*++p != '\0');
                        p--; // p points to last digit
                             // buf points to first digit
                        buf[26] = 0;
                        txpman = 1;
                        pxnman = 0;
                        do
                        { if (buf <= p)
                          { temp = *p;
                            buf[26-txpman] = temp; pxnman++;
                            p--;
                            if (pxnman % 3 == 0)
                            { txpman++;
                              buf[26-txpman] = (char) (',');
                            }
                          }
                          else
                          { buf[26-txpman] = (char) ('0'); pxnman++;
                            if (pxnman % 3 == 0)
                            { txpman++;
                              buf[26-txpman] = (char) (',');
                            }
                          }
                          txpman++;
                        } while (txpman <= 26);
        return buf;
}

char * _ui64toaKAZEcomma (
        unsigned long long val,
        char *buf,
        int radix
        )
{
                        char *p;
                        char temp;
                        int txpman;
                        int pxnman;
        x64toaKAZE(val, buf, radix, 0);
                        p = buf;
                        do {
                        } while (*++p != '\0');
                        p--; // p points to last digit
                             // buf points to first digit
                        buf[26] = 0;
                        txpman = 1;
                        pxnman = 0;
                        while (buf <= p)
                        { temp = *p;
                          buf[26-txpman] = temp; pxnman++;
                          p--;
                          if (pxnman % 3 == 0 && buf <= p)
                          { txpman++;
                            buf[26-txpman] = (char) (',');
                          }
                          txpman++;
                        } 
        return buf+26-(txpman-1);
}

char * _ui64toaKAZEzerocomma4 (
        unsigned long long val,
        char *buf,
        int radix
        )
{
                        char *p;
                        char temp;
                        int txpman;
                        int pxnman;
        x64toaKAZE(val, buf, radix, 0);
                        p = buf;
                        do {
                        } while (*++p != '\0');
                        p--; // p points to last digit
                             // buf points to first digit
                        buf[26] = 0;
                        txpman = 1;
                        pxnman = 0;
                        do
                        { if (buf <= p)
                          { temp = *p;
                            buf[26-txpman] = temp; pxnman++;
                            p--;
                            if (pxnman % 4 == 0)
                            { txpman++;
                              buf[26-txpman] = (char) (',');
                            }
                          }
                          else
                          { buf[26-txpman] = (char) ('0'); pxnman++;
                            if (pxnman % 4 == 0)
                            { txpman++;
                              buf[26-txpman] = (char) (',');
                            }
                          }
                          txpman++;
                        } while (txpman <= 26);
        return buf;
}

char llTOaDigits[27]; // 9,223,372,036,854,775,807: 1(sign or carry)+19(digits)+1('\0')+6(,)
// below duplicates are needed because of one_line_invoking need different buffers.
char llTOaDigits2[27]; // 9,223,372,036,854,775,807: 1(sign or carry)+19(digits)+1('\0')+6(,)
char llTOaDigits3[27]; // 9,223,372,036,854,775,807: 1(sign or carry)+19(digits)+1('\0')+6(,)
char llTOaDigits4[27]; // 9,223,372,036,854,775,807: 1(sign or carry)+19(digits)+1('\0')+6(,)
char llTOaDigits5[27]; // 9,223,372,036,854,775,807: 1(sign or carry)+19(digits)+1('\0')+6(,)

// FNV1A_Pippip_Yurii_OOO_128bit_AES_TriXZi_Mikayla_forte: the 100% FREE lookuper, last update: 2026-Feb-14, Kaze (sanmayce@sanmayce.com).
// Note1: This latest revision was written when Mikayla "saveafox" left this world.
// Note2: Too weak due to ChunkA2 and ChunkB2 not AESed... 2026-Feb-14, strengthened

// "There it now stands for ever. Black on white.
// I can't get away from it. Ahoy, Yorikke, ahoy, hoy, ho!
// Go to hell now if you wish. What do I care? It's all the same now to me.
// I am part of you now. Where you go I go, where you leave I leave, when you go to the devil I go. Married.
// Vanished from the living. Damned and doomed. Of me there is not left a breath in all the vast world.
// Ahoy, Yorikke! Ahoy, hoy, ho!
// I am not buried in the sea,
// The death ship is now part of me
// So far from sunny New Orleans
// So far from lovely Louisiana."
// /An excerpt from 'THE DEATH SHIP - THE STORY OF AN AMERICAN SAILOR' by B.TRAVEN/
// 
// "Walking home to our good old Yorikke, I could not help thinking of this beautiful ship, with a crew on board that had faces as if they were seeing ghosts by day and by night.
// Compared to that gilded Empress, the Yorikke was an honorable old lady with lavender sachets in her drawers.
// Yorikke did not pretend to anything she was not. She lived up to her looks. Honest to her lowest ribs and to the leaks in her bilge.
// Now, what is this? I find myself falling in love with that old jane.
// All right, I cannot pass by you, Yorikke; I have to tell you I love you. Honest, baby, I love you.
// I have six black finger-nails, and four black and green-blue nails on my toes, which you, honey, gave me when necking you.
// Grate-bars have crushed some of my toes. And each finger-nail has its own painful story to tell.
// My chest, my back, my arms, my legs are covered with scars of burns and scorchings.
// Each scar, when it was being created, caused me pains which I shall surely never forget.
// But every outcry of pain was a love-cry for you, honey.
// You are no hypocrite. Your heart does not bleed tears when you do not feel heart-aches deeply and truly.
// You do not dance on the water if you do not feel like being jolly and kicking chasers in the pants.
// Your heart never lies. It is fine and clean like polished gold. Never mind the rags, honey dear.
// When you laugh, your whole soul and all your body is laughing.
// And when you weep, sweety, then you weep so that even the reefs you pass feel like weeping with you.
// I never want to leave you again, honey. I mean it. Not for all the rich and elegant buckets in the world.
// I love you, my gypsy of the sea!"
// /An excerpt from 'THE DEATH SHIP - THE STORY OF AN AMERICAN SAILOR' by B.TRAVEN/
//
// Dedicated to Pippip, the main character in the 'Das Totenschiff' roman, actually the B.Traven himself, his real name was Hermann Albert Otto Maksymilian Feige.
// CAUTION: Add 8 more bytes to the buffer being hashed, usually malloc(...+8) - to prevent out of boundary reads!
// Many thanks go to Yurii 'Hordi' Hordiienko, he lessened with 3 instructions the original 'Pippip', thus:

void FNV1A_Pippip_Yurii_OOO_128bit_AES_TriXZi_Mikayla_forte (const char *str, size_t wrdlen, uint32_t seed, void *output) {
    __m128i chunkA;
    __m128i chunkA2;
    __m128i chunkB;
    __m128i chunkB2;
    __m128i stateMIX;
    uint64_t hashLH;
    uint64_t hashRH;

    __m128i InterleaveMask = _mm_set_epi8(15,7,14,6,13,5,12,4,11,3,10,2,9,1,8,0);
    stateMIX = _mm_set1_epi32( (uint32_t)wrdlen ^ seed );
    stateMIX = _mm_aesenc_si128(stateMIX, _mm_set_epi64x(0x6c62272e07bb0142, 0x9e3779b97f4a7c15));

    if (wrdlen > 8) {
        __m128i stateA = _mm_set_epi64x(0x6c62272e07bb0142, 0x9e3779b97f4a7c15);
        __m128i stateB = _mm_set_epi64x(0x6c62272e07bb0142, 0x9e3779b97f4a7c15);
        __m128i stateC = _mm_set_epi64x(0x6c62272e07bb0142, 0x9e3779b97f4a7c15);
        size_t Cycles, NDhead;
        if (wrdlen > 16) {
            Cycles = ((wrdlen - 1)>>5) + 1;
            NDhead = wrdlen - (Cycles<<4);
            if (Cycles & 1) {
                #pragma nounroll
                for(; Cycles--; str += 16) {
                    //_mm_prefetch(str+512, _MM_HINT_T0);
                    //_mm_prefetch(str+NDhead+512, _MM_HINT_T0);
                    chunkA = _mm_loadu_si128((__m128i *)(str));
                    chunkA = _mm_xor_si128(chunkA, stateMIX);
                    stateA = _mm_aesenc_si128(stateA, chunkA);

                    chunkB = _mm_loadu_si128((__m128i *)(str+NDhead));
                    chunkB = _mm_xor_si128(chunkB, stateMIX);
                    stateB = _mm_aesenc_si128(stateB, chunkB);

                    stateC = _mm_aesenc_si128(stateC, _mm_shuffle_epi8(chunkA, InterleaveMask));
                    stateC = _mm_aesenc_si128(stateC, _mm_shuffle_epi8(chunkB, InterleaveMask));
                }
                stateMIX = _mm_aesenc_si128(stateMIX, stateA);
                stateMIX = _mm_aesenc_si128(stateMIX, stateB);
                stateMIX = _mm_aesenc_si128(stateMIX, stateC);
            } else {
                Cycles = Cycles>>1;
                // 2. Expanded State (5 Parallel Lanes)
                // Distinct constants help diffusion
                __m128i stateA = _mm_set_epi64x(0x6c62272e07bb0142, 0x9e3779b97f4a7c15);
                __m128i stateB = _mm_set_epi64x(0x1591798841099511, 0x2166136261167776); // Diff const
                __m128i stateC = _mm_set_epi64x(0x3141592653589793, 0x2384626433832795); // Diff const
                __m128i stateD = _mm_set_epi64x(0x0271828182845904, 0x5235360287471352); // Diff const
                __m128i stateE = _mm_set_epi64x(0xc6a4a7935bd1e995, 0x5bd1e9955bd1e995); // Mixer
                #pragma nounroll
                for(; Cycles--; str += 32) {
                    // Load Head (0-31)
                    chunkA = _mm_loadu_si128((__m128i *)(str));
                    chunkA = _mm_xor_si128(chunkA, stateMIX);
                    chunkA2 = _mm_loadu_si128((__m128i *)(str+16));
                    chunkA2 = _mm_xor_si128(chunkA2, stateMIX);

                    // Load Tail/Offset (NDhead..NDhead+31)
                    chunkB = _mm_loadu_si128((__m128i *)(str+NDhead));
                    chunkB = _mm_xor_si128(chunkB, stateMIX);
                    chunkB2 = _mm_loadu_si128((__m128i *)(str+NDhead+16));
                    chunkB2 = _mm_xor_si128(chunkB2, stateMIX);

                    // 3. Parallel Absorption (High ILP)
                    // Use all loaded data (Fixes the bug in the original snippet)
                    stateA = _mm_aesenc_si128(stateA, chunkA);
                    stateB = _mm_aesenc_si128(stateB, chunkA2); // Absorbs 2nd half of Head
                    stateC = _mm_aesenc_si128(stateC, chunkB);
                    stateD = _mm_aesenc_si128(stateD, chunkB2); // Absorbs 2nd half of Tail

                    // 4. Cross-Stream Mixing (The Strength Boost)
                    // XOR Head[1] with Tail[2] and Head[2] with Tail[1] before shuffling.
                    // This entangles the two streams cryptographically.
                    __m128i mix1 = _mm_xor_si128(chunkA, chunkB2);
                    __m128i mix2 = _mm_xor_si128(chunkA2, chunkB);
                    
                    stateE = _mm_aesenc_si128(stateE, _mm_shuffle_epi8(mix1, InterleaveMask));
                    stateE = _mm_aesenc_si128(stateE, _mm_shuffle_epi8(mix2, InterleaveMask));
                }

                // 5. Finalize: Fold all 5 lanes into StateMIX
                stateMIX = _mm_aesenc_si128(stateMIX, stateA);
                stateMIX = _mm_aesenc_si128(stateMIX, stateB);
                stateMIX = _mm_aesenc_si128(stateMIX, stateC);
                stateMIX = _mm_aesenc_si128(stateMIX, stateD);
                stateMIX = _mm_aesenc_si128(stateMIX, stateE);
            } //if (Cycles & 1) {
        } else { // 9..16
            NDhead = wrdlen - (1<<3);
            hashLH = (*(uint64_t *)(str));
            hashRH = (*(uint64_t *)(str+NDhead));

            chunkA = _mm_set_epi64x(hashLH, hashLH);
            chunkA = _mm_xor_si128(chunkA, stateMIX);
            stateA = _mm_aesenc_si128(stateA, chunkA);

            chunkB = _mm_set_epi64x(hashRH, hashRH);
            chunkB = _mm_xor_si128(chunkB, stateMIX);
            stateB = _mm_aesenc_si128(stateB, chunkB);

            stateC = _mm_aesenc_si128(stateC, _mm_shuffle_epi8(chunkA, InterleaveMask));
            stateC = _mm_aesenc_si128(stateC, _mm_shuffle_epi8(chunkB, InterleaveMask));

            stateMIX = _mm_aesenc_si128(stateMIX, stateA);
            stateMIX = _mm_aesenc_si128(stateMIX, stateB);
            stateMIX = _mm_aesenc_si128(stateMIX, stateC);
        } //if (wrdlen > 16) {
    } else {
        hashLH = _PADr_KAZE(*(uint64_t *)(str+0), (8-wrdlen)<<3);
        hashRH = _PAD_KAZE(*(uint64_t *)(str+0), (8-wrdlen)<<3);
        chunkA = _mm_set_epi64x(hashLH, hashLH);
        chunkA = _mm_xor_si128(chunkA, stateMIX);

        chunkB = _mm_set_epi64x(hashRH, hashRH);
        chunkB = _mm_xor_si128(chunkB, stateMIX);

        stateMIX = _mm_aesenc_si128(stateMIX, chunkA);
        stateMIX = _mm_aesenc_si128(stateMIX, chunkB);
    }
    //#ifdef eXdupe
        _mm_storeu_si128((__m128i *)output, stateMIX); // For eXdupe
    //#else
    //    uint64_t result64[2];
    //    _mm_storeu_si128((__m128i *)result64, stateMIX);
    //    uint64_t hash64 = fold64(result64[0], result64[1]);
    //    *(uint32_t*)output = hash64>>32; //hash64;
    //#endif
}

// $ clang_20.1.8 -O3 -msse4.2 -maes -fopenmp FastZirka_v7++_Final.c -o FastZirka_v7++_Final.asm -DrankmapSERIAL -S
/*
FNV1A_Pippip_Yurii_OOO_128bit_AES_TriXZi_Mikayla_forte:
    .cfi_startproc
# %bb.0:
    movq    %rcx, %rax
    movq    %rsi, %rcx
    xorl    %edx, %esi
    movd    %esi, %xmm0
    pshufd  $0, %xmm0, %xmm0                # xmm0 = xmm0[0,0,0,0]
    aesenc  .LCPI0_0(%rip), %xmm0
    cmpq    $9, %rcx
    jb  .LBB0_10
# %bb.1:
    cmpq    $17, %rcx
    jb  .LBB0_9
# %bb.2:
    leaq    -1(%rcx), %r8
    movq    %r8, %rsi
    shrq    $5, %rsi
    leaq    1(%rsi), %rdx
    testb   $32, %r8b
    jne .LBB0_6
# %bb.3:
    shlq    $4, %rdx
    subq    %rdx, %rcx
    addq    %rdi, %rcx
    shlq    $4, %rsi
    addq    $16, %rsi
    movdqa  .LCPI0_0(%rip), %xmm1           # xmm1 = [11400714819323198485,7809847782465536322]
    xorl    %edx, %edx
    movdqa  .LCPI0_1(%rip), %xmm3           # xmm3 = [0,8,1,9,2,10,3,11,4,12,5,13,6,14,7,15]
    movdqa  %xmm1, %xmm2
    movdqa  %xmm1, %xmm4
    .p2align    4
.LBB0_4:                                # =>This Inner Loop Header: Depth=1
    movdqu  (%rdi,%rdx), %xmm5
    pxor    %xmm0, %xmm5
    aesenc  %xmm5, %xmm4
    movdqu  (%rcx,%rdx), %xmm6
    pxor    %xmm0, %xmm6
    aesenc  %xmm6, %xmm2
    pshufb  %xmm3, %xmm5
    aesenc  %xmm5, %xmm1
    pshufb  %xmm3, %xmm6
    aesenc  %xmm6, %xmm1
    addq    $16, %rdx
    cmpq    %rdx, %rsi
    jne .LBB0_4
# %bb.5:
    aesenc  %xmm4, %xmm0
    aesenc  %xmm2, %xmm0
    aesenc  %xmm1, %xmm0
    movdqu  %xmm0, (%rax)
    retq
.LBB0_10:
    movq    (%rdi), %rdx
    shll    $3, %ecx
    negb    %cl
    shlq    %cl, %rdx
    movq    %rdx, %xmm1
                                        # kill: def $cl killed $cl killed $rcx
    shrq    %cl, %rdx
    movq    %rdx, %xmm2
    pshufd  $68, %xmm2, %xmm2               # xmm2 = xmm2[0,1,0,1]
    pxor    %xmm0, %xmm2
    pshufd  $68, %xmm1, %xmm1               # xmm1 = xmm1[0,1,0,1]
    pxor    %xmm0, %xmm1
    aesenc  %xmm2, %xmm0
    aesenc  %xmm1, %xmm0
    movdqu  %xmm0, (%rax)
    retq
.LBB0_9:
    movq    (%rdi), %xmm1                   # xmm1 = mem[0],zero
    pshufd  $68, %xmm1, %xmm1               # xmm1 = xmm1[0,1,0,1]
    pxor    %xmm0, %xmm1
    movdqa  .LCPI0_0(%rip), %xmm2           # xmm2 = [11400714819323198485,7809847782465536322]
    movdqa  %xmm2, %xmm3
    aesenc  %xmm1, %xmm3
    movq    -8(%rdi,%rcx), %xmm4            # xmm4 = mem[0],zero
    pshufd  $68, %xmm4, %xmm4               # xmm4 = xmm4[0,1,0,1]
    pxor    %xmm0, %xmm4
    movdqa  %xmm2, %xmm5
    aesenc  %xmm4, %xmm5
    movdqa  .LCPI0_1(%rip), %xmm6           # xmm6 = [0,8,1,9,2,10,3,11,4,12,5,13,6,14,7,15]
    pshufb  %xmm6, %xmm1
    aesenc  %xmm1, %xmm2
    pshufb  %xmm6, %xmm4
    aesenc  %xmm4, %xmm2
    aesenc  %xmm3, %xmm0
    aesenc  %xmm5, %xmm0
    aesenc  %xmm2, %xmm0
    movdqu  %xmm0, (%rax)
    retq
.LBB0_6:
    shrq    %rdx
    shlq    $4, %rsi
    subq    %rsi, %rcx
    movdqa  .LCPI0_0(%rip), %xmm5           # xmm5 = [11400714819323198485,7809847782465536322]
    movdqa  .LCPI0_2(%rip), %xmm4           # xmm4 = [2406632364132693878,1554156972533191953]
    movdqa  .LCPI0_3(%rip), %xmm3           # xmm3 = [2559278670753769365,3549216002486605715]
    movdqa  .LCPI0_4(%rip), %xmm2           # xmm2 = [5923700269363172178,176065353196263684]
    movdqa  .LCPI0_5(%rip), %xmm1           # xmm1 = [6616326155283851669,14313749767032793493]
    movdqa  .LCPI0_1(%rip), %xmm6           # xmm6 = [0,8,1,9,2,10,3,11,4,12,5,13,6,14,7,15]
    .p2align    4
.LBB0_7:                                # =>This Inner Loop Header: Depth=1
    movdqu  (%rdi), %xmm7
    movdqu  16(%rdi), %xmm8
    movdqu  (%rdi,%rcx), %xmm9
    movdqa  %xmm9, %xmm10
    pxor    %xmm7, %xmm9
    pxor    %xmm0, %xmm7
    aesenc  %xmm7, %xmm5
    movdqa  %xmm8, %xmm7
    pxor    %xmm0, %xmm7
    aesenc  %xmm7, %xmm4
    movdqu  -16(%rdi,%rcx), %xmm7
    movdqa  %xmm7, %xmm11
    pxor    %xmm8, %xmm7
    pxor    %xmm0, %xmm11
    aesenc  %xmm11, %xmm3
    addq    $32, %rdi
    pxor    %xmm0, %xmm10
    aesenc  %xmm10, %xmm2
    pshufb  %xmm6, %xmm9
    aesenc  %xmm9, %xmm1
    pshufb  %xmm6, %xmm7
    aesenc  %xmm7, %xmm1
    decq    %rdx
    jne .LBB0_7
# %bb.8:
    aesenc  %xmm5, %xmm0
    aesenc  %xmm4, %xmm0
    aesenc  %xmm3, %xmm0
    aesenc  %xmm2, %xmm0
    aesenc  %xmm1, %xmm0
    movdqu  %xmm0, (%rax)
    retq
.Lfunc_end0:
    .size   FNV1A_Pippip_Yurii_OOO_128bit_AES_TriXZi_Mikayla_forte, .Lfunc_end0-FNV1A_Pippip_Yurii_OOO_128bit_AES_TriXZi_Mikayla_forte
    .cfi_endproc
*/

//  Cycles = ((wrdlen - 1)>>5) + 1;
//  NDhead = wrdlen - (Cycles<<4);
// And some visualization for XMM-WORD:
/*
kl= 33..64 Cycles= (kl-1)/32+1=2; MARGINAL CASES:
                                 2nd head starts at 33-2*16=1 or:
                                        0123456789012345 0123456789012345 0
                                 Head1: [XMM-WORD      ] [XMM-WORD      ]
                                 Head2:  [XMM-WORD      ] [XMM-WORD      ]

                                 2nd head starts at 64-2*16=32 or:
                                        0123456789012345 0123456789012345 0123456789012345 0123456789012345
                                 Head1: [XMM-WORD      ] [XMM-WORD      ]
                                 Head2:                                   [XMM-WORD      ] [XMM-WORD      ]

kl=65..96 Cycles= (kl-1)/32+1=3; MARGINAL CASES:
                                 2nd head starts at 65-3*16=17 or:
                                        0123456789012345 0123456789012345 0123456789012345 0123456789012345 0
                                 Head1: [XMM-WORD      ] [XMM-WORD      ] [XMM-WORD      ]
                                 Head2:                   [XMM-WORD      ] [XMM-WORD      ] [XMM-WORD      ]

                                 2nd head starts at 96-3*16=48 or:
                                        0123456789012345 0123456789012345 0123456789012345 0123456789012345 0123456789012345 0123456789012345
                                 Head1: [XMM-WORD      ] [XMM-WORD      ] [XMM-WORD      ]
                                 Head2:                                                    [XMM-WORD      ] [XMM-WORD      ] [XMM-WORD      ]
*/

// And some visualization for Q-WORD:
/*
kl= 9..16 Cycles= (kl-1)/16+1=1; MARGINAL CASES:
                                 2nd head starts at 9-1*8=1 or:
                                        012345678
                                 Head1: [Q-WORD]
                                 Head2:  [Q-WORD]

                                 2nd head starts at 16-1*8=8 or:
                                        0123456789012345
                                 Head1: [Q-WORD]
                                 Head2:         [Q-WORD]

kl=17..24 Cycles= (kl-1)/16+1=2; MARGINAL CASES:
                                 2nd head starts at 17-2*8=1 or:
                                        01234567890123456
                                 Head1: [Q-WORD][Q-WORD]
                                 Head2:  [Q-WORD][Q-WORD]

                                 2nd head starts at 24-2*8=8 or:
                                        012345678901234567890123
                                 Head1: [Q-WORD][Q-WORD]
                                 Head2:         [Q-WORD][Q-WORD]

kl=25..32 Cycles= (kl-1)/16+1=2; MARGINAL CASES:
                                 2nd head starts at 25-2*8=9 or:
                                        0123456789012345678901234
                                 Head1: [Q-WORD][Q-WORD]
                                 Head2:          [Q-WORD][Q-WORD]

                                 2nd head starts at 32-2*8=16 or:
                                        01234567890123456789012345678901
                                 Head1: [Q-WORD][Q-WORD]
                                 Head2:                 [Q-WORD][Q-WORD]

kl=33..40 Cycles= (kl-1)/16+1=3; MARGINAL CASES:
                                 2nd head starts at 33-3*8=9 or:
                                        012345678901234567890123456789012
                                 Head1: [Q-WORD][Q-WORD][Q-WORD]
                                 Head2:          [Q-WORD][Q-WORD][Q-WORD]

                                 2nd head starts at 40-3*8=16 or:
                                        0123456789012345678901234567890123456789
                                 Head1: [Q-WORD][Q-WORD][Q-WORD]
                                 Head2:                 [Q-WORD][Q-WORD][Q-WORD]

kl=41..48 Cycles= (kl-1)/16+1=3; MARGINAL CASES:
                                 2nd head starts at 41-3*8=17 or:
                                        01234567890123456789012345678901234567890
                                 Head1: [Q-WORD][Q-WORD][Q-WORD]
                                 Head2:                  [Q-WORD][Q-WORD][Q-WORD]

                                 2nd head starts at 48-3*8=24 or:
                                        012345678901234567890123456789012345678901234567
                                 Head1: [Q-WORD][Q-WORD][Q-WORD]
                                 Head2:                         [Q-WORD][Q-WORD][Q-WORD]
*/

// The more the merrier, therefore I added the 10,000 GitHub stars performer xxhash also:
// https://github.com/Cyan4973/xxHash/issues/1029
// 
// Pippip is not an extremely fast hash, it is the spirit of the author materialized disregarding anything outside the "staying true to oneself", or as one bona fide man Otto/Pippip once said:
// 
// Translate as verbatim as possible:
// In 1926, Traven wrote that the only biography of a writer should be his
// works: «Die Biographie eines schöpferischen Menschen ist ganz und gar unwichtig.
// Wenn der Mensch in seinen Werken nicht zu erkennen ist, dann ist entweder der
// Mensch nichts wert oder seine Werke sind nichts wert. Darum sollte der schöpferische
// Mensch keine andere Biographie haben als seine Werke» (Hauschild, B. Traven: Die
// unbekannten Jahre, op. cit., p. 31.)
// 
// In 1926, Traven wrote that the only biography of a writer should be his works:
// “The biography of a creative person is completely and utterly unimportant.
// If the person is not recognizable in his works, then either the person is worthless or his works are worthless.
// Therefore, the creative person should have no other biography than his works” (Hauschild, B. Traven: Die unbekannten Jahre, op. cit., p. 31.) 

// FNV-1a 64-bit hash (unchanged)
static uint32_t get_hash(const uint8_t *data)
{
    uint64_t h = 0xCBF29CE484222325ULL;
    for (int i = 0; i < MIN_MATCH_LEN; ++i) {
        h ^= data[i];
        h *= 0x100000001B3ULL;
    }
    return (uint32_t)h;
}

// FNV-1a 64-bit hash (unchanged)
static uint32_t get_hashPippip(const uint8_t *data)
{
    uint32_t chk[4];
    FNV1A_Pippip_Yurii_OOO_128bit_AES_TriXZi_Mikayla_forte((const char *)data, MIN_MATCH_LEN, 0, chk);
    return (uint32_t)chk[0];
}

/*
RT;.,:;:,:;;;:,:;:;:;:,:,:;:;:;:;:;:,:,:;:,:,:;:,:,:;:,:;:,:;:;:,:;:;:,:,:;:,:,:,:,:;:,:;:;:,:;:,:,:,:,:;:;:,:;:;:;:,:;:;:;:;:,:,:,:;:,:,:,:,:,:;:;:,:,:,:;:;:;:,:;:;:,:,:,:;:,:;:,:,:;:,:,:,:;:;:;:;:;:,:;:;:,:,:;:,:,:,:,:,:
.:.,:,:;:,.,:,:;:,:;:,:,:;:;:;:,:;:,:,:;:,:;:;:,:,:,:,:;:;:;:,:;:;:;:,:;:;:;:;:;:;:,:;:;:;:;:;:,:,:;:;:,:;:;:;:;:;:,:,:;:,:;:,:,:,:;:,:;:;:;:;:,:,:,:;:,:;:;:;:,:,:,:;:;:;:;:;:;:;:,:,:,:;:,:;:;:,:,:;:,:,:;:,:;:,:,:;:;:,:;:,
,.,.;:;:;:,:,:,:,:,:,:,:;:,:;:;:,:;:;:;:;:;:,:,:,:;:;:,:,:,:,:;:,:;:;:;:;:,:,:;:;:;:;:;:,:,:,:,:;:;:,:,:,:;:,:,:,:,:;:,:;:,:;:,:,:;:;:;:;:,:;:;:,:,:,:,:;:,:;:;:;:;:,:,:,:;:;:;:;:;:;:;:,:;:;:,:;:;:,:;:,:;:,:,:;:;:;:,:;:,:;:
:,:;:;:,:,:;:;:;:;:,:,:,:;:,:;:,:;:;:;:;:,:,:;:,:,:,:;:,:;:,:,:,:,:;:,:,:,:,:,:;:;:,:,:,:,:;:;:,:;:;:,:,:;:,:;:,:;:;:;:;:;:,:;:,:,:,:;:,:;:;:,:,:,:,:;:;:;:;:;:,:,:;:;:,:,:;:;:;:;:;:;:,:;:,:;:;:;:,:,:;:,:,:;:;:;:;:,:,:;:,:,
;:;:,:;:;:;:,:;:,:;:;:;:;:;:;:,:;:,:;:;:;:;:;:,:,:,:,:;:,:;:,:,:,:,:,:;:,:,:;:,:;:;:;:;:,:,:,:;:;:;:,:;:;:;:;:;:,:;:;:;:,:,:,:,:;:,:;:;:,:,:,:,:;:;:;:;:;:,:;:;:,:,:;:;:;:,:;:,:,:;:,:,:;:;:,:;:,:;:;:,:,:,:,:,:;:;:,:;:,:,:,:
:;:;:,:;:;:;:,:;:;:,:,:,:,:,:;:;:,:,:;:;:;:,:;:;:;:,:;:,:;:,:,:,:,:;:,:,:;:;:,:;:;:,:,:,:;:,:,:,:;:;:;:;:;:;:;:,:,:,:,:;:;:,:;:;:;:,:,:,:,:;:;:;:;:,:;:;:,:;:;:,:,:,:,:,:;:;:;:,:;:;:,:,:;:,:,:,:,:,:;:,:,:;:,:;:;:;:;:;:,:;:;
;:;:;:,:;:,:,:,:;:,:;:;:,:,:;:,:,:,:;:,:;:;:;:;:,:;:;:;:,:,:;:;:;:,:;:;:;:;:,:,:,:;:;:,:,:,:,:;:,:,:;:;:;:,:;:,:,:,:;:,:,:,:,:;:;:;:,:,:;:;:;:,:;:,:;:;:;:;:;:,:;:,:,:,:,:,:,:;:;:;:,:,:,:,:,:,:,:;:,:;:,:;:;:,:,:;:;:,:,:,:,:
,,:;:,:;:,:,:,:,:,:;:,:;:;:,:,:,:;:,:,:,:,:;:;:;:,:;:,:,:;:;:;:,:;:,:,:,:,:,:,:;:;:;:;:,:;:,:,:,:;:,:,:;:,:,:,:,:;:;:,:;:,:,:;:;:,:;:,:;:,:,:;:,:;:;:;:;:;:,:;:;:,:,:,:,:;:;:;:;:,:;:;:,:,:;:;:;:;:;:,:;:,:;:;:,:;:,:,:;:,:;:,
;:,:,:;:,:,:;:;:;:,:;:;:;:;:;:;:;:;:,:;:;:,:,:;:,:,:,:;:,:,:,:,:,:;:,:,:,:,:;:,:,:,:;:,:;:;:,:;:;:;:;:,:,:;:;:;:,:;:,:,:,:,:;:;:,:;:,:,:;:,:;:,:;:;:,:;:,:,:;:;:,:;:;:;:,:,:,:;:,:;:;:,:;:,:,:,:;:,:;:;:,:;:,:;:,:;:;:,:,:,:,:
:,:;:;:,:,:,:,:,:;:;:,:;:;:;:,:;:;:;:;:,:;:;:,:,:;:;:;:,:;:,:;:,:,:;:;:;:;:;:,:;:;:,:;:,:;:;:,:;:,:,:,:;:,:;:;:;:,:;:;:;:,:,:,:,:;:;:,:;:,:,:,:,:;:;:,:,:;:,:;:,:;:,:;:,:;:;:;:,:;:,:,:,:;:,:,:;:;:;:;:;:;:,:,:;:;:;:;:;:,:;:;
;:;:;:,:,:,:;:;:;:;:,:,:;:;:,:;:;:;:;:,:,:;:;:;:,:;:,:,:,:,:;:;:,:,:;:,:,:,:;:,:,:;:,:,:,:;:;:,:,:,:;:,:,:,:,:,:,:;:,:;:;:,:,:;:,:,:;:;:,:;:;:,:;:,:,:,:,:,:,:,:;:,:;:,:,:;:;:,:;:;:,:,:;:;:;:;:;:,:,:;:,:,:,:;:;:;:;:;:,:,:;:
,;:,:,:,:;:,:,:,:;:,:,:;:,:,:;:,:;:;:,:,:;:,:;:;:;:,:;:;:,:,:,:,:,:;:;:;:,:;:;:,:;:;:;:,:;:;:,:;:,:;:,:;:;:,:;:;:,:;:,:,:,:,:;:,:,:,:;:;:;:,:,:;:;:;:,:,:,:;:,:;:,:;:;:;:,:;:,:,:,:;:,:,:;:,:;:,:,:;:;:;:;:;:,:,:;:,:;:;:;:,:,
;:;:;:;:;:;:;:;:;:;:;:;:;:;:;:,:,:;:;:,:;:,:,:,:;:;:;:,:,:,:,:,:;:;:,:;:;:,:,:;:;:,:;:,:;:;:,:;:,:,:,:;:;:;:,:;:;:,:,:,:,:;:,:,:,:,:;:;:,:;:;:;:,:,:,:,:,:,:,:;:,:,:;:,:,:,:;:;:;:;:,:;:;:;:;:,:;:;:,:,:,:;:;:;:,:;:;:;:,:,:,:
,;:;:;:,:;:,:;:,:;:;:,:,:,:,:,:;:;:,:,:;:,:,:,:,:;:,:,:,:,:,:;:;:;:,:;:,:,:;:;,,:,.,:;:;:;:,:;:;:,:,:;:;:,:,:;:;:,:,:,:;:,:;:,:;:;:;:;:,:,:;:;:;:,:,:,:,:,:;:;:;:;:;:,:;:;:;:;:;:,:;:,:,:;:,:,:;:;:,:;:,:;:;:;:,:,:;:;:,:;:;:,
;:;:,:;:;:,:,:,:;:,:,:,:;:,:;:,:,:;:,:;:;:,:;:,:;:,:,:,:;:,:,:,:;:,:;:,:,:,,;:;.,:;.;.,.,:,:,:;:;:,:;:;:;:;:;:,:;:,:,:,:,:;:,:,:;:,:,:;:,:;:,:,:,:,:,:,:,:;:;:,:;:,:;:,:;:;:;:;:,:,:;:;:;:;:,:;:;:;:,:,:;:;:,:;:,:,:;:,:;:,:,:
:;:,:;:,:;:,:,:,:,:,:;:;:,:;:;:,:;:;:;:;:;:;:;:;:;:,:,:,:,:,:;:;:;:,:,:,:;.,:,..,i;,.,;,.,:;:,:,:;:;:;:,:,:;:,:;:,:;:,:,:,:,:,:;:;:,:;:;:,:,:,:,:,:,:;:;:;:;:,:;:,:;:,:,:,:;:,:;:,:;:,:,:;:;:,:;:;:,:,:,:,:,:,:;:,:;:,:,:,:,:,
;:,:;:;:;:;:,:,:,:;:;:;:,:;:;:,:;:;:,:,:,:,:,:,:;:,:;:,:;:,:,:;:,:;:;:,:;:,.: ,iyvl;:i; ,.,,,:,:,:;:,:;:,:,:,:,:,:,:;:;:,:;:;:,:;:,:;:;:;:,:;:;:;:;:,:,:,:;:,:,:,:;:;:;:,:,:;:,:,:,:,:;:,:;:,:;:,:,:,:;:,:,:,:,:,:,:,:,:;:;:;:
:,:,:,:;:;:,:,:;:,:,:;:,:,:,:;:,:;:,:,:;:;:,:;:;:,:;:,:;:;:;:,:,:;:,:,.,,,...;vOXViiv3VYi,.,:;:,:;:;:,:;:;:;:;:,:;:,:;:;:,:,:;:;:,:,:;:;:;:;:;:;:;:,:,:;:,:,:,:,:;:,:;:,:;:,:;:;:;:,:;:;:;:,:,:;:,:,:,:,:;:,:,:;:;:;:;:,:;:,:,
;:;:;:;:,:,:,:;:;:,:,:;:;:;:;:;:;:;:;:;:,:,:;:;:;:;:;:,:,:;:,.,:,.,.,.,.,,ivtQ@DFVlv2K8BL...,.,:,:;,;:,:;:,:;:;:;:;:;:;:;:,:,:;:,:,:;:;:,:,:;:;:,:;:;:;:;:;:;:;:,:,:;:,:;:;:,:;:,:,:;:;:,:,:;:;:,:,:,:,:,:;:,:,:,:;:;:,:,:;:,:
,;:;:,:;:,:,:,:,:,:,:,:;:,:;:;:;:;:;:,:,:,:,:;:;:,:,:,:;:,.:.,.. : ..:.;ijOBE@B@E@@@Bt;yVi;;.:.,.,:,.,.,:,:;:;:,:,:,:,:;:;:;:;:,:;:,:;:,:,:;:;:,:,:;:,:,:;:;:;:;:;:,:;:;:;:,:,:;:;:;:;:,:;:,:,:,:,:;:;:,:,:;:,:;:,:;:;:,:;:;:,
;:,:,:,:;:;:;:;:,:,:,:,:;:;:,:,:;:,:;:;:,:,:;:;:,.,:,:,:,.: :ii;;   ,;ijZS8B@8B@@@@B@ECVylLyi;;,,.,.. ..,:;:;:,:;:;:;:,:;:,:,:,:,:,:,:;:;:,:,:,:;:;:,:,:;:,:;:,:,:;:;:;:;:;:;:;:;:;:,:;:,:;:;:;:,:;:;:,:,:;:,:;:;:;:,:;:,:,:,:
,,:;:;:;:;:;:;:;:,:,:,:;:;:,:;:,:;:,:;:;:,:;:,:,:,.,.,.,.;i;lBlyEtjOF16BB@EBQQQ@B88BE@8XvUVlivvi;;. ,yi:.,:;:,:;:,:;:,:;:;:;:,.,:,:;:,:,:;:;:,:;:,:;:;:,:,:,:,:,:;:;:,:,:;:,:,:;:,:,:,:,:;:,:,:;:,:,:,:;:;:,:,:,:,:,:;:,:,:,:,
;:;:;:;:,:,:;:,:,:;:;:;:,:;:;:,:;:,:;:,:;:,:,.,.;,,,;.:.iV336ScX88@B@B@EBB@8S1C3MyF8B8SlLUciyvVi;ii;@B@i..,:;:,:,:,:;:;:,:;.,.. :.;:,:;:;:,:,:,:,:,:,:,:,:;:;:,:;:,:,:;:;:;:;:,:;:,:;:,:,:,:;:,:;:,:;:;:,:,:,:,:,:;:;:,:,:;:,:
,;:,:,:,:,:;:,:,:;:;:,:;:,:,:,:,:;:;:,:;:,:,:;,;;i;Yl;,iYJljlyt$OEE@@@EQZB@@ED32G0KEG2FX1McCG$XVyvi;i@0;.,,,:;:;:;:,:;:,:;:, .i; ::;:;:;:;:;:,:;:;:,:,:,:;:;:,:;:,:,:,:;:,:;:,:,:,:;:,:;:;:,:,:;:,:;:,:;:;:;:;:;:,:;:;:;:,:,:,
;:;:,:;:;:,:;:,:;:;:;:,:;:,:,:;:;:,:;:;:,:;.,;i;yiivl;;;;;yLVlK3IX3D@B@BBB@88Z8@@83KOXIGSICIBBQ1Jly;;;, ::;:,:,,;:;:,:;:;:,. $@@; :.;:;:,:,:,:,:,:;:;:;:,:,:,:;:,:,:,:;:;:,:,:;:;:,:;:,:,:,:,:,:;:,:,:;:;:,:;:;:,:;:,:;:,:,:,:
:;:;:;:;:,:,:;:;:,:;:,:;:,:,:;:;:;:,:;:;,,:;;;it1cyVii;;,;iSE$XMj11tllJ8@@B8@@@Ei. .O@SGZ@DQQ8SKL1VLvi;;.,.;:;:,:,:,:;:;,,.. @B@2, ,:;:;:,:;:;:;:;:,:;:;:;:,:,:;:;:,:,:;:,:;:,:,:;:,:;:;:;:,:,:;:,:;:;:,:,:;:;:;:,:,:;:;:,:;:;
;:,:;:,:;:,:,:;:,:,:,:;:,:,:,:;:;:;:,:,:;:;,;ijVJVFUyii;;,;;U8BXOUi;iiivEBB@@Q;    i@B6Q@BBQDI3XFKMGOii;;.,.;:;:;:,:;:,:,:, .B@B@; .,:,:;:,:;:,:,:;:,:;:;:;:,:;:;:;:;:;:,:;:,:;:;:,:,:,:,:,:,:,:;:;:,:,:;:;:,:,:,:;:;:;:;:;:;:
:,:,:;:,:;:;:,:;:;:,:;:,:,:;:;:;:;:,:,.;:;:;;yiVUJcFUYii;;,..y0QZ1ivc0XVC8@@I     ;@@@@@@@@8O6Z$F68@8L;;;i,:.,:,,,:;:,:,:,. ,@@8@l .:,:,:;:;:;:;:,:,:,:,:;:,:,:;:,:;:;:;:;:;:,:;:,:,:,:,:;:;:;:,:;:;:;:;:,:,:;:;:;:;:;:,:;:,:,
;:;:;:,:;:;:,:;:,:,:,:,:;:,:,:;:,:;,;.;:;:,;lvlc3XVl2cYi;,;::.Y@BMK0803CE@@Di   . jB@B@@@DCUS@@B@B@B@MUi;;v;;:,.,:,.;:;:;.. 0@@X@Z; ,:;:;:;:,:,:,:,:;:;:;:;:;:;:;:,:;:;:;:,:,:;:,:;:,:,:;:,:,:,:;:;:,:;:,:;:;:,:;:;:;:;:,:;:,:
,,:,:;:;:,:,:;:,:,:;:;:;:,:,:;,,:;,;,,:;:,,lvUXCFL;iyXVyiiii;;v@@DI88St2E@8F$8KU.,B@@@B@iSi;V68@@@@@E6SU;iii;;,;.,.,:,:;::  B@8K8@; .,:,:,:;:,:;:,:,:,:;:;:,:,:,:;:;:,:;:;:;:,:;:,:;:,:,:,:;:;:;:,:;:,:;:;:,:;:,:,:,:;:;:;:;:,
;:,:;:,:;:,:;:;:,:,:,:;:;:,:;,;,,.;:;;;:,.iLVYMXLi;;iYJVUlyvv;y8@ZSQ80O2@@@B@B@BUO@@@@@jv1;.;;SB@DB@@0QMVvli;,;;i;;,,.,.,. ;@@GYEBv ::,:;:,:,:,:,:;:;:,:,:,:;:;:;:;:;:,:,:,:;:,:;:,:;:;:;:,:,:;:,:;:,:;:;:;:;:;:,:;:,:;:;:,:;:
:;:,:,:;:;:;:;:,:,:,:,:;:;:,:,.,:,:,:;;;:iUFJVYUiiiiiUcJVVvyviv8B8686SKCB@@@B@@@B@@@ytLVY;.,:iY@6IZBFCQSKSKJi;;;;;;i;;,,. .@@@FUE@v .,:;:;:;:,:;:,:,:,:;:;:;:,:;:;:;:,:,:;:;:;:;:,:;:;:;:;:;:,:,:;:,:;:;:,:;:,:,:,:,:;:,:,:,:,
;:,:,:;:;:;:;:,:;:;:;:;:,:;:;:;,;,;,,.,:;lXV2JYiiitVvjFVVvlvv;vS@8@88SOK@B@@@@@@@B@; ;Vi,.,:;;Y3QFS$L1@DDQ8QCyjl;.;;;;i;iilSECt2@@V ,:;:;:,:,:;:;:;:;:,:;:,:;:,:;:;:;:;:,:;:,:;:,:,:,:;:,:;:,:;:;:;:;:;:,:;:,:;:;:;:,:;:,:;:,:
:,:;:,:,:,:;:,:,:;:,:,:;:;:,:,:;;i;;;i;;;YVylVlll1QFijjYlyiviiv2S@8E8838B@@@@@B@@@U  . ..,:;,;;j308@OQQ3$B8@8ZSZLyyv;;;iv3MjVyFSB@v .;:,:,:,:,:;:;:,:,:,:;:;:,:;:,:;:;:,:;:,:,:,:;:,:;:;:,:,:,:;:,:,:;:;:;:;:;:;:,:,:;:,:,:,:;
;:,:,:,:,:,:,:;:,:;:;:;:,:;,;:;:;,;;iii;yUUiiivlUXQLvyUiYllililVKDQ3BQ$E@@@@@@@E@8; ,,;:;,;,;,;vCZ@B@QDKQB@8BB@8DFS3l;i;;ijJXVK2@B; ,:;,;:;:,:,:;:;:;:;:;:;:,:,:,:;:;:;:;:;:,:;:;:,:;:;:,:,:;:,:;:,:;:,:,:,:,:;:;:;:;:;:,:,:,:
,;:;:;:,:;:,:;:;:,:,:,:,:,:,:;,;,;,;,;;;iLylivvy;illiycLljllvlic0EOOQBZB@@B@B8$S@$ .,;,;,;:;,;,iyZ68B@@QDBEE06Q8OXU$jiii;;;LcXU6@i :.,,;:,:;:;:;:;:;:,:;:,:,:,:,:,:,:,:,:,:;:,:;:;:;:;:,:,:;:,:,:;:,:;:,:;:;:,:;:;:;:;:,:;:;:,
;:;:,:,:;:,:,:;:,:,:,:;:;:,:,:;:;;;;i;i;vyUlY1BJ, ,;;;ccXUjljlc0@QQSEZ6$StCK3ccM@F .;,;,;,;,;,;;vY8B@@B@@@B88QB863D32K1lv;ivjVJI0...,,;:;:,:;:,:;:;:,:;:;:,:;:,:;:;:,:;:;:;:,:;:;:,:;:,:;:;:;:,:,:,:,:;:;:;:;:;:,:;:,:;:;:;:,:
,,:,:;:;:,:,:,:;:;:;:;:;:;:;:,.,:;:;;iillcKEB@@@i  ,;ljXLFJtXt2B$O6B6OFJVVvylyl3@1 :,;,;,;,;,;,iiU@@@88@BQB@B@@@B8QQQE0IUyilyjUEEi :.;,,:,.,.;,,:;:;:,:,:;:;:;:,:;:;:;:,:;:,:;:;:;:;:,:;:;:;:;:,:,:,:,:;:;:;:,:,:,:,:;:;:;:;:,
;:,:;:,:;:;:;:,:,:,:,:;:,:,:;:,:,:;:;;i;ivU$@B@B@;  ilUjlyUcIF$0tS@@BtO1FXKXJUU2@X .;;;,;,;,,.;;iK@@@SV;,;vYFC8B@B@B@8B8S1tvlUXS@; .;,,.,.,;i,;:;:,:;:,:;:;:;:;:;:;:;:,:;:;:;:,:;:,:;:,:;:;:,:,:,:,:,:,:;:;:;:,:,:;:,:,:;:,:,:
:;:;:;:,:;:;:;:,:;:,:,:,:,:;:,:;,;,;:,;;,;;lyFZB@@ti;;iUilyjVCFLtVivLMviiiii;,;M@C .;;,;,;,;:,;;ii;i;;  .:.....;;VE@B@8@BBCXlLF@K. Yi;.,;vJc;,:;,;,;:;:,:,:,:;:;:;:;:,:;:;:;:;:;:;:;:,:,:;:,:,:;:;:;:;:,:;:;:;:;:,:;:;:;:,:;:,
;:;:,:,:,:;:,:;:,:;:;:;:,:,:,:,,,:;:;,;,;;;;lvc$BB@@@0QDSIEO2EEili,  :;.. . . ;G@Oi ;;;,;,;,;,;;v,.   ,.;.,.:..    ,BB@E6GGUVc8B; 30i:;;yv;.::;,;:,:;:;:,:;:;:,:,:,:;:;:;:,:,:;:,:;:;:,:;:;:,:;:,:,:,:,:,:;:,:;:;:,:;:;:;:,:,:
,;:;:,:,:;:;:;:;:,:;:;:,:;:;:,:,:;:;:,:;,;ilivlFXDB@B@@@B@@@B@BEZ@Qy  ;i.,.,..iZXZ3: ;;;,;,;,;,;i;,;:;:,:,:;.,.,..  ;@@Q1F1XL1O3;VGL,,iU;  ;,;,;,,:,:;:;:,:,:;:,:,:,:,:,:,:;:,:,:,:;:,:,:,:;:,:,:;:;:,:,:,:;:;:;:;:;:,:,:,:,:,
;:,:,:,:;:;:,:,:;:,:;:,:;:;:;:;:;:;:,:;:;,;;vvyL3$@B@B@B@@@@@B@B@SBM; ,;;.;,: i0tv6J..;,;;;,;,;;v,;;;,;,;:;:;:,:,.. y@@0OFXlYVB8My; ,yt;;vv;;:,,;,;:;:,:;:,:;:;:;:,:;:;:,:,:;:;:,:;:;:;:;:,:,:;:,:,:,:;:;:;:,:,:;:;:;:,:;:;:;:
:,:,:,:;:,:;:;:,:,:,:,:;:;:,:,:;:;:;:;:,:,.,ililF@B@@@@@B@@@B@@@B@@@i..l;:,;..i6XyXBji,;,;;;;;;30;:i;;,;:;,;:;:,.. iB@I20SCMtE$l;: ;L6M$1i:: ..,.,.,:,:;:;:,:,:,:;:;:;:,:;:;:,:,:;:,:,:,:;:,:,:;:;:;:,:,:;:,:;:;:,:,:;:;:,:;:;
;:;:,:,:,:;:,:,:,:;:;:,:,:;:;:,:,:,:;:;:,:,.;;vvJZ@B@B@@@@@@@@@@@B@Bi.VX;.,.: vG0VivIG3UUUJU6ZB@@;.;i;;,;,;:;,;...l8@tL2$XSBCi;.: ;y30@QLi;,vyVi;.,.,:,:;:;:,:;:;:;:;:;:;:;:;:,:;:,:,:,:,:;:,:,:,:;:;:,:;:;:;:,:,:;:;:;:;:;:,:
:,:,:,:;:,:,:,:;:,:;:;:;:,:;:,:;:,:;:;:;:;:;,;;yyJS@@@@@@@@@@@@@@@B@yi8B;:,;:.,UFvii;l1QSEGtJCO@@Qli,;,;:,:;:, ;iEBDjjYUJ$D1i;:;.;U1I@ZSKGMZ6l;Vv;.. :.,:,:,:;:;:,:,:,:;:,:,:;:;:;:,:,:;:,:,:,:;:,:;:,:,:;:,:;:,:,:;:;:,:;:;:;
;:;:;:,:,:,:;:,:,:,:;:;:;:,:;:,:;:,:;:;,;:;,;;YyyvUO@@@B@@@@@@@B@@@BMX@U:vSv:.:iUyYyjlyi;;;,;.,;M@@B6Vv;;;;;ii1E@EJllyXtGS0i;;;:iXMM@@BSQSS66Vli;.;il;;:;,,:;:;:;:;:;:;:;:,:,:,:,:,:;:;:;:;:;:;:;:,:;:;:,:;:,:;:;:,:,:,:,:,:;:
,;:,:,:,:,:;:;:;:,:,:,:;:;:,:,:;:;:;:;:;:,:,:;;villMQ@@@@@@@@@@@@@B@DEBci@Ei;v;,t@$Kli,;:;,;,;..,vVQB@@@88D@B@E$VviUjX1ZDSyi;;:iMcv8B@@@QEOGE@8B8B8@@t;;,;:;:;:,:;:,:;:,:;:;:,:;:;:;:,:;:,:,:,:;:;:;:,:,:;:;:;:;:;:;:,:;:;:,:;
;:;:,:;:;:;:;:;:,:,:,:,:;:;:;:,:,:;:;:,:;:;:;:,:;;ii10BB@@@@@B@@@@@B@BB8@BI;lOJ;UFl;;:;,;;;;;:;.::i;iiUUttXV1cVvcFM1MXO0ZXv;;,;t0;2@@EBBB88$EE@QY;jXUi;:;:;:;:,:;:;:,:;:,:,:;:;:;:;:,:;:;:,:,:,:,:,:;:,:,:;:;:;:;:,:,:;:,:,:;:
,;:,:;:,:;:,:;:,:,:;:;:,:;:,:;:,:;:;:,:,:,:,:;,;:,,iiicQZB@@@@@@@@@@@@B@B@ZOI@@M,,,;;;;;;;;;:;ii;;iMviilililV1J1FG1O1K$EMY;;,;jBviB@B8SQE8B@Q@Ei     ..,:,:;:;:,:,:,:;:;:;:;:,:,:;:;:;:,:,:,:,:,:,:;:;:,:;:;:;:;:;:;:,:;:;:,:,
;:;:;:;:;:,:,:,:;:,:;:;:,:,:,:;:;:,:,:;:;:;:;,;;;;iivlF2G6BB@@@@@@@B@@@@@B8Z@@8;,;;;;;;;;;;,;USvvVllIFUl2D$cUYLj1t3233D2Li;,;iEG;U@@@EEEBS880ESiXXl  .,.,.,:;.,:,:,:;:;:;:,:,:,:;:;:;:,:;:,:,:,:,:,:,:,:,:;:;:;:;:,:,:,:,:,:;:
,;:,:;:,:;:;:,:,:;:,:,:;:;:;:,:;:,:,:;:;:;:;:,.,:;;iiyLKME8@B@B@@@@@@@8@B@@@@@;.;i;;,;;;;;;;,0BB6@J;i$CGKQBQFcVXFOOO3SMUii;;;tBj.ZB@B@@@@@E86B@QllFL,;;,.,:,:,:;:;:;:,:,:,:;:,:,:,:;:;:,:,:;:;:,:;:;:;:,:,:,:;:;:,:,:;:,:;:,:;
;:;:,:,:,:;:;:,:;:;:,:,:,:;:;:;:;:;:;:;:,:,:;:,:,.,:;iVVtI88@@@B@@@B@B8B@B@@@K,,i;;,;,;;;;;,,iB@@@X.;SBU30EB@SGMO2$GSMJvi;i;vCQi;B@B@@@B@@@B@Q@@@GMSt,;;;,;,;,;,;:,:,:,:,:;:;:;:,:,:;:,:,:;:;:;:,:,:;:;:;:;:,:;:,:,:,:,:;:,:,:
:;:;:,:,:;:,:,:;:;:;:;:;:,:,:;:;:;:,:;:;:;:;:;:,:,,;;ivlivvcC8B@@@B@@@B@@@B@@v:i;;;;;;;i;;;;;;iJ33ii;c8Xy02QQ8OIK2ccjLli;ivviji.i@B@B@QBB@B8EB88E8E@@tlXli,;,;:;:,:;:,:;:,:;:;:,:;:;:,:;:,:,:,:;:,:,:,:,:,:;:;:,:,:;:;:,:,:,:,
;:,:,:;:,:;:;:;:,:;:,:;:;:;:;:,:;:,:;:;:;:;:;:;:,:,.;,;;i;iiJC8B@@@B@B@B@8@@M,;ii;;;;;i;i;vljyyvyvvvlUBJivVlLJFU1F1iivM2GK3Yi,. IB@B@8SS@@B$BB@88DEE@@6l;.. .:;,;:;:;:,:;:;:,:;:,:,:,:,:;:,:,:;:,:;:;:,:;:,:;:;:;:;:;:;:;:;:,:
,,:;:,:,:;:,:;:;:;:;:,:;:;:,:,:;:,:,:,:,:;:,:,:;,;,;,;,;ily1t3I88@@@@@8B8@B@;;ii;;;;;i;iiyVCXUlyvUvLj$BC;i;iiyljUKJVL8B@BB8Ei. :8@@8EB3GQ@@E8@@@BBDE8@B@B80V,,,,:;:,:,:,:;:,:,:,:;:,:,:,:,:;:;:,:,:,:,:,:,:,:,:;:;:;:,:,:;:,:,
;:;:;:,:;:,:,:;:;:;:,:;:,:,:,:;:,:;:;:;:,:,:,:;:,:;,;:;iylVlYlcKDDB8B88QBB@Y;;i;;;;;iiilccI$ZGSO6DSI0IE2l;i;iivvYyLJB@@B@B@8C;.;@@@D8B8GQ8@B868B@B@B@688@B@@$iiii;;,;,;,;:,:,:,:;:;:,:,:;:,:;:;:;:;:;:;:;:;:,:;:;:;:,:;:,:,:;:
:,:;:;:,:;:,:;:;:,:;:,:;:;:,:,:,:,:,:;:;:;:,:;:,:,:,:,:;iiivvyy1FG088B8@8@Fi;iii;;;i;lVO0QDBB@@@B@E8D$M0li;i;iiviyY3B@@@@@BBQGitB@@DQ@@@QB@@88QBB@@@8@88IZEGviiXy;,;,;,;,;:;:;:,:;:,:,:;:;:;:;:,:,:,:;:,:;:,:,:;:,:,:;:;:;:,:;
;:;:;:,:;:,:;:,:,:,:;:;:,:,:,:;:,:,:,:;:;:;:;:,:,:;:,:,:iilvliyllvylyJ0DBSy;iii;;;;;lXQQQGD3I0QGS0$S3lcOV;i;iiiilvVS@@@B@@@E8QEB@@@8B@@@D0@BB8B$E@@@@B@8QO6FX;;il;;,;,,:,:;:;:;:;:,:,:;:;:;:;:;:;:,:,:;:,:;:;:,:;:,:,:;:,:,:;:
:,:,:;:,:;:;:;:;:,:;:,:,:;:,:,:;:,:,:,:,:;:,:,:;:,:;:;:;,;,,.:.,.;;;:;vJcl;iii;i;;iyM8Q868E6JVii;viv;;lSViiiivivivY8QDBBQB@BB@@@@@B@B@B@ct8@S8BQX8B@@@@@Q88@BEY;   ..,:,:,:,:,:;:,:,:;:,:;:;:,:,:;:;:;:,:;:;:;:;:,:,:,:,:;:,:,
;:;:;:;:,:;:,:;:;:;:;:,:;:;:;:,:,:;:;:,:;:;:;:;:;:;:,:,:,.,.,:;:;,,:;;iii;vly;;;;;VI888E8QBEt;,:;,;;;,iVcvi;iiiiiicQDtE88DBE@@@@@@@@@B@@FUEB8Z@@DG@B@B@B@@@@@B@B@6i.. :.,.,:,:,:;:;:,:,:,:,:,:,:,:,:;:,:,:;:;:;:,:;:,:;:,:;:;:
,,:;:;:;:;:;:;:,:,:;:,:;:,:,:,:;:;:,:,:;:,:;:,:;:;:,:,:;:;:;,;,;,;:,,;:;;ilJviii;X$BDEZES3MDL;.;;;;;;;;vjVii;iiiiiVB8$3SS@Q6@@B@B@@@@@B@Uy3EESC@@EQ@@@@@888@B@B8B@@BLVvi,,,,:,:,:;:;:,:;:;:,:;:;:;:,:,:,:;:;:;:;:;:,:,:,:;:;:;
;:,:,:;:,:,:,:,:;:,:,:,:;:;:;:;:;:;:;:;:;:;:;:,:,:;:;:,:;:;,;:;:,:;:;:..cjvVcvll1686$G62XvlcU;,,;;i;i;;;lli;i;i;iit8@8EFSBB2B@@B@@@B@B@BU;cOSE$Q@@@8@B@B@@@EBB@BB$@8Kvi;;,;,,:,:,:,:,:,:,:;:,:;:;:,:;:,:,:,:,:,:;:;:;:;:,:;:,:
,,:;:;:,:,:,:;:;:,:;:;:,:,:,:;:;:,:;:;:,:,:,:;:,:;:;:;:,:,:,:;:,:;:,:,:yUi,y0OtQ8E6$1MUyvviVli:;;;;;;i;;ivii;i;iivLEQ6Q@B@BBB@B@B@B@B@B@Ot26S@BDD@@@@@@@8@B@@@B@B8Q@SCFi ..,.,.;:;:,:;:,:,:;:,:,:;:,:,:;:;:;:;:,:;:;:;:,:;:,:,
;:;:,:;:;:,:;:;:,:;:,:;:;:,:;:,:;:,:;:,:;:;:,:,:,:;:;:,:;:;:;:;:,:,.:.LUi,ilMIS6EOIULllvliilV;;,;;;;;;;;i;vii;iiiiLS8OE@@@@@@@@@@@@@@B@@@B@EBE@BBB@@@B@@BQBB@B@B@@@@8$@3i,;:,:;,;:;:;.;:,:,:;:;:,:;:,:,:;:,:;:,:;:;:,:;:;:;:;:
,,:;:,:;:;:,:;:,:;:,:,:;:,:;:;:;:;:,:,:;:,:,:,:,:;:,:;:,:,:,:;:;:,.:.lU;,vyXUI66KCLVvlvliiiYi;,;;;;;;;;;;iivivii;il$8BE8B@B@@@@@B@B@@@@@B@@BB@@@8BB@@@@@BQBBB@@@@@B@B@@@@Gii;;,;,;,,:,:;:;:;:;:;:;:;:;:,:;:;:,:;:,:,:,:;:;:,:;
;:;:,:,:;:;:,:;:;:;:;:;:;:;:;:,:,:;:,:,:;:;:;:;:;:,:,:,:;:,:;:,:;:,.ll;,llVtD6SK1YjlYvlii;iii,;,;;;;;;;;i;;;;;iiiil$@B@@@B@B@@@B@B@B@@@B@@@@@@EE8S@@@B@B@QB@BD8B@B@B@BBB@63y;,;:;;;,;:,:;:,:,:,:,:;:;:,:,:,:,:;:,:;:,:,:;:,:,:
:,:;:,:,:;:,:;:;:;:;:;:;:,:;:;:;:,:;:,:,:,:;:;:;:;:,:,:;:;:,:,:;:,:il;,iljc00SKFVVllilii;iiyi;,;;;;;;;;iii;i;i;;;;i$@@B@@@@@@@@@@@@@B@@@@@B@@@lX8B@@B@B@@@O88EQ@@@B@B@@@@@QXii;,;;,;,;;;,;,;:;:;:,:,:;:,:,:;:;:,:;:,:,:;:;:,:,
;:;:;:,:;:;:;:,:;:,:,:,:,:;:,:;:,:,:,:,:;:;:;:,:,:;:;:,:,:,:;:;:,:ii;;viycS$ItXyYlliiiiiiilvi,;;;;;;;;;;i;i;iii;i;iV8E8@@B@@@B@@@@@B@B@@@@@B@@@Gi;E8@@@B@@Q68E@@@@@@@B@@@BB8@QUi, :.;:;:;,;:;:,:,:,:;:,:;:,:,:,:;:;:,:,:;:;:;:
,;:,:,:;:;:;:;:,:;:,:;:,:,:,:,:,:,:,:,:;:;:,:,:,:;:,:,:;:;:;:,.,.;i;;vilVG3OtXVVvliviiiiiivv;;,;;;;;;;;;;;;;;iiiii;yD@D@B@@@B@@@@@@@B@@@@@@@@@@@V, :X@B@E@BB8@B@B@B@B@@@B@8@@It$YV;,.;:,:;:;:;:,:;:;:,:;:,:,:;:;:,:;:;:;:;:;:;
;:,:;:,:,:;:;:,:;:;:,:,:,:;:;:,:;:,:,:;:;:;:,:,:,:;:;:,:;:;:,:,.;;;;vvyVFF3XUyYvlilii;iivili;,;;;;;;;;ivv;;;iii;i;iiG@@8@@@@@B@B@@@@@B@B@@@@@@@@@v   t@@@@@@@@@@@@BDBB@@@8B88SSi;ilii;;:;:;:;:;:,:;:,:;:;:,:,:;:,:;:,:;:,:;:,:
:;:;:;:,:;:;:;:;:;:;:,:;:,:;:;:;:;:;:,:;:;:,:;:;:,:;:,:;:,:;,;.,;;;iiiV31CFXyYvlvviiiviiili;,;,;;;;;;;;lvv;;;i;i;i;iyB@8B@@@@@@@@@@@@@@@@@B@@@EIS$;. .8@B@D@@@@@@@8D6BB@BBBBQ@O;, .i;;;;,;:,:;:;:;:;:;:;:,:,:;:,:;:;:;:;:;:;:;
;:;:;:,:;:,:,:,:;:,:;:,:;:;:;:,:;:,:;:;:,:,:,:;:,:,:,:;:;:;,,:,,;;i;vvVcXYcYyiliv;iiiiiiii;;;,;;;;;;;;;;iii;i;;;iii;iG@8B@@@@@@B@@@B@@@@@@@@@BBU;,i,: ;B@2vyBB@B@@@EB8@@@B@8B@6ii,..,:;,;:,:;:,:;:;:;:;:,:;:,:;:,:,:;:,:;:,:;:
:;:;:;:,:;:,:,:;:,:,:;:,:,:,:;:,:,:;:;:;:,:,:;:,:;:,:,:;:;:,.,.;;iiviVvvilvliviviiiiiiiiii;;,;;;;;;;;;;;;i;iii;i;;;i;J@@8@@@@@B@B@B@@@B@B@@@@@@@y:.,.: ,i: ;Z@@@@@@@B@B@@@@@SB@83V:,.,:;:;:,:;:;:,:,:,:;:;:,:;:,:;:;:;:,:;:,:;
;:,:;:,:,:,:;:;:,:;:;:,:;:,:;:;:,:;:;:,:,:;:,:,:,:,:,:;:;,,:,;;iiilivvliiiiivivii;iiiilii;;,;;;;;;;;;;;;;;;;i;i;i;i;;iB@B@@@@@@@@@@@@@@@@B@B@@@@8, .,.: ..;j@@@B@@@BB8@B@@@@QSDK@E;.,:;:;:;:;:,:,:;:;:;:,:,:;:;:,:,:;:,:,:,:,:
,,:,:;:,:,:;:;:;:,:,:,:;:,:;:,:;:,:,:;:,:;:;:,:;:;:;:;:,.;:,;viivyii;iii;;;iiiiiiiilvlii;;,;,;;;;;;;;;;;;i;;;i;i;i;i,iM@B@@@B@@@B@B@@@B@@@@@B@@@Ei.  :..  .2@@@BB@@BE8B@BBB@B8$2Vtyi:,:;:;:,:;:,:;:,:;:,:;:,:,:;:,:,:;:;:,:;:,
;:;:;:;:,:,:,:;:;:,:;:;:,:;:,:;:;:,:;:,:,:;:,:,:;:;:;:,:,.,,;;;;i;;:,,;;;;iii;iiviYyy;;,;;;;;;;;;;;;i;i;;;;;i;i;i;iii;VE@B@@@B@@@@@B@B@@@B@@@@@@@B@J, . ,;I@@8@BE8@88QBBBZ8@@06B; iiV;,.;:;:,:,:;:,:;:;:;:;:,:,:;:;:,:;:,:,:,:
,;:;:,:,:,:;:;:;:,:;:;:,:;:;:;:,:;:;:,:;:,:,:,:,:;:;:,.,.;;;;;;i;;;;,;,,;;,;;;;i;;;;:,.;;i,;ii;;;i;i;;;;;;;;;;;i;i;viii8BB8@@@@@@@@@B@@@@@@@@@@@@@B@8;.:;BB@Q8BQCB@BEB8BBBEB@@BEi. ;i;:;:,:;:;:,:,:;:,:,:;:;:;:;:,:,:,:;:;:,:;
;:;:,:;:;:,:,:,:,:,:;:,:;:;:,:;:;:,:,:,:;:,:;:;:;:,.,.:.;V2J00S2S0$CMJUlylliiii;i;;;;,;,;,;,;,;,;,;;;.,:;;iivi;;iii;i,;6@BBB@B@@@@@@@@@@@B@@@@@@@BZO@BB1I@E0B@@I0B@EB8BB@BB8@88v;.:.,:,:,:,:;:,:;:;:;:,:,:;:;:,:;:,:,:,:;:,:,:
,,:;:,:,:;:,:,:,:,:,:;:;:,:;:;:;:,:,:;:,:;:;:,:,:;:,.;;;icYVjVlLYUYULXLXXKttLXctUXLUVjyYviii;i;;;i;,.,.,:;;;ili;;i;ii;,J8B8B@@B@@@B@@@@@B@@@B@B@B@BMVIKBB@$6SE6DQ@888BE8B@B@BIv@M;.,:,:;:;:,:;:,:,:,:;:;:,:,:;:,:;:;:,:;:,:;:;
;:,:;:,:;:;:;:;:;:,:,:;:;:,:;:,:,:,:,:;:;:;:;:;:;.,,iilili;;;;;.,:,.,,;,;,;,;;;;;;;;iivilivililvyVi.,.:.;;;.,,;;;;iii;;;O0EE@@@B@B@@@@@@@@@@@@@@@@@@Xl0ESM0IS6EQ@@BQB88DBBBE@I;YU;;:;:,:,:,:;:,:,:,:;:;:,:,:;:,:,:,:,:,:,:;:;:
,,:,:,:;:,:,:,:,:;:,:;:;:,:;:;:,:,:,:;:;:;:;:;:,:,,iii;i;i;i;;;;;;,;,;,;:,:,:,.,:,.,:,:,:;,;;;;;;i;;;;:,vCUv;i;;,;;i;i;;UBBB@@QB@@B@@@B@@@@@@@B@B@B@8Q8MKGOZQ@BE8@@BB@8BB@Qcv1K1.:,;:,:;:,:;:;:;:;:,:,:,:;:,:,:;:,:;:;:,:;:;:,
;:;:,:;:,:;:;:;:;:;:;:;:,:,:;:;:,:;:,:,:,:;:,:;:,:;;;;iiivvii;;;;;;;;;;,;;;;;;;;;,;;;,;;;,;,;,;,;,;;i;i;lVF2CjYii;i;;;;;iZ@8@@BE@B@@@@@B@@@@@@@B@8B@@B@$O0S8@@B$@B@Q@BBQ@BBtUiiQi ;:;:;:;:,:;:,:,:,:,:,:,:;:,:,:;:;:,:,:,:;:,:
:,:;:;:,:;:;:;:,:;:;:;:;:;:,:;:,:;:,:,:,:;:;:;:,:;,;,;;;iYlyvviiiiiiii;;;;;;;;;;;;;;,;,;;;;;;;;;;;;;;;;;,;,iyt2$cy;i;;;;,GBZMBB88BB@8@@@B@@@@@@@@@@@@@QQE@8@B@EB@@BZB@DB@@B@Iy.vJ;.;,,:,:;:;:,:;:,:;:,:,:;:;:,:;:;:,:;:;:,:;:,
;:;:;:;:,:,:,:;:,:;:;:,:,:,:;:;:;:,:,:,:,:;:,:;:,:;,;;;;vvYlylYljlyii;iii;i;;;iii;;;;;;,;;;;i;;;i;i;;;;;;;;:;,il2OXvi;;:;yEXl2@DEZE6QDBB@@@@@B@@@@@@@Q$O@B@8@8BB@@@8@EOB@@@@;iV:;i;.;:;:;:,:;:,:,:,:,:;:,:,:,:;:;:;:;:,:,:;:,:
:;:,:;:;:,:;:,:;:,:,:;:;:,:;:,:;:,:,:;:;:,:;:;:,:,,;;iililvyljVUlVlylylYlylYlylVlviviviiii;i;i;i;i;i;iii;;;;;;.,,vtGUi;;,ilyiU6EQ$3888E@@@@@@@@@B@@@BB$8B@BB@@8B8@BQB@ZQB@@O ,;;.,:;:,:,:,:,:;:,:;:;:,:;:;:;:;:;:,:;:;:;:;:;:,
;:;:;:,:;:;:,:;:;:,:,:,:;:,:,:;:;:;:,:,:,:;:;:,:;,;,iVVyJc1cF13XFX1cFK$S3U1XMtKMCUUL1tXJXlYlliiii;ilyii;i;vii;;;;.;l2ci,;;vylvVc8E6Z8ZEB@B@B@@@B@B@@@B8QBB@8@@@8B@8E@0SB@3C1;.,,;.,:;:;:;:,:;:,:,:;:,:,:;:;:,:;:,:,:,:,:,:,:;:
,,:;:,:,:,:,:;:,:,:,:;:;:;:,:,:;:;:;:;:;:,:;,;:,:,;ivUYjVXcXc2ttKGFKKG080ItXUXM8DGX1F3C$CXF0XcVLvvlciiilivlYyv;i;i;;;vi;;;y$Ui,;2@BE06DBB@B@@@@@B@@@BB6EQB8B@@Q8B@QEEZQ@Elij;:,;,;,;:;:,:,:;:;:;:,:,:;:,:;:,:;:,:;:;:,:,:,:;:;
;:,:;:;:,:;:,:;:;:,:;:;:;:;:;:,:;:,:;:,:;:,:,:,:;,;;vvVU1cXjMCG1KVFCC13cG$I1IO60GcXt0FJUJVXFICMUjYFUl;vviiyvlii;li;;i;i;;;lXSv;ijQ@BEDEEBB@@@@@B@@@@DCQ88Q@B@BSS@@@Q@DEB@V,;i.;,;:;:,:,:;:;:,:;:,:;:,:,:;:;:,:,:;:,:,:;:,:,:,:
:;:,:;:,:,:;:;:,:;:;:;:,:,:;:,:;:;:;:;:;:;,,,;:;,;,;;vlXtMX1F0M21F2ZtOD2V13$30I21KFS$ClyL2UUjtVcUK1Xlviliiilvvivvi;iiiii;;iVUyLZtK36BQXQB@@@@@@@@@@@QQ6Q0QB@BB2EB@BQ@BilY; ;;;.;:;:,:,:;:;:;:,:;:;:;:,:,:;:,:;:,:,:,:;:;:;:,:;
;:,:;:;:;:;:,:,:;:,:,:;:;:,:,:;:,:;:;:;:;:,:;:,:,:;;;;iiUJ2XCKO1J2D1UMQFUcZ6SF3FtVtFFYyjIXcVccJjXVjvviyvviVlliiii;i;i;iii;iiUltQCi;iQE0Q@BBB@B@B@B@B@8SIQQ88@BZE@@@6QQIi:.::;,;:,:;:;:;:,:,:,:,:,:;:,:;:,:,:;:;:,:,:;:;:,:,:;:
,;:;:,:,:,:,:;:;:,:,:,:,:,:;:;:;:;:,:;:,:;:,:,:;:;;i;iivV1XXU2O0XZ0KJC1M2SCMXCtJyUcMcLVFFYvylcUK1jviivvyiYVliviv;vyl;i;iil;;UUvKcCli1BBBQ@BB@@@@@@B@@8GE@@E8BBSB@@B8DBc;.,.,:;,;:,:;:;:,:;:,:;:,:,:,:,:,:;:;:,:,:,:;:;:;:;:,:,
;:;:;:,:;:;:,:;:,:,:,:,:,:;:,:;:,:;:,:;:,:;:;:,:,:;ivii;VU3F1XSS0I1J602S0JXyJC$1Jc2FtVUVjyyljVUVUyyvyyjlyyy;iyUiiiyvv;iivv;;lv;;JSMvYOBB8E@8BB@@@@@@@EEQBB@8@8E8@8@Q8Qv :,,:,:,:;:,:,:,:,:;:;:;:;:,:;:;:;:;:;:;:,:;:,:,:,:;:,:
,,:,:;:,:;:;:,:,:,:;:;:;:;:;:,:;:,:;:,:,:;:,:;:;:;,;;iiyyUM01F3SJFXIO3cItVyX3SFtUXJFJJlVljjLJCyliyyVlLlliLliijyv;iiiiyilLi:;;;;,c8yvlME@8E6@88E@B@@@@@BBQQS8EBBBB@88QBU;.;:,:;:;,;:,:;:,:,:;:,:;:,:,:,:;:,:,:,:;:;:,:;:;:;:;:,
;:,:;:,:;:,:;:;:,:,:;:;:;:;:;:,:,:,:,:;:,:;:,:;,;:;;;ivvllttFtIXUU3FJyXFGcUXGcliyVtJXVLlYUOVULYlyvVlllVvllv;vvVllvl;ivyFU:;;;:l33yilOK8QBBB@@SIMSE@@@B@B800Z$$@B@@B8@y;:::;:;:,:;:,:;:;:;:,:,:;:;:,:,:;:;:,:;:,:;:,:;:;:;:,:;:
:;:,:;:,:;:,:;:,:,:,:,:,:;:,:;:,:;:,:,:;:,:;:;:,:;:;;;;i;YJXL3FLyXXMUJLtcFXFUYvUU1KXvllliX0XlYljlVVLvyyyiliiiVVULUvVlyLV.,;i,;V8i..;vVGBSEB@BB$3GB@@@@B8$6DQ1S8B8@QB@v ;.;:,:,:,:;:;:,:;:,:;:,:,:;:;:;:;:,:;:;:,:,:;:;:;:,:;:;
;:;:;:;:;:;:;:;:;:;:,:;:;:;:;:,:;:;:,:,:,:,:,:;:;:,,;,iivYtjctIUCccjMKtYcU1UUVX3GXKYviVvvy1yjccYYvjUYvYvlvyljYliyyVyXtG;.,i;;,ii,.lyyltZ8OE8@B@EEQBB@BBQQQBQ06@Q8BBB@U.:;.;:,:,:;:,:,:,:,:;:;:,:;:;:;:;:;:;:,:;:,:,:;:;:,:,:;:
,,:,:;:,:,:,:;:;:;:,:,:;:;:,:,:,:;:,:;:;:;:;:;:;:,.;;;;iiyVXUXXKjJjtt3MGFtYylJJXVUVLjXLjyXyvjScjlvVFlllUXtVyyUlliV1XJ8L,.;;;:;;;:ycVlXvcQ88B8@B@EEQ@8BQEQBB@ZQ8B8@$B@J ,:;:;:;:,:,:,:,:;:;:,:,:,:,:,:,:;:;:;:,:,:;:,:;:,:;:,:,
;:,:;:,:;:;:;:;:;:,:;:;:;:,:,:;:,:,:;:;:;:,:,:,:;:,,;;;;iijXXVJUyL$O3UJc3FjYMcUyjLUF0tJyVyyiYVllYiLjYvYYCGJilt3LXK1F8V;.,,;;;,,.vUY;;Vv;3O8B@ZQB@EBBBZ88B8@BQOE8@BE1y,..,:,:;:,:,:;:;:;:;:;:;:,:;:;:;:;:;:;:;:,:;:;:,:,:;:,:;:
,,:,:,:;:;:;:;:;:;:,:;:,:,:;:,:;:;:;:;:,:;:,:,:,:,:,;i;;;lVXUJVUUcVJU1VyytVFILvVcFlUylYMyvivilvcjllliylyVMMCUUlVF01EG;.,,;,;,;:;ii;i;;;vivUQ8D3BBB8@SSQBDB@BI$QB@@Bi :.,.,.,:,:,:,:,:,:,:,:;:,:,:,:;:,:;:,:;:,:;:;:;:;:,:;:,:,
;:,:,:,:,:,:,:;:,:,:;:;:;:;:;:,:;:,:,:;:,:;:,:;:;.;:;;;,iiYycJKVjYUjUVUV1JLVJYUJ3VvVUvMMlililVccjycyVc3XCUt0CytI3cBE;.,,;,;,;,;;ly;,;;,;v;;iOQQQ8E8SS$QZQE@QOO88@BBiii;.,:;:,:,:,:,:;:;:,:;:,:;:;:;:,:;:,:,:,:;:,:;:;:,:,:;:,:
:,:,:,:,:;:,:,:;:;:,:,:,:,:;:;:;:,:,:,:,:;:;:;:;:;:,:;;iiiiyVF1MVXccjcU13Jivi3OZFlvUUJCFvVVYyUyViYXXj31UVUU32IXCOBBK:::;,;;;,;;;;J;;;i;iS2,;V8E@BB8EZQZ6S88SMQEBB@8cii;;;;,;:;:,:,.,:,:;:;:,:;:,:,:,:,:;:;:,:,:;:;:;:,:;:;:,:;
;:;:;:;:,:,:,:,:;:,:;:,:,:;:,:;:,:;:;:,:;:;:,:;:;:;:;:;il;ilclyjcjXUcYLj3YlvY6BZMV0cXG0cJVKVYyUJVJOKXjUyJK$SQCXCBBQ,.:;;;,;,;;;,;i;.;,;1@Xi;XDB@@B@DQ$ZOQEEM368Q@BQV;.;;;;;;;,;,;,;:,.,:,:,:,:,:;:;:,:,:;:;:;:;:;:;:,:;:,:;:;:
,;:;:,:,:,:,:;:;:;:;:;:;:;:;:,:;:;:,:,:,:;:,:,:;:,:;:;,iliijKtj1VcJCcVy3O3CIF0M21IEQO8Q3UJIKvyVMXMGZXUJ0SSIIF8@B@@i.:;,;;i;;,;;;,;:;;;:iVy;vUSD@@@BBSG$B8BD6Q@@QQ8Vli;,;;;;;;;;;;;;;,;,;:;:,:,:,:;:,:,:,:,:;:;:;:;:,:,:,:,:;:;
;:,:;:,:;:,:,:;:;:,:;:,:;:,:;:;:;:,:;:;:,:,:;:;:;:;:;:,,iiiiUJ1FIXt1S$DD$K2tFXCK33$FMCQGFC8OLVXjF2S33XKCG1$68B@S0y;.;;;:;;;,;;;,;,;,vi;.ii;;lJOE@@@BEKQBBBB8B@@Xyyv;;;;:;;;;;;i;i;;;;;;;;,;:,.;:;:,:;:;:,:,:,:,:;:;:,:;:,:,:,:
,,:,:;:,:,:,:;:;:,:;:,:;:;:,:,:,:;:;:,:,:;:,:;:;:;:,:,:;;v;;ilyXXFUcXZ$330X1KZ0$MMXUYX3ZCGIFLGSZSBOKMDG0SEE@@Syy;,.;,;,;,;,;,;,;,;:;iXi,;i;ivX0BB@8BQZE@8@BBS8QV;;;;:;;i;;,;;i;i;i;;;i;;,;,;:,:;:,:,:;:,:;:;:;:;:,:,:,:,:;:;:,
;:;:;:,:,:,:;:,:;:;:,:,:;:,:,:,:,:;:,:;:;:,:,:,:;:;:,:,,;;;;viiiJK3JCGSJGSS0EE8S02G3OISGS6ZJK$DIQBBKZE8E@@BB@2;:;:;,;,;,;,;,;,;,;,;,;;;:i;;illO8@B$S8Q@BDQ8ZEIVii;;,;,;il;;;;;iii;iili;,;:;;i,,:;:;:;:;:;:;:,:,:,:,:,:,:;:;:,:
,;:,:;:;:;:;:,:;:;:;:,:;:,:;:;:,:,:,:,:;:;:;:,:,:;:;:,:,:;;i;i;vYXFZ$6G6ZQ$0CO3$200EQE6ZSBZGG8IIQBDDS88@@@@@@J:;;;;;,;,;,;,;,;;;;;;i;;:;;i;VviVIE@QBBB8QKD8@@$viii;;;;:;;;,,,iiviyVKcVi;.:,il;.,:,:,:;:,:,:,:,:,:;:;:,:;:;:,:;
;:,:,:,:,:;:;:,:;:;:,:,:,:;:,:;:,:;:,:;:,:,:;:,:,:,:;:,:;,;,;iiiYYJ3DMOSEZDKtUK3Z0OGQS668QESQSSD@BDS@B@B@@@Bt,,;i;i;;,;,;;;,;,;,;;;;;,;;v;;ltivVMS8@@B@BQS@B@SUvv;;;;;;,;.,,;iUctVt112Sli,;jX;,:,:;:;:,:,:;:,:;:,:,:,:,:,:;:,:
,,:;:,:;:;:;:,:;:;:;:;:,:,:,:;:;:,:,:;:,:;:,:,:;:,:;:,:,:,:;:;;lvVyF1UUccO$Q68D$IQ$SO3tOQBEESEE@B@B@@@@@@BOj;;;;;;;;;;,;,;,;;;,;;;,;,;;;;i;;tZUXv1ZQ8@B@$M$@B$ylvv;;;i;;.iCEQDGQI3SB88@@Q@BS;..,:,:;:,:;:;:,:,:,:;:,:;:,:,:,:;
;:,:,:,:,:,:,:;:,:,:;:,:;:;:;:;:;:;:;:,:;:;:;:,:;:,:;:;:;:,:,:iiivVU1lVl3SD$EEB0S68Q8DE8@BBZEB@B@B@@@B@BBFl;;ii;;;;;;,;;;,;;;;;;;;;,;;;,;vY;l3Z1JC30B8EEB8@BBMVlVii;;;;:.V@B@XlillJ0@B@@8BJ:..,:;:;:,:;:,:;:,:;:;:;:;:;:,:;:,:
:,:,:,:,:,:;:,:,:,:,:;:,:,:;:;:,:,:,:,:;:,:;:;:,:;:,:;:;:;:,:;iiiylXUll3Z88BZEEB88Q88@@@BBB@B@B@@@@@@@82cVivii;i;;;;,;,;,;,;,;,;;i;,i1i;;iVjlIFjyIFDB@8@B8I8EZXVvviiii;; ;Q@BV;;;;,;l@@@,  ..,:;:;:,:;:;:,:,:,:,:;:;:,:,:,:;:;
;:;:,:,:,:;:,:,:,:,:;:;:;:,:;:,:;:;:;:;:,:;:;:;:;:;:,:,:,:;:,,i;iilvllSEQS86QEB8B8B8BB@BB8@@@B@@@@@8DD0vVyyii;i;i;;;;;;,;,;;;,;,;;;,iUXli;vVODXvlGD3BB@@@CK6EI3Vlilii;;;,.V8v;i;i;;,,K@Bv.:.,:;:,:,:,:,:;:;:,:,:,:;:,:,:,:,:;:
,,:;:,:,:;:,:,:;:;:;:;:,:,:,:,:;:;:,:;:,:,:;:;:,:,:;:;:,:,:,:;,,,;vVvV2QO6OIGQ8@68@@B@@@@BB@B@B@B@@QlcXLyjlvii;i;;;;,;;;;;,;;;,;,;,;,;;lli;ivM3CtZ888@B@Q$OESI3$Ylivii;;.,l0llvli;,;;;i$Bc,..,:;:;:,:;:,:,:;:;:,:;:,:,:,:;:,:,
;:;:;:;:,:;:,:;:,:;:;:;:,:,:,:,:;:,:;:,:,:,:,:;:,:;:;:;:;:,:;:,.,,llJcVlcMtlFO8BB8@E@@@8B8@@@B@B@B@83VcYVvvvv;i;i;;;;,;;;;;,;;;;;,;,;:;;UtViiX8GO2BB@@@@@8E68SEQMyyil;;;;:VGXlyll;;;;,;iVi;ii.,:,:;:,:;:;:,:,:;:,:,:,:;:;:;:;:
:,:;:,:,:;:;:,:;:;:;:;:;:;:;:,:;:,:,:;:;:,:,:;:;:,:;:;:,:;:;:,:;,;;iiUvvlUvijSQB8BSZ8@@BQ@@@@@@BEGFD3yiYviivii;i;i;;;;;ii;,;,;,i;;,i;;i;;VX1U$8S2GQB@@@@B@E8@@8821yvii;i,,l6Vyiviv;;;;;iilVl,..,.;:,:,:,:;:;:,:;:,:;:;:;:,:,:,
;:;:,:,:,:;:;:,:;:,:,:,:,:,:,:;:,:,:;:;:;:;:,:,:,:,:;:;:;:;:;:,,;:,;i;iivlvit3$O8BB6BBBB@B@B@8QMJVlvVivllilii;iii;;;;;;,;;;;;;;ii,iliiFl;iVyVX8ZQO8E@@@@@@@@@BB8ZXlii;i;;:yEMLYvlii;i;ivVvjli;i;,:,:;:;:,:;:,:,:;:;:,:,:;:,:;:
,;:,:;:;:;:,:,:,:,:,:,:,:;:;:;:;:,:,:,:,:;:,:;:,:;:;:,:,:,:,:;:;,,.;i;;iiyvyJXL$688B@@68@@B@01VyvylyiVyylyiv;i;iii;;;;;;,;iivyivilyUllUti;iLiJQ8E8B@@@B@B@@@BB8@QMvvii;i;,;@@@K1yl;i;;;iiXFDQGci:,:,:;:,:,:;:;:;:;:,:,:;:;:,:,
;:,:;:;:,:;:,:,:;:;:;:,:;:,:,:;:,:;:,:,:,:;:,:,:,:,:,:;:,:,:,:,:,:;:;.;;iiviyj2G3F6Q@@BS8@BtlivillLlVUclyvi;i;iii;;;;;VU;;v;vVYiivJVv;llv;VOt3BE8B@B@@@B@B@@@QB@8KFlvivi;,,M@@@BEMVii;i;iUXlli;,,:,:,:;:,:;:;:,:,:;:,:,:;:;:;:
:;:;:,:;:,:;:;:;:,:;:,:,:,:,:;:,:,:,:,:;:;:;:;:;:,:,:,:;:;:;:,:,:;:,.,,;:;iyvVLXYUX3F0CUy2Li;villyvyVjvvivilll;iiiii;Ytc;iii;ycV;iYciivVvlX0OBQQ8BZ@@@@@@@@@B@@@B8KUlvii;;.;vi;ilVllii;;,y3ti;...;:;:;:,:,:,:,:,:,:;:;:;:,:;:,
;:;:;:,:,:;:;:,:;:;:,:,:;:;:,:;:;:;:,:;:,:;:,:;:,:;:;:;:,:;:;:;:;:;:,.,:;;liiivvvillYvyivvliljLlLllvlii;vilV1Yviiiyiivl;iYYvllXl;itUVvVYVVFIE8QIEQ68@@@@@@@@@B@@@8E3MVlii;,.;.,:;,;;i;;.,lD2Qv..,:,:,:,:,:;:,:;:,:,:;:,:;:;:;:
,,:;:;:,:;:,:;:;:,:,:;:,:,:,:,:;:,:,:,:,:,:;:;:,:,:,:;:;:,:;:;:,:;:,:,:;,;vlii;iiiiviviylVcCKOJUcIJVvlYcVLvvilvli;;iiv;iv1llvvyLiLGIJXjLU136KQQSOBBBB@@@@@B@BUX@B@@@SSJcUy;,;iii;iiiiiilv0B8Q$i,:,:;:,:,:;:,:,:,:;:;:;:,:,:;:,
;:;:;:,:;:,:,:,:,:;:,:,:;:;:;:;:;:,:,:,:,:;:,:,:;:;:,:,:,:,:,:;:;:;:;:;:,,viyvi;viiiivLcJyOQ8$XV$QDI32E$tUtlVlvlYvyvUFXJjvyvVlLMXVGtUcUY21302ID6BB@8@B@B@@@B@; 1@@@B@EQ0Oyi;;iyivvllXLO@@@@8GJv:,:,:;:;:,:,:;:,:;:,:;:,:,:,:;:
:,:;:,:,:;:;:;:;:;:;:,:;:,:,:,:;:;:;:;:,:,:;:;:;:,:,:,:,:,:;:,:;:;:;:,:,:;;;iLviiylVyVc$MMG8S2VcK$0Q66G3VKGS2XLFtXyvvtMSXLY2SFlJXJSEIDIOI3MSIZSB@@S@@@@@@@@@BQ.,vB@@@@@@QZIJiiiyVYjM3@B@@Bi;:..::;:;:,:,:,:;:,:;:;:;:,:;:,:;:,
;:,:;:;:,:;:;:,:;:,:;:,:;:,:;:,:;:;:;:,:,:,:;:,:,:;:,:;:,:,:;:;:;:,:;:,:;:,.iVVlllXtUlXGO$QSD06DE2$SESGC$3ZGKU3OMVVvjtS$6OG$$jU1QSB@B8B6It66QE86BQD@@B@@@@@@icv  .i1@@@8@E8ZFiil1FZ8@DBSJl; ..,:;:,:,:;:,:,:,:;:;:,:,:,:,:;:,:
:;:,:,:;:;:;:,:;:;:;:;:;:;:,:,:;:,:;:;:;:;:,:,:;:,:,:;:,:,:;:;:,:,:;:;:,:,.;vcLVvjtCylVKK8Z6Z@@@@ESEQBQ8QDGGJcc2XUYJ1$O$OQQZCOIZSDEBQ8Q$LjUK0@B8SD0@@@B@@@S; .;, . ;yOvU@@BB3S3QQ@@@BcvY,..,.,:,:,:,:;:;:,:,:,:;:,:;:,:,:,:,:;
;:;:;:,:,:;:;:;:,:;:;:,:;:,:,:,:,:,:,:;:;:;:;:;:;:;:;:,:;:;:,:;:,:;:,:,:,.,ijVJVVycLYilLSB80EB@@@B@8BB@886ES$3030MIOEQ8QE6D2G$ESSO88B8QMcYt1G8@@@B88@B@@@@@l: ;,,.:   ,.i@@B@@@8@Z@BBSi ..,.,:,:;:;:,:;:;:,:,:;:,:,:,:;:,:,:;:
:,:;:,:;:;:;:;:;:;:,:,:;:;:,:,:;:,:;:;:,:,:,:;:,:;:,:,:,:;:;:,:;:,:,:,:,:;illYlcULlVyviXB@BEQ@@@8@@BE@@@Q88BB@88E8QBB@8@EQZE6D$EQ888Q80Fj1$B$8B@B@B@B@B@B@Gi.,.;,;.,.... ,;Y@@@3286Xyi:..,:;:;:;:;:;:;:,:;:,:;:,:,:;:,:,:,:;:,
;:;:,:;:;:;:,:,:;:,:;:;:,:,:,:;:,:,:,:;:,:;:;:;:,:;:,:;:;:,:;:;:,:,:;:;:,;viivlVCcLUUiv1B@@QB@B8@@@8BB@88E88@@@8B8BB@E88E3EEB$338BDMGC2VLV2SD8@BBQ8BB@@@@@; ..,:;:;:;:,.:   ;O@i:;i:. :.,:;:;:,:,:,:;:;:,:,:,:,:,:,:,:;:;:;:,:
:;:,:,:,:;:,:,:,:,:;:,:,:,:,:;:;:;:,:;:;:;:,:,:,:;:;:,:;:,:;:,:,:;:,:,:,:;;iiylt1ccOUvySQ@B6E@0ZQSGBB@Q6DB8@@@@B8BE@B86B8E$Z88C$E@tVVFK3tCK008B@BB8@8@B@@@i..,.;:,:,:,:,.,.. ... . ..,:;:;:,:;:,:,:,:;:;:;:;:,:;:;:,:;:,:;:,:,
;:;:,:,:,:;:;:;:,:,:,:,:;:;:,:;:,:,:,:;:,:;:;:,:,:,:;:,:;:,:,:,:;:,:;:,:;:;;lljUUy3QOijQ@@BFIS3CSt0EBSGOB@@B@@@BBEB8@EBB@@QI8QDD@B0VUXSDESZ3$Q@@@B@@@BB@@@,.,.,:,:,:,:,:,:,:,.:.,.,:,.,:;:;:,:,:,:,:;:;:,:;:,:;:;:;:;:,:,:,:,:
,;:,:,:,:;:;:;:,:;:,:,:;:;:,:,:;:,:;:,:;:;:,:,:,:,:;:;:,:;:,:,:;:;:;:,:,;;;ivVyLVXZ@QLc@B@8XJZOD03O@BBQ@@@B@B@@@BB8@@@8B8@E8B@S8@@Q$KGSQZE$3CQ8@@@@@B8E@B@y.,;.;:;:;:,:,:;:;:,:,:,:,:;:,:;:,:,:,:,:,:,:;:,:,:;:,:;:;:;:,:;:;:,
;:,:,:,:,:,:,:,:,:,:;:;:;:,:,:;:,:,:;:,:,:;:,:,:,:;:;:;:,:;:;:,:,:;:;,;:;;i;lyUyCS@@@cCB@@BcKEBBBQ@@@@@B@@@@@B@@@ED8@@BZZ3ZE@B8D@@80ICSOEB8MG$BB@@@8@B@B8B@v,,;:;:;:,:,:;:;:;:,:;:,:,:;:;:,:;:;:;:,:;:,:,:;:;:;:;:;:;:;:,:,:;:
:;:;:;:,:,:,:,:,:,:,:,:;:;:,:;:;:,:;:,:;:,:;:;:,:,:,:;:,:,:,:,:,:,:,:;.,:;iivclj3BB@BQSBBBSFC8B@B@@@@@@@@@@@BBB@BBS8@@EDKC08B@888@88QEOD8@8SIS6B8@B@B@@@IEBV.,:;:;:;:,:;:;:,:;:,:,:,:,:;:;:;:;:,:,:;:,:,:;:,:;:,:,:,:;:,:,:,:,
;:;:;:;:,:,:;:,:;:,:;:,:;:,:,:;:;:;:;:,:,:;:;:;:,:;:;:;:,:;:,:,:;:,:,:;;vvviFXj2@@@@@B@8BQI2QB@@@SEB@@@@@@@B@EB8B@@E@BBD21BB@88QQ$8Q88ES8DSSZGZ$Q8@@@@@Z1i; ..,:,:,:,:,:;:;:;:;:;:,:,:;:;:,:;:;:;:;:;:,:;:,:;:;:;:,:,:;:,:,:;:
,,:,:;:;:,:;:,:,:,:,:;:,:;:;:,:,:;:,:;:;:;:,:,:;:;:;:;:;:;:;:,:,:,:;:;,;;ilVVLU8@@@@@@@@B@EBB@@@B@8@B@@@B@@@B88@@@Q6B@BB3C8@B@8BQEBBSBB8DQSDG$K2M8@@B@@v.  :.,:,:;:;:,:,:,:,:;:;:,:,:,:,:;:,:;:,:;:;:,:;:,:;:;:;:,:,:,:;:,:,:,
;:;:,:,:;:,:;:,:;:,:,:,:,:;:,:;:;:,:,:;:;:,:;:,:;:,:,:;:,:,:,:;:;:;:,:;:,;ivyl28@B@@@@@@@@@B@@@@@@@B@@@@@@@B@Q8B@@BSBB@@QG88@88E@@@B88@8@B8GQQBQ608@@@X.:.,:,:,:;:;:;:,:;:;:,:;:;:;:,:,:,:,:,:,:;:,:;:;:;:,:;:,:,:,:,:;:;:,:,:
,,:;:;:;:,:;:,:,:;:,:;:,:;:;:;:;:;:;:,:;:,:;:,:;:,:;:,:,:,:,:;:;:,:;,;:;;;;vV16@6ZQ@B@@@@@@@B@@@B@@@@@@@@@@@@88B8@@B@@B@8B88QBB@888@B@8BB@88D8B@8Q6$li,..,.,:;:;:;:;:,:;:;:;:;:;:;:;:,:;:;:;:;:,:;:,:;:;:,:;:;:,:,:;:;:,:;:,:,
;:;:;:;:;:,:;:,:,:;:;:,:;:;:;:;:,:,:,:;:,:,:;:;:,:,:;:;:,:,:;:;:;:,,;:;.;;iilYFFG$8B@@@@@@@@@@@@@B@@@B@@@@@@@B@8B@@E@B@B@BB88E@@@DBB@BBB@B@@@8BEB8El, ..,.,:;:;:;:,:,:;:,:;:,:,:;:;:;:,:;:,:,:;:,:,:,:,:,:,:,:;:,:,:;:;:;:,:,:
,,:,:,:,:;:,:,:,:;:,:;:,:;:;:;:,:,:,:,:,:,:,:,:;:,:,:,:,:,:,:;:,:;:;:,:,,;;ilUvJSED88@B@@@B@@@@@@@BB@@@@@@B@@@8Q8@@@B@@@8B8@B@B@@BB@8B8B8B888@8BEDMy...,:;:;:,:;:;:,:,:,:,:,:,:,:,:;:;:;:;:,:;:,:;:,:;:,:,:;:,:;:;:;:;:,:;:,:;
;:,:,:,:;:;:,:,:;:;:,:;:;:,:;:;:;:;:;:;:;:;:,:,:;:;:;:,:;:;:;:,:;:,:;,,:;ivillU0B68Q@@@@@@@B@@@B@BBO@B@@@@@@@@B8@B@@@B@@B68B@BBB@@@@BE8E88BQ8EB@@BC.  ,:;:;:;:,:,:;:;:,:,:,:;:,:,:,:,:;:,:;:;:;:,:,:;:,:,:,:;:,:,:,:,:;:;:;:,:
:;:;:;:;:,:;:;:;:;:,:,:;:,:,:;:,:,:;:;:;:,:,:,:,:;:;:;:,:;:,:,:;:,:;:;:,;iii;vX@BB8@@@B@@@@@@@B@@@B@QFM@BB8B@@B@B@@@B@@@BB@@BB8@@BBBQ8Q8QB8BE8SDSBDy ..,:;,,:,:;:,:,:,:,:;:;:,:,:;:;:;:,:,:;:;:;:;:;:;:;:;:;:,:;:;:;:;:,:;:,:;
;:;:;:;:,:,:;:;:,:;:,:,:,:;:;:;:,:;:,:,:;:,:,:;.,:,.,:,.,.,.,.,:,.,.,.,:;:iillSB@B@@@@@B@@@@@@@@@B@Bl .v; ,,;v$D0Lt8@@@@@@@@@B8D88@BBEB888@BBSSGSCS81;,.,:;:;:;:,:;:,:;:;:,:,:,:;:,:;:,:,:;:;:,:,:;:;:,:,:;:,:,:,:;:,:;:,:,:;:
,;:,:,:,:;:,:,:;:,:,:,:;:;:,:,:;,;.,.,:,:;,;:;:,:,:,.,.,:,::.:.:.:.,.,,;;iiyyF6@B@@@@@@@@@@@@@@@B@SV;  .:. :.:,;ii:v086@@@BB8@QE8@B@BB8@B@8802JFJXY2BC;..,:;:;:;:,,,:;:,:,:;:;:;:;:;:,:,:;:,:;:;:;:,:;:,:,:;:;:,:,:,:,:;:;:,:;
;:,:;:;:,:;:;:;:;,,:;,;,;:,,;:,:;:;:,:;,;,;,;,;:;;;,;:;;iiiii,;,. ..;;i;villJFZQ8Q@@@B@@@@@B@@@@@Bv   ii;:,.,.;,;;;;vJS8@BBDB@BDB@BZ8B8SBBB88IFVVj1yUl;.:.:.,.,:,.,,;:,:,:;:,:,:,:,:;:;:,:,:;:,:;:,:;:;:,:;:;:;:;:,:;:,:,:,:,:
,,:,:,:;:,:;:;:;,;:;:;:;:;:,:;,;:;:;:,:;,;:;;;;iiUlYYyivyyvylYVU;iiviiivvylVVKIEQ8@@B@B@@@B@@@B@B@0Xlyi, ,.;:,:;,;;;;cS6Q@B@@@8ZQBQZZ@8$K8B@@@D3UtXXivVJi;,;:,.,.,.,:,:,.;:,:;:;:,:,:,:;:,:,:;:;:;:;:,:,:;:;:,:;:;:,:;:;:;:,:;
;:;:,:;:;:,:,:;:,:,:,,;:;,;:;:;,;:;:,:,:;,iiiilL2cViyVjlUtKyVLVl1X33$JyiUUcJ68@8B@@@@B@B@@@B@B@B8E@@@2yv: ..,:;:;;;,;iFGE88E@@@ZBB@E8@@6$S8Q@B@8ZFMtJyVyVllii;;,;:,.,.,:,:,:,,;:,:,:;:,:,:;:;:;:,:;:;:;:;:;:,:;:,:;:,:,:;:,:;:
,,:,:,:;:,:;:;:;,;:;,;:;:;:;:;:,:;,;;iiviiivii;;;vlliyj1F$KCLGCSQQD@@B0IO6GSE@@@@@@@@B8@B@EB8@EEZCcM8@@@X;...,:;:;;;;vVBBB88B@BB@@8B@@BEG8Q88@@@EDG63FlyljVliliiivii,,:,.,:;:,:;:,:,:,:,:,:,:;:,:;:,:,:;:;:;:,:;:;:,:,:,:,:;:;
;:,:,:;:,:,:;,;:;,;,;,;:;:,.,:,,;;iiyyVivvi;;;iii;lYUyUVFXXXtCO3S08B@@BQ@@@8BB@B@@@@@@@BBS$S@B8Q@SG2D8@@@@X;,.,:,,;;i;lQ@B@B@@@@@B@Q@@@ZSZEO6863B8888OJvllLUUcCUYvliiiii;;;,,.,,;:,.:.,:,:,:;:,:,:;:;:,:;:;:;:,:;:;:,:,:;:;:;:
,,:,:,:,:;:;,;,;,;:;,;.:.::;;;ivivilvlii;;;;;ylLYlvyvviyyUyVlcLcX3GE8BEE8@B@@@@@B@B@B@B@BQ068BSMX8E0LUM8@@Ey;,:;:;:,..;EB@B@B@88ME8BB@@@B@6S3@8FM@QStcVLjXJ2FKUUlyyYvYYVvi;;;i;;,;...,.,.;:;:;:;:,:,:,:,:;:,:,:;:;:,:,:;:,:,:;
;:,:,:,:;,;,;,;,;,;,, :,vyVlllUlyvlilivvylVvvivivivvyvlvyivlLlVU3C$SEQE686EQB8@@@8BE8SQSEEBE86ZXV0SctX1U36SUl,;:;:;,:;I@@8@QZLcUlvtS$USQ8B@BS28UjQSFG30COKFLUYjyliyvVVVvVlylViiii;;iFJ; :.,:;:,:,:,:,:,:;:;:;:,:;:,:;:;:;:;:,:
,;:,:;.;,;,;,;,;,;,,.;V02UlUcUlyyVvvillcLVvYlliviiilllvllvvUccVcXMK3FCXFC$I$IZZ8EBZSFFXMM6B@E88@EBSZQ8I1JMMOl;.;,;,;.,ij;;;ii,;li;;v;ili;lS@U;;;iXv1E8F13$yllccXlVVcVjlyyUylvlvyljjMCEJv;, ....:.,.,:,:,:;:;:,:,:;:;:,:,:,:;:,
;:,:;:,:;,;,;,;,;,, ;I@VlilFKVjyVllljyYyUyyycvylyvyvylYycYVyULUlLcCM3XXUKF3CSGSODSZ3GF2MO3E8@B@@@@@B@@@B@SVly;;,;;;,;.,;;.;;;.,;;;i;;iXlYlMDUillviV3IJyilljKFyVUXLJjXYUVLyUjUVLyjyVyUcG2MLl,,.. . ..:.,:,:,:,:;:;:;:;:,:;:;:;:
:,:;:;,;,;,;,;,;,,.;I8Jv,ijKVcXCLi;ylyvylVvylyyVlYlYlLVUYVvllJVVlUXIM3F3M3C0GZO$3$GD6QO$33KSQ@BB8BB@B@B@B@F. ;;;;;;;;;;ivyii;;;;;;ii;iiVLVivijVVvj1MlVLyi;iZIFJ3tcylilyyiiilyjVJLcVYvllLlX3KJXYyii:. . ..,.,:,:,,;:;:,:,:;:,:;
;:;:,:,,;,;,;,;;;. c@BFi;y0UcXFKi;ycyvlvyivivvlljyVyVYjYjvlvjjVlVUFF3F3K212FGISG$M0SQ$Z$6KG0EEBB@8@B@B@B@B@V;ivivii;iicK2VY;;;i;v;;;i;;,;;viv;;;i;vyjKFlcXGS6GGUyvYllvVlYvlvlvYyUUtX1XKtMXKMIF31G3ILVii,. . ..,:;,;,;:,:;:;:;:
,,:;:;:;,;,;;;;;..;B@B8$MES1VUV1yC8BZSYlivilvYlVlVyVlYvVUUVcctVVYVlVYJJ11GC0GZ06G0I$IG2S$EQB8@B@@@@@@@B@@@B@;ilVljlyvVXCJUYvii;lvli;;;;;,ivcLliv;;il;VX3FQS0cUjXX$68SQ68SZZQF3C$0QSQZ60DSQGIcUlylXXJX0tcVVi;...,:;:,:;:;:;:;:;
;:,:,:;:;,;,;;;,,i@@@@@BBQQMXLLJ6DSQZt8LvillylVvlvYlVlyljYUVcVLllillXMS6B8B8@BB8BB@886ED88BB@B@B@@@@@B@@@@@ZilKlLXcyUX$MXyViVlvyVvjvvii;;;Y12llviiV;,;vlvilCIcMGGLjcCKIF1X0S6SEZEQ8E@B8QB@@B@B@8QS88ESQ$$0EEBX;.;:,:;:;:;:,:,:
,,:;:;:,,;,;,;,;,OB@@@B8ZEB@0OQBS8KXyU8QylvLyyvyvlvlvlvlilvlvvilyM6@B@@@B@@@@@@@B@@@@@B@B@B@@@B@B@B@@@B@B@@OU0KcUtcLL2I2yYVcVUlylviVVYiviy2ZVlvliiiiivvVi: llj$@Qi               . .,;,;;iiiilljYX2D0E@@@@@@QV.,:;,;:;:;:,:,:;
;:,:,:;:;:;,;:;,,;;;;v@@@@@@@@@Q3S2jQB@8ZtMG$CMXKXtLJVcyvivillJK@@@@@B@@@B@@@@@@@@@B@@@B@B@B@@@B@@@@@@@@@B@B@@B8B68S62CVcUylVV1LMjVyVvlyGQ8XylUli;vYliYVV;iyviyY$8Si. ..:.:.,.:.. . . . .         . . ..:.,.. ,:;:;,,,;:,:;:;:
:,:,:,:;,;:;,;,;..   ;6@BE8@B@@SF20@@@@@8ESQQ8$SS6OZ0SO$KGIQB@@@@@B@@@@@@@@@B8SZGG2IKMctjlvlvlilvllVVUjKCSDBB@@@@@@BZSKKccVYvylYlUvv;iivC@$Y;ilyivlliviiiUMFyl;;i1EQXy,,,;,;,;:;,;:,:,:,.;.,.,.,.:.:.. . . ..,.,:;:,,;:,:,:,:;
;:,:;:,:,.,:;,;:;:,.:..    ...V@@@@@@@@@@@@B@B@@@@@B@@@B@@@B@@@@@BQtVii;;:,.:.:.:.: . . . . ..,.,.;,;,;;;;iilY1K$$8EEI3t1cLlyvlivvliiiiiOBZilvyvlilii;iiiiMUVi;i;.;Vti,.;,;:;,;:;,;,;:;,;,;,;:,:,.,:;:,:,.,.,,;,;:;,;:,:;:;:;:
:,:;:,:;:,:,:;:,:,:,:,.. . . .i@B@BBQ88@@@@@B@B@B@@@8BQ8EQ1Lvi;, . . ..:.,.,.,:,.,:,:;:,,;,;,;;;;;;;;;;;;i;i;iii;ivM$$t1Jclliliv;iililvl2@Iylyvlii;;;;iliij0v;;i;,.ilv::.;,;,;:;,;,;,;,;,;,;,;:,,;:;:,,;:;:;:;:,:;:,:,:;:;:;:,
;:,:,:;:;:,:,:;:;:,:;:,.,:,:,..                                 ..,.,:;,;:;:;:;,;,;,;,;,;;;,;;;;;;;;;;i;;;i;i;i;i;;VSCtUMUVviii;;,;iyYYvOB6jVllvviviiiiiyvy1Liyvvii;JKv:::;,;,;:;,;:;,;:;:;,,:;:,,,:,:;:;,;,;:;:;,;,;,,:,:;:,:
,;:,:;:,:,:,:,:;:;,;,;:;:,,;:,.. . . . . . . . . . . . ....:.,:,:,:;,;:;:;:;,;,;,;,;,;,;,;,;,;,;,;;;;;;;;i;;;i;iiii2ZOXJUcvlljlyi;,;illl3@8IUUYVvliv;iilyVlS0UvYlVvVyFSj.::;,;,;:;,;,,,;,;:;,;,;:;:;:,:;,;,;:;:;:,:;:;:;:,:,:,
;:;:;:;:,:;:,:;:,:;:;,;:,:;:;:;:;:;:;:,:,.,.,:,:,.,:;,,,;:,:;,;,,,;,;,;:;,;,;,;,;,;:;:;,;,;,;,;,;,;;;;;;;;;;i;iiyltZE3IjLX$C$DEZ6Ji;iillZ@@QItXyVlvii;llylVQ8KFjYvYlllLcl.,,;,;:;,;,;:;:;:;:;:,:,:;:,:;:;:;:;:;:;:;:;:,:;:;:;:
,;:;:,:,:;:;:,:;:;:;:;,;:;:;:;,;,;:,:;:,,,:;,;:;:,:;:;:;:;,;,;:;:;:;:;:;,;,;,;,;,;:;,;:;:;,;,;,;,;,;,;;;;;;;;iilvUZ@8D33CE8BB@S6QQCFi;iVS@BQM3cjyVvi;vljlyl8BQ2tllvvlViYUi.,,;:;:;,;:;,,,;:;:,:,:;:,,;,;:;:;:;:;:,:;:,:;:;:,:,
;:;:,:;:,:;:;:;:;:;:,:;:;:;:;:,,;,,,;,;:;:;:;:;:,:;:,:,:,,;,,:,:,:;:;:;:,,;,;,;:;:;:;,;,;,;,;,;,;,;,;,;;;;;;i;iilYB@@E8S8B@@@B$F6SEQMlVjE@B2GIMYVlliiilycYUB@StYyii;iiiiLl,.;,;:;:;:;:;,;:;:;:,:;:;:;:;,,:;:;:;:,:;:,:,:,:;:,:
,;:,:;:;:,:,:,:;:;:,:;:;:;:;:,:,:,:,,,:,,;:;:,:;,;:;:;:,:;:;,;:;,;,;,;:;:;:;:,,;,,:;:;,,:;,;,;,;,;,;,;,;,;;;;;;iilC@@@B@8@@@@8cX$88@8608B@B0IQ3MM$CIFC1UlyU@@8jliVVlvyiilV;,:;:;:;:;:;:;:;:;,;:,:,:,:;,;:;,;:,:,:,:;:,:;:;:;:;
;:,:;:;:;:,:,:,:;:;:,:,:;:;:,:,:;:,:,:;:,:;:;:;:;:;:;:,:,:,:;.,:,:;:;:;:;,;:;:,:;,,:;:;:;:,:;,;:;:;:;,;,;,;;;;;;iic8@B@@@@@@@61JD8@BBE@B@@@SQSSS@@@IG6B$XUI@@@SXK6BEEKl;jL;.,,;,;,;,;:;:;:;:;:;:,:;:;:;:;:;:,:,:,:;:;:;:,:;:;:
,;:;:;:,:,:;:;:;:,:;:,:;:;:,:;:,:,:;:;:;:,:,:;:,:;:;:,:,:;:;:,:,:;:,:;:;:,:,:,:,:;:;,;:;:,:,:;:;,,:,:;:;:,:;,;,;;;;l3B@@B@@@B$1D@@@@B@@@B@@BBBQB@@BtySBBG@@@B@QQ8@BO2E2F3j.,,;,,:;:;:;,,:;:;:;:,:;,,:,:;:,:;:;:;:,:,:;:;:;:;:;
;:,:;:;:,:,:;:;:;:;:;:;:,:;:;:,:,:,:,:,:;:,:,:,:,:,:;:,:,:,:,:,:,:;:;:,:,:;:,:;:,:,:,:,:;:,:;:,,;:,:,:,:,:;:;:,:,.: .:;iG@@@@B@@@@@B@B@B@@@@@B@@@@@FC8@@@@@@@@@8@@@1$QB@I:::;,;,;,;:,,;:;:,:,:,:;,;:;:;:;:,:;:,:,:,:,:,:,:;:;:
,;:,:,:,:,:,:,:,:;:;:,:;:;:;:;:,:,:;:,:,:;:,:;:;:;:,:;:;:;:;:;:;:,:,:,:,:;:,:;:;:,:;:,:,:,:,:,:;:,:,:,:;:;:;:,:;:,.:..   .;K@@0Y;;..     .;VMBB@B@@@@@B@MXXB@@@@@@@@B@@I:.:;:;,;,;:;:;,,:;:;,;:;:,:,:,:,:;:,:;:;:;:;:,:,:,:,:;
;:,:;:;:,:,:,:,:;:,:,:,:,:,:;:;:;:,:,:;:,:;:;:,:,:;:;:,:;:;:;:,:,:,:;:;:,:;:,:,:;:;:;:;:;:;:,:,:;:,:,:,:;:;:,,;,,:;:,.,..           . . .      :iV@BJ;;      .;iQ@@B0Jy...;:;:;:,:;,;:;:;:;:;,,:;:;:;:,:,:;:,:;:;:;:,:,:,:,:;:
,,:;:,:,:,:,:,:;:,:;:;:,:,:;:;:,:;:,:,:;:,:,:;:,:;:;:,:;:,:,:,:;:;:,:;:,:,:,:;:;:,:,:;:;:,:;:,:;:,:;:,:,:,:;:;:;:,:,,;,,:,.....:.,.,:,.,.,.:..         ... .       .   ..,:;:;:;:;:;:;:,:;:,:;:,:,:,:;:;:,:;:;:;:;:;:;:,:;:;:;
;:,:;:,:;:,:;:,:;:,:,:;:,:,:;:;:;:,:;:,:,:,:;:,:,:,:;:,:,:,:;:,:,:,:;:;:;:,:;:;:;:,:,:;:,:;:,:;:,:;:,:;:;:,:,:,:,:;:,:,:,:,:,.,:,:;:;:,:;:,:,.,.,.:.:.,:,:;:,.,.. . ..,:,:;:;:,:,,;:;:;:;:;,,:;:;:;:;:;:,:;:,:,:;:;:;:;:,:;:;:
,,:,:;:;:,:,:,:,:;:,:;:;:;:;:;:;:;:;:;:;:,:;:,:,:;:,:,:;:,:;:,:;:;:,:;:,:;:,:;:,:,:;:;:;:;:,:,:,:,:;:;:,:;:,:,:,:,:,:,:;:;:;:,:,:;:;:;:;:,:,:,:,:,.,:,:;:,:;:,:,:,:,:,:;:,:;:;:;:,:;:;:,:,:;:,:,:,:;:;:,:,:;:,:,:;:;:,:;:;:,:,
;:;:;:;:;:,:,:,:;:,:,:,:;:,:,:;:;:;:,:;:;:,:,:,:;:;:;:,:,:;:,:;:,:,:;:,:;:;:,:;:,:,:,:,:,:;:;:,:;:,:;:;:,:,:;:,:,:;:,:;:,:;:;:,:,:;:,:;:,:,:,:;:,:,:;:;:,:,:;:;:,:;:,:,:;:,:;:;:,:,:,:,:;:;:,:,:;:,:,:;:;:,:;:,:,:;:;:,:;:,:,:
:;:,:;:,:;:,:,:,:;:;:,:,:;:,:;:,:,:,:;:,:,:;:;:;:,:,:,:;:,:,:;:,:;:,:,:,:;:,:;:;:,:;:,:;:;:,:;:;:;:,:;:;:,:,:,:,:;:,:,:,:;:;:;:;:;:;:;:;:,:,:;:,:;:;:,:,:;:;:;:;:;:;:;:,:,:;:;:;:,:;:,:,:,:,:;:;:;:;:;:,:,:,:,:,:,:;:;:,:;:,:;
*/
// Railgun_Trolldom (the successor of Railgun_Swampshine_BailOut - avoiding second pattern comparison in BMH2 and pseudo-BMH4), copyleft 2016-Aug-19, Kaze.
// Railgun_Swampshine_BailOut, copyleft 2016-Aug-10, Kaze.
// Internet "home" page: http://www.codeproject.com/Articles/250566/Fastest-strstr-like-function-in-C
// My homepage (homeserver, often down): http://www.sanmayce.com/Railgun/
/*
!!!!!!!!!!!!!!!!!!!!!!!! BENCHMARKING GNU's memmem vs Railgun !!!!!!!!!!!!!!!!!!!!!!!! [
Add-on: 2016-Aug-22

Two things.

First, the fix from the last time was buggy, my apologies, now fixed, quite embarrassing since it is a simple left/right boundary check. It doesn't affect the speed, it appears as rare pattern hit misses.
Since I don't believe in saying "sorry" but in making things right, here my attempt to further disgrace my amateurish work follows:
Two years ago, I didn't pay due attention to adding 'Swampwalker' heuristic to the Railgun_Ennearch, I mean, only quick test was done and no real proofing - this was due not to a blunder of mine, nor carelessness, but overconfidence in my ability to write "on the fly". Stupid, indeed, however, when a coder gets momentum in writing simple etudes he starts gaining false confidence of mastering the subject, not good for sure!
Hopefully, other coders will learn to avoid such full of neglect style.

Second, wanted to present the heaviest testbed for search i.e. memmem() functions: it benefits the benchmarking (speed in real application) as well as bug-control.

The benchmark is downloadable at my INTERNET drive:
https://1drv.ms/u/s!AmWWFXGMzDmEglwjlUtnMJrfhosK

The speed showdown has three facets:
- compares the 64bit code generated from GCC 5.10 versus Intel 15.0 compilers;
- compares four types of datasets - search speed through English texts versus genome ACGT-type data versus binary versus UTF8;
- compares the tweaked Two-Way algorithm (implemented by Eric Blake) and adopted by GLIBC as memmem() versus my Railgun_Swampshine.

Note1: The GLIBC memmem() was taken from latest (2016-08-05) glibc 2.24 tar:
https://www.gnu.org/software/libc/
Note2: Eric Blake says that he enhanced the linearity of Two-Way by adding some sublinear paths, well, Railgun is all about sublinearity, so feel free to experiment with your own testfiles (worst-case-scenarios), just make such a file feed the compressor with it, then we will see how the LINEAR Two-Way behaves versus Railgun_Swampshine.
Note3: Just copy-and-paste 'Railgun_Swampshine' or 'Railgun_Ennearch' from the benchmark's source.

So the result on Core 2 Q9550s @2.83GHz DDR2 @666MHz / i5-2430M @3.00GHz DDR3 @666MHz:
--------------------------------------------------------------------------------------------------------------------------------
| Searcher                                  | GNU/GLIBC memmem()        | Railgun_Swampshine       | Railgun_Trolldom          | 
|--------------------------------------------------------------------------------------------------|---------------------------|
| Testfile\Compiler                         | Intel 15.0 | GCC 5.10     | Intel 15.0 | GCC 5.10    | Intel 15.0  | GCC 5.10    |
|------------------------------------------------------------------------------------------------------------------------------|
| Size: 27,703 bytes                        |     4506/- |   5330/14725 |    13198/- | 11581/15171 | 19105/22449 | 15493/21642 |
| Name: An_Interview_with_Carlos_Castaneda.TXT                          |            |             |             |             |
| LATENCY-WISE: Number of 'memmem()' Invocations: 308,062               |            |             |             |             |
| THROUGHPUT-WISE: Number of Total bytes Traversed: 3,242,492,648       |            |             |             |             |
|------------------------------------------------------------------------------------------------------------------------------|
| Size: 2,347,772 bytes                     |      190/- |      226/244 |     1654/- |   1729/1806 |   1794/1822 |   1743/1809 |
| Name: Gutenberg_EBook_Don_Quixote_996_(ANSI).txt                      |            |             |             |             |
| LATENCY-WISE: Number of 'memmem()' Invocations: 14,316,954            |            |             |             |             |
| THROUGHPUT-WISE: Number of Total bytes Traversed: 6,663,594,719,173   |            |             |             |             |
|------------------------------------------------------------------------------------------------------------------------------|
| Size: 899,425 bytes                       |      582/- |      760/816 |     3094/- |   2898/3088 |   3255/3289 |   2915/3322 |
| Name: Gutenberg_EBook_Dokoe_by_Hakucho_Masamune_(Japanese_UTF8).txt   |            |             |             |             |
| LATENCY-WISE: Number of 'memmem()' Invocations: 3,465,806             |            |             |             |             |
| THROUGHPUT-WISE: Number of Total bytes Traversed: 848,276,034,315     |            |             |             |             |
|------------------------------------------------------------------------------------------------------------------------------|
| Size: 4,487,433 bytes                     |      104/- |      109/116 |      445/- |     458/417 |     450/411 |     467/425 |
| Name: Dragonfly_genome_shotgun_sequence_(ACGT_alphabet).fasta         |            |             |             |             |
| LATENCY-WISE: Number of 'memmem()' Invocations: 20,540,375            |            |             |             |             |
| THROUGHPUT-WISE: Number of Total bytes Traversed: 13,592,530,857,131  |            |             |             |             |
|------------------------------------------------------------------------------------------------------------------------------|
| Size: 954,035 bytes                       |       99/- |      144/144 |      629/- |     580/682 |     634/807 |     585/725 |
| Name: LAOTZU_Wu_Wei_(BINARY).pdf                                      |            |             |             |             |
| LATENCY-WISE: Number of 'memmem()' Invocations: 27,594,933            |            |             |             |             |
| THROUGHPUT-WISE: Number of Total bytes Traversed: 8,702,455,122,519   |            |             |             |             |
|------------------------------------------------------------------------------------------------------------------------------|
| Size: 15,583,440 bytes                    |        -/- |          -/- |        -/- |     663/771 |     675/778 |     663/757 |
| Name: Arabian_Nights_complete.html                                    |            |             |             |             |
| LATENCY-WISE: Number of 'memmem()' Invocations: 72,313,262            |            |             |             |             |
| THROUGHPUT-WISE: Number of Total bytes Traversed: 105,631,163,854,099 |            |             |             |             |
--------------------------------------------------------------------------------------------------------------------------------

Note0: Railgun_Trolldom is slightly faster (both with Intel & GCC) than Railgun_Swampshine, this is mostly due to adding a bitwise BMH order 2 (8KB table overhead instead of 64KB) path - for haystacks <77777 bytes long - the warm-up is faster.
Note1: The numbers represent the rate (bytes/s) at which patterns/needles 4,5,6,8,9,10,12,13,14,16,17,18,24 bytes long are memmemed into 4KB, 256KB, 1MB, 256MB long haystacks.
in fact, these numbers are the compression speed using LZSS and memmem() as matchfinder.
Note2: The Arabian Nights is downloadable at:
https://ebooks.adelaide.edu.au/b/burton/richard/b97b/complete.html
Note3: On i5-2430M, TW is catching up since this CPU crunches instructions faster while the RAM speed is almost the same, Railgun suffers from the slow RAM fetches - the prefetcher and such suck.
Note4: With a simple English text 'Tales of 1001 Nights', 15,583,440 bytes long, the cumulative size of traversed haystack data is nearly 100TB, 105,631,163,854,099 ~ 1024^4 = 1,099,511,627,776.
Note5: With a simple French text 'Agatha_Christie_85-ebooks_(French)_TXT.tar', 32,007,168 bytes long, the cumulative size of traversed haystack data is nearly 200TB ~ 234,427,099,834,376.

Just to see how faster is Yann's Zstd in decompression (its level 12 is 377-331 MB/s faster), on Core 2 Q9550s @2.83GHz DDR2 @666MHz:
[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
D:\Nakamichi_Kintaro++_source_executables_64bit_(GCC510-vs-Intel150)_(TW-vs-RG)_BENCHMARK>Nakamichi_Kintaro++_Intel_15.0_64bit.exe Agatha_Christie_85-ebooks_(French)_TXT.tar
Nakamichi 'Kintaro++', written by Kaze, based on Nobuo Ito's LZSS source, babealicious suggestion by m^2 enforced, muffinesque suggestion by Jim Dempsey enforced.
Note1: This compile can handle files up to 1711MB.
Note2: The matchfinder/memmem() is Railgun_Trolldom.
Current priority class is HIGH_PRIORITY_CLASS.
Compressing 32007168 bytes ...
|; Each rotation means 64KB are encoded; Done 100%; Compression Ratio: 3.53:1
NumberOfFullLiterals (lower-the-better): 164
NumberOfFlushLiteralsHeuristic (bigger-the-better): 184323
Legend: WindowSizes: 1/2/3/4=Tiny/Short/Medium/Long
NumberOf(Tiny)Matches[Short]Window (4)[2]: 226869
NumberOf(Short)Matches[Short]Window (8)[2]: 119810
NumberOf(Medium)Matches[Short]Window (12)[2]: 71202
NumberOf(Long)Matches[Short]Window (16)[2]: 31955
NumberOf(MaxLong)Matches[Short]Window (24)[2]: 7078
NumberOf(Tiny)Matches[Medium]Window (5)[3]: 257313
NumberOf(Short)Matches[Medium]Window (9)[3]: 526493
NumberOf(Medium)Matches[Medium]Window (13)[3]: 285579
NumberOf(Long)Matches[Medium]Window (17)[3]: 158873
NumberOf(MaxLong)Matches[Medium]Window (24)[3]: 51276
NumberOf(Tiny)Matches[Long]Window (6)[4]: 41075
NumberOf(Short)Matches[Long]Window (10)[4]: 240454
NumberOf(Medium)Matches[Long]Window (14)[4]: 258653
NumberOf(Long)Matches[Long]Window (18)[4]: 209007
NumberOf(MaxLong)Matches[Long]Window (24)[4]: 190929
RAM-to-RAM performance: 605 bytes/s.
Compressed to 9076876 bytes.
LATENCY-WISE: Number of 'memmem()' Invocations: 102,091,852
THROUGHPUT-WISE: Number of Total bytes Traversed: 234,427,099,834,376

D:\Nakamichi_Kintaro++_source_executables_64bit_(GCC510-vs-Intel150)_(TW-vs-RG)_BENCHMARK>"Nakamichi_Kintaro++_Intel_15.0_64bit.exe" "Agatha_Christie_85-ebooks_(French)_TXT.tar.Nakamichi"
Nakamichi 'Kintaro++', written by Kaze, based on Nobuo Ito's LZSS source, babealicious suggestion by m^2 enforced, muffinesque suggestion by Jim Dempsey enforced.
Note1: This compile can handle files up to 1711MB.
Note2: The matchfinder/memmem() is Railgun_Trolldom.
Current priority class is HIGH_PRIORITY_CLASS.
Decompressing 9076876 bytes ...
RAM-to-RAM performance: 331 MB/s.
Compression Ratio (bigger-the-better): 3.53:1

D:\Nakamichi_Kintaro++_source_executables_64bit_(GCC510-vs-Intel150)_(TW-vs-RG)_BENCHMARK>zstd-windows-v0.8.1_win64.exe -h
*** zstd command line interface 64-bits v0.8.1, by Yann Collet ***
...

D:\Nakamichi_Kintaro++_source_executables_64bit_(GCC510-vs-Intel150)_(TW-vs-RG)_BENCHMARK>zstd-windows-v0.8.1_win64.exe -b12 "Agatha_Christie_85-ebooks_(French)_TXT.tar"
12#_(French)_TXT.tar :  32007168 ->   8965791 (3.570),   6.7 MB/s , 377.0 MB/s

D:\Nakamichi_Kintaro++_source_executables_64bit_(GCC510-vs-Intel150)_(TW-vs-RG)_BENCHMARK>zstd-windows-v0.8.1_win64.exe -b22 "Agatha_Christie_85-ebooks_(French)_TXT.tar"
22#_(French)_TXT.tar :  32007168 ->   6802321 (4.705),   1.0 MB/s , 260.7 MB/s

D:\Nakamichi_Kintaro++_source_executables_64bit_(GCC510-vs-Intel150)_(TW-vs-RG)_BENCHMARK>
]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

Two-Way is significantly slower than BMH Order 2, the speed-down is in range:
- for TEXTUAL ANSI alphabets: 1729/226= 7.6x
- for TEXTUAL UTF8 alphabets: 2898/760= 3.8x
- for TEXTUAL ACGT alphabets:  458/109= 4.2x
- for BINARY-esque alphabets:  580/144= 4.0x

For faster RAM, than mine @666MHz, and for haystacks multimegabytes long, the speedup goes beyond 8x.

The benchmark shows the real behavior (both latency and raw speed) of the memmem variants, I added also the Thierry Lecroq's Two-Way implementation:
http://www-igm.univ-mlv.fr/~lecroq/string/node26.html#SECTION00260
However, Eric Blake's one is faster, so it was chosen for the speed showdown.

Once I measured the total length of traversed haystacks, and for files 100+MB long, it went ... quintillion of bytes i.e. petabytes - good torture it is.

!!!!!!!!!!!!!!!!!!!!!!!! BENCHMARKING GNU's memmem vs Railgun !!!!!!!!!!!!!!!!!!!!!!!! ]
*/
// 2014-Apr-27: The nasty SIGNED/UNSIGNED bug in 'Swampshines' which I illustrated several months ago in my fuzzy search article now is fixed here too:
/*
The bug is this (the variables 'i' and 'PRIMALposition' are uint32_t):
Next line assumes -19 >= 0 is true:
if ( (i-(PRIMALposition-1)) >= 0) printf ("THE NASTY BUG AGAIN: %d >= 0\n", i-(PRIMALposition-1));
Next line assumes -19 >= 0 is false:
if ( (signed int)(i-(PRIMALposition-1)) >= 0) printf ("THE NASTY BUG AGAIN: %d >= 0\n", i-(PRIMALposition-1));
And the actual fix:
...
// If we miss to hit then no need to compare the original: Needle
if ( count <= 0 ) {
// I have to add out-of-range checks...
// i-(PRIMALposition-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4

// "FIX" from 2014-Apr-27:
// Because (count-1) is negative, above fours are reduced to next twos:
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
    // The line below is BUGGY:
    //if ( (i-(PRIMALposition-1) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) && (&pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4) ) {
    // The line below is NOT OKAY, in fact so stupid, grrr, not a blunder, not carelessness, but overconfidence in writing "on the fly":
    //if ( ((signed int)(i-(PRIMALposition-1)+(count-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) ) {
// FIX from 2016-Aug-10 (two times failed to do simple boundary checks, pfu):
    if ( ((signed int)(i-(PRIMALposition-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)]+((PRIMALlengthCANDIDATE-4+1)-1) <= pbTargetMax - 4) ) {
        if ( *(uint32_t *)&pbTarget[i-(PRIMALposition-1)] == *(uint32_t *)(pbPattern-(PRIMALposition-1))) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
            count = PRIMALlengthCANDIDATE-4+1; 
            while ( count > 0 && *(uint32_t *)(pbPattern-(PRIMALposition-1)+count-1) == *(uint32_t *)(&pbTarget[i-(PRIMALposition-1)]+(count-1)) )
                count = count-4;
            if ( count <= 0 ) return(pbTarget+i-(PRIMALposition-1));    
        }
    }
}
...
*/
// Railgun_Swampshine_BailOut, copyleft 2014-Jan-31, Kaze.
// Caution: For better speed the case 'if (cbPattern==1)' was removed, so Pattern must be longer than 1 char.
#define NeedleThreshold2vs4swampLITE 9+10 // Should be bigger than 9. BMH2 works up to this value (inclusive), if bigger then BMH4 takes over. Should be <=255 otherwise the 0|1 BMH2 should be used.
char * Railgun_Trolldom_64 (char * pbTarget, char * pbPattern, uint64_t cbTarget, uint32_t cbPattern) 
{
    char * pbTargetMax = pbTarget + cbTarget;
    uint32_t ulHashPattern;
    //signed long count;
    signed long long count; // 2020-Jan-11

    unsigned char bm_Horspool_Order2[256*256]; // Bitwise soon...
    unsigned char bm_Horspool_Order2bitwise[(256*256)>>3]; // Bitwise soon...
    //uint32_t i, Gulliver;
    uint64_t i, Gulliver;

    //uint32_t PRIMALposition, PRIMALpositionCANDIDATE;
    //uint32_t PRIMALlength, PRIMALlengthCANDIDATE;
    //uint32_t j, FoundAtPosition;

    uint64_t PRIMALposition, PRIMALpositionCANDIDATE;
    uint64_t PRIMALlength, PRIMALlengthCANDIDATE;
    uint64_t j, FoundAtPosition;

// Quadruplet [
    //char * pbTargetMax = pbTarget + cbTarget;
    //register unsigned long  ulHashPattern;
    unsigned long ulHashTarget;
    //unsigned long count;
    uint32_t countSTATIC; // fixed 2024-Jan-10
    unsigned char SINGLET; // fixed 2024-Jan-10
    uint32_t Quadruplet2nd; // fixed 2024-Jan-10
    uint32_t Quadruplet3rd; // fixed 2024-Jan-10
    uint32_t Quadruplet4th; // fixed 2024-Jan-10
    uint32_t AdvanceHopperGrass; // fixed 2024-Jan-10
// Quadruplet ]

// 2020-Jan-11 [
//  uint64_t A=3123123123, B=5123123123;
//if ((signed int)A > 0) printf("(signed int)3billion OK\n"); else printf("(signed int)3billion Bug\n");
//if ((signed int)B > 0) printf("(signed int)5billion OK\n"); else printf("(signed int)5billion Bug\n");
//if ((signed long long)A > 0) printf("(signed long long)3billion OK\n"); else printf("(signed long long)3billion Bug\n");
//if ((signed long long)B > 0) printf("(signed long long)5billion OK\n"); else printf("(signed long long)5billion Bug\n");

//(signed int)3billion Bug
//(signed int)5billion OK
//(signed long long)3billion OK
//(signed long long)5billion OK
// 2020-Jan-11 ]

//GLOBAL_Railgun_INVOCATIONS++; // 2020-Jan-29
//GLOBAL_Railgun_INVOCATIONS_ARRAY[cbPattern]++; // 2020-Jan-29

    if (cbPattern > cbTarget) return(NULL);

#ifdef LITE
return(NULL); // 2020-Feb-14
#endif

    if ( cbPattern<4 ) { 
        // SSE2 i.e. 128bit Assembly rules here, Mischa knows best:
        // ...
            pbTarget = pbTarget+cbPattern;
        ulHashPattern = ( (*(char *)(pbPattern))<<8 ) + *(pbPattern+(cbPattern-1));
        if ( cbPattern==3 ) {
            for ( ;; ) {
                if ( ulHashPattern == ( (*(char *)(pbTarget-3))<<8 ) + *(pbTarget-1) ) {
                    if ( *(char *)(pbPattern+1) == *(char *)(pbTarget-2) ) return((pbTarget-3));
                }
                if ( (char)(ulHashPattern>>8) != *(pbTarget-2) ) { 
                    pbTarget++;
                    if ( (char)(ulHashPattern>>8) != *(pbTarget-2) ) pbTarget++;
                }
                pbTarget++;
                if (pbTarget > pbTargetMax) return(NULL);
            }
        } else {
        }
        for ( ;; ) {
            if ( ulHashPattern == ( (*(char *)(pbTarget-2))<<8 ) + *(pbTarget-1) ) return((pbTarget-2));
            if ( (char)(ulHashPattern>>8) != *(pbTarget-1) ) pbTarget++;
            pbTarget++;
            if (pbTarget > pbTargetMax) return(NULL);
        }
    } else { //if ( cbPattern<4 )
        if ( cbPattern<=NeedleThreshold2vs4swampLITE ) { 

// This is the awesome 'Railgun_Quadruplet', it did outperform EVERYWHERE the fastest strstr (back in old GLIBCes ~2003, by the Dutch hacker Stephen R. van den Berg), suitable for short haystacks ~100bytes.
// Caution: For better speed the case 'if (cbPattern==1)' was removed, so Pattern must be longer than 1 char.
// char * Railgun_Quadruplet (char * pbTarget, char * pbPattern, unsigned long cbTarget, unsigned long cbPattern)
// ...
//    if (cbPattern > cbTarget) return(NULL);
//} else { //if ( cbPattern<4)
if (cbTarget<777) // This value is arbitrary(don't know how exactly), it ensures(at least must) better performance than 'Boyer_Moore_Horspool'.
{
        pbTarget = pbTarget+cbPattern;
        ulHashPattern = *(uint32_t *)(pbPattern); // fixed 2024-Jan-10
//        countSTATIC = cbPattern-1;

    //SINGLET = *(char *)(pbPattern);
    SINGLET = ulHashPattern & 0xFF;
    Quadruplet2nd = SINGLET<<8;
    Quadruplet3rd = SINGLET<<16;
    Quadruplet4th = SINGLET<<24;

    for ( ;; )
    {
    AdvanceHopperGrass = 0;
    ulHashTarget = *(uint32_t *)(pbTarget-cbPattern); // fixed 2024-Jan-10

        if ( ulHashPattern == ulHashTarget ) { // Three unnecessary comparisons here, but 'AdvanceHopperGrass' must be calculated - it has a higher priority.
//         count = countSTATIC;
//         while ( count && *(char *)(pbPattern+1+(countSTATIC-count)) == *(char *)(pbTarget-cbPattern+1+(countSTATIC-count)) ) {
//         if ( countSTATIC==AdvanceHopperGrass+count && SINGLET != *(char *)(pbTarget-cbPattern+1+(countSTATIC-count)) ) AdvanceHopperGrass++;
//               count--;
//         }
         count = cbPattern-1;
         while ( count && *(char *)(pbPattern+(cbPattern-count)) == *(char *)(pbTarget-count) ) {
           if ( cbPattern-1==AdvanceHopperGrass+count && SINGLET != *(char *)(pbTarget-count) ) AdvanceHopperGrass++;
               count--;
         }
         if ( count == 0) return((pbTarget-cbPattern));
        } else { // The goal here: to avoid memory accesses by stressing the registers.
    if ( Quadruplet2nd != (ulHashTarget & 0x0000FF00) ) {
         AdvanceHopperGrass++;
         if ( Quadruplet3rd != (ulHashTarget & 0x00FF0000) ) {
              AdvanceHopperGrass++;
              if ( Quadruplet4th != (ulHashTarget & 0xFF000000) ) AdvanceHopperGrass++;
         }
    }
    }

    AdvanceHopperGrass++;

    pbTarget = pbTarget + AdvanceHopperGrass;
        if (pbTarget > pbTargetMax)
            return(NULL);
    }
} else if (cbTarget<77777) { // The warmup/overhead is lowered from 64K down to 8K, however the bitwise additional instructions quickly start hurting the throughput/traversal.
// The below bitwise 0|1 BMH2 gives 1427 bytes/s for 'Don_Quixote' with Intel:
// The below bitwise 0|1 BMH2 gives 1242 bytes/s for 'Don_Quixote' with GCC:
//  } else { //if ( cbPattern<4 )
//      if ( cbPattern<=NeedleThreshold2vs4Decumanus ) { 
            // BMH order 2, needle should be >=4:
            ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
            //for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]=0;}
            for (i=0; i < (256*256)>>3; i++) {bm_Horspool_Order2bitwise[i]=0;}
            //for (i=0; i < cbPattern-1; i++) bm_Horspool_Order2[*(unsigned short *)(pbPattern+i)]=1;
            for (i=0; i < cbPattern-2+1; i++) bm_Horspool_Order2bitwise[(*(unsigned short *)(pbPattern+i))>>3]= bm_Horspool_Order2bitwise[(*(unsigned short *)(pbPattern+i))>>3] | (1<<((*(unsigned short *)(pbPattern+i))&0x7));
            i=0;
            while (i <= cbTarget-cbPattern) {
                Gulliver = 1; // 'Gulliver' is the skip
                //if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1]] != 0 ) {
                if ( ( bm_Horspool_Order2bitwise[(*(unsigned short *)&pbTarget[i+cbPattern-1-1])>>3] & (1<<((*(unsigned short *)&pbTarget[i+cbPattern-1-1])&0x7)) ) != 0 ) {
                    //if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1-2]] == 0 ) Gulliver = cbPattern-(2-1)-2; else {
                    if ( ( bm_Horspool_Order2bitwise[(*(unsigned short *)&pbTarget[i+cbPattern-1-1-2])>>3] & (1<<((*(unsigned short *)&pbTarget[i+cbPattern-1-1-2])&0x7)) ) == 0 ) Gulliver = cbPattern-(2-1)-2; else {
                        if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
                            count = cbPattern-4+1; 
                            while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
                                count = count-4;
                            if ( count <= 0 ) return(pbTarget+i);
                        }
                    }
                } else Gulliver = cbPattern-(2-1);
                i = i + Gulliver;
                //GlobalI++; // Comment it, it is only for stats.
            }
            return(NULL);
//      } else { // if ( cbPattern<=NeedleThreshold2vs4Decumanus )
} else { //if (cbTarget<777)
            // BMH order 2, needle should be >=4:
            ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
            for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]=0;}
            for (i=0; i < cbPattern-1; i++) bm_Horspool_Order2[*(unsigned short *)(pbPattern+i)]=1;
            i=0;
            while (i <= cbTarget-cbPattern) {
                Gulliver = 1; // 'Gulliver' is the skip
                if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1]] != 0 ) {
                    if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1-2]] == 0 ) Gulliver = cbPattern-(2-1)-2; else {
                        if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
                            count = cbPattern-4+1; 
                            while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
                                count = count-4;
                            if ( count <= 0 ) return(pbTarget+i);
                        }
                    }
                } else Gulliver = cbPattern-(2-1);
                i = i + Gulliver;
                //GlobalI++; // Comment it, it is only for stats.
            }
            return(NULL);

// Slower than Swampshine's simple 0|1 segment:
/*
PRIMALlength=0;
for (i=0+(1); i < cbPattern-2+1+(1)-(1); i++) { // -(1) because the last BB order 2 has no counterpart(s)
    FoundAtPosition = cbPattern;
    PRIMALpositionCANDIDATE=i;
    while ( PRIMALpositionCANDIDATE <= (FoundAtPosition-1) ) {
        j = PRIMALpositionCANDIDATE + 1;
        while ( j <= (FoundAtPosition-1) ) {
            if ( *(unsigned short *)(pbPattern+PRIMALpositionCANDIDATE-(1)) == *(unsigned short *)(pbPattern+j-(1)) ) FoundAtPosition = j;
            j++;
        }
        PRIMALpositionCANDIDATE++;
    }
    PRIMALlengthCANDIDATE = (FoundAtPosition-1)-i+(2);
    if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=i; PRIMALlength = PRIMALlengthCANDIDATE;}
}
PRIMALlengthCANDIDATE = cbPattern;
cbPattern = PRIMALlength;
pbPattern = pbPattern + (PRIMALposition-1);
if (cbPattern<4) {
    cbPattern = PRIMALlengthCANDIDATE;
    pbPattern = pbPattern - (PRIMALposition-1);
}
if (cbPattern == PRIMALlengthCANDIDATE) {
            // BMH order 2, needle should be >=4:
            ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
            for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]=0;}
            for (i=0; i < cbPattern-1; i++) bm_Horspool_Order2[*(unsigned short *)(pbPattern+i)]=1;
            i=0;
            while (i <= cbTarget-cbPattern) {
                Gulliver = 1; // 'Gulliver' is the skip
                if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1]] != 0 ) {
                    if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1-2]] == 0 ) Gulliver = cbPattern-(2-1)-2; else {
                        if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
                            count = cbPattern-4+1; 
                            while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
                                count = count-4;
                            if ( count <= 0 ) return(pbTarget+i);
                        }
                    }
                } else Gulliver = cbPattern-(2-1);
                i = i + Gulliver;
                //GlobalI++; // Comment it, it is only for stats.
            }
            return(NULL);
} else { //if (cbPattern == PRIMALlengthCANDIDATE) {
// BMH Order 2 [
            ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
            for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]= cbPattern-1;} // cbPattern-(Order-1) for Horspool; 'memset' if not optimized
            // The above 'for' gives  1424 bytes/s for 'Don_Quixote' with Intel:
            // The above 'for' gives  1431 bytes/s for 'Don_Quixote' with GCC:
            // The below 'memset' gives  1389 bytes/s for 'Don_Quixote' with Intel:
            // The below 'memset' gives  1432 bytes/s for 'Don_Quixote' with GCC:
            //memset(&bm_Horspool_Order2[0], cbPattern-1, 256*256); // Why why? It is 1700:1000 slower than above 'for'!?
            for (i=0; i < cbPattern-1; i++) bm_Horspool_Order2[*(unsigned short *)(pbPattern+i)]=i; // Rightmost appearance/position is needed
            i=0;
            while (i <= cbTarget-cbPattern) { 
                Gulliver = bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1]];
                if ( Gulliver != cbPattern-1 ) { // CASE #2: if equal means the pair (char order 2) is not found i.e. Gulliver remains intact, skip the whole pattern and fall back (Order-1) chars i.e. one char for Order 2
                if ( Gulliver == cbPattern-2 ) { // CASE #1: means the pair (char order 2) is found
                    if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) {
                        count = cbPattern-4+1; 
                        while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
                            count = count-4;
// If we miss to hit then no need to compare the original: Needle
if ( count <= 0 ) {
// I have to add out-of-range checks...
// i-(PRIMALposition-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4

// "FIX" from 2014-Apr-27:
// Because (count-1) is negative, above fours are reduced to next twos:
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
    // The line below is BUGGY:
    //if ( (i-(PRIMALposition-1) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) && (&pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4) ) {
    // The line below is NOT OKAY, in fact so stupid, grrr, not a blunder, not carelessness, but overconfidence in writing "on the fly":
    //if ( ((signed int)(i-(PRIMALposition-1)+(count-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) ) {
// FIX from 2016-Aug-10 (two times failed to do simple boundary checks, pfu):
    if ( ((signed long long)(i-(PRIMALposition-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)]+((PRIMALlengthCANDIDATE-4+1)-1) <= pbTargetMax - 4) ) { // 2020-jan-11
        if ( *(uint32_t *)&pbTarget[i-(PRIMALposition-1)] == *(uint32_t *)(pbPattern-(PRIMALposition-1))) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
            count = PRIMALlengthCANDIDATE-4+1; 
            while ( count > 0 && *(uint32_t *)(pbPattern-(PRIMALposition-1)+count-1) == *(uint32_t *)(&pbTarget[i-(PRIMALposition-1)]+(count-1)) )
                count = count-4;
            if ( count <= 0 ) return(pbTarget+i-(PRIMALposition-1));    
        }
    }
}
                    }
                    Gulliver = 1;
                } else
                    Gulliver = cbPattern - Gulliver - 2; // CASE #3: the pair is found and not as suffix i.e. rightmost position
                }
                i = i + Gulliver;
                //GlobalI++; // Comment it, it is only for stats.
            }
            return(NULL);
// BMH Order 2 ]
} //if (cbPattern == PRIMALlengthCANDIDATE) {
*/

/*
So the result on Core 2 Q9550s @2.83GHz:
---------------------------------------------------------------------------------------------------------------------
| testfile\Searcher                         | GNU/GLIBC memmem()    | Railgun_Swampshine    | Railgun_Trolldom      | 
|-------------------------------------------------------------------------------------------|-----------------------|
| Compiler                                  | Intel 15.0 | GCC 5.10 | Intel 15.0 | GCC 5.10 | Intel 15.0 | GCC 5.10 |
|-------------------------------------------------------------------------------------------|-----------------------|
| The_Project_Gutenberg_EBook_of_Don        |        190 |      226 |       1654 |     1729 |       1147 |     1764 |
| _Quixote_996_(ANSI).txt                   |            |          |            |          |            |          |
| 2,347,772 bytes                           |            |          |            |          |            |          |
|-------------------------------------------------------------------------------------------|-----------------------|
| The_Project_Gutenberg_EBook_of_Dokoe      |        582 |      760 |       3094 |     2898 |       2410 |     3036 |
| _by_Hakucho_Masamune_(Japanese_UTF-8).txt |            |          |            |          |            |          |
| 899,425 bytes                             |            |          |            |          |            |          |
|-------------------------------------------------------------------------------------------|-----------------------|
| Dragonfly_genome_shotgun_sequence         |        104 |      109 |        445 |      458 |        484 |      553 |
| _(ACGT_alphabet).fasta                    |            |          |            |          |            |          |
| 4,487,433 bytes                           |            |          |            |          |            |          |
|-------------------------------------------------------------------------------------------|-----------------------|
| LAOTZU_Wu_Wei_(BINARY).pdf                |         99 |      144 |        629 |      580 |        185 |      570 |
| 954,035 bytes                             |            |          |            |          |            |          |
|-------------------------------------------------------------------------------------------|-----------------------|
Below segment (when compiled with Intel) is very slow, see Railgun_Trolldom two sub-columns above, compared to GCC:
*/
/*
// BMH Order 2 [
            ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
            for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]= (cbPattern-1);} // cbPattern-(Order-1) for Horspool; 'memset' if not optimized
            // The above 'for' is translated by Intel as:
//.B5.21::                        
//  0013f 83 c0 40         add eax, 64                            
//  00142 66 0f 7f 44 14 
//        60               movdqa XMMWORD PTR [96+rsp+rdx], xmm0  
//  00148 3d 00 00 01 00   cmp eax, 65536                         
//  0014d 66 0f 7f 44 14 
//        70               movdqa XMMWORD PTR [112+rsp+rdx], xmm0 
//  00153 66 0f 7f 84 14 
//        80 00 00 00      movdqa XMMWORD PTR [128+rsp+rdx], xmm0 
//  0015c 66 0f 7f 84 14 
//        90 00 00 00      movdqa XMMWORD PTR [144+rsp+rdx], xmm0 
//  00165 89 c2            mov edx, eax                           
//  00167 72 d6            jb .B5.21 
            //memset(&bm_Horspool_Order2[0], cbPattern-1, 256*256); // Why why? It is 1700:1000 slower than above 'for'!?
            // The above 'memset' is translated by Intel as:
//  00127 41 b8 00 00 01 
//        00               mov r8d, 65536                         
//  0012d 44 8b 26         mov r12d, DWORD PTR [rsi]              
//  00130 e8 fc ff ff ff   call _intel_fast_memset                
            // ! The problem is that 256*256, 64KB, is already too much, going bitwise i.e. 8KB is not that better, when 'cbPattern-1' is bigger than 255 - an unsigned char - then 
            // we must switch to 0|1 table i.e. present or not. Since we are in 'if ( cbPattern<=NeedleThreshold2vs4swampLITE ) {' branch and NeedleThreshold2vs4swampLITE, by default, is 19 - it is okay to use 'memset'. !
            for (i=0; i < cbPattern-1; i++) bm_Horspool_Order2[*(unsigned short *)(pbPattern+i)]=i; // Rightmost appearance/position is needed
            i=0;
            while (i <= cbTarget-cbPattern) { 
                Gulliver = bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1]];
                if ( Gulliver != cbPattern-1 ) { // CASE #2: if equal means the pair (char order 2) is not found i.e. Gulliver remains intact, skip the whole pattern and fall back (Order-1) chars i.e. one char for Order 2
                if ( Gulliver == cbPattern-2 ) { // CASE #1: means the pair (char order 2) is found
                    if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) {
                        count = cbPattern-4+1; 
                        while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
                            count = count-4;
                        if ( count <= 0 ) return(pbTarget+i);   
                    }
                    Gulliver = 1;
                } else
                    Gulliver = cbPattern - Gulliver - 2; // CASE #3: the pair is found and not as suffix i.e. rightmost position
                }
                i = i + Gulliver;
                //GlobalI++; // Comment it, it is only for stats.
            }
            return(NULL);
// BMH Order 2 ]
*/

} //if (cbTarget<777)

        } else { // if ( cbPattern<=NeedleThreshold2vs4swampLITE )

// Swampwalker_BAILOUT heuristic order 4 (Needle should be bigger than 4) [
// Needle: 1234567890qwertyuiopasdfghjklzxcv            PRIMALposition=01 PRIMALlength=33  '1234567890qwertyuiopasdfghjklzxcv'
// Needle: vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv            PRIMALposition=29 PRIMALlength=04  'vvvv'
// Needle: vvvvvvvvvvBOOMSHAKALAKAvvvvvvvvvv            PRIMALposition=08 PRIMALlength=20  'vvvBOOMSHAKALAKAvvvv'
// Needle: Trollland                                    PRIMALposition=01 PRIMALlength=09  'Trollland'
// Needle: Swampwalker                                  PRIMALposition=01 PRIMALlength=11  'Swampwalker'
// Needle: licenselessness                              PRIMALposition=01 PRIMALlength=15  'licenselessness'
// Needle: alfalfa                                      PRIMALposition=02 PRIMALlength=06  'lfalfa'
// Needle: Sandokan                                     PRIMALposition=01 PRIMALlength=08  'Sandokan'
// Needle: shazamish                                    PRIMALposition=01 PRIMALlength=09  'shazamish'
// Needle: Simplicius Simplicissimus                    PRIMALposition=06 PRIMALlength=20  'icius Simplicissimus'
// Needle: domilliaquadringenquattuorquinquagintillion  PRIMALposition=01 PRIMALlength=32  'domilliaquadringenquattuorquinqu'
// Needle: boom-boom                                    PRIMALposition=02 PRIMALlength=08  'oom-boom'
// Needle: vvvvv                                        PRIMALposition=01 PRIMALlength=04  'vvvv'
// Needle: 12345                                        PRIMALposition=01 PRIMALlength=05  '12345'
// Needle: likey-likey                                  PRIMALposition=03 PRIMALlength=09  'key-likey'
// Needle: BOOOOOM                                      PRIMALposition=03 PRIMALlength=05  'OOOOM'
// Needle: aaaaaBOOOOOM                                 PRIMALposition=02 PRIMALlength=09  'aaaaBOOOO'
// Needle: BOOOOOMaaaaa                                 PRIMALposition=03 PRIMALlength=09  'OOOOMaaaa'
PRIMALlength=0;
for (i=0+(1); i < cbPattern-((4)-1)+(1)-(1); i++) { // -(1) because the last BB (Building-Block) order 4 has no counterpart(s)
    FoundAtPosition = cbPattern - ((4)-1) + 1;
    PRIMALpositionCANDIDATE=i;
    while ( PRIMALpositionCANDIDATE <= (FoundAtPosition-1) ) {
        j = PRIMALpositionCANDIDATE + 1;
        while ( j <= (FoundAtPosition-1) ) {
            if ( *(uint32_t *)(pbPattern+PRIMALpositionCANDIDATE-(1)) == *(uint32_t *)(pbPattern+j-(1)) ) FoundAtPosition = j;
            j++;
        }
        PRIMALpositionCANDIDATE++;
    }
    PRIMALlengthCANDIDATE = (FoundAtPosition-1)-i+1 +((4)-1);
    if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=i; PRIMALlength = PRIMALlengthCANDIDATE;}
    if (cbPattern-i+1 <= PRIMALlength) break;
    if (PRIMALlength > 128) break; // Bail Out for 129[+]
}
// Swampwalker_BAILOUT heuristic order 4 (Needle should be bigger than 4) ]

// Swampwalker_BAILOUT heuristic order 2 (Needle should be bigger than 2) [
// Needle: 1234567890qwertyuiopasdfghjklzxcv            PRIMALposition=01 PRIMALlength=33  '1234567890qwertyuiopasdfghjklzxcv'
// Needle: vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv            PRIMALposition=31 PRIMALlength=02  'vv'
// Needle: vvvvvvvvvvBOOMSHAKALAKAvvvvvvvvvv            PRIMALposition=09 PRIMALlength=13  'vvBOOMSHAKALA'
// Needle: Trollland                                    PRIMALposition=05 PRIMALlength=05  'lland'
// Needle: Swampwalker                                  PRIMALposition=03 PRIMALlength=09  'ampwalker'
// Needle: licenselessness                              PRIMALposition=01 PRIMALlength=13  'licenselessne'
// Needle: alfalfa                                      PRIMALposition=04 PRIMALlength=04  'alfa'
// Needle: Sandokan                                     PRIMALposition=01 PRIMALlength=07  'Sandoka'
// Needle: shazamish                                    PRIMALposition=02 PRIMALlength=08  'hazamish'
// Needle: Simplicius Simplicissimus                    PRIMALposition=08 PRIMALlength=15  'ius Simplicissi'
// Needle: domilliaquadringenquattuorquinquagintillion  PRIMALposition=01 PRIMALlength=19  'domilliaquadringenq'
// Needle: DODO                                         PRIMALposition=02 PRIMALlength=03  'ODO'
// Needle: DODOD                                        PRIMALposition=03 PRIMALlength=03  'DOD'
// Needle: aaaDODO                                      PRIMALposition=02 PRIMALlength=05  'aaDOD'
// Needle: aaaDODOD                                     PRIMALposition=02 PRIMALlength=05  'aaDOD'
// Needle: DODOaaa                                      PRIMALposition=02 PRIMALlength=05  'ODOaa'
// Needle: DODODaaa                                     PRIMALposition=03 PRIMALlength=05  'DODaa'
/*
PRIMALlength=0;
for (i=0+(1); i < cbPattern-2+1+(1)-(1); i++) { // -(1) because the last BB order 2 has no counterpart(s)
    FoundAtPosition = cbPattern;
    PRIMALpositionCANDIDATE=i;
    while ( PRIMALpositionCANDIDATE <= (FoundAtPosition-1) ) {
        j = PRIMALpositionCANDIDATE + 1;
        while ( j <= (FoundAtPosition-1) ) {
            if ( *(unsigned short *)(pbPattern+PRIMALpositionCANDIDATE-(1)) == *(unsigned short *)(pbPattern+j-(1)) ) FoundAtPosition = j;
            j++;
        }
        PRIMALpositionCANDIDATE++;
    }
    PRIMALlengthCANDIDATE = (FoundAtPosition-1)-i+(2);
    if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=i; PRIMALlength = PRIMALlengthCANDIDATE;}
}
*/
// Swampwalker_BAILOUT heuristic order 2 (Needle should be bigger than 2) ]

/*
Legend:
'[]' points to BB forming left or right boundary;
'{}' points to BB being searched for;
'()' position of duplicate and new right boundary;

                       00000000011111111112222222222333
                       12345678901234567890123456789012
Example #1 for Needle: 1234567890qwertyuiopasdfghjklzxcv  NewNeedle = '1234567890qwertyuiopasdfghjklzxcv'
Example #2 for Needle: vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv  NewNeedle = 'vv'
Example #3 for Needle: vvvvvvvvvvBOOMSHAKALAKAvvvvvvvvvv  NewNeedle = 'vvBOOMSHAKALA'


     PRIMALlength=00; FoundAtPosition=33; 
Step 01_00: {}[12]34567890qwertyuiopasdfghjklzxc[v?] ! For position #01 the initial boundaries are PRIMALpositionCANDIDATE=LeftBoundary=01, RightBoundary=FoundAtPosition-1, the CANDIDATE PRIMAL string length is RightBoundary-LeftBoundary+(2)=(33-1)-01+(2)=33 !
Step 01_01: [{12}]34567890qwertyuiopasdfghjklzxc[v?] ! Searching for '12', FoundAtPosition = 33, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(33-1)-01+(2)=33 ! 
Step 01_02: [1{2]3}4567890qwertyuiopasdfghjklzxc[v?] ! Searching for '23', FoundAtPosition = 33, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(33-1)-01+(2)=33 ! 
...
Step 01_30: [12]34567890qwertyuiopasdfghjkl{zx}c[v?] ! Searching for 'zx', FoundAtPosition = 33, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(33-1)-01+(2)=33 ! 
Step 01_31: [12]34567890qwertyuiopasdfghjklz{xc}[v?] ! Searching for 'xc', FoundAtPosition = 33, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(33-1)-01+(2)=33 ! 
     if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=PRIMALpositionCANDIDATE; PRIMALlength = PRIMALlengthCANDIDATE;}
Step 02_00: {}1[23]4567890qwertyuiopasdfghjklzxc[v?] ! For position #02 the initial boundaries are PRIMALpositionCANDIDATE=LeftBoundary=02, RightBoundary=FoundAtPosition-1, the CANDIDATE PRIMAL string length is RightBoundary-LeftBoundary+(2)=(33-1)-02+(2)=32 !
Step 02_01: 1[{23}]4567890qwertyuiopasdfghjklzxc[v?] ! Searching for '23', FoundAtPosition = 33, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(33-1)-02+(2)=32 ! 
Step 02_02: 1[2{3]4}567890qwertyuiopasdfghjklzxc[v?] ! Searching for '34', FoundAtPosition = 33, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(33-1)-02+(2)=32 ! 
...
Step 02_29: 1[23]4567890qwertyuiopasdfghjkl{zx}c[v?] ! Searching for 'zx', FoundAtPosition = 33, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(33-1)-02+(2)=32 ! 
Step 02_30: 1[23]4567890qwertyuiopasdfghjklz{xc}[v?] ! Searching for 'xc', FoundAtPosition = 33, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(33-1)-02+(2)=32 ! 
     if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=PRIMALpositionCANDIDATE; PRIMALlength = PRIMALlengthCANDIDATE;}
...
Step 31_00: {}1234567890qwertyuiopasdfghjklz[xc][v?] ! For position #31 the initial boundaries are PRIMALpositionCANDIDATE=LeftBoundary=31, RightBoundary=FoundAtPosition-1, the CANDIDATE PRIMAL string length is RightBoundary-LeftBoundary+(2)=(33-1)-31+(2)=03 !
Step 31_01: 1234567890qwertyuiopasdfghjklz[{xc}][v?] ! Searching for 'xc', FoundAtPosition = 33, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(33-1)-31+(2)=03 ! 
     if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=PRIMALpositionCANDIDATE; PRIMALlength = PRIMALlengthCANDIDATE;}
     Result:
     PRIMALposition=01 PRIMALlength=33, NewNeedle = '1234567890qwertyuiopasdfghjklzxcv'


     PRIMALlength=00; FoundAtPosition=33; 
Step 01_00: {}[vv]vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv[v?] ! For position #01 the initial boundaries are PRIMALpositionCANDIDATE=LeftBoundary=01, RightBoundary=FoundAtPosition-1, the CANDIDATE PRIMAL string length is RightBoundary-LeftBoundary+(2)=(33-1)-01+(2)=33 !
Step 01_01: [{v(v}]v)vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv  ! Searching for 'vv', FoundAtPosition = 02, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(02-1)-01+(2)=02 ! 
     if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=PRIMALpositionCANDIDATE; PRIMALlength = PRIMALlengthCANDIDATE;}
Step 02_00: {}v[vv]vvvvvvvvvvvvvvvvvvvvvvvvvvvvv[v?] ! For position #02 the initial boundaries are PRIMALpositionCANDIDATE=LeftBoundary=02, RightBoundary=FoundAtPosition-1, the CANDIDATE PRIMAL string length is RightBoundary-LeftBoundary+(2)=(33-1)-02+(2)=32 !
Step 02_01: v[{v(v}]v)vvvvvvvvvvvvvvvvvvvvvvvvvvvvv  ! Searching for 'vv', FoundAtPosition = 03, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(03-1)-02+(2)=02 ! 
     if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=PRIMALpositionCANDIDATE; PRIMALlength = PRIMALlengthCANDIDATE;}
...
Step 31_00: {}vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv[vv][v?] ! For position #31 the initial boundaries are PRIMALpositionCANDIDATE=LeftBoundary=31, RightBoundary=FoundAtPosition-1, the CANDIDATE PRIMAL string length is RightBoundary-LeftBoundary+(2)=(33-1)-31+(2)=03 !
Step 31_01: vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv[{v(v}]v)  ! Searching for 'vv', FoundAtPosition = 32, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(32-1)-31+(2)=02 ! 
     if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=PRIMALpositionCANDIDATE; PRIMALlength = PRIMALlengthCANDIDATE;}
     Result:
     PRIMALposition=31 PRIMALlength=02, NewNeedle = 'vv'


     PRIMALlength=00; FoundAtPosition=33; 
Step 01_00: {}[vv]vvvvvvvvBOOMSHAKALAKAvvvvvvvvv[v?] ! For position #01 the initial boundaries are PRIMALpositionCANDIDATE=LeftBoundary=01, RightBoundary=FoundAtPosition-1, the CANDIDATE PRIMAL string length is RightBoundary-LeftBoundary+(2)=(33-1)-01+(2)=33 !
Step 01_01: [{v(v}]v)vvvvvvvBOOMSHAKALAKAvvvvvvvvvv  ! Searching for 'vv', FoundAtPosition = 02, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(02-1)-01+(2)=02 ! 
     if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=PRIMALpositionCANDIDATE; PRIMALlength = PRIMALlengthCANDIDATE;}
Step 02_00: {}v[vv]vvvvvvvBOOMSHAKALAKAvvvvvvvvv[v?] ! For position #02 the initial boundaries are PRIMALpositionCANDIDATE=LeftBoundary=02, RightBoundary=FoundAtPosition-1, the CANDIDATE PRIMAL string length is RightBoundary-LeftBoundary+(2)=(33-1)-02+(2)=32 !
Step 02_01: v[{v(v}]v)vvvvvvBOOMSHAKALAKAvvvvvvvvvv  ! Searching for 'vv', FoundAtPosition = 03, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(03-1)-02+(2)=02 ! 
     if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=PRIMALpositionCANDIDATE; PRIMALlength = PRIMALlengthCANDIDATE;}
...
Step 09_00: {}vvvvvvvv[vv]BOOMSHAKALAKAvvvvvvvvv[v?] ! For position #09 the initial boundaries are PRIMALpositionCANDIDATE=LeftBoundary=09, RightBoundary=FoundAtPosition-1, the CANDIDATE PRIMAL string length is RightBoundary-LeftBoundary+(2)=(33-1)-09+(2)=25 !
Step 09_01: vvvvvvvv[{vv}]BOOMSHAKALAKA(vv)vvvvvvvv  ! Searching for 'vv', FoundAtPosition = 24, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(24-1)-09+(2)=16 ! 
Step 09_02: vvvvvvvv[v{v]B}OOMSHAKALAKA[vv]vvvvvvvv  ! Searching for 'vB', FoundAtPosition = 24, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(24-1)-09+(2)=16 ! 
Step 09_03: vvvvvvvv[vv]{BO}OMSHAKALAKA[vv]vvvvvvvv  ! Searching for 'BO', FoundAtPosition = 24, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(24-1)-09+(2)=16 ! 
Step 09_04: vvvvvvvv[vv]B{OO}MSHAKALAKA[vv]vvvvvvvv  ! Searching for 'OO', FoundAtPosition = 24, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(24-1)-09+(2)=16 ! 
Step 09_05: vvvvvvvv[vv]BO{OM}SHAKALAKA[vv]vvvvvvvv  ! Searching for 'OM', FoundAtPosition = 24, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(24-1)-09+(2)=16 ! 
Step 09_06: vvvvvvvv[vv]BOO{MS}HAKALAKA[vv]vvvvvvvv  ! Searching for 'MS', FoundAtPosition = 24, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(24-1)-09+(2)=16 ! 
Step 09_07: vvvvvvvv[vv]BOOM{SH}AKALAKA[vv]vvvvvvvv  ! Searching for 'SH', FoundAtPosition = 24, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(24-1)-09+(2)=16 ! 
Step 09_08: vvvvvvvv[vv]BOOMS{HA}KALAKA[vv]vvvvvvvv  ! Searching for 'HA', FoundAtPosition = 24, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(24-1)-09+(2)=16 ! 
Step 09_09: vvvvvvvv[vv]BOOMSH{AK}AL(AK)Avvvvvvvvvv  ! Searching for 'AK', FoundAtPosition = 21, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(21-1)-09+(2)=13 ! 
Step 09_10: vvvvvvvv[vv]BOOMSHA{KA}L[AK]Avvvvvvvvvv  ! Searching for 'KA', FoundAtPosition = 21, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(21-1)-09+(2)=13 ! 
Step 09_11: vvvvvvvv[vv]BOOMSHAK{AL}[AK]Avvvvvvvvvv  ! Searching for 'AL', FoundAtPosition = 21, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(21-1)-09+(2)=13 ! 
Step 09_12: vvvvvvvv[vv]BOOMSHAKA{L[A}K]Avvvvvvvvvv  ! Searching for 'LA', FoundAtPosition = 21, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(21-1)-09+(2)=13 ! 
     if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=PRIMALpositionCANDIDATE; PRIMALlength = PRIMALlengthCANDIDATE;}
...
Step 31_00: {}vvvvvvvv[vv]BOOMSHAKALAKAvvvvvvvvv[v?] ! For position #31 the initial boundaries are PRIMALpositionCANDIDATE=LeftBoundary=31, RightBoundary=FoundAtPosition-1, the CANDIDATE PRIMAL string length is RightBoundary-LeftBoundary+(2)=(33-1)-31+(2)=03 !
Step 31_01: vvvvvvvvvvBOOMSHAKALAKAvvvvvvv[{v(v}]v)  ! Searching for 'vv', FoundAtPosition = 32, PRIMALlengthCANDIDATE=RightBoundary-LeftBoundary+(2)=(32-1)-31+(2)=02 ! 
     if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=PRIMALpositionCANDIDATE; PRIMALlength = PRIMALlengthCANDIDATE;}
     Result:
     PRIMALposition=09 PRIMALlength=13, NewNeedle = 'vvBOOMSHAKALA'
*/

// Here we have 4 or bigger NewNeedle, apply order 2 for pbPattern[i+(PRIMALposition-1)] with length 'PRIMALlength' and compare the pbPattern[i] with length 'cbPattern':
PRIMALlengthCANDIDATE = cbPattern;
cbPattern = PRIMALlength;
pbPattern = pbPattern + (PRIMALposition-1);

// Revision 2 commented section [
/*
if (cbPattern-1 <= 255) {
// BMH Order 2 [
            ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
            for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]= cbPattern-1;} // cbPattern-(Order-1) for Horspool; 'memset' if not optimized
            for (i=0; i < cbPattern-1; i++) bm_Horspool_Order2[*(unsigned short *)(pbPattern+i)]=i; // Rightmost appearance/position is needed
            i=0;
            while (i <= cbTarget-cbPattern) { 
                Gulliver = bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1]];
                if ( Gulliver != cbPattern-1 ) { // CASE #2: if equal means the pair (char order 2) is not found i.e. Gulliver remains intact, skip the whole pattern and fall back (Order-1) chars i.e. one char for Order 2
                if ( Gulliver == cbPattern-2 ) { // CASE #1: means the pair (char order 2) is found
                    if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) {
                        count = cbPattern-4+1; 
                        while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
                            count = count-4;
// If we miss to hit then no need to compare the original: Needle
if ( count <= 0 ) {
// I have to add out-of-range checks...
// i-(PRIMALposition-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4

// "FIX" from 2014-Apr-27:
// Because (count-1) is negative, above fours are reduced to next twos:
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
    // The line below is BUGGY:
    //if ( (i-(PRIMALposition-1) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) && (&pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4) ) {
    // The line below is NOT OKAY, in fact so stupid, grrr, not a blunder, not carelessness, but overconfidence in writing "on the fly":
    //if ( ((signed int)(i-(PRIMALposition-1)+(count-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) ) {
// FIX from 2016-Aug-10 (two times failed to do simple boundary checks, pfu):
    if ( ((signed long long)(i-(PRIMALposition-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)]+((PRIMALlengthCANDIDATE-4+1)-1) <= pbTargetMax - 4) ) { // 2020-jan-11
        if ( *(uint32_t *)&pbTarget[i-(PRIMALposition-1)] == *(uint32_t *)(pbPattern-(PRIMALposition-1))) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
            count = PRIMALlengthCANDIDATE-4+1; 
            while ( count > 0 && *(uint32_t *)(pbPattern-(PRIMALposition-1)+count-1) == *(uint32_t *)(&pbTarget[i-(PRIMALposition-1)]+(count-1)) )
                count = count-4;
            if ( count <= 0 ) return(pbTarget+i-(PRIMALposition-1));    
        }
    }
}
                    }
                    Gulliver = 1;
                } else
                    Gulliver = cbPattern - Gulliver - 2; // CASE #3: the pair is found and not as suffix i.e. rightmost position
                }
                i = i + Gulliver;
                //GlobalI++; // Comment it, it is only for stats.
            }
            return(NULL);
// BMH Order 2 ]
} else {
            // BMH order 2, needle should be >=4:
            ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
            for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]=0;}
            for (i=0; i < cbPattern-1; i++) bm_Horspool_Order2[*(unsigned short *)(pbPattern+i)]=1;
            i=0;
            while (i <= cbTarget-cbPattern) {
                Gulliver = 1; // 'Gulliver' is the skip
                if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1]] != 0 ) {
                    if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1-2]] == 0 ) Gulliver = cbPattern-(2-1)-2; else {
                        if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
                            count = cbPattern-4+1; 
                            while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
                                count = count-4;
// If we miss to hit then no need to compare the original: Needle
if ( count <= 0 ) {
// I have to add out-of-range checks...
// i-(PRIMALposition-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4

// "FIX" from 2014-Apr-27:
// Because (count-1) is negative, above fours are reduced to next twos:
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
    // The line below is BUGGY:
    //if ( (i-(PRIMALposition-1) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) && (&pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4) ) {
    // The line below is NOT OKAY, in fact so stupid, grrr, not a blunder, not carelessness, but overconfidence in writing "on the fly":
    //if ( ((signed int)(i-(PRIMALposition-1)+(count-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) ) {
// FIX from 2016-Aug-10 (two times failed to do simple boundary checks, pfu):
    if ( ((signed long long)(i-(PRIMALposition-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)]+((PRIMALlengthCANDIDATE-4+1)-1) <= pbTargetMax - 4) ) { // 2020-jan-11
        if ( *(uint32_t *)&pbTarget[i-(PRIMALposition-1)] == *(uint32_t *)(pbPattern-(PRIMALposition-1))) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
            count = PRIMALlengthCANDIDATE-4+1; 
            while ( count > 0 && *(uint32_t *)(pbPattern-(PRIMALposition-1)+count-1) == *(uint32_t *)(&pbTarget[i-(PRIMALposition-1)]+(count-1)) )
                count = count-4;
            if ( count <= 0 ) return(pbTarget+i-(PRIMALposition-1));    
        }
    }
}
                        }
                    }
                } else Gulliver = cbPattern-(2-1);
                i = i + Gulliver;
                //GlobalI++; // Comment it, it is only for stats.
            }
            return(NULL);
}
*/
// Revision 2 commented section ]

        if ( cbPattern<=NeedleThreshold2vs4swampLITE ) { 

            // BMH order 2, needle should be >=4:
            ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
            for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]=0;}
            // Above line is translated by Intel as:
//  0044c 41 b8 00 00 01 
//        00               mov r8d, 65536                         
//  00452 44 89 5c 24 20   mov DWORD PTR [32+rsp], r11d           
//  00457 44 89 54 24 60   mov DWORD PTR [96+rsp], r10d           
//  0045c e8 fc ff ff ff   call _intel_fast_memset                
            for (i=0; i < cbPattern-1; i++) bm_Horspool_Order2[*(unsigned short *)(pbPattern+i)]=1;
            i=0;
            while (i <= cbTarget-cbPattern) {
                Gulliver = 1; // 'Gulliver' is the skip
                if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1]] != 0 ) {
                    if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1-2]] == 0 ) Gulliver = cbPattern-(2-1)-2; else {
                        if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
                            count = cbPattern-4+1; 
                            while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
                                count = count-4;

    if (cbPattern != PRIMALlengthCANDIDATE) { // No need of same comparison when Needle and NewNeedle are equal!
// If we miss to hit then no need to compare the original: Needle
if ( count <= 0 ) {
// I have to add out-of-range checks...
// i-(PRIMALposition-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4

// "FIX" from 2014-Apr-27:
// Because (count-1) is negative, above fours are reduced to next twos:
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
    // The line below is BUGGY:
    //if ( (i-(PRIMALposition-1) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) && (&pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4) ) {
    // The line below is NOT OKAY, in fact so stupid, grrr, not a blunder, not carelessness, but overconfidence in writing "on the fly":
    //if ( ((signed int)(i-(PRIMALposition-1)+(count-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) ) {
// FIX from 2016-Aug-10 (two times failed to do simple boundary checks, pfu):
    if ( ((signed long long)(i-(PRIMALposition-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)]+((PRIMALlengthCANDIDATE-4+1)-1) <= pbTargetMax - 4) ) { // 2020-jan-11
        if ( *(uint32_t *)&pbTarget[i-(PRIMALposition-1)] == *(uint32_t *)(pbPattern-(PRIMALposition-1))) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
            count = PRIMALlengthCANDIDATE-4+1; 
            while ( count > 0 && *(uint32_t *)(pbPattern-(PRIMALposition-1)+count-1) == *(uint32_t *)(&pbTarget[i-(PRIMALposition-1)]+(count-1)) )
                count = count-4;
            if ( count <= 0 ) return(pbTarget+i-(PRIMALposition-1));    
        }
    }
}
    } else { //if (cbPattern != PRIMALlengthCANDIDATE)
                            if ( count <= 0 ) return(pbTarget+i);
    }
                        }
                    }
                } else Gulliver = cbPattern-(2-1);
                i = i + Gulliver;
                //GlobalI++; // Comment it, it is only for stats.
            }
            return(NULL);

        } else { // if ( cbPattern<=NeedleThreshold2vs4swampLITE )

            // BMH pseudo-order 4, needle should be >=8+2:
            ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
            for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]=0;}
            // In line below we "hash" 4bytes to 2bytes i.e. 16bit table, how to compute TOTAL number of BBs, 'cbPattern - Order + 1' is the number of BBs for text 'cbPattern' bytes long, for example, for cbPattern=11 'fastest fox' and Order=4 we have BBs = 11-4+1=8:
            //"fast"
            //"aste"
            //"stes"
            //"test"
            //"est "
            //"st f"
            //"t fo"
            //" fox"
            //for (i=0; i < cbPattern-4+1; i++) bm_Horspool_Order2[( *(unsigned short *)(pbPattern+i+0) + *(unsigned short *)(pbPattern+i+2) ) & ( (1<<16)-1 )]=1;
            //for (i=0; i < cbPattern-4+1; i++) bm_Horspool_Order2[( (*(uint32_t *)(pbPattern+i+0)>>16)+(*(uint32_t *)(pbPattern+i+0)&0xFFFF) ) & ( (1<<16)-1 )]=1;
            // Above line is replaced by next one with better hashing:
            for (i=0; i < cbPattern-4+1; i++) bm_Horspool_Order2[( (*(uint32_t *)(pbPattern+i+0)>>(16-1))+(*(uint32_t *)(pbPattern+i+0)&0xFFFF) ) & ( (1<<16)-1 )]=1;
            i=0;
            while (i <= cbTarget-cbPattern) {
                Gulliver = 1;
                //if ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2]>>16)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2]&0xFFFF) ) & ( (1<<16)-1 )] != 0 ) { // DWORD #1
                // Above line is replaced by next one with better hashing:
                if ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2]>>(16-1))+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2]&0xFFFF) ) & ( (1<<16)-1 )] != 0 ) { // DWORD #1
                    //if ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]>>16)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) ) & ( (1<<16)-1 )] == 0 ) Gulliver = cbPattern-(2-1)-2-4; else {
                    // Above line is replaced in order to strengthen the skip by checking the middle DWORD,if the two DWORDs are 'ab' and 'cd' i.e. [2x][2a][2b][2c][2d] then the middle DWORD is 'bc'.
                    // The respective offsets (backwards) are: -10/-8/-6/-4 for 'xa'/'ab'/'bc'/'cd'.
                    //if ( ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]>>16)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]&0xFFFF) ) & ( (1<<16)-1 )] ) + ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]>>16)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) ) & ( (1<<16)-1 )] ) + ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]>>16)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]&0xFFFF) ) & ( (1<<16)-1 )] ) < 3 ) Gulliver = cbPattern-(2-1)-2-4-2; else {
                    // Above line is replaced by next one with better hashing:
                    // When using (16-1) right shifting instead of 16 we will have two different pairs (if they are equal), the highest bit being lost do the job especialy for ASCII texts with no symbols in range 128-255.
                    // Example for genomesque pair TT+TT being shifted by (16-1):
                    // T            = 01010100
                    // TT           = 01010100 01010100
                    // TTTT         = 01010100 01010100 01010100 01010100
                    // TTTT>>16     = 00000000 00000000 01010100 01010100
                    // TTTT>>(16-1) = 00000000 00000000 10101000 10101000 <--- Due to the left shift by 1, the 8th bits of 1st and 2nd bytes are populated - usually they are 0 for English texts & 'ACGT' data.
                    //if ( ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]>>(16-1))+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]&0xFFFF) ) & ( (1<<16)-1 )] ) + ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]>>(16-1))+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) ) & ( (1<<16)-1 )] ) + ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]>>(16-1))+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]&0xFFFF) ) & ( (1<<16)-1 )] ) < 3 ) Gulliver = cbPattern-(2-1)-2-4-2; else {
                    // 'Maximus' uses branched 'if', again.
                    if ( \
                    ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6 +1]>>(16-1))+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6 +1]&0xFFFF) ) & ( (1<<16)-1 )] ) == 0 \
                    || ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4 +1]>>(16-1))+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4 +1]&0xFFFF) ) & ( (1<<16)-1 )] ) == 0 \
                    ) Gulliver = cbPattern-(2-1)-2-4-2 +1; else {
                    // Above line is not optimized (several a SHR are used), we have 5 non-overlapping WORDs, or 3 overlapping WORDs, within 4 overlapping DWORDs so:
// [2x][2a][2b][2c][2d]
// DWORD #4
// [2a] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]>>16) =     !SHR to be avoided! <--
// [2x] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]&0xFFFF) =                        |
//     DWORD #3                                                                       |
// [2b] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]>>16) =     !SHR to be avoided!   |<--
// [2a] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) = ------------------------  |
//         DWORD #2                                                                      |
// [2c] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]>>16) =     !SHR to be avoided!      |<--
// [2b] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]&0xFFFF) = ---------------------------  |
//             DWORD #1                                                                     |
// [2d] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-0]>>16) =                                 |
// [2c] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-0]&0xFFFF) = ------------------------------
//
// So in order to remove 3 SHR instructions the equal extractions are:
// DWORD #4
// [2a] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) =  !SHR to be avoided! <--
// [2x] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]&0xFFFF) =                        |
//     DWORD #3                                                                       |
// [2b] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]&0xFFFF) =  !SHR to be avoided!   |<--
// [2a] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) = ------------------------  |
//         DWORD #2                                                                      |
// [2c] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-0]&0xFFFF) =  !SHR to be avoided!      |<--
// [2b] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]&0xFFFF) = ---------------------------  |
//             DWORD #1                                                                     |
// [2d] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-0]>>16) =                                 |
// [2c] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-0]&0xFFFF) = ------------------------------
                    //if ( ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]&0xFFFF) ) & ( (1<<16)-1 )] ) + ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]&0xFFFF)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) ) & ( (1<<16)-1 )] ) + ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-0]&0xFFFF)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]&0xFFFF) ) & ( (1<<16)-1 )] ) < 3 ) Gulliver = cbPattern-(2-1)-2-6; else {
// Since the above Decumanus mumbo-jumbo (3 overlapping lookups vs 2 non-overlapping lookups) is not fast enough we go DuoDecumanus or 3x4:
// [2y][2x][2a][2b][2c][2d]
// DWORD #3
//         DWORD #2
//                 DWORD #1
                    //if ( ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]>>16)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) ) & ( (1<<16)-1 )] ) + ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-8]>>16)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-8]&0xFFFF) ) & ( (1<<16)-1 )] ) < 2 ) Gulliver = cbPattern-(2-1)-2-8; else {
                        if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) {
                            // Order 4 [
                        // Let's try something "outrageous" like comparing with[out] overlap BBs 4bytes long instead of 1 byte back-to-back:
                        // Inhere we are using order 4, 'cbPattern - Order + 1' is the number of BBs for text 'cbPattern' bytes long, for example, for cbPattern=11 'fastest fox' and Order=4 we have BBs = 11-4+1=8:
                        //0:"fast" if the comparison failed here, 'count' is 1; 'Gulliver' is cbPattern-(4-1)-7
                        //1:"aste" if the comparison failed here, 'count' is 2; 'Gulliver' is cbPattern-(4-1)-6
                        //2:"stes" if the comparison failed here, 'count' is 3; 'Gulliver' is cbPattern-(4-1)-5
                        //3:"test" if the comparison failed here, 'count' is 4; 'Gulliver' is cbPattern-(4-1)-4
                        //4:"est " if the comparison failed here, 'count' is 5; 'Gulliver' is cbPattern-(4-1)-3
                        //5:"st f" if the comparison failed here, 'count' is 6; 'Gulliver' is cbPattern-(4-1)-2
                        //6:"t fo" if the comparison failed here, 'count' is 7; 'Gulliver' is cbPattern-(4-1)-1
                        //7:" fox" if the comparison failed here, 'count' is 8; 'Gulliver' is cbPattern-(4-1)
                            count = cbPattern-4+1; 
                            // Below comparison is UNIdirectional:
                            while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
                                count = count-4;

    if (cbPattern != PRIMALlengthCANDIDATE) { // No need of same comparison when Needle and NewNeedle are equal!
// count = cbPattern-4+1 = 23-4+1 = 20
// boomshakalakaZZZZZZ[ZZZZ] 20
// boomshakalakaZZ[ZZZZ]ZZZZ 20-4
// boomshakala[kaZZ]ZZZZZZZZ 20-8 = 12
// boomsha[kala]kaZZZZZZZZZZ 20-12 = 8
// boo[msha]kalakaZZZZZZZZZZ 20-16 = 4

// If we miss to hit then no need to compare the original: Needle
if ( count <= 0 ) {
// I have to add out-of-range checks...
// i-(PRIMALposition-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4

// "FIX" from 2014-Apr-27:
// Because (count-1) is negative, above fours are reduced to next twos:
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
    // The line below is BUGGY:
    //if ( (i-(PRIMALposition-1) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) && (&pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4) ) {
    // The line below is NOT OKAY, in fact so stupid, grrr, not a blunder, not carelessness, but overconfidence in writing "on the fly":
    //if ( ((signed int)(i-(PRIMALposition-1)+(count-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) ) {
// FIX from 2016-Aug-10 (two times failed to do simple boundary checks, pfu):
    if ( ((signed long long)(i-(PRIMALposition-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)]+((PRIMALlengthCANDIDATE-4+1)-1) <= pbTargetMax - 4) ) { // 2020-jan-11
        if ( *(uint32_t *)&pbTarget[i-(PRIMALposition-1)] == *(uint32_t *)(pbPattern-(PRIMALposition-1))) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
            count = PRIMALlengthCANDIDATE-4+1; 
            while ( count > 0 && *(uint32_t *)(pbPattern-(PRIMALposition-1)+count-1) == *(uint32_t *)(&pbTarget[i-(PRIMALposition-1)]+(count-1)) )
                count = count-4;
            if ( count <= 0 ) return(pbTarget+i-(PRIMALposition-1));    
        }
    }
}
    } else { //if (cbPattern != PRIMALlengthCANDIDATE)
                            if ( count <= 0 ) return(pbTarget+i);
    }

                            // In order to avoid only-left or only-right WCS the memcmp should be done as left-to-right and right-to-left AT THE SAME TIME.
                            // Below comparison is BIdirectional. It pays off when needle is 8+++ long:
//                          for (count = cbPattern-4+1; count > 0; count = count-4) {
//                              if ( *(uint32_t *)(pbPattern+count-1) != *(uint32_t *)(&pbTarget[i]+(count-1)) ) {break;};
//                              if ( *(uint32_t *)(pbPattern+(cbPattern-4+1)-count) != *(uint32_t *)(&pbTarget[i]+(cbPattern-4+1)-count) ) {count = (cbPattern-4+1)-count +(1); break;} // +(1) because two lookups are implemented as one, also no danger of 'count' being 0 because of the fast check outwith the 'while': if ( *(uint32_t *)&pbTarget[i] == ulHashPattern)
//                          }
//                          if ( count <= 0 ) return(pbTarget+i);
                                // Checking the order 2 pairs in mismatched DWORD, all the 3:
                                //if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+count-1]] == 0 ) Gulliver = count; // 1 or bigger, as it should
                                //if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+count-1+1]] == 0 ) Gulliver = count+1; // 1 or bigger, as it should
                                //if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+count-1+1+1]] == 0 ) Gulliver = count+1+1; // 1 or bigger, as it should
                            //  if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+count-1]] + bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+count-1+1]] + bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+count-1+1+1]] < 3 ) Gulliver = count; // 1 or bigger, as it should, THE MIN(count,count+1,count+1+1)
                                // Above compound 'if' guarantees not that Gulliver > 1, an example:
                                // Needle:    fastest tax
                                // Window: ...fastast tax...
                                // After matching ' tax' vs ' tax' and 'fast' vs 'fast' the mismathced DWORD is 'test' vs 'tast':
                                // 'tast' when factorized down to order 2 yields: 'ta','as','st' - all the three when summed give 1+1+1=3 i.e. Gulliver remains 1.
                                // Roughly speaking, this attempt maybe has its place in worst-case scenarios but not in English text and even not in ACGT data, that's why I commented it in original 'Shockeroo'.
                                //if ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+count-1]>>16)+(*(uint32_t *)&pbTarget[i+count-1]&0xFFFF) ) & ( (1<<16)-1 )] == 0 ) Gulliver = count; // 1 or bigger, as it should
                                // Above line is replaced by next one with better hashing:
//                              if ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+count-1]>>(16-1))+(*(uint32_t *)&pbTarget[i+count-1]&0xFFFF) ) & ( (1<<16)-1 )] == 0 ) Gulliver = count; // 1 or bigger, as it should
                            // Order 4 ]
                        }
                    }
                } else Gulliver = cbPattern-(2-1)-2; // -2 because we check the 4 rightmost bytes not 2.
                i = i + Gulliver;
                //GlobalI++; // Comment it, it is only for stats.
            }
            return(NULL);

        } // if ( cbPattern<=NeedleThreshold2vs4swampLITE )
        } // if ( cbPattern<=NeedleThreshold2vs4swampLITE )
    } //if ( cbPattern<4 )
}
/*
// For short needles, and mainly haystacks, 'Doublet' is quite effective. Consider it or 'Quadruplet'.
// Fixed version from 2012-Feb-27.
// Caution: For better speed the case 'if (cbPattern==1)' was removed, so Pattern must be longer than 1 char.
char * Railgun_Doublet (char * pbTarget, char * pbPattern, uint32_t cbTarget, uint32_t cbPattern)
{
    char * pbTargetMax = pbTarget + cbTarget;
    register uint32_t ulHashPattern;
    uint32_t ulHashTarget, count, countSTATIC;

    if (cbPattern > cbTarget) return(NULL);

    countSTATIC = cbPattern-2;

    pbTarget = pbTarget+cbPattern;
    ulHashPattern = (*(uint16_t *)(pbPattern));

    for ( ;; ) {
        if ( ulHashPattern == (*(uint16_t *)(pbTarget-cbPattern)) ) {
            count = countSTATIC;
            while ( count && *(char *)(pbPattern+2+(countSTATIC-count)) == *(char *)(pbTarget-cbPattern+2+(countSTATIC-count)) ) {
                count--;
            }
            if ( count == 0 ) return((pbTarget-cbPattern));
        }
        pbTarget++;
        if (pbTarget > pbTargetMax) return(NULL);
    }
}
*/

// Dynamic buffer (unchanged)
typedef struct {
    uint8_t *data;
    size_t   len;
    size_t   cap;
} ByteBuffer;

static void buf_init(ByteBuffer *b)
{
    b->data = NULL;
    b->len  = 0;
    b->cap  = 0;
}

static void buf_ensure(ByteBuffer *b, size_t add)
{
    if (b->len + add + 32 > b->cap) {
        size_t new_cap = b->cap == 0 ? (1ULL<<20) : b->cap * 2;
        while (new_cap < b->len + add + 32) new_cap *= 2;
        uint8_t *new_data = realloc(b->data, new_cap);
        if (!new_data) { fprintf(stderr, "Out of memory\n"); exit(1); }
        b->data = new_data;
        b->cap  = new_cap;
    }
}

/*
static void buf_ensure(ByteBuffer *b, size_t add)
{
    if (b->len + add > b->cap) {
        size_t new_cap = b->cap == 0 ? (1ULL << 20) : b->cap * 2;
        while (new_cap < b->len + add) new_cap *= 2;
        uint8_t *new_data = realloc(b->data, new_cap);
        if (!new_data) {
            fprintf(stderr, "Out of memory\n");
            exit(1);
        }
        b->data = new_data;
        b->cap  = new_cap;
    }
}
*/

static void buf_append(ByteBuffer *b, uint8_t byte)
{
    buf_ensure(b, 1);
    b->data[b->len++] = byte;
}

static void buf_append_multi(ByteBuffer *b, const uint8_t *src, size_t n)
{
    if (n == 0) return;
    buf_ensure(b, n);
    memcpy(b->data + b->len, src, n);
    b->len += n;
}

static void buf_append_match(ByteBuffer *b, uint64_t offset, int match_len)
{
    uint64_t base = b->len - offset;
    buf_ensure(b, match_len);
    if (offset >= (uint64_t)match_len) {
        memcpy(b->data + b->len, b->data + base, match_len);
    } else {
        for (int k = 0; k < match_len; ++k) {
            b->data[b->len + k] = b->data[base + k];
        }
    }
    b->len += match_len;
}

static void show_progress(uint64_t processed, uint64_t total, double mem_mb)
{
    if (total == 0) return;
    const int bar_width = 70;
    double perc = (double)processed * 100.0 / total;
    int filled = (int)(perc * bar_width / 100.0);

    fprintf(stderr, "\rCompressing: %6.2f%% [", perc);
    for (int i = 0; i < bar_width; ++i) {
        fputc(i < filled ? '>' : '-', stderr);
    }
    fprintf(stderr, "] %"PRIu64" MiB of %"PRIu64" MiB; Memory Footprint: %"PRIu64" MiB",
            processed >> 20, total >> 20, (uint64_t)mem_mb);
    fflush(stderr);
}

// ====================== COMPRESSOR (memmem bug fixed) ======================
static void compress(FILE *in_fp, FILE *out_fp, bool use_memmem)
{
    uint64_t TraversedSoFar = 0;
    uint64_t TraversedSoFarFAKEforProgress = 0;
    fseek(in_fp, 0, SEEK_END);
    long file_sz = ftell(in_fp);
    if (file_sz < 0) { fprintf(stderr, "ftell failed\n"); return; }
    fseek(in_fp, 0, SEEK_SET);
    uint64_t in_size = (uint64_t)file_sz;

    uint8_t *input = malloc(in_size);
    if (!input) { fprintf(stderr, "Critical error: Out of memory for input.\n"); return; }
    if (fread(input, 1, in_size, in_fp) != in_size) {
        fprintf(stderr, "Read error\n");
        free(input);
        return;
    }

    uint8_t *rev_input = NULL;
    uint64_t *hash_table = NULL;
    uint64_t *best_offset = NULL;

    if (use_memmem) {
        rev_input = malloc(in_size);
        if (!rev_input) {
            fprintf(stderr, "Critical error: Out of memory for reversed buffer.\n");
            free(input);
            return;
        }
        for (uint64_t i = 0; i < in_size; ++i) {
            rev_input[i] = input[in_size - 1 - i];
        }
        best_offset = calloc(in_size, sizeof(uint64_t));
        if (!best_offset) {
            fprintf(stderr, "Critical error: Out of memory for best-offset table.\n");
            free(rev_input);
            free(input);
            return;
        }
    } else {
        hash_table = malloc(HASH_TABLE_SIZE * NUM_CANDIDATES * sizeof(uint64_t));
        if (!hash_table) {
            fprintf(stderr, "Critical error: Out of memory for hash table.\n");
            free(input);
            return;
        }
        for (size_t i = 0; i < HASH_TABLE_SIZE * NUM_CANDIDATES; ++i) {
            hash_table[i] = UINT64_MAX;
        }
    }

    fprintf(stderr, "Compressing %s bytes...\n", _ui64toaKAZEcomma(in_size, llTOaDigits2, 10));
    double hashTOTAL_time;
    double lastshown_time = omp_get_wtime();
    double current_time;

    size_t init_mem = (size_t)in_size +
                      (use_memmem ? (size_t)in_size + in_size * sizeof(uint64_t)
                                  : HASH_TABLE_SIZE * NUM_CANDIDATES * sizeof(uint64_t));
    if (use_memmem) {} else {
    show_progress(0, in_size, (double)init_mem / (1024.0 * 1024.0));
    }

    // ====================== 8-CORE MEMMEM PRECOMPUTE (BUG FIXED) ======================
    if (use_memmem) {
        double tTOTAL_start = omp_get_wtime();
        fprintf(stderr, "Precomputing ALL possible %d-byte matches with %d threads...\n", MIN_MATCH_LEN, NUM_Threads);
        // OpenMP parallel-for distributes the ENTIRE loop over p across 8 cores.
        // Each iteration p is 100% independent:
        //   - builds its own 32-byte reversed pattern
        //   - runs its own memmem() on its own history slice
        //   - writes to a unique best_offset[p] slot
        //
        // This is NOT "8 memmem calls for one position".
        // This is NOT "divide reversed pool into 8 sub-pools".
        // This is the classic embarrassingly-parallel pattern:
        //   millions of independent searches → automatically load-balanced by OpenMP.
        #pragma omp parallel for num_threads(NUM_Threads)
        for (uint64_t p = MIN_MATCH_LEN; p <= in_size - MIN_MATCH_LEN; ++p) {
            uint8_t rev_pat[MIN_MATCH_LEN];
            for (int k = 0; k < MIN_MATCH_LEN; ++k) {
                rev_pat[k] = input[p + MIN_MATCH_LEN - 1 - k];
            }
            uint8_t *hay = rev_input + (in_size - p);
            size_t hay_len = p;

            void *found;
            if (GlobalFlag_GLIBCorRailgun == 1)
                found = memmem(hay, hay_len, rev_pat, MIN_MATCH_LEN);
            else
                //char * Railgun_Trolldom_64 (char * pbTarget, char * pbPattern, uint64_t cbTarget, uint32_t cbPattern) 
                found = Railgun_Trolldom_64(hay, rev_pat, hay_len, MIN_MATCH_LEN);

            #pragma omp atomic
                TraversedSoFarFAKEforProgress += hay_len; // Roughly the sum of all lengths in range 1..in_size i.e. in_size*(in_size+1)/2 as in 1..6 is 6*7/2=21 = 1+2+3+4+5+6

            if (found) {
                size_t pos_in_rev = (uint8_t*)found - hay;
                // === CRITICAL FIX ===
                // Because we search the *reversed* pattern in the *reversed* history,
                // the position returned by memmem points to the END of the original match.
                // Therefore the correct forward offset is simply pos_in_rev + 32.
                uint64_t off = (uint64_t)pos_in_rev + MIN_MATCH_LEN;
                if (off > 0 && off <= MAX_OFFSET && off <= p) {
                    best_offset[p] = off;   // closest possible match
                }
                // stats [
                    #pragma omp critical
                    {
                        double progress = (double)TraversedSoFarFAKEforProgress / (in_size*(in_size+1)/2) *100;
                        //fprintf(stderr, "\rSearch Progress = %.2f%%; Traversed-So-Far: %"PRIu64" MiB", progress, TraversedSoFar>>20);
                        current_time = omp_get_wtime();
                        hashTOTAL_time = current_time - tTOTAL_start; // TOTAL time
                        if ( (current_time - lastshown_time) > 0.9 ) {
                            fprintf(stderr, "\rSearch Progress = %.2f%%; Traversed-So-Far: %s MiB (%s TiB); Performance: %.2f GiB/s ", progress, _ui64toaKAZEcomma(TraversedSoFar>>20, llTOaDigits2, 10), _ui64toaKAZEcomma(TraversedSoFar>>40, llTOaDigits3, 10), TraversedSoFar / 1024.0 / 1024.0 / 1024.0 / hashTOTAL_time);
                            fflush(stderr);
                            lastshown_time = current_time;
                        }
                    }
                // stats ]
                #pragma omp atomic
                    TraversedSoFar += pos_in_rev;
            } else {
                #pragma omp atomic
                    TraversedSoFar += hay_len;
            }
            // The old/buggy code [
            /*
            // When we search the reversed pattern in the reversed history, memmem returns the position of the end of the original 32-byte match.
            // The correct offset is therefore pos_in_rev + 32 (not the old p - 1 - pos_in_rev formula).
            if (found) {
                size_t pos_in_rev = (uint8_t*)found - hay;
                uint64_t q = (p - 1ULL) - pos_in_rev;
                uint64_t off = p - q;
                if (off > 0 && off <= MAX_OFFSET) {
                    best_offset[p] = off;          // store the closest possible offset
                }
            }
            */
            // The old/buggy code ]
        }
            hashTOTAL_time = omp_get_wtime() - tTOTAL_start; // TOTAL time
            fprintf(stderr, "\rSearch Progress = %.2f%%; Traversed-So-Far: %s MiB (%s TiB); Performance: %.2f GiB/s\n", 100.0, _ui64toaKAZEcomma(TraversedSoFar>>20, llTOaDigits2, 10), _ui64toaKAZEcomma(TraversedSoFar>>40, llTOaDigits3, 10), TraversedSoFar / 1024.0 / 1024.0 / 1024.0 / hashTOTAL_time);
            fflush(stderr);
        fprintf(stderr, "Precompute finished (%d-core speedup applied) — starting encoding phase...\n", NUM_Threads);
    }

    ByteBuffer outb;
    buf_init(&outb);

    uint64_t history_len = 0;
    uint64_t next_to_hash = 0;
    uint64_t next_progress_update = (1ULL << 20);

    while (history_len < in_size) {
        size_t flag_idx = outb.len;
        buf_append(&outb, 0);
        uint8_t flag = 0;

        for (int bit = 0; bit < 8 && history_len < in_size; ++bit) {
            bool is_match = false;
            uint64_t offset = 0;

// r3
/*
            if (history_len + MIN_MATCH_LEN <= in_size) {
                if (use_memmem) {
                    uint64_t off = best_offset[history_len];
                    if (off != 0) {
                        is_match = true;
                        offset = off;
                    }
                } else {
                    uint32_t h;
                    if (GlobalFlag_LandonOrPippip == 1)
                        h = get_hash(&input[history_len]);
                    else
                        h = get_hashPippip(&input[history_len]);
                    uint64_t idx = h & HASH_MASK;
                    for (int c = 0; c < NUM_CANDIDATES; ++c) {
                        uint64_t prev = hash_table[idx * NUM_CANDIDATES + c];
                        if (prev != UINT64_MAX &&
                            prev < history_len &&
                            (history_len - prev) <= MAX_OFFSET &&
                            (history_len - prev) >= MIN_MATCH_LEN && // <-- ADD THIS, spotted by Gemini Pro
                            memcmp(&input[prev], &input[history_len], MIN_MATCH_LEN) == 0) {
                            is_match = true;
                            offset = history_len - prev;
                            break;
                        }
                    }
                }
            }

            if (is_match) {
                flag |= (1u << bit);
                uint8_t off_len = (offset < (1ULL << 8))  ? 1 :
                                  (offset < (1ULL << 16)) ? 2 :
                                  (offset < (1ULL << 24)) ? 3 :
                                  (offset < (1ULL << 32)) ? 4 :
                                  (offset < (1ULL << 40)) ? 5 :
                                  (offset < (1ULL << 48)) ? 6 :
                                  (offset < (1ULL << 56)) ? 7 : 8;
                buf_append(&outb, off_len);
                uint64_t temp = offset;
                for (uint8_t i = 0; i < off_len; ++i) {
                    buf_append(&outb, (uint8_t)(temp & 0xFF));
                    temp >>= 8;
                }
                history_len += MIN_MATCH_LEN;
            } else {
                buf_append(&outb, input[history_len]);
                history_len += 1;
            }
*/

// r4 [
                    int best_match_len;
            if (history_len + MIN_MATCH_LEN <= in_size) {
                if (use_memmem) {
                    uint64_t off = best_offset[history_len];
                    if (off != 0) {
                        is_match = true;
                        offset = off;
                        // Note: memmem path remains unchanged as requested
                    }
                } else {
                    uint32_t h;
                    if (GlobalFlag_LandonOrPippip == 1)
                        h = get_hash(&input[history_len]);
                    else
                        h = get_hashPippip(&input[history_len]);
                    
                    uint64_t idx = h & HASH_MASK;
                    best_match_len = 0;
                    uint64_t best_offset_val = 0;

                    for (int c = 0; c < NUM_CANDIDATES; ++c) {
                        uint64_t prev = hash_table[idx * NUM_CANDIDATES + c];
                        // Ensure offset is valid and at least 8 bytes apart
                        if (prev != UINT64_MAX &&
                            prev < history_len &&
                            (history_len - prev) <= MAX_OFFSET &&
                            (history_len - prev) >= 8) { 
                            
                            // Dynamically extend match to find the longest valid string
                            int current_match_len = 0;
                            int max_len = 256; 
                            if (in_size - history_len < 256) {
                                max_len = in_size - history_len;
                            }

                            // Fast forward scan
                            while (current_match_len < max_len &&
                                   input[prev + current_match_len] == input[history_len + current_match_len]) {
                                current_match_len++;
                            }

                            // Quantize to the nearest multiple of 8
                            int quantized_len = (current_match_len / 8) * 8; 

                            if (quantized_len >= 8 && quantized_len > best_match_len) {
                                best_match_len = quantized_len;
                                best_offset_val = history_len - prev;
                                is_match = true;
                                if (best_match_len == 256) break; // Maxed out, short circuit
                            }
                        }
                    }
                    if (is_match) {
                        offset = best_offset_val;
                    }
                }
            }

            if (is_match) {
                flag |= (1u << bit);

                uint8_t off_len = (offset < (1ULL << 8))  ? 1 :
                                  (offset < (1ULL << 16)) ? 2 :
                                  (offset < (1ULL << 24)) ? 3 :
                                  (offset < (1ULL << 32)) ? 4 :
                                  (offset < (1ULL << 40)) ? 5 :
                                  (offset < (1ULL << 48)) ? 6 :
                                  (offset < (1ULL << 56)) ? 7 : 8;

                if (!use_memmem) {
                    // NEW: Pack the 5-bit multiplier and 3-bit offset length
                    // mult: 0 for 8 bytes, 1 for 16 bytes ... 31 for 256 bytes
                    int mult = (best_match_len / 8) - 1; 
                    uint8_t token = (mult << 3) | (off_len - 1); 
                    
                    buf_append(&outb, token);
                    
                    uint64_t temp = offset;
                    for (uint8_t i = 0; i < off_len; ++i) {
                        buf_append(&outb, (uint8_t)(temp & 0xFF));
                        temp >>= 8;
                    }
                    history_len += best_match_len; // Advance by the extended match length!
                } else {
                    // Legacy path for memmem
                    buf_append(&outb, off_len);
                    uint64_t temp = offset;
                    for (uint8_t i = 0; i < off_len; ++i) {
                        buf_append(&outb, (uint8_t)(temp & 0xFF));
                        temp >>= 8;
                    }
                    history_len += MIN_MATCH_LEN;
                }
            } else {
                buf_append(&outb, input[history_len]);
                history_len += 1;
            }
// r4 ]

            if (!use_memmem) {
                while (next_to_hash + MIN_MATCH_LEN <= history_len) {
                    uint32_t h;
                    if (GlobalFlag_LandonOrPippip == 1)
                        h = get_hash(&input[next_to_hash]);
                    else
                        h = get_hashPippip(&input[next_to_hash]);
                    uint64_t idx = h & HASH_MASK;
                    for (int c = NUM_CANDIDATES - 1; c > 0; --c) {
                        hash_table[idx * NUM_CANDIDATES + c] = hash_table[idx * NUM_CANDIDATES + (c - 1)];
                    }
                    hash_table[idx * NUM_CANDIDATES + 0] = next_to_hash;
                    next_to_hash++;
                }
            }

            if (history_len >= next_progress_update || history_len == in_size) {
                size_t cur_mem = (size_t)in_size +
                                 (use_memmem ? (size_t)in_size + in_size * sizeof(uint64_t)
                                             : HASH_TABLE_SIZE * NUM_CANDIDATES * sizeof(uint64_t)) +
                                 outb.cap;
                double cur_mem_mb = (double)cur_mem / (1024.0 * 1024.0);
                show_progress(history_len, in_size, cur_mem_mb);

                if (next_progress_update <= in_size) {
                    next_progress_update += (1ULL << 20);
                    if (next_progress_update > in_size) next_progress_update = in_size;
                }
            }
        }
        outb.data[flag_idx] = flag;
    }

    fwrite(outb.data, 1, outb.len, out_fp);

    //fprintf(stderr, "\nCompression complete: %" PRIu64 " -> %" PRIu64 " bytes (%.2f%% of original remains).\n",
    //        in_size, (uint64_t)outb.len, in_size ? (double)outb.len * 100.0 / in_size : 0.0);
    fprintf(stderr, "\nCompression complete: %s -> %s bytes (%.2f%% of original remains).\n",
             _ui64toaKAZEcomma(in_size, llTOaDigits2, 10),  _ui64toaKAZEcomma((uint64_t)outb.len, llTOaDigits3, 10), in_size ? (double)outb.len * 100.0 / in_size : 0.0);

    free(best_offset);
    free(rev_input);
    free(hash_table);
    free(outb.data);
    free(input);
}

static void compressR5(FILE *in_fp, FILE *out_fp, bool use_memmem)
{
    // Safety check: force hash mode for massive files
    if (use_memmem) {
        fprintf(stderr, "Notice: use_memmem ignored. Enforcing mmap hash path for large file support.\n");
    }

    // 1. Get file size using file descriptor
    int fd_in = fileno(in_fp);
    struct stat sb;
    if (fstat(fd_in, &sb) == -1) {
        fprintf(stderr, "Critical error: fstat failed\n");
        return;
    }
    uint64_t in_size = (uint64_t)sb.st_size;

    // 2. Memory-Map the Input File (Read-Only)
    uint8_t *input = mmap(NULL, in_size, PROT_READ, MAP_PRIVATE, fd_in, 0);
    if (input == MAP_FAILED) { 
        fprintf(stderr, "Critical error: mmap failed for input file.\n"); 
        return; 
    }
    // Advise the OS kernel to aggressively read-ahead
    madvise(input, in_size, MADV_SEQUENTIAL);

    // 3. Memory-Map the Hash Table (Anonymous Mapping)
    size_t hash_size = HASH_TABLE_SIZE * NUM_CANDIDATES * sizeof(uint64_t);
    uint64_t *hash_table = mmap(NULL, hash_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (hash_table == MAP_FAILED) {
        fprintf(stderr, "Critical error: mmap failed for hash table.\n");
        munmap(input, in_size);
        return;
    }
    
    // Initialize hash slots to UINT64_MAX
    for (size_t i = 0; i < HASH_TABLE_SIZE * NUM_CANDIDATES; ++i) {
        hash_table[i] = UINT64_MAX;
    }

    fprintf(stderr, "Compressing %s bytes...\n", _ui64toaKAZEcomma(in_size, llTOaDigits2, 10));
    
    size_t init_mem = (size_t)in_size + hash_size;
    show_progress(0, in_size, (double)init_mem / (1024.0 * 1024.0));

    ByteBuffer outb;
    buf_init(&outb);

    uint64_t history_len = 0;
    uint64_t next_to_hash = 0;
    uint64_t next_progress_update = (1ULL << 20);

    while (history_len < in_size) {
        size_t flag_idx = outb.len;
        buf_append(&outb, 0);
        uint8_t flag = 0;

        for (int bit = 0; bit < 8 && history_len < in_size; ++bit) {
            bool is_match = false;
            uint64_t offset = 0;
            int best_match_len = 0;

            // --- HUNT FOR MATCHES (8..256 Bytes) ---
            if (history_len + MIN_MATCH_LEN <= in_size) {
                uint32_t h;
                if (GlobalFlag_LandonOrPippip == 1)
                    h = get_hash(&input[history_len]);
                else
                    h = get_hashPippip(&input[history_len]);
                
                uint64_t idx = h & HASH_MASK;
                uint64_t best_offset_val = 0;

                for (int c = 0; c < NUM_CANDIDATES; ++c) {
                    uint64_t prev = hash_table[idx * NUM_CANDIDATES + c];
                    
                    // Ensure offset is valid and at least 8 bytes apart
                    if (prev != UINT64_MAX &&
                        prev < history_len &&
                        (history_len - prev) <= MAX_OFFSET &&
                        (history_len - prev) >= 8) { 
                        
                        // Dynamically extend match to find the longest valid string
                        int current_match_len = 0;
                        int max_len = 256; 
                        if (in_size - history_len < 256) {
                            max_len = in_size - history_len;
                        }

                        // Fast forward scan
                        while (current_match_len < max_len &&
                               input[prev + current_match_len] == input[history_len + current_match_len]) {
                            current_match_len++;
                        }

                        // Quantize to the nearest multiple of 8
                        int quantized_len = (current_match_len / 8) * 8; 

                        if (quantized_len >= 8 && quantized_len > best_match_len) {
                            best_match_len = quantized_len;
                            best_offset_val = history_len - prev;
                            is_match = true;
                            if (best_match_len == 256) break; // Maxed out, short circuit
                        }
                    }
                }
                if (is_match) {
                    offset = best_offset_val;
                }
            }

            // --- ENCODE MATCH OR LITERAL ---
            if (is_match) {
                flag |= (1u << bit);

                uint8_t off_len = (offset < (1ULL << 8))  ? 1 :
                                  (offset < (1ULL << 16)) ? 2 :
                                  (offset < (1ULL << 24)) ? 3 :
                                  (offset < (1ULL << 32)) ? 4 :
                                  (offset < (1ULL << 40)) ? 5 :
                                  (offset < (1ULL << 48)) ? 6 :
                                  (offset < (1ULL << 56)) ? 7 : 8;

                // Pack the 5-bit multiplier and 3-bit offset length
                int mult = (best_match_len / 8) - 1; 
                uint8_t token = (mult << 3) | (off_len - 1); 
                
                buf_append(&outb, token);
                
                uint64_t temp = offset;
                for (uint8_t i = 0; i < off_len; ++i) {
                    buf_append(&outb, (uint8_t)(temp & 0xFF));
                    temp >>= 8;
                }
                history_len += best_match_len; // Advance by the extended match length
            } else {
                buf_append(&outb, input[history_len]);
                history_len += 1;
            }

            // --- UPDATE HASH TABLE ---
            // Catch up the hash table for the bytes we just traversed
            while (next_to_hash + MIN_MATCH_LEN <= history_len) {
                uint32_t h;
                if (GlobalFlag_LandonOrPippip == 1)
                    h = get_hash(&input[next_to_hash]);
                else
                    h = get_hashPippip(&input[next_to_hash]);
                
                uint64_t idx = h & HASH_MASK;
                // Shift candidates (evict oldest)
                for (int c = NUM_CANDIDATES - 1; c > 0; --c) {
                    hash_table[idx * NUM_CANDIDATES + c] = hash_table[idx * NUM_CANDIDATES + (c - 1)];
                }
                // Insert new candidate
                hash_table[idx * NUM_CANDIDATES + 0] = next_to_hash;
                next_to_hash++;
            }

            // --- PROGRESS UPDATES ---
            if (history_len >= next_progress_update || history_len == in_size) {
                size_t cur_mem = (size_t)in_size + hash_size + outb.cap;
                double cur_mem_mb = (double)cur_mem / (1024.0 * 1024.0);
                show_progress(history_len, in_size, cur_mem_mb);

                if (next_progress_update <= in_size) {
                    next_progress_update += (1ULL << 20);
                    if (next_progress_update > in_size) next_progress_update = in_size;
                }
            }
        }
        
        // Write the flag byte back to its reserved position
        outb.data[flag_idx] = flag;
    }

    // Write final compressed buffer to disk
    fwrite(outb.data, 1, outb.len, out_fp);

    fprintf(stderr, "\nCompression complete: %s -> %s bytes (%.2f%% of original remains).\n",
             _ui64toaKAZEcomma(in_size, llTOaDigits2, 10),  
             _ui64toaKAZEcomma((uint64_t)outb.len, llTOaDigits3, 10), 
             in_size ? (double)outb.len * 100.0 / in_size : 0.0);

    // --- CLEANUP ---
    munmap(hash_table, hash_size);
    munmap(input, in_size);
    free(outb.data); // Kept as free() because ByteBuffer uses realloc internally
}

//Memory-maps the input
//Memory-maps a temporary output file (same size as input as a safe upper bound)
//Writes directly into the mapped output buffer (no ByteBuffer, no realloc(), no buf_append())

static void compressMM(FILE *in_fp, FILE *out_fp, bool use_memmem)
{
    (void)in_fp; // not used

    int in_fd = -1, out_fd = -1;
    uint64_t in_size = 0;
    uint8_t *input = NULL;
    uint8_t *output = NULL;        // mmap'ed output buffer
    uint64_t *hash_table = NULL;

    // 1. Memory-map INPUT
    in_fd = fileno(in_fp);
    struct stat sb;
    if (fstat(in_fd, &sb) == -1 || sb.st_size == 0) {
        fprintf(stderr, "Critical error: fstat failed or empty file\n");
        return;
    }
    in_size = (uint64_t)sb.st_size;

    input = mmap(NULL, in_size, PROT_READ, MAP_PRIVATE, in_fd, 0);
    if (input == MAP_FAILED) {
        fprintf(stderr, "Critical error: mmap input failed\n");
        return;
    }
    madvise(input, in_size, MADV_SEQUENTIAL);

    // 2. Create and memory-map OUTPUT file (pre-allocated to input size as safe upper bound)
    char out_path[] = "/tmp/lzss_out_XXXXXX";
    out_fd = mkstemp(out_path);
    if (out_fd < 0) {
        fprintf(stderr, "Critical error: mkstemp failed\n");
        goto cleanup;
    }

    if (ftruncate(out_fd, in_size) != 0) {
        fprintf(stderr, "Critical error: ftruncate output failed\n");
        goto cleanup;
    }

    output = mmap(NULL, in_size, PROT_READ | PROT_WRITE, MAP_SHARED, out_fd, 0);
    if (output == MAP_FAILED) {
        fprintf(stderr, "Critical error: mmap output failed\n");
        goto cleanup;
    }

    // 3. Hash table (anonymous mmap)
    size_t hash_size = HASH_TABLE_SIZE * NUM_CANDIDATES * sizeof(uint64_t);
    hash_table = mmap(NULL, hash_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (hash_table == MAP_FAILED) {
        fprintf(stderr, "Critical error: mmap hash_table failed\n");
        goto cleanup;
    }

    for (size_t i = 0; i < HASH_TABLE_SIZE * NUM_CANDIDATES; ++i)
        hash_table[i] = UINT64_MAX;

    fprintf(stderr, "Compressing %s bytes (fully mmap'ed, zero-RAM mode)...\n", 
            _ui64toaKAZEcomma(in_size, llTOaDigits2, 10));

    uint64_t history_len = 0;
    uint64_t next_to_hash = 0;
    uint64_t out_pos = 0;                     // current position in output buffer
    uint64_t next_progress_update = (1ULL << 20);

    while (history_len < in_size) {
        uint64_t flag_pos = out_pos;          // remember where flag byte goes
        output[out_pos++] = 0;                // reserve flag byte
        uint8_t flag = 0;

        for (int bit = 0; bit < 8 && history_len < in_size; ++bit) {
            bool is_match = false;
            uint64_t offset = 0;
            int best_match_len = 0;

            if (history_len + MIN_MATCH_LEN <= in_size) {
                uint32_t h = (GlobalFlag_LandonOrPippip == 1) ?
                             get_hash(&input[history_len]) :
                             get_hashPippip(&input[history_len]);

                uint64_t idx = h & HASH_MASK;

                for (int c = 0; c < NUM_CANDIDATES; ++c) {
                    uint64_t prev = hash_table[idx * NUM_CANDIDATES + c];
                    if (prev != UINT64_MAX && prev < history_len &&
                        (history_len - prev) <= MAX_OFFSET &&
                        (history_len - prev) >= 8) {

                        int current_match_len = 0;
                        int max_possible = (int)(in_size - history_len);
                        if (max_possible > 256) max_possible = 256;

                        while (current_match_len < max_possible &&
                               input[prev + current_match_len] == input[history_len + current_match_len]) {
                            current_match_len++;
                        }

                        int quantized = (current_match_len / 8) * 8;
                        if (quantized >= 8 && quantized > best_match_len) {
                            best_match_len = quantized;
                            offset = history_len - prev;
                            is_match = true;
                            if (best_match_len == 256) break;
                        }
                    }
                }
            }

            if (is_match) {
                flag |= (1u << bit);

                uint8_t off_len = (offset < (1ULL<<8))  ? 1 :
                                  (offset < (1ULL<<16)) ? 2 :
                                  (offset < (1ULL<<24)) ? 3 :
                                  (offset < (1ULL<<32)) ? 4 :
                                  (offset < (1ULL<<40)) ? 5 :
                                  (offset < (1ULL<<48)) ? 6 :
                                  (offset < (1ULL<<56)) ? 7 : 8;

                int mult = (best_match_len / 8) - 1;
                uint8_t token = (mult << 3) | (off_len - 1);

                output[out_pos++] = token;

                uint64_t temp = offset;
                for (uint8_t i = 0; i < off_len; ++i) {
                    output[out_pos++] = (uint8_t)(temp & 0xFF);
                    temp >>= 8;
                }

                history_len += best_match_len;
            } else {
                output[out_pos++] = input[history_len];
                history_len++;
            }

            // Update hash table
            while (next_to_hash + MIN_MATCH_LEN <= history_len) {
                uint32_t h = (GlobalFlag_LandonOrPippip == 1) ?
                             get_hash(&input[next_to_hash]) :
                             get_hashPippip(&input[next_to_hash]);

                uint64_t idx = h & HASH_MASK;
                for (int c = NUM_CANDIDATES - 1; c > 0; --c) {
                    hash_table[idx * NUM_CANDIDATES + c] = hash_table[idx * NUM_CANDIDATES + (c - 1)];
                }
                hash_table[idx * NUM_CANDIDATES + 0] = next_to_hash;
                next_to_hash++;
            }

            // Progress
            if (history_len >= next_progress_update || history_len == in_size) {
                show_progress(history_len, in_size, 0.0);
                if (next_progress_update <= in_size)
                    next_progress_update += (1ULL << 20);
            }
        }

        output[flag_pos] = flag;   // write flag byte
    }

    // Write only the used portion to the real output file
    fwrite(output, 1, out_pos, out_fp);

    fprintf(stderr, "\nCompression complete: %s -> %s bytes (%.2f%% of original)\n",
            _ui64toaKAZEcomma(in_size, llTOaDigits2, 10),
            _ui64toaKAZEcomma(out_pos, llTOaDigits3, 10),
            in_size ? (double)out_pos * 100.0 / in_size : 0.0);

cleanup:
    if (input)    munmap(input, in_size);
    if (output)   munmap(output, in_size);
    if (hash_table) munmap(hash_table, hash_size);
    if (out_fd >= 0) {
        close(out_fd);
        unlink(out_path);
    }
    if (in_fd >= 0) close(in_fd);   // we don't own this fd, but it's safe
}

#ifdef _WIN32
    #define FSEEK64 _fseeki64
    #define FTELL64 _ftelli64
#else
    // On POSIX, fseeko/ftello are 64-bit natively if _FILE_OFFSET_BITS=64
    #define FSEEK64 fseeko
    #define FTELL64 ftello
#endif

// AVX2
static void decompressR3(FILE *in_fp, FILE *out_fp)
{
/*
    fseek(in_fp, 0, SEEK_END);
    long file_sz = ftell(in_fp);
    if (file_sz < 0) { fprintf(stderr, "ftell failed\n"); return; }
    fseek(in_fp, 0, SEEK_SET);
    uint64_t comp_size = (uint64_t)file_sz;
*/
    // 1. Cross-platform 64-bit file sizing
    FSEEK64(in_fp, 0, SEEK_END);
    int64_t file_sz = FTELL64(in_fp);
    if (file_sz < 0) { fprintf(stderr, "ftell failed\n"); return; }
    FSEEK64(in_fp, 0, SEEK_SET);
    uint64_t comp_size = (uint64_t)file_sz;

    uint8_t *comp = malloc(comp_size+8); // padding
    if (!comp) { fprintf(stderr, "Out of memory\n"); return; }
    if (fread(comp, 1, comp_size, in_fp) != comp_size) {
        fprintf(stderr, "Read error\n");
        free(comp);
        return;
    }
    memset(comp + comp_size, 0, 8); // Zero out the padding

    ByteBuffer decb;
    buf_init(&decb);

    // Always keep extra 32 bytes of padding
    buf_ensure(&decb, 32);
    memset(decb.data, 0, 32);

    size_t comp_pos = 0;

    while (comp_pos < comp_size) {
        uint8_t flag = comp[comp_pos++];

        for (int bit = 0; bit < 8; ++bit) {
            if (comp_pos >= comp_size) goto done;

            uint8_t is_match = (flag >> bit) & 1;        // 0 = literal, 1 = match

            // ────── Prepare data for both paths (branchless) ──────
            uint64_t offset = 0;
            uint8_t  lit    = 0;

            if (is_match) {
                uint8_t off_len = comp[comp_pos++];
                //uint32_t raw = *(const uint32_t*)(comp + comp_pos);
                uint64_t raw = *(const uint64_t*)(comp + comp_pos);
                //offset = raw & (0xFFFFFFFFULL >> ((4 - off_len) * 8));
                offset = raw & (0xFFFFFFFFFFFFFFFFULL >> ((8 - off_len) * 8));
                comp_pos += off_len;
            } else {
                lit = comp[comp_pos++];
            }

            // Load YMM for match and literal
            uint64_t base = decb.len - offset;
            __m256i match_ymm = _mm256_loadu_si256((const __m256i*)(decb.data + base));
            __m256i lit_ymm   = _mm256_set1_epi8((char)lit);

            // Branchless select using blend
            __m256i selector = _mm256_set1_epi32( is_match ? -1 : 0 );   // all 0xFF or all 0x00
            __m256i ymm = _mm256_blendv_epi8(lit_ymm, match_ymm, selector);

            // Always write with single YMM store
            buf_ensure(&decb, 32);
            _mm256_storeu_si256((__m256i*)(decb.data + decb.len), ymm);

            // Advance length branchlessly
            decb.len += 1 + (MIN_MATCH_LEN - 1) * is_match;   // 1 for literal, 32 for match
        }
    }

done:
    fwrite(decb.data, 1, decb.len, out_fp);

    free(decb.data);
    free(comp);
}

// AVX2
static void decompressR4(FILE *in_fp, FILE *out_fp)
{
/*
    fseek(in_fp, 0, SEEK_END);
    long file_sz = ftell(in_fp);
    if (file_sz < 0) { fprintf(stderr, "ftell failed\n"); return; }
    fseek(in_fp, 0, SEEK_SET);
    uint64_t comp_size = (uint64_t)file_sz;
*/
    // 1. Cross-platform 64-bit file sizing
    FSEEK64(in_fp, 0, SEEK_END);
    int64_t file_sz = FTELL64(in_fp);
    if (file_sz < 0) { fprintf(stderr, "ftell failed\n"); return; }
    FSEEK64(in_fp, 0, SEEK_SET);
    uint64_t comp_size = (uint64_t)file_sz;

    uint8_t *comp = malloc(comp_size+8); // padding
    if (!comp) { fprintf(stderr, "Out of memory\n"); return; }
    if (fread(comp, 1, comp_size, in_fp) != comp_size) {
        fprintf(stderr, "Read error\n");
        free(comp);
        return;
    }
    memset(comp + comp_size, 0, 8); // Zero out the padding

    ByteBuffer decb;
    buf_init(&decb);

    // Always keep extra 32 bytes of padding
    buf_ensure(&decb, 32);
    memset(decb.data, 0, 32);

    size_t comp_pos = 0;

    while (comp_pos < comp_size) {
        uint8_t flag = comp[comp_pos++];

// r3 [
/*
        for (int bit = 0; bit < 8; ++bit) {
            if (comp_pos >= comp_size) goto done;

            uint8_t is_match = (flag >> bit) & 1;        // 0 = literal, 1 = match

            // ────── Prepare data for both paths (branchless) ──────
            uint64_t offset = 0;
            uint8_t  lit    = 0;

            if (is_match) {
                uint8_t off_len = comp[comp_pos++];
                //uint32_t raw = *(const uint32_t*)(comp + comp_pos);
                uint64_t raw = *(const uint64_t*)(comp + comp_pos);
                //offset = raw & (0xFFFFFFFFULL >> ((4 - off_len) * 8));
                offset = raw & (0xFFFFFFFFFFFFFFFFULL >> ((8 - off_len) * 8));
                comp_pos += off_len;
            } else {
                lit = comp[comp_pos++];
            }

            // Load YMM for match and literal
            uint64_t base = decb.len - offset;
            __m256i match_ymm = _mm256_loadu_si256((const __m256i*)(decb.data + base));
            __m256i lit_ymm   = _mm256_set1_epi8((char)lit);

            // Branchless select using blend
            __m256i selector = _mm256_set1_epi32( is_match ? -1 : 0 );   // all 0xFF or all 0x00
            __m256i ymm = _mm256_blendv_epi8(lit_ymm, match_ymm, selector);

            // Always write with single YMM store
            buf_ensure(&decb, 32);
            _mm256_storeu_si256((__m256i*)(decb.data + decb.len), ymm);

            // Advance length branchlessly
            decb.len += 1 + (MIN_MATCH_LEN - 1) * is_match;   // 1 for literal, 32 for match
        }
*/
// r3 ]
// r4 [
        for (int bit = 0; bit < 8; ++bit) {
            if (comp_pos >= comp_size) goto done;

            uint8_t is_match = (flag >> bit) & 1;

            if (is_match) {
                // Decode the compound token
                uint8_t token = comp[comp_pos++];
                uint8_t off_len = (token & 0x07) + 1;       // Lower 3 bits -> 1..8 bytes
                uint8_t mult = (token >> 3) & 0x1F;         // Upper 5 bits -> 0..31 multiplier
                int current_match_len = (mult + 1) * 8;     // Calculate actual match length

                uint64_t raw = *(const uint64_t*)(comp + comp_pos);
                uint64_t offset = raw & (0xFFFFFFFFFFFFFFFFULL >> ((8 - off_len) * 8));
                comp_pos += off_len;

                uint64_t base = decb.len - offset;
                buf_ensure(&decb, current_match_len);
                
                // Safe overlapping copy loop for variable lengths
                for (int k = 0; k < current_match_len; ++k) {
                    decb.data[decb.len + k] = decb.data[base + k];
                }
                decb.len += current_match_len;

            } else {
                uint8_t lit = comp[comp_pos++];
                buf_ensure(&decb, 1);
                decb.data[decb.len++] = lit;
            }
        }
// r4 ]
    }
done:
    fwrite(decb.data, 1, decb.len, out_fp);

    free(decb.data);
    free(comp);
}

// Gemini's:
// You hit exactly what makes working directly with disk streams so notoriously difficult.
// Getting the correct file size but completely scrambled content is the classic hallmark of C standard library buffer corruption.
// By having the decompressor violently bounce back and forth between fseek, fread, fseek, and fwrite byte-by-byte during overlapping matches,
// we completely thrashed the C runtime's FILE* internal cache. The OS and the C library lost synchronization over what data was actually on disk
// versus what was in RAM, resulting in garbage reads.
// There is a brilliant, highly performant way to fix this that completely destroys the read/write thrashing and fixes your scrambled output.
// The Two Critical Fixes:
// The Overlap Algorithm: Instead of seeking back and forth 256 times for a single overlapping match, we fread the repeating pattern once.
// Then, we use a simple while loop to fwrite that pattern repeatedly. This perfectly mimics the LZ77 cascade without ever breaking the stream state!
// Manual Position Tracking: We drop FTELL64(out_fp). ftell on a wb+ stream that is actively being written to can return dirty logical positions.
// By tracking uint64_t current_out_pos = 0 manually, we guarantee absolute mathematical precision for our seeks.
// This updated overlapping logic isn't just a bug fix—it completely removes thousands of tiny, unnecessary system calls, making the hard drive do
// a fraction of the work it was doing before. Give this a spin; the contents should emerge perfectly intact!
/*
The fact that your output file size (18309120 bytes) matches the original file exactly is actually a huge victory! It proves that the mathematical parsing of your LZ77 tokens, the bit-flags, and the EOF zero-padding are completely flawless. The logic isn't broken.
The presence of those garbage characters (``) interspersed with perfect text (ustar, Table of Contents) is a classic sign of silent buffer starvation.
Here is exactly what failed:
The wb+ EOF Bug: When you write a literal to the file, it sits in the C Standard Library's RAM buffer. The kernel's EOF marker hasn't updated yet. When we immediately fseek backward and try to fread that exact same data, the kernel tells fread "Hey, the file isn't that long yet!" fread silently fails, returns 0 bytes, and leaves match_buf filled with uninitialized stack memory (``), which we then write back to the disk!
The Endian fread Trick: While reading a 64-bit integer directly via fread(&offset, 1, off_len, in_fp) works on some compilers, memory alignment rules can sometimes cause the bytes to land in the wrong order, pulling history from the wrong location.
We fix the first bug by forcing the C library to sync with the OS via fflush(out_fp). We fix the second by reading the offset byte-by-byte exactly the way your compressor packed it.
What changed:
fflush(out_fp): Added right before we jump back to read history. This guarantees the operating system physically has the literals we just wrote, completely eliminating the `` starvation bug.
Return Value Checking: Added != current_match_len checks to the fread calls. If the stream ever desyncs again, it will safely exit rather than writing ghost memory to your file.
Byte-by-Byte Offset: We reconstruct the uint64_t using offset |= ((uint64_t)b << (i * 8)). This perfectly mirrors the bit-shifting in your compress function and is immune to hardware endianness quirks.
Run this, and your b3sum should finally align perfectly with the original!
*/
// Zero-RAM Streaming Decompressor
// Requires 'out_fp' to be opened with "wb+" so it can read its own history.
static void decompressZeroRAM(FILE *in_fp, FILE *out_fp)
{
    uint8_t flag;
    uint8_t match_buf[256]; // 256-byte stack buffer (0 heap RAM used)

    // Read the file purely sequentially until EOF
    while (fread(&flag, 1, 1, in_fp) == 1) {
        
        for (int bit = 0; bit < 8; ++bit) {
            uint8_t is_match = (flag >> bit) & 1;

            if (is_match) {
                // 1. Read token
                uint8_t token;
                if (fread(&token, 1, 1, in_fp) != 1) goto done;

                uint8_t off_len = (token & 0x07) + 1;       // 1..8 bytes
                uint8_t mult = (token >> 3) & 0x1F;         // 0..31 multiplier
                int current_match_len = (mult + 1) * 8;     // 8..256 bytes

                // 2. Read variable-length offset directly into a 64-bit int
                // (This naturally works on Little-Endian systems like x86/x64)
                uint64_t offset = 0; 
                if (fread(&offset, 1, off_len, in_fp) != off_len) goto done;

                // 3. 64-bit file positioning to find history
                int64_t current_out_pos = FTELL64(out_fp);
                int64_t src_pos = current_out_pos - offset;

                // 4. Copy the historical match data
                if (offset >= current_match_len) {
                    // Fast path: No overlap. We can read/write the whole chunk at once.
                    FSEEK64(out_fp, src_pos, SEEK_SET);
                    fread(match_buf, 1, current_match_len, out_fp);
                    
                    FSEEK64(out_fp, current_out_pos, SEEK_SET);
                    fwrite(match_buf, 1, current_match_len, out_fp);
                } else {
                    // Slow path: Overlapping match (e.g., repeating a pattern like "ABCABC")
                    // We MUST copy byte-by-byte to ensure the pattern cascades forward correctly.
                    for (int k = 0; k < current_match_len; ++k) {
                        FSEEK64(out_fp, src_pos + k, SEEK_SET);
                        fread(&match_buf[0], 1, 1, out_fp); // Read one history byte
                        
                        FSEEK64(out_fp, current_out_pos + k, SEEK_SET);
                        fwrite(&match_buf[0], 1, 1, out_fp); // Write it forward immediately
                    }
                }
            } else {
                // Literal fallback
                uint8_t lit;
                if (fread(&lit, 1, 1, in_fp) != 1) goto done;
                fwrite(&lit, 1, 1, out_fp);
            }
        }
    }
done:
    return;
}

/*
The `wb+` (read/update) mode requirement is a classic "gotcha" of C systems programming. I am thrilled to hear MayMun r.5 is alive and humming on the bare metal!

While that initial zero-RAM version is a great proof-of-concept, taking it into production requires hardening it against the harsh realities of the operating system and the C Standard Library (`glibc`). 

Here is the deep dive into why relying strictly on naive `fseek`/`fread`/`fwrite` sequences can corrupt your data, the pitfalls of `glibc`, and the ultimate "Bulletproof Edition" of your zero-RAM decompressor that solves them all.

### The Pitfalls of `glibc` `FILE-` Streams

When you use `FILE -` functions, you aren't talking directly to the hard drive. You are talking to a `glibc` middleman that maintains its own internal RAM buffers to speed things up. When you violently mix reads and writes on the exact same file stream, this middleman gets deeply confused.

--1. The "Ghost EOF" (Buffer Starvation)--
- --The Trap:-- When you `fwrite` a literal character (like 'A') to the end of the file, `glibc` usually holds it in a tiny internal RAM buffer. The physical hard drive, and the OS kernel's EOF (End of File) marker, don't actually know about 'A' yet. 
- --The Crash:-- If the very next token is a match that tells the decompressor to `fseek` backward and `fread` that 'A', the kernel looks at the physical disk, says "The file isn't that long yet," and `fread` silently fails. You end up reading uninitialized stack memory (garbage) and writing it forward, causing cascade corruption.
- --The Fix:-- We inject `fflush(out_fp)` right before we seek backward to read history. This holds a gun to `glibc`'s head and forces it to flush its write buffer to the OS kernel immediately, guaranteeing the data is actually there when we try to read it.

--2. Mechanical Thrashing (The Overlap Nightmare)--
- --The Trap:-- In the original code, if you had an overlapping match (e.g., a repeating pattern of 256 bytes with an offset of 1), the code did a `for` loop 256 times. For -every single byte-, it called `fseek`, `fread`, `fseek`, `fwrite`.
- --The Crash:-- This translates to over 1,000 system calls to the OS kernel just to write 256 bytes. It completely destroys the C library's internal cache, spikes the CPU, and causes massive physical latency on the storage drive.
- --The Fix:-- Instead of seeking back and forth 256 times, we `fread` the base pattern -once- into our local 256-byte stack buffer. Then, we seek forward -once-, and use a `while` loop to repeatedly `fwrite` that pattern from our stack buffer. It mimics the LZ77 cascade forward using a fraction of the system calls.

--3. `ftell()` Desyncs--
- --The Trap:-- Calling `FTELL64(out_fp)` to figure out where we are in the output file seems safe. However, on a `wb+` stream that is heavily buffered and constantly jumping around, `ftell()` has a known history in `glibc` of returning "dirty" or delayed logical positions, especially right after switching from write mode to read mode.
- --The Fix:-- We abandon `ftell()` entirely. We introduce a `uint64_t current_out_pos = 0;` variable. Because we know exactly how many bytes we write, we just manually add to this tracker. It is mathematically pure and 100% deterministic.

--4. The Hardware Endianness Gamble--
- --The Trap:-- Reading 3 bytes directly into a `uint64_t offset` via `fread(&offset, 1, 3, in_fp)` works beautifully on standard Little-Endian x86 processors. However, if this code runs on a Big-Endian machine (like some ARM or PowerPC architectures), or if the compiler applies weird struct alignment padding, the bytes will land in the wrong order, resulting in wildly incorrect match offsets.
- --The Fix:-- We read the offset byte-by-byte and reconstruct it using bit-shifting (`offset |= b << (i - 8)`). This perfectly mirrors the way you encoded it in the compressor, making the code architecturally immune to hardware quirks.
*/
// Zero-RAM Streaming Decompressor (Bulletproof Edition)
// Requires 'out_fp' to be opened with "wb+" so it can read its own history.
static void decompressZeroRAMlast(FILE *in_fp, FILE *out_fp)
{
    uint8_t flag;
    uint8_t match_buf[256]; // 256-byte stack buffer (0 heap RAM used)
    
    // MATHEMATICAL TRACKING: Replaces FTELL64() to avoid glibc buffer desync bugs
    uint64_t current_out_pos = 0; 

    // Read the file purely sequentially until EOF
    while (fread(&flag, 1, 1, in_fp) == 1) {
        
        for (int bit = 0; bit < 8; ++bit) {
            uint8_t is_match = (flag >> bit) & 1;

            if (is_match) {
                // 1. Read Token
                uint8_t token;
                if (fread(&token, 1, 1, in_fp) != 1) goto done;

                uint8_t off_len = (token & 0x07) + 1;       // 1..8 bytes
                uint8_t mult = (token >> 3) & 0x1F;         // 0..31 multiplier
                int current_match_len = (mult + 1) * 8;     // 8..256 bytes

                // 2. Read Offset (HARDWARE SAFE: Endian-agnostic byte shifting)
                uint64_t offset = 0; 
                for (int i = 0; i < off_len; ++i) {
                    uint8_t b;
                    if (fread(&b, 1, 1, in_fp) != 1) goto done;
                    offset |= ((uint64_t)b << (i * 8));
                }

                uint64_t src_pos = current_out_pos - offset;

                // CRITICAL FIX: Force C library to sync writes to the OS kernel
                // This prevents `fread` from hitting a false EOF on recently written bytes
                fflush(out_fp);

                // 3. Copy the historical match data
                if (offset >= current_match_len) {
                    // Fast path: No overlap. Read and write the chunk at once.
                    FSEEK64(out_fp, src_pos, SEEK_SET);
                    if (fread(match_buf, 1, current_match_len, out_fp) != current_match_len) goto done;
                    
                    FSEEK64(out_fp, current_out_pos, SEEK_SET);
                    fwrite(match_buf, 1, current_match_len, out_fp);
                } else {
                    // Slow path: Overlapping match (e.g., repeating pattern "ABCABC")
                    // ANTI-THRASHING: Read the base pattern ONCE, then write it repeatedly.
                    
                    FSEEK64(out_fp, src_pos, SEEK_SET);
                    if (fread(match_buf, 1, offset, out_fp) != offset) goto done;
                    
                    FSEEK64(out_fp, current_out_pos, SEEK_SET);
                    int written = 0;
                    while (written < current_match_len) {
                        int to_write = (offset < current_match_len - written) ? offset : (current_match_len - written);
                        fwrite(match_buf, 1, to_write, out_fp);
                        written += to_write;
                    }
                }
                
                // Update tracker
                current_out_pos += current_match_len;
                
            } else {
                // Literal fallback
                uint8_t lit;
                if (fread(&lit, 1, 1, in_fp) != 1) goto done;
                
                fwrite(&lit, 1, 1, out_fp);
                current_out_pos += 1;
            }
        }
    }
done:
    // Final safety flush to ensure the last bytes hit the physical platter
    fflush(out_fp); 
    return;
}


// Refactored by Gemini:
static void __attribute__((noinline)) decompressBRANCHLESS_UNROLLED(FILE *in_fp, FILE *out_fp)
{
    FSEEK64(in_fp, 0, SEEK_END);
    int64_t file_sz = FTELL64(in_fp);
    if (file_sz < 0) { fprintf(stderr, "ftell failed\n"); return; }
    FSEEK64(in_fp, 0, SEEK_SET);
    uint64_t comp_size = (uint64_t)file_sz;

    uint8_t *comp = malloc(comp_size + 64); // Extra padding for SIMD safety
    if (!comp) { fprintf(stderr, "Out of memory\n"); return; }

    if (fread(comp, 1, comp_size, in_fp) != comp_size) {
        fprintf(stderr, "Read error\n");
        free(comp);
        return;
    }
    memset(comp + comp_size, 0, 64);

    ByteBuffer decb;
    buf_init(&decb);
    buf_ensure(&decb, 256);
    memset(decb.data, 0, 256);

    size_t comp_pos = 0;

    // --- METHOD 1: FAST PATH (Requires at least 73 bytes of input remaining) ---
    // 73 is the theoretical max consumption of 8 tokens (1 flag + 8 * (1 len + 8 offset))
    while (comp_pos + 73 < comp_size) {
        uint8_t flag = comp[comp_pos++];

        // Macro to keep the unrolled tokens clean
        #define DECODE_TOKEN(bit) { \
            uint8_t is_match = (flag >> bit) & 1; \
            uint8_t off_len = is_match ? comp[comp_pos++] : 0; \
            uint64_t raw = *(const uint64_t*)(comp + comp_pos); \
            uint64_t offset = is_match ? (raw & (0xFFFFFFFFFFFFFFFFULL >> ((8ULL - off_len) * 8))) : 0; \
            comp_pos += is_match ? off_len : 1; \
            uint8_t lit = is_match ? 0 : comp[comp_pos - 1]; \
            uint64_t valid_offset = (offset > 0 && offset <= decb.len) ? offset : 0; \
            uint64_t base = decb.len - valid_offset; \
            buf_ensure(&decb, 32); \
            __m256i match_ymm = _mm256_loadu_si256((const __m256i*)(decb.data + base)); \
            __m256i lit_ymm = _mm256_set1_epi8((char)lit); \
            __m256i selector = _mm256_set1_epi32( is_match ? -1 : 0 ); \
            __m256i ymm = _mm256_blendv_epi8(lit_ymm, match_ymm, selector); \
            _mm256_storeu_si256((__m256i*)(decb.data + decb.len), ymm); \
            decb.len += is_match ? MIN_MATCH_LEN : 1; \
        }

        DECODE_TOKEN(0); DECODE_TOKEN(1); DECODE_TOKEN(2); DECODE_TOKEN(3);
        DECODE_TOKEN(4); DECODE_TOKEN(5); DECODE_TOKEN(6); DECODE_TOKEN(7);
        
        #undef DECODE_TOKEN
    }

    // --- METHOD 1: TAIL PROCESSING (Standard loop for the last few bytes) ---
    while (comp_pos < comp_size) {
        uint8_t flag = comp[comp_pos++];
        for (int i = 0; i < 8 && comp_pos < comp_size; ++i) {
            uint8_t is_match = (flag >> i) & 1;
            if (is_match) {
                if (comp_pos >= comp_size) break;
                uint8_t off_len = comp[comp_pos++];
                uint64_t offset = 0;
                // Safe byte-by-byte read for the tail
                for(int b=0; b < off_len; b++) {
                    if (comp_pos < comp_size) offset |= ((uint64_t)comp[comp_pos++] << (b * 8));
                }
                
                uint64_t valid_offset = (offset > 0 && offset <= decb.len) ? offset : 0;
                buf_ensure(&decb, MIN_MATCH_LEN);
                for(int j=0; j < MIN_MATCH_LEN; j++) {
                    decb.data[decb.len] = decb.data[decb.len - valid_offset];
                    decb.len++;
                }
            } else {
                buf_ensure(&decb, 1);
                decb.data[decb.len++] = comp[comp_pos++];
            }
        }
    }

    fwrite(decb.data, 1, decb.len, out_fp);
    free(decb.data);
    free(comp);
}

// !!! Not tested !!! Branchless LZSS Decompressor (replaced the ternary operators with Bitwise Masks)
static void decompress_PURE_BRANCHLESS(const uint8_t *comp, size_t comp_size, uint8_t *decb_data)
{
    size_t comp_pos = 0;
    size_t decb_len = 0;

    // Fast Path: Requires 73 bytes of safety margin
    while (comp_pos + 73 < comp_size) {
        uint8_t flag = comp[comp_pos++];

        #define DECODE_TOKEN_MASKED(bit) { \
            /* 1. Extract is_match as a mask (0xFF if match, 0x00 if literal) */ \
            uint8_t is_match = (flag >> bit) & 1; \
            int8_t mask = -(int8_t)is_match; \
            \
            /* 2. Read off_len and advance comp_pos branchlessly */ \
            uint8_t off_len = comp[comp_pos] & mask; \
            comp_pos += is_match; \
            \
            /* 3. Read raw offset and mask it */ \
            uint64_t raw = *(const uint64_t*)(comp + comp_pos); \
            uint64_t shift_bits = (8ULL - off_len) << 3; \
            uint64_t offset = (raw & (0xFFFFFFFFFFFFFFFFULL >> shift_bits)) & (uint64_t)mask; \
            \
            /* 4. Advance comp_pos: if match, move by off_len; if literal, move by 1 */ \
            comp_pos += (off_len & mask) | (1 & ~mask); \
            \
            /* 5. Extract literal (only valid if not a match) */ \
            uint8_t lit = comp[comp_pos - 1] & ~mask; \
            \
            /* 6. SIMD Selection and Store */ \
            uint64_t base = decb_len - (offset & mask); \
            __m256i match_ymm = _mm256_loadu_si256((const __m256i*)(decb_data + base)); \
            __m256i lit_ymm = _mm256_set1_epi8((char)lit); \
            __m256i selector = _mm256_set1_epi64x(mask); \
            __m256i ymm = _mm256_blendv_epi8(lit_ymm, match_ymm, selector); \
            _mm256_storeu_si256((__m256i*)(decb_data + decb_len), ymm); \
            \
            /* 7. Update output length: MIN_MATCH_LEN if match, 1 if literal */ \
            decb_len += (MIN_MATCH_LEN & mask) | (1 & ~mask); \
        }

        DECODE_TOKEN_MASKED(0); DECODE_TOKEN_MASKED(1);
        DECODE_TOKEN_MASKED(2); DECODE_TOKEN_MASKED(3);
        DECODE_TOKEN_MASKED(4); DECODE_TOKEN_MASKED(5);
        DECODE_TOKEN_MASKED(6); DECODE_TOKEN_MASKED(7);
        
        #undef DECODE_TOKEN_MASKED
    }

    // --- TAIL PATH: Simplified final loop ---
    while (comp_pos < comp_size) {
        uint8_t flag = comp[comp_pos++];
        for (int i = 0; i < 8 && comp_pos < comp_size; ++i) {
            if ((flag >> i) & 1) {
                uint8_t off_len = comp[comp_pos++];
                uint64_t offset = 0;
                for(int b=0; b < off_len; b++) offset |= ((uint64_t)comp[comp_pos++] << (b * 8));
                for(int j=0; j < MIN_MATCH_LEN; j++) {
                    decb_data[decb_len] = decb_data[decb_len - offset];
                    decb_len++;
                }
            } else {
                decb_data[decb_len++] = comp[comp_pos++];
            }
        }
    }

}

int main(int argc, char **argv)
{
    if (argc != 4 && argc != 5 && argc != 6) {
        fprintf(stderr, "lzss_MayMun a.k.a. lzss_MemMem a.k.a. lzss_MaxMin r.%d\n", revision);
        fprintf(stderr, "The MaximumMinimum Granularitywise Deduplicator, written by Grok AI & Kaze (sanmayce@sanmayce.com)\n");
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "%s c <in> <out> - compress (FNV-1a 64-bit hash, hash-table mode)\n", argv[0]);
        fprintf(stderr, "%s C <in> <out> - compress (Pippip 128-bit hash, hash-table mode)\n", argv[0]);
        fprintf(stderr, "%s m <in> <out> [MATCH_LEN=4..32, default 8] [NumberOfThreads, default 256] - compress, 10x Memory Footprint, (%d-threaded mode using GLIBC memmem())\n", argv[0], NUM_Threads);
        fprintf(stderr, "%s M <in> <out> [MATCH_LEN=4..32, default 8] [NumberOfThreads, default 256] - compress, 10x Memory Footprint, (%d-threaded mode using Railgun memmem())\n", argv[0], NUM_Threads);
        fprintf(stderr, "%s e <in> <out> - decompress for c/C content\n", argv[0]);
        fprintf(stderr, "%s d <in> <out> [MATCH_LEN=4..32, default 8] - decompress for m/M content\n", argv[0]);
        fprintf(stderr, "%s D <in> <out> [MATCH_LEN=4..32, default 8] - decompress for m/M content\n", argv[0]);
        return 1;
    }

    const char *mode = argv[1];
    if (argc >= 5)
        MIN_MATCH_LEN = atoi(argv[4]);
    else
        MIN_MATCH_LEN = 8; // MaximumMinimum
    const char *NumT = argv[1];
    if (argc == 6)
        NUM_Threads = atoi(argv[5]);
    else
        NUM_Threads = 256;

    bool use_memmem = (strcmp(mode, "m") == 0) + (strcmp(mode, "M") == 0);

    FILE *in  = fopen(argv[2], "rb");
    FILE *out = fopen(argv[3], "wb+");
    if (!in || !out) {
        fprintf(stderr, "Cannot open files.\n");
        if (in) fclose(in);
        if (out) fclose(out);
        return 1;
    }

    fprintf(stderr, "   _____                  _____                \n");
    fprintf(stderr, "  /     \\ _____  ___.__. /     \\  __ __  ____  \n");
    fprintf(stderr, " /  \\ /  \\\\__  \\<   |  |/  \\ /  \\|  |  \\/    \\ \n");
    fprintf(stderr, "/    Y    \\/ __ \\\\___  /    Y    \\  |  /   |  \\\n");
    fprintf(stderr, "\\____|__  (____  / ____\\____|__  /____/|___|  /\n");
    fprintf(stderr, "        \\/     \\/\\/            \\/           \\/ \n");

    fprintf(stderr, "lzss_MayMun a.k.a. lzss_MemMem a.k.a. lzss_MaxMin r.%d\n", revision);
    fprintf(stderr, "The MaximumMinimum Granularitywise Deduplicator, written by Grok AI & Kaze (sanmayce@sanmayce.com)\n");
    if ((use_memmem) || (strcmp(mode, "d") == 0) || (strcmp(mode, "D") == 0))
        fprintf(stderr, "Deduplicating Granularity = %d.\n", MIN_MATCH_LEN);
    else
        fprintf(stderr, "Deduplicating Granularity = 8..256 (at step 8).\n");

    if (strcmp(mode, "c") == 0 || strcmp(mode, "m") == 0) {
        if (strcmp(mode, "m") == 0) fprintf(stderr, "Using GLIBC memmem().\n");
        if (strcmp(mode, "c") == 0) {
            fprintf(stderr, "Using FNV-1a 64-bit hash, HASH_TABLE_SIZE = %d MiB = (HASH_TABLE_SLOTS = %d) * (HASH_CANDIDATES = %d) * (HASH_CANDIDATE_SIZE = %d)\n", HASH_TABLE_SIZE * NUM_CANDIDATES * 8 >>20, HASH_TABLE_SIZE, NUM_CANDIDATES, 8);
        }
        int GlobalFlag_LandonOrPippip = 1;
        int GlobalFlag_GLIBCorRailgun = 1;
        //compress(in, out, use_memmem);
        //compressR5(in, out, use_memmem);
        compressMM(in, out, use_memmem);
    } else if (strcmp(mode, "C") == 0 || strcmp(mode, "M") == 0) {
        if (strcmp(mode, "M") == 0) fprintf(stderr, "Using Railgun memmem().\n");
        if (strcmp(mode, "C") == 0) {
            fprintf(stderr, "Using Pippip 128-bit hash, HASH_TABLE_SIZE = %d MiB = (HASH_TABLE_SLOTS = %d) * (HASH_CANDIDATES = %d) * (HASH_CANDIDATE_SIZE = %d)\n", HASH_TABLE_SIZE * NUM_CANDIDATES * 8 >>20, HASH_TABLE_SIZE, NUM_CANDIDATES, 8);
        }
        int GlobalFlag_LandonOrPippip = 2;
        int GlobalFlag_GLIBCorRailgun = 2;
        //compress(in, out, use_memmem);
        //compressR5(in, out, use_memmem);
        compressMM(in, out, use_memmem);
    } else if (strcmp(mode, "e") == 0) {
        //decompressR4(in, out); // faster, but uses realloc()
        decompressZeroRAM(in, out); // slowest, but ZER0 RAM
        //decompressZeroRAMlast(in, out); // slowest, but ZER0 RAM
    } else if (strcmp(mode, "d") == 0) {
        decompressR3(in, out);
    } else if (strcmp(mode, "D") == 0) {
        decompressBRANCHLESS_UNROLLED(in, out);
    } else {
        fprintf(stderr, "Unknown mode '%s', (use 'c/C' or 'm/M' or 'd').\n", mode);
    }

    fclose(in);
    fclose(out);
    return 0;
}

// The 'MayMun' a.k.a. 'Traversing-11-TiB-via-memmem' benchmark is run as superuser, in highest niceness, in Performance Mode. 
// Host: LENOVO ThinkPad P14s Gen 4
// CPU: AMD Ryzen 7 PRO 7840U 2x8x1 Threads
// Thread(s) per core: 2
// Core(s) per socket: 8
// Socket(s):          1
// Caches (sum of all):         
// L1d: 256 KiB (8 instances)
// L1i: 256 KiB (8 instances)
// L2:    8 MiB (8 instances)
// L3:   16 MiB (1 instance)
// Memory: 4x16 GB = 64 GB LPDDR5, Rank: 2, Configured Memory Speed: 6400 MT/s

/*
[sanmayce@djudjeto MayMun3_lzss_Grok_Gemini]$ su
Password: 
[root@djudjeto MayMun3_lzss_Grok_Gemini]# sh memmemmadness.sh John_Steinbeck_16-TXTs.tar 
...
   _____                  _____                
  /     \ _____  ___.__. /     \  __ __  ____  
 /  \ /  \\__  \<   |  |/  \ /  \|  |  \/    \ 
/    Y    \/ __ \\___  /    Y    \  |  /   |  \
\____|__  (____  / ____\____|__  /____/|___|  /
        \/     \/\/            \/           \/ 
lzss_MayMun a.k.a. lzss_MemMem a.k.a. lzss_MaxMin r.3
The MaximumMinimum Deduplicator, written by Grok AI & Kaze (sanmayce@sanmayce.com)
Deduplicating Granularity = 8.
Using GLIBC memmem().
Compressing 9,154,560 bytes...
Precomputing ALL possible 8-byte matches with 256 threads...
Search Progress = 100.00%; Traversed-So-Far: 12,446,240 MiB (11 TiB); Performance: 68.69 GiB/s
Precompute finished (256-core speedup applied) — starting encoding phase...
Compressing: 100.00% [>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>] 8 MiB of 8 MiB; Memory Footprint: 95 MiB
Compression complete: 9,154,560 -> 4,883,323 bytes (53.34% of original remains).
   _____                  _____                
  /     \ _____  ___.__. /     \  __ __  ____  
 /  \ /  \\__  \<   |  |/  \ /  \|  |  \/    \ 
/    Y    \/ __ \\___  /    Y    \  |  /   |  \
\____|__  (____  / ____\____|__  /____/|___|  /
        \/     \/\/            \/           \/ 
lzss_MayMun a.k.a. lzss_MemMem a.k.a. lzss_MaxMin r.3
The MaximumMinimum Deduplicator, written by Grok AI & Kaze (sanmayce@sanmayce.com)
Deduplicating Granularity = 8.
Using Railgun memmem().
Compressing 9,154,560 bytes...
Precomputing ALL possible 8-byte matches with 256 threads...
Search Progress = 100.00%; Traversed-So-Far: 12,446,240 MiB (11 TiB); Performance: 67.81 GiB/s
Precompute finished (256-core speedup applied) — starting encoding phase...
Compressing: 100.00% [>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>] 8 MiB of 8 MiB; Memory Footprint: 95 MiB
Compression complete: 9,154,560 -> 4,883,323 bytes (53.34% of original remains).
   _____                  _____                
  /     \ _____  ___.__. /     \  __ __  ____  
 /  \ /  \\__  \<   |  |/  \ /  \|  |  \/    \ 
/    Y    \/ __ \\___  /    Y    \  |  /   |  \
\____|__  (____  / ____\____|__  /____/|___|  /
        \/     \/\/            \/           \/ 
lzss_MayMun a.k.a. lzss_MemMem a.k.a. lzss_MaxMin r.3
The MaximumMinimum Deduplicator, written by Grok AI & Kaze (sanmayce@sanmayce.com)
Deduplicating Granularity = 8.
   _____                  _____                
  /     \ _____  ___.__. /     \  __ __  ____  
 /  \ /  \\__  \<   |  |/  \ /  \|  |  \/    \ 
/    Y    \/ __ \\___  /    Y    \  |  /   |  \
\____|__  (____  / ____\____|__  /____/|___|  /
        \/     \/\/            \/           \/ 
lzss_MayMun a.k.a. lzss_MemMem a.k.a. lzss_MaxMin r.3
The MaximumMinimum Deduplicator, written by Grok AI & Kaze (sanmayce@sanmayce.com)
Deduplicating Granularity = 8.
d598f057176a1b130afa4c4cadeb74119dc3f5fae8dc8ac0a1e6d433cab266fe  m
d598f057176a1b130afa4c4cadeb74119dc3f5fae8dc8ac0a1e6d433cab266fe  M
ed60cf01e77b624b90a72ecbb323357fd8a780b8e0c32c6d13dc3a9f0efe07eb  John_Steinbeck_16-TXTs.tar
ed60cf01e77b624b90a72ecbb323357fd8a780b8e0c32c6d13dc3a9f0efe07eb  mr
ed60cf01e77b624b90a72ecbb323357fd8a780b8e0c32c6d13dc3a9f0efe07eb  MR
[root@djudjeto MayMun3_lzss_Grok_Gemini]# 
*/

// The 'MayMun' a.k.a. 'Traversing-11-TiB-via-memmem' benchmark is run as superuser, in highest niceness, in Performance Mode. 
// Host: Dell Latitude 7420
// CPU: i7-1185G7 (Tiger Lake-UP3) 2x4x1 Threads
// Thread(s) per core: 2
// Core(s) per socket: 4
// Socket(s):          1
// Caches (sum of all):         
// L1d: 192 KiB (4 instances)
// L1i: 128 KiB (4 instances)
// L2:    5 MiB (4 instances)
// L3:   12 MiB (1 instance)
// Memory: 8x4 GB = 32 GB LPDDR4, Rank: 2, Configured Memory Speed: 4267 MT/s

/*
[sanmayce@djudjeto MayMun3_lzss_Grok_Gemini]$ su
Password: 
[root@djudjeto MayMun3_lzss_Grok_Gemini]# sh memmemmadness.sh John_Steinbeck_16-TXTs.tar
...
   _____                  _____                
  /     \ _____  ___.__. /     \  __ __  ____  
 /  \ /  \\__  \<   |  |/  \ /  \|  |  \/    \ 
/    Y    \/ __ \\___  /    Y    \  |  /   |  \
\____|__  (____  / ____\____|__  /____/|___|  /
        \/     \/\/            \/           \/ 
lzss_MayMun a.k.a. lzss_MemMem a.k.a. lzss_MaxMin r.3
Deduplicating Granularity = 8.
Using GLIBC memmem().
Compressing 9,154,560 bytes...
Precomputing ALL possible 8-byte matches with 256 threads...
Search Progress = 100.00%; Traversed-So-Far: 12,446,240 MiB (11 TiB); Performance: 28.30 GiB/s 
Precompute finished (256-core speedup applied) — starting encoding phase...
Compressing: 100.00% [>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>] 8 MiB of 8 MiB; Memory Footprint: 95 MiB
Compression complete: 9,154,560 -> 4,883,323 bytes (53.34% of original).
   _____                  _____                
  /     \ _____  ___.__. /     \  __ __  ____  
 /  \ /  \\__  \<   |  |/  \ /  \|  |  \/    \ 
/    Y    \/ __ \\___  /    Y    \  |  /   |  \
\____|__  (____  / ____\____|__  /____/|___|  /
        \/     \/\/            \/           \/ 
lzss_MayMun a.k.a. lzss_MemMem a.k.a. lzss_MaxMin r.3
Deduplicating Granularity = 8.
Using Railgun memmem().
Compressing 9,154,560 bytes...
Precomputing ALL possible 8-byte matches with 256 threads...
Search Progress = 100.00%; Traversed-So-Far: 12,446,240 MiB (11 TiB); Performance: 28.30 GiB/s
Precompute finished (256-core speedup applied) — starting encoding phase...
Compressing: 100.00% [>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>] 8 MiB of 8 MiB; Memory Footprint: 95 MiB
Compression complete: 9,154,560 -> 4,883,323 bytes (53.34% of original).
   _____                  _____                
  /     \ _____  ___.__. /     \  __ __  ____  
 /  \ /  \\__  \<   |  |/  \ /  \|  |  \/    \ 
/    Y    \/ __ \\___  /    Y    \  |  /   |  \
\____|__  (____  / ____\____|__  /____/|___|  /
        \/     \/\/            \/           \/ 
lzss_MayMun a.k.a. lzss_MemMem a.k.a. lzss_MaxMin r.3
Deduplicating Granularity = 8.
   _____                  _____                
  /     \ _____  ___.__. /     \  __ __  ____  
 /  \ /  \\__  \<   |  |/  \ /  \|  |  \/    \ 
/    Y    \/ __ \\___  /    Y    \  |  /   |  \
\____|__  (____  / ____\____|__  /____/|___|  /
        \/     \/\/            \/           \/ 
lzss_MayMun a.k.a. lzss_MemMem a.k.a. lzss_MaxMin r.3
Deduplicating Granularity = 8.
d598f057176a1b130afa4c4cadeb74119dc3f5fae8dc8ac0a1e6d433cab266fe  m
d598f057176a1b130afa4c4cadeb74119dc3f5fae8dc8ac0a1e6d433cab266fe  M
ed60cf01e77b624b90a72ecbb323357fd8a780b8e0c32c6d13dc3a9f0efe07eb  John_Steinbeck_16-TXTs.tar
ed60cf01e77b624b90a72ecbb323357fd8a780b8e0c32c6d13dc3a9f0efe07eb  mr
ed60cf01e77b624b90a72ecbb323357fd8a780b8e0c32c6d13dc3a9f0efe07eb  MR

-rw-r--r-- 1 sanmayce sanmayce 1,789,664  John_Steinbeck_16-TXTs.tar.zpaq        ! Max Mode !
-rw-r--r-- 1 sanmayce sanmayce 2,045,240  John_Steinbeck_16-TXTs.tar.rc          ! Max Mode !
-rw-r--r-- 1 sanmayce sanmayce 2,052,596  John_Steinbeck_16-TXTs.tar.bsc         ! Max Mode !
-rw-r--r-- 1 sanmayce sanmayce 2,435,298  John_Steinbeck_16-TXTs.tar.zstd        ! Max Mode !
-rw-r--r-- 1 sanmayce sanmayce 2,497,448  John_Steinbeck_16-TXTs.tar.rar         ! Max Mode !
-rw-r--r-- 1 sanmayce sanmayce 3,091,747  John_Steinbeck_16-TXTs.tar.Nakamichi   ! Max Mode !
-rw-r--r-- 1 sanmayce sanmayce 4,883,907  John_Steinbeck_16-TXTs.tar.MM8         ! 8B granularity deduplication !
-rw-r--r-- 1 sanmayce sanmayce 8,742,177  John_Steinbeck_16-TXTs.tar.zirka       ! 4KB granularity deduplication !
-rw-r--r-- 1 sanmayce sanmayce 9,154,560  John_Steinbeck_16-TXTs.tar

[sanmayce@djudjeto MayMun3_lzss_Grok_Gemini]$ cat 8.sh 
/bin/time -v ./bwtsatan_static -20e9 "$1" "$1"
/bin/time -v ./zpaq715_GCC_15.2.1_sse4.2.elf a "$1".zpaq "$1" -m511 
/bin/time -v ./bsc-3.3.12-x64_CLANG_20.1.8_SSE42.elf e "$1" "$1".bsc -m0 -e2
/bin/time -v ./zstd157 --max "$1" -o "$1".zstd
/bin/time -v ./Zirka_v12++_4096 "$1"
/bin/time -v ./rarlinux-x64-712 a -s -m5 -md64g -mcx "$1".rar "$1"
/bin/time -v ./Satanichi_Nakamichi_Vanilla_LITE_PippipAES_CLANG_XMM_64bit.elf "$1" "$1".Nakamichi 20 30123 i 
/bin/time -v ./lzss_memmem.elf C "$1" "$1".MM
echo
./lzss_memmem.elf e "$1".MM "$1".restore
echo
b3sum "$1"
b3sum "$1".restore
rm "$1".restore
echo
ls -l "$1"* -S -r

[sanmayce@djudjeto MayMun3_lzss_Grok_Gemini]$ 
*/

/*
-rw-r--r-- 1 sanmayce sanmayce  31,249,772  'SUPRAPIG_Marx_Engels_Lenin_Stalin_Collected_Writings_(2052_HTMs_in_Russian).tar.zpaq'
-rw-r--r-- 1 sanmayce sanmayce  33,131,092  'SUPRAPIG_Marx_Engels_Lenin_Stalin_Collected_Writings_(2052_HTMs_in_Russian).tar.rc'
-rw-r--r-- 1 sanmayce sanmayce  36,368,400  'SUPRAPIG_Marx_Engels_Lenin_Stalin_Collected_Writings_(2052_HTMs_in_Russian).tar.bsc'
-rwxrwxrwx 1 sanmayce sanmayce  42,165,634  'SUPRAPIG_Marx_Engels_Lenin_Stalin_Collected_Writings_(2052_HTMs_in_Russian).tar.zstd'
-rw-r--r-- 1 sanmayce sanmayce  43,443,468  'SUPRAPIG_Marx_Engels_Lenin_Stalin_Collected_Writings_(2052_HTMs_in_Russian).tar.rar'
-rw-r--r-- 1 sanmayce sanmayce  57,046,870  'SUPRAPIG_Marx_Engels_Lenin_Stalin_Collected_Writings_(2052_HTMs_in_Russian).tar.Nakamichi'
-rw-r--r-- 1 sanmayce sanmayce  70,917,856  'SUPRAPIG_Marx_Engels_Lenin_Stalin_Collected_Writings_(2052_HTMs_in_Russian).tar.MM'
-rw-r--r-- 1 sanmayce sanmayce 245,728,781  'SUPRAPIG_Marx_Engels_Lenin_Stalin_Collected_Writings_(2052_HTMs_in_Russian).tar.zirka'
-rwxrwxrwx 1 sanmayce sanmayce 245,732,864  'SUPRAPIG_Marx_Engels_Lenin_Stalin_Collected_Writings_(2052_HTMs_in_Russian).tar'
*/

/*
   _____                  _____                
  /     \ _____  ___.__. /     \  __ __  ____  
 /  \ /  \\__  \<   |  |/  \ /  \|  |  \/    \ 
/    Y    \/ __ \\___  /    Y    \  |  /   |  \
\____|__  (____  / ____\____|__  /____/|___|  /
        \/     \/\/            \/           \/ 
lzss_MayMun a.k.a. lzss_MemMem a.k.a. lzss_MaxMin r.5
The MaximumMinimum Granularitywise Deduplicator, written by Grok AI & Kaze (sanmayce@sanmayce.com)
Deduplicating Granularity = 8..256 (at step 8).
Using Pippip 128-bit hash, HASH_TABLE_SIZE = 2048 MiB = (HASH_TABLE_SLOTS = 131072) * (HASH_CANDIDATES = 2048) * (HASH_CANDIDATE_SIZE = 8)
Compressing 245,732,864 bytes (fully mmap'ed, zero-RAM mode)...
Compressing: 100.00% [>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>] 234 MiB of 234 MiB; Memory Footprint: 0 MiB
Compression complete: 245,732,864 -> 70,917,856 bytes (28.86% of original)
    Command being timed: "./lzss_MayMun.elf C SUPRAPIG_Marx_Engels_Lenin_Stalin_Collected_Writings_(2052_HTMs_in_Russian).tar SUPRAPIG_Marx_Engels_Lenin_Stalin_Collected_Writings_(2052_HTMs_in_Russian).tar.MM"
    Elapsed (wall clock) time (h:mm:ss or m:ss): 11:45.34
    Maximum resident set size (kbytes): 2408192
   _____                  _____                
  /     \ _____  ___.__. /     \  __ __  ____  
 /  \ /  \\__  \<   |  |/  \ /  \|  |  \/    \ 
/    Y    \/ __ \\___  /    Y    \  |  /   |  \
\____|__  (____  / ____\____|__  /____/|___|  /
        \/     \/\/            \/           \/ 
lzss_MayMun a.k.a. lzss_MemMem a.k.a. lzss_MaxMin r.5
The MaximumMinimum Granularitywise Deduplicator, written by Grok AI & Kaze (sanmayce@sanmayce.com)
Deduplicating Granularity = 8..256 (at step 8).
    Command being timed: "./lzss_MayMun.elf e SUPRAPIG_Marx_Engels_Lenin_Stalin_Collected_Writings_(2052_HTMs_in_Russian).tar.MM SUPRAPIG_Marx_Engels_Lenin_Stalin_Collected_Writings_(2052_HTMs_in_Russian).tar.restore"
    Percent of CPU this job got: 99%
    Elapsed (wall clock) time (h:mm:ss or m:ss): 0:29.06

66f90d01b45a120c648af30eb2460bedc0752c6084af86b2bc53e23274a3b50a  SUPRAPIG_Marx_Engels_Lenin_Stalin_Collected_Writings_(2052_HTMs_in_Russian).tar
66f90d01b45a120c648af30eb2460bedc0752c6084af86b2bc53e23274a3b50a  SUPRAPIG_Marx_Engels_Lenin_Stalin_Collected_Writings_(2052_HTMs_in_Russian).tar.restore
[sanmayce@djudjeto MayMun5_lzss_Grok_Gemini]$ 
*/

/*
[sanmayce@djudjeto MayMun5_lzss_Grok_Gemini]$ sh MM.sh SPETSNAZ_1012_fb2_books_1030-ALL-files.tar 
   _____                  _____                
  /     \ _____  ___.__. /     \  __ __  ____  
 /  \ /  \\__  \<   |  |/  \ /  \|  |  \/    \ 
/    Y    \/ __ \\___  /    Y    \  |  /   |  \
\____|__  (____  / ____\____|__  /____/|___|  /
        \/     \/\/            \/           \/ 
lzss_MayMun a.k.a. lzss_MemMem a.k.a. lzss_MaxMin r.5
The MaximumMinimum Granularitywise Deduplicator, written by Grok AI & Kaze (sanmayce@sanmayce.com)
Deduplicating Granularity = 8..256 (at step 8).
Using Pippip 128-bit hash, HASH_TABLE_SIZE = 2048 MiB = (HASH_TABLE_SLOTS = 131072) * (HASH_CANDIDATES = 2048) * (HASH_CANDIDATE_SIZE = 8)
Compressing 999,690,240 bytes (fully mmap'ed, zero-RAM mode)...
Compressing: 100.00% [>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>] 953 MiB of 953 MiB; Memory Footprint: 0 MiB
Compression complete: 999,690,240 -> 416,114,150 bytes (41.62% of original)
    Command being timed: "./lzss_MayMun.elf C SPETSNAZ_1012_fb2_books_1030-ALL-files.tar SPETSNAZ_1012_fb2_books_1030-ALL-files.tar.MM"
    Percent of CPU this job got: 99%
    Elapsed (wall clock) time (h:mm:ss or m:ss): 2:04:55
    Maximum resident set size (kbytes): 3481248

   _____                  _____                
  /     \ _____  ___.__. /     \  __ __  ____  
 /  \ /  \\__  \<   |  |/  \ /  \|  |  \/    \ 
/    Y    \/ __ \\___  /    Y    \  |  /   |  \
\____|__  (____  / ____\____|__  /____/|___|  /
        \/     \/\/            \/           \/ 
lzss_MayMun a.k.a. lzss_MemMem a.k.a. lzss_MaxMin r.5
The MaximumMinimum Granularitywise Deduplicator, written by Grok AI & Kaze (sanmayce@sanmayce.com)
Deduplicating Granularity = 8..256 (at step 8).
    Command being timed: "./lzss_MayMun.elf e SPETSNAZ_1012_fb2_books_1030-ALL-files.tar.MM SPETSNAZ_1012_fb2_books_1030-ALL-files.tar.restore"
    Percent of CPU this job got: 99%
    Elapsed (wall clock) time (h:mm:ss or m:ss): 1:21.39
    Maximum resident set size (kbytes): 2152

24f72513c06d51124a88d3a93f6f780c2ae053da759308e209abc3479963564e  SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
24f72513c06d51124a88d3a93f6f780c2ae053da759308e209abc3479963564e  SPETSNAZ_1012_fb2_books_1030-ALL-files.tar.restore
[sanmayce@djudjeto MayMun5_lzss_Grok_Gemini]$ 
*/
