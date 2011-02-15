C LAST UPDATE: 
C+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
C
C****************************
C**     LDM_SET_IMDAT      **
C****************************
C
C
CD-Doc:
C IMAGE_Data       SUBROUTINE LDM_SET_IMDAT (VALSTR, IVAL, IERR, ERRSTR)
CD-end
      SUBROUTINE LDM_SET_IMDAT (VALSTR, IVAL, IERR, ERRSTR)
C
      INCLUDE 'implicit_none.finc'
C
C Purpose: Set value of code/integer parameter IMAGE_DATA
C
C Author:  John W. Campbell, July 1994
C
C Arguments:
C
      CHARACTER*(*) VALSTR
      INTEGER IVAL
      INTEGER IERR
      CHARACTER*(*) ERRSTR

C  VALSTR    (R)   Code to be checked and decoded; if blank and the code
C                  is a standard one then may use IVAL instead.
C                  Standard code may be 'byte', 'i2', 'mar', 'pfbyte', 'ps'
C                  Site specific code is of form site:type with no embedded
C                  spaces e.g. dl:mar)
C  IVAL      (R)   Value for the parameter (used if a standard option
C                  and VALSTR = ' ') 
C                  1=byte, 2=i2, 3=mar, 4=pfbyte, 5=ps
C                  ps = Photonic Science detector (TIFF format)
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
C====== FUNCTION TYPES:
C
      INTEGER LENSTR
C
C====== LOCALS:
C
      INTEGER I
      INTEGER IT
      CHARACTER*6 TYPE(5)
      CHARACTER*6 CODE

C     INTEGER I              !Loop variable
C     INTEGER IT             !Type code
C     CHARACTER*6 TYPE(5)    !Valid image data type strings
C     CHARACTER*6 CODE       !Code for matching (upper case

      DATA TYPE /'BYTE','I2','MAR','PFBYTE','PS'/
C
C-------------------------------------------------------------------------------
C
      IERR = 0
      ERRSTR = ' '
      IF (VALSTR.NE.' ') THEN
         CODE = VALSTR
         CALL CCPUPC(CODE)
         DO 100 I = 1,5
            IT = I
            IF (CODE.EQ.TYPE(I)) GO TO 120
100      CONTINUE
         I = INDEX (VALSTR,':')
         IF (I.GT.1.AND.LENSTR(VALSTR).GT.I) THEN
            IT = 0
            GO TO 120
         END IF
         IERR = 1
         ERRSTR = 
     +   'Invalid image data type'
         RETURN
120      IMDAT = IT
         IMDAT_STR = VALSTR
      ELSE
         CALL LDM_ERR_I (IVAL, 1, 5, IERR, ERRSTR)
         IF (IERR.NE.0) RETURN
         IMDAT = IVAL
         IMDAT_STR = TYPE(IMDAT)
         CALL CCPLWC(IMDAT_STR)
      END IF
      IS_IMDAT = 1
      CH_IMDAT = CH_MASK
      RETURN
      END
