/*
 XXXXXXXXXXXXXXXXXXXXXXXXX  XXXXXXXXXXXXXXXXXXXXX   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX               XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
 XXXXXXXXXXXXXXXXXXXXXXXXX  XXXXXXXXXXXXXXXXXXXXX   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX          XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
 XXXXXXXXXXXXXXXXXXXXXXXXX  XXXXXXXXXXXXXXXXXXXXX   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX       XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
      XXXXXXXXXXXXXXX            XXXXXXXXXXX             XXXXXXXXXXXXXX         XXXXXXXXXXXXXXXX           XXXXXXXXXXXXX        XXXXXXXXXXXXXXX
        XXXXXXXXXXXXXX          XXXXXXXXX                 XXXXXXXXXXXX             XXXXXXXXXXXXXX           XXXXXXXXXXX             XXXXXXXXXXXX
         XXXXXXXXXXXXXXX      XXXXXXXXXX                  XXXXXXXXXXXX              XXXXXXXXXXXXX           XXXXXXXXXXX              XXXXXXXXXXX
           XXXXXXXXXXXXXX    XXXXXXXXX                    XXXXXXXXXXXX              XXXXXXXXXXXXX           XXXXXXXXXXX              XXXXXXXXXXX
            XXXXXXXXXXXXXX XXXXXXXXX                      XXXXXXXXXXXX             XXXXXXXXXXXXXX           XXXXXXXXXXX             XXXXXXXXXXXX
              XXXXXXXXXXXXXXXXXXXX                        XXXXXXXXXXXX            XXXXXXXXXXXXXX            XXXXXXXXXXX           XXXXXXXXXXXXX
               XXXXXXXXXXXXXXXXX                          XXXXXXXXXXXX         XXXXXXXXXXXXXXXX             XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
                 XXXXXXXXXXXXXX                           XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX               XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
                  XXXXXXXXXXXXXX                          XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX                  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
                XXXXXXXXXXXXXXXXXX                        XXXXXXXXXXXXXXXXXXXXXXXXXXXX                      XXXXXXXXXXX           XXXXXXXXXXXX
              XXXXXXXXXXXXXXXXXXXXX                       XXXXXXXXXXXX    XXXXXXXXXXXXXX                    XXXXXXXXXXX              XXXXXXXXXXX
             XXXXXXXX  XXXXXXXXXXXXXX                     XXXXXXXXXXXX     XXXXXXXXXXXXXXX                  XXXXXXXXXXX               XXXXXXXXXXX
           XXXXXXXX     XXXXXXXXXXXXXX                    XXXXXXXXXXXX       XXXXXXXXXXXXXX                 XXXXXXXXXXX               XXXXXXXXXXXX
         XXXXXXXXX        XXXXXXXXXXXXXX                  XXXXXXXXXXXX        XXXXXXXXXXXXXXX               XXXXXXXXXXX               XXXXXXXXXXXX
        XXXXXXXX           XXXXXXXXXXXXXX                 XXXXXXXXXXXX          XXXXXXXXXXXXXX              XXXXXXXXXXX              XXXXXXXXXXXXX
      XXXXXXXXX              XXXXXXXXXXXXXX               XXXXXXXXXXXX           XXXXXXXXXXXXXXX           XXXXXXXXXXXX             XXXXXXXXXXXXX
   XXXXXXXXXXXXX             XXXXXXXXXXXXXXXXX          XXXXXXXXXXXXXXX            XXXXXXXXXXXXXXX        XXXXXXXXXXXXXXX       XXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXX     XXXXXXXXXXXXXXXXXXXXXXXXX  XXXXXXXXXXXXXXXXXXXXXXX         XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXX     XXXXXXXXXXXXXXXXXXXXXXXXX  XXXXXXXXXXXXXXXXXXXXXXX          XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXX     XXXXXXXXXXXXXXXXXXXXXXXXX  XXXXXXXXXXXXXXXXXXXXXXX            XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
*/
#include <stdio.h>

 pc(c){putchar(c);}spaces(  n){if(n<0)pc(10),n=~n   ;for(;n--;)pc(042);}string(char*s){               for(;*s;++s)*s-0100?*s<0101|*s>90
 ?pc(*s):0:pc(32);}literal  (char*s){int n;pc(34)   ;for(;*s;++s){if(*s==10){pc(92),pc(110);          }else if(*s>64&&*s<91){pc(*s);n=*s-78;
 if(*s==65){++s;pc(*s);n-=  *s-0101;}else if(*s==   90){++s;pc(*s);n+=*s-65;}pc(042);spaces(n);       pc(34);}else pc(*s);}pc(34);}main(){char
      *s[2]={"#incZA"            "lude@<sZB"             "tdio.h>\n\nW"         "@pc(c){putchaY"           "r(c);}spacV"        "es(@@n){if(nE"
        "<0)pc(10),nX"          "=~n@@ZF"                 "@;for(;nZB"             "--;)pc(042)Y"           ";}strinZB"             "g(char*s)D"
         "{@@@@@@@@@@@T"      "@@@@foZG"                  "r(;*s;++ZC"              "s)*s-0100?Y"           "*s<0101ZC"              "|*s>90\nY"
           "@?pc(*s):0:R"    "pc(32ZI"                    ");}literZC"              "al@@(char*Y"           "s){int@nC"              ";pc(34)ZA"
            "@@@;for(;*sO" ";++s)ZK"                      "{if(*s==ZB"             "10){pc(92),Y"           "pc(110);B"             "@@@@@@@@ZC"
              "@@}else@if(*s>64ZM"                        "&&*s<91)ZA"            "{pc(*s);n=ZA"            "*s-78;\nY"           "@if(*s==6ZD"
               "5){++s;pc(*s)ZO"                          ";n-=@@*s-W"         "0101;}else@iZB"             "f(*s==@@@90){++s;pc(*s);n+=*s-AF"
                 "65;}pc(042ZP"                           ");spaces(n);@@@@@@@pc(34);}elseZD"               "@pc(*s);}pc(34);}main(){charAG"
                  "\n@@@@@@*s[2]={",""};string(*s);literal(*s);pc(44);literal(s[1]);string(s[1]);}
