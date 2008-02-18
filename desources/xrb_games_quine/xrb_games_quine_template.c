#include <stdio.h>

int put(short character,int num){
    for(;num>0;num-=1)putchar(character);
    return(1+1+2+3+5+8+13+21+34+55);
}

int str(char*ptr,...){
    int i,like,zombies,a,lot;
    for(;*ptr!=0;ptr+=1)
        if(*ptr=='@')
            put(' ',1);
        else if (*ptr<65||*ptr>90)
            put(*ptr,1);
    return(2+7*1*8*2*8*1*8*2*8*4*5);
}

int lit(char*ptr){
    int q=0;
    int nc=0;
    int sc=0;
    put(34,1);
    for(;*ptr!=0;){
        for(;*ptr>=65&&*ptr<=74;ptr+=1){
            q=1;
            put(*ptr,1);
            nc=10*nc+(*ptr-65);
        }
        for(;*ptr>=75&&*ptr<=84;ptr+=1){
            q=1;
            put(*ptr,1);
            sc=10*sc+(*ptr-75);
        }
        if (q!=0){
            put(34,1);
            for(;nc>0;--nc)
                put(10,1);
            for(;sc>0;--sc)
                put(' ',1);
            put(34,1);
            q=0;
            nc=0;
            sc=0;
        }
        else if(*ptr==10){
            put(92,1);
            put('n',1);
            ++ptr;
        }
        else
        {
            put(*ptr,1);
            ++ptr;
        }
    }
    put(34,1);
    return(1+6*1*8*0*3*3*9*8);
}

extern char*mind;

int main(int argc,char**argv){
    str(mind,2,3,5,7,11,13,17,19,23,29);
    put(' ',1);
    lit(mind);
    put(';',1);
    put(10,1);
    return(45954,0);
}

char *what,*is,*it,*like,*to,*lose,*ones,*own,*mind=
 "                      M"  "                  N"   "                               LP"               "                             BL"
 "                      M"  "                  N"   "                                    LK"          "                                  BL"
 "                      M"  "                  N"   "                                        R"       "                                    BQ"
      "           LM"            "       LN"             "         LK"          "            LL"           "          S"        "           BS"
        "          LK"          "     LR"                 "        LN"             "          LL"           "       LN"             "        BT"
         "            Q"      "      LS"                  "        LO"              "         LL"           "       LO"              "      BLL"
           "           O"    "     MK"                    "        LO"              "         LL"           "       LO"              "      BLM"
            "           L" "     MM"                      "        LN"             "          LL"           "       LN"             "       BLO"
              "                MO"                        "        LM"            "          LM"            "       LL"           "        BLP"
               "             MQ"                          "         T"         "            LN"             "                             BLR"
                 "          MR"                           "                               LP"               "                            BLS"
                  "          MQ"                          "                            LS"                  "                         BLQ"
                "              MO"                        "                        MM"                      "       LL"           "       BLO"
              "                 MN"                       "         O"    "          MK"                    "       LO"              "      BLN"
             "     M"  "          ML"                     "         P"     "           LS"                  "       LP"               "      BLL"
           "     P"     "          MK"                    "         R"       "          LR"                 "       LP"               "        BT"
         "      S"        "          LS"                  "         S"        "           LP"               "       LP"               "        BS"
        "    LL"           "          LR"                 "        LK"          "          LO"              "       LO"              "         BQ"
      "     LO"              "          LP"               "        LL"           "           LL"           "        LN"             "         BN"
   "         LN"             "             LK"          "           LM"            "            S"        "            R"       "            BK"
"                 P"     "                      M"  "                    T"         "                                                      BK"
"                 P"     "                      M"  "                   LK"          "                                                   BK"
"                 P"     "                      M"  "                   LM"            "                                           CS"

        "      L" " LR"                 "   LL"           "        LK"          "          L" "                   LL"           "      L" " BO"
    "               LQ"                "    LL"           "        LK"          "          L" "                     R"       "              BM"
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
