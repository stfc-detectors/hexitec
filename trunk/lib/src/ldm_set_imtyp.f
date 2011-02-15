C LAST UPDATE: 
C+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
C
C****************************
C**     LDM_SET_IMTYP      **
C****************************
C
C
CD-Doc:
C IMAGE_Type       SUBROUTINE LDM_SET_IMTYP (VALSTR, IVAL, IERR, ERRSTR)
CD-end
      SUBROUTINE LDM_SET_IMTYP (VALSTR, IVAL, IERR, ERRSTR)
C
      INCLUDE 'implicit_none.finc'
C
C Purpose: Set value of code/integer parameter IMAGE_TYPE
C
C Author:  John W. Campbell, June 1993
C
C Arguments:
C
      CHARACTER*(*) VALSTR
      INTEGER IVAL
      INTEGER IERR
      CHARACTER*(*) ERRSTR

C  VALSTR    (R)   Code to be checked and decoded; if blank use IVAL instead
C                  full code checked must be 'film', 'ip' or 'ccd'
C  IVAL      (R)   Value for the parameter (used if VALSTR = ' ') 1, 2 or 3
C                  1=film, 2=ip, 3=ccd
C  IERR      (W)   Error flag =0 OK
C                             =1 Invalid code or syntax error in string
C                             =3 Value must be >0
C                             =4 Value out of range (other)
C  ERRSTR    (W)   Error description string if error found
C
C====== GLOBALS:
C
      INCLUDE 'ldm_data.finc'
      EXTERNAL LDM_BDAT
C
C====== LOCALS:
C
      INTEGER I
      INTEGER IT
      CHARACTER*4 TYPE(3)
      CHARACTER*4 CODE

C     INTEGER I              !Loop variable
C     INTEGER IT             !Type code
C     CHARACTER*4 TYPE(3)    !Valid image type strings
C     CHARACTER*4 CODE       !Code for matching (upper case

      DATA TYPE /'FILM','IP','CCD'/
C
C-------------------------------------------------------------------------------
C
      IERR = 0
      ERRSTR = ' '
      IF (VALSTR.NE.' ') THEN
         CODE = VALSTR
         CALL CCPUPC(CODE)
         DO 100 I = 1,3
            IT = I
            IF (CODE.EQ.TYPE(I)) GO TO 120
100      CONTINUE
         IERR = 1
         ERRSTR = 
     +   'Invalid code - must be film, ip or ccd'
         RETURN
120      IMTYP = IT
      ELSE
         CALL LDM_ERR_I (IVAL, 1, 3, IERR, ERRSTR)
         IF (IERR.NE.0) RETURN
         IMTYP = IVAL
      END IF
      IS_IMTYP = 1
      CH_IMTYP = CH_MASK
      RETURN
      END
