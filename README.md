This is **MayMun** - the Linux texttoy actually capable to deduplicate e.g. a 300GB file on a 4GB laptop.    
In essence it is a microdeduplicator using Matches/Blocks/Granularities from 8..256 at step 8, encoding them in 1+(1..8) bytes, that is, using 2^(1x8)=256B, 2^(2x8)=64KB,... 2^(8x8)=16EiB windows for backwards lookups, so it is kinda non-entropy LZSS compressor.

![MM_v5_2](https://github.com/user-attachments/assets/f99b6892-43e7-4b92-aa93-d41647ee2957)

So, let us find out how well it deduplicates the 'SPETSNAZ' collection of FB2 ebooks (a XML-like format):

![SPETSNAZ](https://github.com/user-attachments/assets/2af1937a-63f2-4844-8588-4e74bfb5c7d0)


```
lzbench 2.2.1 | GCC 14.2.1 | 64-bit Linux | 11th Gen Intel(R) Core(TM) i7-11850H @ 2.50GHz
The results sorted by column number 4:
Compressor name         Compress. Decompress. Compr. size  Ratio Filename
zstd 1.5.7 -22           1.74 MB/s  1184 MB/s   286562294  28.67 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
tornado 0.6a -16         1.85 MB/s   264 MB/s   297291925  29.74 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
bzip2 1.0.8 -9           18.1 MB/s  46.4 MB/s   300980161  30.11 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
lizard 2.1 -49           2.66 MB/s  1559 MB/s   342899962  34.30 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
brieflz 1.3.0 -9         1.14 MB/s   465 MB/s   351266391  35.14 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
kanzi 2.5.1 -3            126 MB/s   683 MB/s   357108426  35.72 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
Nakamichi                                       362786273
libdeflate 1.24 -12      5.67 MB/s  1318 MB/s   363985811  36.41 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
lizard 2.1 -39           9.19 MB/s  2631 MB/s   379386133  37.95 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
zlib 1.3.1 -9            9.21 MB/s   423 MB/s   380566254  38.07 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
crush 1.0 -2             0.48 MB/s   467 MB/s   391958809  39.21 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
lizard 2.1 -29           2.76 MB/s  2321 MB/s   399253949  39.94 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
zstd 1.5.7 -1             448 MB/s  1628 MB/s   409763027  40.99 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
kanzi 2.5.1 -2            255 MB/s   755 MB/s   414796012  41.49 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
MayMun                                          416114150                 
lzsse2 2019-04-18 -16    9.78 MB/s  4858 MB/s   428456404  42.86 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
lzsse4 2019-04-18 -16    11.7 MB/s  6000 MB/s   438180977  43.83 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
lz4hc 1.10.0 -12         14.9 MB/s  4719 MB/s   447490013  44.76 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
lizard 2.1 -19           9.60 MB/s  4870 MB/s   448680873  44.88 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
lzav 5.7 -2               101 MB/s  3238 MB/s   450554932  45.07 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
zxc 0.9.1 -5             60.2 MB/s  5973 MB/s   493595555  49.37 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
kanzi 2.5.1 -1            292 MB/s  1293 MB/s   518653158  51.88 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
lz4hc 1.10.0 -1           366 MB/s  4051 MB/s   528452722  52.86 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
yappy 2014-03-22 -12     96.4 MB/s  3075 MB/s   533734140  53.39 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
quicklz 1.5.1 beta 7 -3  62.9 MB/s   784 MB/s   538478562  53.86 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
zxc 0.9.1 -4              155 MB/s  6183 MB/s   561080067  56.13 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
zxc 0.9.1 -3              218 MB/s  6189 MB/s   592203021  59.24 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
lz4 1.10.0                613 MB/s  4018 MB/s   602114935  60.23 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
snappy 1.2.2              721 MB/s  1546 MB/s   615884187  61.61 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
zxc 0.9.1 -2              362 MB/s  7674 MB/s   684695080  68.49 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
zxc 0.9.1 -1              580 MB/s  8819 MB/s   792354406  79.26 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
memcpy                  19019 MB/s 20105 MB/s   999690240 100.00 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar

lzbench 2.2.1 | GCC 14.2.1 | 64-bit Linux | 11th Gen Intel(R) Core(TM) i7-11850H @ 2.50GHz
The results sorted by column number 4:
Compressor name I_Threads Compress. Decompress. Compr. size  Ratio Filename
bsc 3.3.11 -m0 -e2      8  28.2 MB/s  74.3 MB/s   236644360  23.67 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
kanzi 2.5.1 -7          8  41.7 MB/s  82.4 MB/s   266921283  26.70 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
kanzi 2.5.1 -6          8  67.8 MB/s   165 MB/s   275973853  27.61 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
lzma 25.01 -9           8  6.17 MB/s   271 MB/s   281081875  28.12 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
kanzi 2.5.1 -9          8  8.57 MB/s  8.57 MB/s   285979170  28.61 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
zstd 1.5.7 -22          8  3.13 MB/s  1174 MB/s   286354200  28.64 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
kanzi 2.5.1 -5          8  93.8 MB/s   308 MB/s   291147932  29.12 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
kanzi 2.5.1 -8          8  15.1 MB/s  14.5 MB/s   300090133  30.02 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
kanzi 2.5.1 -4          8   334 MB/s  1123 MB/s   343610982  34.37 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
kanzi 2.5.1 -3          8   567 MB/s  2668 MB/s   357108426  35.72 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
Nakamichi                                         362786273
zstd 1.5.7 -1           8  1861 MB/s  1618 MB/s   410437527  41.06 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
kanzi 2.5.1 -2          8  1074 MB/s  3176 MB/s   414796012  41.49 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
MayMun                                            416114150                 
kanzi 2.5.1 -1          8  1103 MB/s  4543 MB/s   518653158  51.88 FULG8_benchmarking_33-testfiles/SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
```

And the superb TurboBench roster:
```
TurboBench:  - Fri May  1 23:34:34 2026
      C Size  ratio%     C MB/s     D MB/s   SCORE      Name            File
   253123174    25.3       0.59     358.32    1948.07   brotli 11d30    SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
   255304506    25.5       1.39    1415.34     976.54   lzturbo 39      SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
   263975518    26.4       1.83    1253.47     812.13   zstd 22d30      SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
   286319092    28.6       1.72    1170.55     868.04   zstd 22         SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
   342899962    34.3       2.65    1378.12     721.93   lizard 49       SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
   354152059    35.4       1.58    2827.28     987.77   lzturbo 19      SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
   362786273                                            Nakamichi
   363985817    36.4       5.65    1270.97     542.59   libdeflate 12   SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
   368164406    36.8       1.32    1997.98    1124.59   lzturbo 29      SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
   379386133    38.0       9.07    2256.85     490.55   lizard 39       SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
   399253949    39.9       2.74    1981.56     765.06   lizard 29       SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
   416114150                                            MayMun
   428456404    42.9       9.35    4822.29     535.78   lzsse2 16       SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
   438180977    43.8      11.30    5875.55     526.99   lzsse4 16       SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
   447492238    44.8      14.63    4957.21     516.22   lz4 16          SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
   448680873    44.9       9.29    3908.25     556.84   lizard 19       SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
   451164914    45.1       7.90    5666.15     577.98   lz4ultra 12     SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
   496090296    49.6      83.14    5694.26     508.47   zxc 5           SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
   602118581    60.2     621.29    4291.46     604.19   lz4 0           SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
   615884187    61.6     496.20    1500.86     619.23   snappy          SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
   999690240   100.0   19731.77   20078.13     999.84   memcpy          SPETSNAZ_1012_fb2_books_1030-ALL-files.tar
```
    
Enfun!    
2026-May-04, Kaze
