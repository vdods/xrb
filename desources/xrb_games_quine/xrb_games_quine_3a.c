#include <stdio.h>

 put(int c){putchar((c));}  space(int sc){if(sc<0   )put(10),sc=~sc;for(;sc>0;sc--)put(               32);}str(char*s){for(;*s-0;++s)if
 (*s==64)put(32);else if(*  s<65||*s>90)put(*s);}   lit(char*s){int n;put(32);for(;*s!=0;++s          ){if(*s==10){put(92);put(110);}else if
 (*s>64&&*s<91){put(*s);n=  *s-78;if(*s==0101){++   s;put(*s);n-=*s-65;}else if(*s==90){++s;put       (*s);n+=*s-65;}put(34);space(n);put(042)
      ;}else put(*s);            }put(042);}             main(int argc          ,char**arg){char           *s[2]={"#iW"         "nclude@<stdiE"
        "o.h>\n\n@puX"          "t(intZF"                 "@c){putcZB"             "har((c));}@Y"           "@space(ZB"             "int@sc){iD"
         "f(sc<0@@@)puT"      "t(10)ZH"                   ",sc=~sc;ZC"              "for(;sc>0;Y"           "sc--)puZC"              "t(@@@@@@B"
           "@@@@@@@@@32R"    ");}stZI"                    "r(char*sZC"              "){for(;*s-Y"           "0;++s)iZC"              "f\n@(*sAA"
            "==64)put(32O" ");elsZK"                      "e@if(*@@ZB"             "s<65||*s>90Y"           ")put(*sZB"             ");}@@@liAC"
              "t(char*s){int@n;ZM"                        "put(32);ZA"            "for(;*s!=0ZA"            ";++s@@@@Y"           "@@@@@@){iAD"
               "f(*s==10){putZO"                          "(92);put(W"         "110);}else@iZB"             "f\n@(*s>64&&*s<91){put(*s);n=@AF"
                 "@*s-78;if(ZP"                           "*s==0101){++@@@s;put(*s);n-=*s-ZD"               "65;}else@if(*s==90){++s;put@@AG"
                  "@@@@@(*s);ZO"                          "n+=*s-65;}put(34);space(n);pZG"                  "ut(042)\n@@@@@@;}else@put(AE"
                "*s);@@@@@@@@@@ZM"                        "@@}put(042);}@@@@@@@@@@@ZK"                      "@@main(iY"           "nt@argc@AC"
              "@@@@@@@@@,char**aZL"                       "rg){char@R"    "@@@@@@@@@@ZI"                    "*s[2]={"
/*
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
,"};blah"};str(s[0]);lit(s[0]);put(44);lit(s[1]);str(s[1]);}
