#include <stdio.h>

 int put(char c,int n){for  (;n>0;n-=1)putchar(c)   ;return(1+1+2+3+5+8+13+21+34+55+89)               ;}int str(char*ptr,...){for(;*ptr
 !=0;ptr+=1)if(*ptr==0x40)  put(040,1);else if (*   ptr<65||*ptr>90)put(*ptr,1);return(2+7*1          *8*2*8*1*8*2*8*4*5);}int lit(char*ptr)
 {int q=0;int nc=0;int sc=  0;put(34,1);for(;*ptr   !=0;){for(;*ptr>=65&&*ptr<=74;ptr+=1){ q=1;       put(*ptr,1); nc=10*nc+(*ptr-65); }for(;*
      ptr>=75&&*ptr<=            84;ptr+=1){             q=1;put(*ptr,          1);sc=0x0a*sc+(*           ptr-75);}if(q        !=0){put(34,1);
        for(;nc>0;--nc          )put(10,1                 );for(;sc>0;             --sc)put(' ',1           );put(34,1)             ;q=0;nc=0;sc
         =0x0;}else if(*      ptr==0xa){                  put(92,0x1);              put('n',1);++           ptr;} else{              put(*ptr,1)
           ;++ptr;}}put(4    +5+6+19,1                    );return(1+6              *1*8*0*3*3*9*           8);} extern              char*fraid;
            ;;int main(int argc,char                      **argv){str(             fraid,2,3,5,7,           11,13,17,19             ,23,29);put(
              ' ',0x01);lit(fraid)                        ;put(';',1);            put(1+2+3+4,1)            ;return(4,3           ,2,1,0);}char
               **dave,*my,*mind,                          **is,*going;         char*i,*can,feel             ,it;char**im,a='.'+'.'+'.',*fraid=
                 "#include@<MR"                           "stdio.h>\n\n@int@put(char@c,intLP"               "@n){for@@(;n>0;n-=1)putchar(BLS"
                  "c)@@@;retuMQ"                          "rn(1+1+2+3+5+8+13+21+34+55+8LS"                  "9)@@@@@@@@@@@@@@@;}int@stBLQ"
                "r(char*ptr,...MO"                        "){for(;*ptr\n@!=0;ptr+=1MM"                      ")if(*ptLL"           "r==0x40BLO"
              ")@@put(040,1);elsMN"                       "e@if@(*@@O"    "@ptr<65||*MK"                    "ptr>90)LO"              "put(*pBLN"
             "tr,1)M"  ";return(2+ML"                     "7*1@@@@@@P"     "@@@@*8*2*8*LS"                  "1*8*2*8LP"               "*4*5);BLL"
           "}int@P"     "lit(char*pMK"                    "tr)\n@{inR"       "t@q=0;int@LR"                 "nc=0;inLP"               "t@sc=@@0BT"
         ";put(3S"        "4,1);for(;LS"                  "*ptr@@@!=S"        "0;){for(;*pLP"               "tr>=65&LP"               "&*ptr<=7BS"
        "4;ptLL"           "r+=1){@q=1LR"                 ";@@@@@@@LK"          "put(*ptr,1LO"              ");@nc=1LO"              "0*nc+(*ptBQ"
      "r-65)LN"             ";@}for(;*\nLP"               "@@@@@@ptLL"           "r>=75&&*ptrLL"           "<=@@@@@@LN"             "@@@@@@84;BN"
   "ptr+=1){@LN"             "@@@@@@@@@@@@qLK"          "=1;put(*ptrLM"            ",@@@@@@@@@@1S"        ");sc=0x0a*scR"       "+(*@@@@@@@@@BK"
"@@ptr-75);}if(q@KP"     "@@@@@@@!=0){put(34,1)KM"  ";\n@@@@@@@@for(;nc>KT"         "0;--nc@@@@@@@@@@)put(10,1@@@@@@@@@@@@@@@@@);for(;sc>0;BK"
"@@@@@@@@@@@@@--sKP"     "c)put('@',1@@@@@@@@@@KM"  "@);put(34,1)@@@@@@KLK"          "@@@@@@@;q=0;nc=0;sc\n@@@@@@@@@=0x0;}else@if(*@@@@@@BK"
"ptr==0xa){@@@@@@KP"     "@@@@@@@@@@@@put(92,0xKM"  "1);@@@@@@@@@@@@@@pKLM"            "ut('n',1);++@@@@@@@@@@@ptr;}@else{@@@@@@@@@CS"

        "@@@@@pL" "uLR"                 "t(KLL"           "*ptr,1)KLK"          "\n@@@@@@@KL" "@@@@;++ptr;}}put(4KLL"           "@@@@+KL" "5BO"
    "+6+19,1@@@@@@@KLQ"                "@@@KLL"           "@@@@@@@KLK"          "@@@);retuKL" "rn(1+6@@@@@@@@@@@@@@KR"       "*1*8*0*3*3*9*@BM"
  "@@@@KS"        "@KLO"              "@@@@@KLN"             "8);}@eKS"        "xtern@@KR"       "@@@@KR"       "@@@KP"     "@@KS"        "@@BL"
 "@chKLK"          "aLO"              "r*fraiKLM"            "d;\n@@@KQ"      "@@@@@@@KT"         "@KLL"           "@KN"   ";KLK"          ";BL"
 "inKLM"            "tLM"            "@M"  "maKLL"           "iL" "n(KQ"      "iL" "ntKT"         "@aKO"    "rP"     "gM"  "c,chaKS"        "rBK"
"@@KMT"                             "@N"   "@@@KLK"          "@L" "@@@KO"    "@M"  "@@KT"         "@@KM"  "@KLM"            "@@@@@@**argBK"
"v)KMS"                            "{P"     "stKLK"          "rM"  "(@@KM"  "@KM"  "@@KT"         "@@@@@@@@fKLN"             "raid,2,3,5,7,@@BK"
"@@@KT"         "@@@@@@11KQ"      ",13,17,19@@@@@KT"         "@N"   "@@KM"  "@N"   "@@KT"         "@@,23,29)KLP"               ";put(\n@@@@@@@@BK"
"@@KLK"          "@@@@'@KQ"      "',0x01);lit(fraiKS"        "dN"   ")@@@@@@KO"    "@@KT"         "@@KM"  "@KMM"                      "@@@@@@@@BL"
 "@@KLL"           "@@KR"       "@KT"         ";putKR"       "(O"    "';',1KP"     ");KT"         "@@KO"    "@P"     "@L" "@LN"             "@@BL"
 "@@@KLK"          "@@KQ"      "pLL"           "ut(1KQ"      "+P"     "2+3+KP"     "4,KT"         "1KLL"           ")KO"    "@LL"           "@@BM"
  "@@@@@KQ"      "@@@@KP"     ";KLM"            "retuKP"     "rP"     "n(4KP"     ",3@@KR"       "@@@@KR"       "@@@KP"     "@@,KT"         "2,BO"
    "1,0);}char\n@@@@@KM"  "@@@@@@@@KP"     "@@**dave,*my,*mind,@KN"   "@KN"   "@@@@@@@@@@@@@@@@@@@@@@@@**is,*goingKR"       ";@@@@@@@@@charBS"
        "*i,*cKO"    "aM"  "n,feel@@KP"     "@@@@@@@@@@@,it;char*KN"   "*KN"   "im,a='.'+'.'+'.',*fraid=\n@@@@@@@@KS"        "@L" "@@@@@@@";