/*
        XXXXXXXXX  XXXX                 XXXXXXX           XXXXXXXXXXXX          XXXXXXXXXXXXX XXXXXXXXXXXXXXXXXXXXXXX           XXXXXXXXXX  XXX
    XXXXXXXXXXXXXXXXXXX                XXXXXXXX           XXXXXXXXXXXXX         XXXXXXXXXXXXX XXXXXXXXXXXXXXXXXXXXXXXX       XXXXXXXXXXXXXXXXXX
  XXXXXXXX        XXXXXX              XXXXXXXXXX             XXXXXXXXXX        XXXXXXXXXXX       XXXXXXXX        XXXXXX     XXXXXX        XXXXXX
 XXXXXXXX           XXXX              XXXXXXXXXXX            XXXXXXXXXXX      XXXXXXXXXXX         XXXXXX           XXXXX   XXXXXX           XXXX
 XXXXXXX             XXXX            XXXX  XXXXXXX           XXXX XXXXXX      XXXX XXXXXX         XXXXXX     XXX     XXX   XXXXXXXXX          XXX
XXXXXXX                             XXXX   XXXXXXXX          XXXX XXXXXXX    XXXX  XXXXXX         XXXXXX   XXXXX            XXXXXXXXXXXXXXX
XXXXXXX                            XXXX     XXXXXXX          XXXX  XXXXXXX  XXXXX  XXXXXX         XXXXXXXXXXXXXX             XXXXXXXXXXXXXXXXXXX
XXXXXXX         XXXXXXXXXXXX      XXXXXXXXXXXXXXXXXX         XXXX   XXXXXX  XXXX   XXXXXX         XXXXXXXXXXXXXX               XXXXXXXXXXXXXXXXXXX
XXXXXXX          XXXXXXXXXX       XXXXXXXXXXXXXXXXXXX        XXXX   XXXXXXXXXXX    XXXXXX         XXXXXX   XXXXX                      XXXXXXXXXXXX
 XXXXXXX           XXXXXX        XXXX         XXXXXXXX       XXXX    XXXXXXXXX     XXXXXX         XXXXXX     XXX     XXXX  XXXX             XXXXXX
 XXXXXXXX          XXXXXX       XXXX           XXXXXXXX      XXXX     XXXXXXXX     XXXXXX         XXXXXX           XXXXX    XXXXX           XXXXXX
  XXXXXXXXX      XXXXXXXX     XXXXXX            XXXXXXXX     XXXX     XXXXXXX     XXXXXXXX       XXXXXXXX       XXXXXXX     XXXXXXX        XXXXXX
    XXXXXXXXXXXXXXXXXXXXX  XXXXXXXXXXXX     XXXXXXXXXXXXXXXXXXXXXXXX   XXXXX   XXXXXXXXXXXXXX XXXXXXXXXXXXXXXXXXXXXXXX       XXXXXXXXXXXXXXXXXX
        XXXXXXXXX    XXXX  XXXXXXXXXXXX     XXXXXXXXXXXXXXXXXXXXXXXX   XXXXX   XXXXXXXXXXXXXX XXXXXXXXXXXXXXXXXXXXXXX        XXX   XXXXXXXXX
*/

