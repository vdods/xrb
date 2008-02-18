/*
 XXXXXXXXXXXXXXXXXXXXXXXXX  XXXXXXXXXXXXXXXXXXXXX   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX               XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
 XXXXXXXXXXXXXXXXXXXXXXXXX  XXXXXXXXXXXXXXXXXXXXX   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX          XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
 XXXXXXXXXXXXXXXXXXXXXXXXX  XXXXXXXXXXXXXXXXXXXXX   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX       XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
      XXXXXXXXXXXXXXX            XXXXXXXXXXX             XXXXXXXXXXXXX          XXXXXXXXXXXXXXXX           XXXXXXXXXXXXX        XXXXXXXXXXXXXXX
        XXXXXXXXXXXXXX          XXXXXXXXX                 XXXXXXXXXXXX             XXXXXXXXXXXXXX           XXXXXXXXXXX             XXXXXXXXXXXX
         XXXXXXXXXXXXXXX      XXXXXXXXX                   XXXXXXXXXXXX              XXXXXXXXXXXXX           XXXXXXXXXXX              XXXXXXXXXXX
           XXXXXXXXXXXXXX    XXXXXXXX                     XXXXXXXXXXXX              XXXXXXXXXXXXX           XXXXXXXXXXX              XXXXXXXXXXX
            XXXXXXXXXXXXXX XXXXXXXXX                      XXXXXXXXXXXX             XXXXXXXXXXXXXX           XXXXXXXXXXX             XXXXXXXXXXXX
              XXXXXXXXXXXXXXXXXXXX                        XXXXXXXXXXXX            XXXXXXXXXXXXXX            XXXXXXXXXXX           XXXXXXXXXXXXX
               XXXXXXXXXXXXXXXXX                          XXXXXXXXXXXX         XXXXXXXXXXXXXXXX             XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
                 XXXXXXXXXXXXXX                           XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX               XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
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


#include <stdio.h>

// take out all the comments before using

// do not use capital letters in the code -- they are only for control chars.
// do not use backslash or doublequote in the code.

put(int c){putchar(c);}

// if n >= 0, this function prints n spaces
// otherwise (n < 0), this function prints a newline and -n-1 spaces
space(int n){
    if(n<0)
        put(10),n=-n-1;
    for(;n>0;n--)
        put(32);
}

// this function prints a string, without the control codes
// (@ becomes space, and capital letters are skipped)
string(char*s){
    for(;*s!=0;++s)
        if(*s==64)
            put(32);
        else if (*s<65||*s>90)
            put(*s);
}

// this function prints a C string literal, with formatting dictated
// by the control code contents -- capital letters are the control
// codes.
// N - 0 spaces
// O - 1
// P - 2
// Q - 3
// R - 4
// S - 5
// T - 6
// U - 7
// V - 8
// W - 9
// X - 10
// Y - 11
// ZA - 12
// ZB - 13
// ZC - 14
// ZD - 15
// ZE - 16
// ZF - 17
// ZG - 18
// ZH - 19
// ZI - 20
// ZJ - 21
// ZK - 22
// ZL - 23
// ZM - 24
// ZN - 25
// ZO - 26
// ZP - 27
// ZQ - 28
// ZR - 29
// ZS - 30
// ZT - 31
// ZU - 32
// ZV - 33
// ZW - 34
// ZX - 35
// ZY - 36
// ZZ - 27
// M - newline, 0 spaces
// L - 1
// K - 2
// J - 3
// I - 4
// H - 5
// G - 6
// F - 7
// E - 8
// D - 9
// C - 10
// B - 11
// AA - 12
// AB - 13
// AC - 14
// AD - 15
// AE - 16
// AF - 17
// AG - 18
lit(char*s){
    int n;
    put(34);
    for(;*s!=0;++s){
        if(*s==10){
            put(92);
            put(110);
        }
        else if(*s>64&&*s<91){
            put(*s);
            n=*s-78;
            if(*s==65){
                ++s;
                put(*s);
                n-=*s-65;
            }
            else if(*s==90){
                ++s;
                put(*s);
                n+=*s-65;
            }
            put(34);
            space(n);
            put(34);
        }
        else
            put(*s);
    }
    put(34);
}

main(int argc,char**argv){
    char*s[2]={"",""};
    string(s[0]);
    lit(s[0]);
    put(44);
    lit(s[1]);
    string(s[1]);
}
