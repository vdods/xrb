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
        XXXXXXXXX    XXXX  XXXXXXXXXXXX     XXXXXXXXXXXXXXXXXXXXXXXX   XXXXX   XXXXXXXXXXXXXX XXXXXXXXXXXXXXXXXXXXXXX        XXX  XXXXXXXXXX
*/


#include <stdio.h>

// take out all the comments before using

// do not use capital letters in the code -- they are only for control chars.
// do not use backslash or doublequote in the code.

void put(int chr,int num){for(;num>0;--num)putchar(chr);}

// this function prints a str, without the control codes
// (@ becomes space, and capital letters are skipped)
void str(char*ptr){
    for(;*ptr!=0;++ptr)
        if(*ptr=='@')
            put(' ',1);
        else if (*ptr<'A'||*ptr>'Z')
            put(*ptr,1);
}

// this function prints a C str literal, with formatting dictated
// by the control code contents -- capital letters are the control
// codes.

// A-J are digits 0-9 in the number of newlines
// K-T are digits 0-9 in the number of spaces
void lit(char*ptr){
    int q=0;
    int nc=0;
    int sc=0;
    put(34,1);
    for(;*ptr!=0;){
        for(;*ptr>='A'&&*ptr<='J';++ptr){
            q=1;
            put(*ptr,1);
            nc=10*nc+(*ptr-'A');
        }
        for(;*ptr>='K'&&*ptr<='T';++ptr){
            q=1;
            put(*ptr,1);
            sc=10*sc+(*ptr-'K');
        }
        if (q!=0){
            put(34,1);
            while(nc--!=0)
                put(10,1);
            while(sc--!=0)
                put(' ',1);
            put(34,1);
            q=0;
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
}

extern char *code;

int main(int argc,char**argv){ 
    str(code);
    lit(code);
    put(';',1);
    return 0;
}

char *code="blahCLKass";

