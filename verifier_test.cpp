/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "verifier.h"
#include "mincrypt/sha.h"
#include "mincrypt/sha256.h"

// This is build/target/product/security/testkey.x509.pem after being
// dumped out by dumpkey.jar.
RSAPublicKey test_key =
    { 64, 0xc926ad21,
      { 1795090719, 2141396315, 950055447, -1713398866,
        -26044131, 1920809988, 546586521, -795969498,
        1776797858, -554906482, 1805317999, 1429410244,
        129622599, 1422441418, 1783893377, 1222374759,
        -1731647369, 323993566, 28517732, 609753416,
        1826472888, 215237850, -33324596, -245884705,
        -1066504894, 774857746, 154822455, -1797768399,
        -1536767878, -1275951968, -1500189652, 87251430,
        -1760039318, 120774784, 571297800, -599067824,
        -1815042109, -483341846, -893134306, -1900097649,
        -1027721089, 950095497, 555058928, 414729973,
        1136544882, -1250377212, 465547824, -236820568,
        -1563171242, 1689838846, -404210357, 1048029507,
        895090649, 247140249, 178744550, -747082073,
        -1129788053, 109881576, -350362881, 1044303212,
        -522594267, -1309816990, -557446364, -695002876},
      { -857949815, -510492167, -1494742324, -1208744608,
        251333580, 2131931323, 512774938, 325948880,
        -1637480859, 2102694287, -474399070, 792812816,
        1026422502, 2053275343, -1494078096, -1181380486,
        165549746, -21447327, -229719404, 1902789247,
        772932719, -353118870, -642223187, 216871947,
        -1130566647, 1942378755, -298201445, 1055777370,
        964047799, 629391717, -2062222979, -384408304,
        191868569, -1536083459, -612150544, -1297252564,
        -1592438046, -724266841, -518093464, -370899750,
        -739277751, -1536141862, 1323144535, 61311905,
        1997411085, 376844204, 213777604, -217643712,
        9135381, 1625809335, -1490225159, -1342673351,
        1117190829, -57654514, 1825108855, -1281819325,
        1111251351, -1726129724, 1684324211, -1773988491,
        367251975, 810756730, -1941182952, 1175080310 },
      3
    };

RSAPublicKey test_f4_key = 
                  {64,0xe737c77d,
		    {2559301163,1518742568,3812513943,394403660,
		    2374126730,175358713,1975504815,1427317958,
		    3701951342,490899990,1053504506,158625481,
		   1851257768,983065579,700946754,1860489145,
		   1529135569,1819662671,3980640488,4038687997,
		   4000816758,8718261,2821998704,3836176928,
		   3185283458,2847327946,1243721766,3186097669,
		   628603970,2506022706,327284520,3666367020,
		   3712483667,463893933,3660026836,552115219,
		   2502641322,1855765611,2245166584,423382689,
		   2040986882,1595291279,1075564644,877603054,
		   3083669960,580909833,3312973948,1599623288,
		   641531148,4156777516,2271947172,1103468982,
		   2221511127,1361540815,1333920627,2225472493,
		   4282151161,1609373640,3244294444,2442861889,
		   3472521177,2912243112,1036886560,3490538121},
		   {473691662,2849159452,1303045090,2860905833,
	           155756427,1110696044,1391496807,1531529517,
		   1001520175,2743323490,3311877470,2566591015,
		   1320568598,2173779977,1151466377,133514291,
		   3884125056,408921904,718463470,3929935182,
		   39808105,162870170,350972000,727496258,
		   400060934,2126621849,1480264130,507658526,
		   2445475513,1465625571,277975251,1440355066,
		   2433834894,313696107,988877927,337428770,
		   407325661,1447664231,2203242203,889298626,
		   2589560015,4030648733,1073797176,4275634063,
		   1729975591,2848402127,953584499,435695639,
		   1171776694,2950240799,1315651036,1282208777,
		   53305817,2015748905,792262003,322960736,
		   3929576753,2428630471,3765808261,2521420712,
		   946647443,4118137023,2942675196,636660251},
		   65537
};

             

void ui_print(const char* fmt, ...) {
    char buf[256];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, 256, fmt, ap);
    va_end(ap);

    fputs(buf, stderr);
}

void ui_set_progress(float fraction) {
}

int main(int argc, char **argv) {
    if (argc != 2  || argc > 3) {
	    fprintf(stderr, "Usage: %s [-sha256] [-f4 | -file <keys>] <package>\n", argv[0]);
        return 2;
    }

    Certificate default_cert;
    Certificate* cert = &default_cert;
    cert->public_key = &test_key;
    cert->hash_len = SHA_DIGEST_SIZE;
    int num_keys = 1;
    ++argv;
    if (strcmp(argv[0], "-sha256") == 0) {
        ++argv;
        cert->hash_len = SHA256_DIGEST_SIZE;
    }
    if (strcmp(argv[0], "-f4") == 0) {
        ++argv;
        cert->public_key = &test_f4_key;
    } else if (strcmp(argv[0], "-file") == 0) {
        ++argv;
        cert = load_keys(argv[0], &num_keys);
        ++argv;
    }
    int result = verify_file(*argv, cert, num_keys);
    if (result == VERIFY_SUCCESS) {
        printf("VERIFIED\n");
        return 0;
    } else if (result == VERIFY_FAILURE) {
        printf("NOT VERIFIED\n");
        return 1;
    } else {
        printf("bad return value\n");
        return 3;
    }
}
