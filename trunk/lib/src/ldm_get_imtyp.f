C LAST UPDATE: 
C+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
C
C**************************
C**     LDM_GET_IMTYP    **
C**************************
C
C
CD-Doc:
C IMAGE_Type       SUBROUTINE LDM_GET_IMTYP (VALSTR, IVAL, IFLAG)
CD-end
      SUBROUTINE LDM_GET_IMTYP (VALSTR, IVAL, IFLAG)
C
      INCLUDE 'implicit_none.finc'
C
C Purpose: Get value of string/code parameter IMAGE_TYPE
C
C Author:  John W. Campbell, June 1993
C
C Arguments:
C
      CHARACTER*(*) VALSTR
      INTEGER IVAL
      INTEGER IFLAG

C  VALSTR    (W)   Value for the parameter as a string 'film' or 'ip'
C  IVAL      (W)   Code number 1=film, 2=ip
C  IFLAG     (W)   Flag = -1 undefined value
C                       =  0 default value
C                       =  1 value set explicitly
C                       =  2 value set globally
C
C====== GLOBALS:
C
      INCLUDE 'ldm_data.finc'
      EXTERNAL LDM_BDAT
C
C====== LOCALS:
C
      CHARACTER*4 STR(3)

      DATA STR/'film','ip','ccd'/
C
C-------------------------------------------------------------------------------
C
      IVAL = IMTYP
      VALSTR = STR(IVAL)
      IFLAG = IS_IMTYP
      RETURN
      END
