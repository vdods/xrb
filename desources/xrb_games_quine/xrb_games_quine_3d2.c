#include <stdio.h>  /*                                                                                                                          */

 void put(short character,  short num){for(;num>0   ;--num)putchar(character);}void str               (char*ptr){for(;*ptr!=0;++ptr)if(
 *ptr=='@')put(' ',1);else  if(*ptr<0101||*ptr>90   )put(*ptr,1);}void lit(char*ptr){int q=0          ;int nc=0;int sc=0;put(34,1);for(;*ptr
 !=0;){for(;*ptr>=65&&*ptr  <=74;++ptr){q=1;put(*   ptr,1);nc=10*nc+(*ptr-65);}for(;*ptr>=75&&*       ptr<=84;++ptr){q=1;put(*ptr,1);sc=10*sc+
      (*ptr-75);}if(q            !=0){put(34             ,1);for(;nc>0          ;--nc)put(012,1)           ;for(;sc>0;--        sc)put(' ',01);
        put(34,1);q=0;          nc=0;sc=0                 ;;}else if(*             ptr==012){put(           0134,1);put             ('n',01);ptr
         ++;;}else{put(*      ptr,01);++                  ptr;}}put(34              ,(1));}extern           char*srcstr              ;int main (
           int argc,char*    *arg){str                    (srcstr);put              (' ',(1));lit           ((srcstr));              put(';',1);
            (void)put(012, 1);return                      45954;}char*             srcstr="#inLL"           "clude@<LN"             "stdio.hBLO"
              ">\n\n@void@put(sMO"                        "hort@chaLM"            "racter,@@sLM"            "hort@nuLL"           "m){for(;BLP"
               "num>0@@@;--nuMQ"                          "m)putcharT"         "(character);LN"             "}void@str@@@@@@@@@@@@@@@(charBLR"
                 "*ptr){for(MR"                           ";*ptr!=0;++ptr)if(\n@*ptr=='@')LP"               "put('@',1);else@@if(*ptr<010BLS"
                  "1||*ptr>90MQ"                          "@@@)put(*ptr,1);}void@lit(chLS"                  "ar*ptr){int@q=0@@@@@@@@@@BLQ"
                ";int@nc=0;int@MO"                        "sc=0;put(34,1);for(;*ptrMM"                      "\n@!=0;LL"           "){for(;BLO"
              "*ptr>=65&&*ptr@@<MN"                       "=74;++ptrO"    "){q=1;put(MK"                    "*@@@ptrLO"              ",1);ncBLN"
             "=10*nM"  "c+(*ptr-65ML"                     ");}for(;*P"     "ptr>=75&&*@LS"                  "@@@@@@pLP"               "tr<=84BLL"
           ";++ptP"     "r){q=1;putMK"                    "(*ptr,1);S"        "sc=10*sc+LR"                 "\n@@@@@LP"               "@(*ptr-7BT"
         "5);}ifS"        "(q@@@@@@@@LS"                  "@@@@!=0){T"         "put(34@@@@LP"               "@@@@@@@LP"               "@@,1);foBS"
        "r(;nLL"           "c>0@@@@@@@LR"                 "@@@;--ncLK"          ")put(012,1LO"              ")@@@@@@LO"              "@@@@@;forBQ"
      "(;sc>LO"              "0;--@@@@@@LP"               "@@sc)putLL"           "('@',01);\nLL"           "@@@@@@@@LN"             "put(34,1)BN"
   ";q=0;@@@@LN"             "@@@@@@nc=0;scLK"          "=0@@@@@@@@@LM"            "@@@@@@@@;;}eS"        "lse@if(*@@@@Q"      "@@@@@@@@@ptr=BK"
"=012){put(@@@@@@@P"     "@@@@0134,1);put@@@@@@@M"  "@@@@@@('n',01);ptr\nT"         "@@@@@@@@@++;;}else{put(*@@@@@@ptr,01);++@@@@@@@@@@@@@@BK"
"@@@@ptr;}}put(34@P"     "@@@@@@@@@@@@@,(1));}exM"  "tern@@@@@@@@@@@charLK"          "*srcstr@@@@@@@@@@@@@@;int@main@(\n@@@@@@@@@@@int@arBK"
"gc,char*@@@@*arg)P"     "{str@@@@@@@@@@@@@@@@@@M"  "@@(srcstr);put@@@@@LM"            "@@@@@@@@@('@',(1));lit@@@@@@@@@@@((srcstr))CS"

        ";@@@@@L" "@LR"                 "@@@LL"           "@@@@@putLK"          "(';',1);\nL" "@@@@@@@@@@@@(void)pLL"           "ut(012L" ",BO"
                                                                                                                             "rcstr=        BM"
  "     S"        "  LO"              "      LN"             "       S"        "        R"       "     R"       "    P"     "   S"        "  BL"
 "    LK"          " LO"              "       LM"            "        Q"      "        T"         "  LL"           "  N"   "  LK"          " BL"
 "   LM"            " LM"            " M"  "   LL"           " L" "   Q"      " L" "   T"         "   O"    " P"     " M"  "      S"        " BK"
"   MT"                             " N"   "    LK"          " L" "    O"    " M"  "   T"         "   M"  "  LM"            "           BK"
"   MS"                            " P"     "   LK"          " M"  "    M"  "  M"  "   T"         "          LN"             "               BK"
"    T"         "         Q"      "               T"         " N"   "   M"  " N"   "   T"         "          LP"               "               BK"
"   LK"          "       Q"      "                 S"        " N"   "        O"    "   T"         "   M"  "  MM"                      "        BL"
 "   LL"           "   R"       "  T"         "     R"       " O"    "      P"     "   T"         "   O"    " P"     " L" " LN"             "  BL"
 "    LK"          "   Q"      " LL"           "     Q"      " P"     "     P"     "   T"         "  LL"           "  O"    " LL"           "  BM"
  "      Q"      "     P"     "  LM"            "     P"     " P"     "    P"     "     R"       "     R"       "    P"     "    T"         "  BO"
    "                  M"  "         P"     "                     N"   "  N"   "           L" "                     R"       "              BS"
        "      O"    " M"  "         P"     "                     N"   "  N"   "                                   S"        " L" "        ";
