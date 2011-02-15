C      *****************************************************************
C       *                                                *
C       *  This version of INTLAUE prepared for export 12/10/92.           *
C      *  The history is somewhat chequered but runs something         *
C      *  like this:                                          *
C      *            The basic framework still resembles the            *
C      * original Alan Wonacott MOSFLM which was implemented on the      *
C      * Daresbury VAX by Pella Machin and Trevor Greenhough in 1982   *
C      * (ref DL/CSE/TM23). This suite of programs was exported to UAB *
C      * (Alabama) by TJG in 1984 where a major rewrite to include       *
C      * profile fitting was performed. (J.A.C., 19,400-409, 1986).      *
C      * A further major rewrite for Laue films, begun by PAM and TJG  *
C      * in 1986, has been completed by TJG and Annette Shrive in 1988.*
C      * Further modifications for new BG determination and Overlap    *
C      * deconvolution and radial masks by AKS and TJG are included in *
C      * this version, as are image plate modifications by  TJG, AKS   *
C       * and Richard Denny (Keele). To the best of our knowledge      *
C      * accreditation and ownership now reside with T.J. Greenhough   *
C      * this program being maintained and distributed from Keele       *
C      * University to both individuals and Daresbury to then form a       *
C      * part of the Daresbury Laboratory Laue Software Suite.            *
C      *                                                *
C      *****************************************************************
c
C
C
C           SUBROUTINE TO SOLVE THE BG PLANES FOR THE
C                       CURRENT BIN OR SPOT.
C                     USING THE STANDARD AM MATRIX.
C
      SUBROUTINE ABCBCK( J, AM )
C
      INCLUDE 'comuab.finc'
      INCLUDE 'combck.finc'
      INCLUDE 'comprf.finc'
C
      REAL  AM(3,3), BM3
C
      BM3 = BO(J) + 20000.0
C
C-------BG PLANE COEFFICIENTS
C
      A(J) = BOP(J)*AM(1,1) + BOQ(J)*AM(1,2) + BM3*AM(1,3)
      BZ(J)= BOP(J)*AM(2,1) + BOQ(J)*AM(2,2) + BM3*AM(2,3)
      C(J) = BOP(J)*AM(3,1) + BOQ(J)*AM(3,2) + BM3*AM(3,3)
C
      NMBN = NMBIN + 1
      IF(J.EQ.NMBN)THEN
      if(change(j)) then
c
c ---- allow tilt variation based on BGCUT*100% change in od from 
c      centre to 10 pixels away. Increase this by 25% for safety.
c
            xydev = ((fcut*1.25)/10.0) * c(j)
            if(abs(a(j)).gt.xydev.or.abs(bz(j)).gt.xydev) then
                  intpro = 99999
            end if
      end if
      RETURN
        END IF
      if(.not.batch)write(6,3)
      WRITE(8,3)
3      FORMAT(/1X,'  NBIN     A        BZ          C    ')
      if(.not.batch)write(6,4)J, A(J), BZ(J), C(J)
      WRITE(8,4)J, A(J), BZ(J), C(J)
4      FORMAT(1X,I5,2F10.4,f11.4)
C
C--------------------------------- RESET CHANGE
C
      CHANGE(J) = .FALSE.
      RETURN
      END
C
      SUBROUTINE ALPHA
C
C
C      CALL QGALPH
      RETURN
      END
C
C           SUBROUTINE TO SET UP STANDARD AM MATRIX AND
C                           INVERT
C
C
      SUBROUTINE AM1BCK(jb)
C
      INCLUDE 'combck.finc'
      INCLUDE 'commaskl.finc'
C
      REAL  AM(3,3), BUF1(3), BUF2(3)
C
C
      j = 1
      if(streak) then
        j = jb
      end if
      AM(1,1) = BPP(j)
      AM(1,2) = BPQ(j)
      AM(1,3) = BP(j)
      AM(2,1) = BPQ(j)
      AM(2,2) = BQQ(j)
      AM(2,3) = BQ(j)
      AM(3,1) = BP(j)
      AM(3,2) = BQ(j)
      AM(3,3) = BN(j)
C
C------------- STORE STANDARD AM IN AMST
C
      DO 101 IK = 1,3
      DO 101 IJ = 1,3
101      AMST(IJ,IK) = AM(IJ,IK)
C
C-------INVERT AM
C
      CALL MINV(AM, 3, DET, BUF1, BUF2)
C
C-------------PUT INVERTED AM INTO AMPRO IN COMBCK
C
      DO 3 J=1,3
      DO 3 I=1,3
3      AMPRO(I,J) = AM(I,J)
C
      RETURN
      END
C
C
C           SUBROUTINE TO ADD IN A BG POINT OF VALUE
C         B AT X,Y (RELATIVE TO CENTRE) AND SET UP THE
C            AM MATRIX WHICH HOLDS FOR ALL SPOTS UNLESS
C              OFFBCK IS CALLED TO REMOVE A POINT.
C
C
      SUBROUTINE AMBCK(jb, P, Q)
C
      INCLUDE 'combck.finc'
      INCLUDE 'comprf.finc'
      INCLUDE 'commaskl.finc'
C
      INTEGER P, Q
C
      RP = FLOAT(P)
      RQ = FLOAT(Q)
      RPQ = RP * RQ
      RPP = RP * RP
      RQQ = RQ * RQ
C
C----------------------------SET UP STANDARD AM FOR ALL BINS
C
      ist = 1
      iend = numbin + 1
      if (streak) then
         ist = jb
         iend = jb
      end if
      DO 1 J=ist,iend
      BN(J) = BN(J) + 1
      BP(J) = BP(J) + P
      BQ(J) = BQ(J) + Q
      BPQ(J) = BPQ(J) +  RPQ
      BPP(J) = BPP(J) +  RPP
      BQQ(J) = BQQ(J) +  RQQ
1      CONTINUE
      RETURN
      END
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE AREAD   ****      *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
        SUBROUTINE AREAD(IBUF,ILEN,ICHAN)
        INTEGER*2 IBUF(ILEN)
        READ (ICHAN) IBUF
        RETURN
        END
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE AVSPOT ****       *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C                      ******      AVSPOT     ******
C
C=============================================================
C
C
C
        SUBROUTINE AVSPOT
c
cRCD ...Image Plate logical
        common/iplate/image,mar,moldyn
        logical image,mar,moldyn
C
      INCLUDE 'comori.finc'
      INCLUDE 'compro.finc'
      INCLUDE 'comras.finc'
      INCLUDE 'comrep.finc'
      INCLUDE 'comuab.finc'
      INCLUDE 'comprf.finc'
C
C---------  Include SCN, RFS, MCS, SND for old SEND file
C
      INCLUDE 'comscn.finc'
      INCLUDE 'comrfs.finc'
      INCLUDE 'commcs.finc'
      INCLUDE 'comsnd.finc'
C
        COMMON/IOO/IOUT
        COMMON/SW/ISWTCH(10)
        REAL ASPOT(5),XSH(16),YSH(16),SUMPQ(6,0:15)
        INTEGER GENBUF(10),BUFSIZ,SVMASK(5000)
        INTEGER OD(5000)
        INTEGER*2 IBUF(4),KKREC,IPXX,IPYY
        INTEGER NSH(16),TABLE(0:255),IH(6),IHD(3,50)
        INTEGER LRAS(5)
        LOGICAL FULL,YES,LDUMP
        COMMON/PEL/IOD,BUF(5120),nextr
      integer*2 buf,iod
        DATA FATNO/2/
c         DATA IRANGE /-30,0,50,100,200,400,800,1600,3200/
        EQUIVALENCE (LRAS(1),NXX),(LRAS(2),NYY)
        EQUIVALENCE (IBUF(1),KKREC),(IBUF(2),IPXX),(IBUF(3),IPYY)
        EQUIVALENCE (ASPOT(1),SPOT),(ASPOT(2),BGND),(ASPOT(3),RMSBG)
        EQUIVALENCE (ASPOT(4),DELX),(ASPOT(5),DELY)
C
C
C-----------  FACTOR USED TO ADJUST SD. OF STRONG SPOTS
        DATA EFAC/4.0/
      if(.not.batch)write(6,8877)
      WRITE(IOUT,8877)
 8877   FORMAT(' ENTER AVSPOT')
      IDR = -1
        DO 35 I=3,5
   35   LRAS(I)=IRAS(I)
C
C
C-----  DETERMINE AVERAGE FULLY RECORDED SPOT FOR EACH EXPANDED
C-----  RASTER SIZE. CHECK FOR SATISFACTORY RASTER DIMENSIONS AND
C-----  SPOT CENTERING.
C
        IFAIL=0
        call ccpdpn(-4,'AVSPOT','UNKNOWN','U',0,ifail)
c         OPEN(UNIT=4,FILE='AVSPOT.DAT',STATUS='UNKNOWN',
c      1              FORM='UNFORMATTED')
        NECX=XCEN*FACT+0.5
        NECY=YCEN*FACT+0.5
        NXCUR=0
        NNY=0
        DO 200 I=1,NREF
        READ(3)klrec,LEN,IBUF
        CALL PREAD(BUF(1),LEN,3)
        KREC=KlREC
        IX=IPXX
        IY=IPYY
        NXX=IBYTE(IBUF,7)
        NYY=IBYTE(IBUF,8)
        NXY=NXX*NYY
        IF(NXY.GT.MAXR) WRITE(IOUT,4200) MAXR,NXX,NYY,NXY
        IF(NXY.GT.MAXR.and..not.batch)write(6,4200)MAXR,NXX,NYY,NXY
4200      FORMAT(' MEASUREMENT BOX LARGER THAN EXPECTED; MAXR=',I6/
     +         'NX=',I4,'NY=',I4,'NXY=',I4)
      if(.not.image)then
        NSIZ=(NXY+1)/2
      else
      nsiz=nxy
      endif
C
C--------------  READ ARRAY OF OD.S FOR EACH SPOT
C
        IF(KREC.LT.0) GOTO 200
        IF(NXX.EQ.NXCUR) GOTO 250
        IF(NNY.NE.0.and..not.batch)write(6,1600)NNY,NXCUR
        IF(NNY.NE.0)WRITE(IOUT,1600)NNY,NXCUR
 1600   FORMAT(/,' THERE ARE ',I5,' OVEREXPANDED SPOTS FOR BOX ',
     1               'WIDTH ',I5)
        NNY=0
C       IF(NXCUR.NE.0) WRITE(4) MASK
        IF(NXCUR.EQ.0)GOTO 205
C
C----------------- HERE WHEN CURRENT MASK IS COMPLETED
C
        DO 204 J=-15,15
        WRITE(4)(MASK(K,J),K=1,5000)
  204   CONTINUE
  205   CONTINUE
        DO 210 K=1,5000
        DO 210 J=-15,15
  210   MASK(K,J)=0
        NXCUR=NXX
  250   NY=(NYY-NYS)/2
        IF(NY.LE.15)GO TO 251
        NNY=NNY+1
        GO TO 200
  251   CONTINUE
        IF(IY.LT.NECY) NY=-NY
        MASK(1,NY)=NXX*256+NYY
C
C-------------------  SUM OD.S FOR GIVEN RASTER SIZE
C
        DO 230 K=1,NXY
        if(.not.image)then
        CALL CBYTE(K)
        else
        call ip_cbyte(k)
        endif
  230   MASK(K+1,NY)=MASK(K+1,NY)+abs(int(IOD))
  200   CONTINUE
        IF(NNY.NE.0.and..not.batch)write(6,1600)NNY,NXCUR
        IF(NNY.NE.0)WRITE(IOUT,1600)NNY,NXCUR
        DO 235 J=-15,15
        WRITE(4)(MASK(K,J),K=1,5000)
  235   CONTINUE
        REWIND 4
C
C-------------------  FORM AVGE. SPOT FOR EACH RASTER SIZE
C
        WRITE(IOUT,1300)
        if(.not.batch)write(6,1300)
 1300   FORMAT('  NX   NY  RMSBG  DELX  DELY')
  300   READ(4,END=350) OD
        IF(OD(1).EQ.0) GOTO 300
        NYY=MOD(OD(1),256)
        NXX=OD(1)/256
        NXY=NXX*NYY
        IF(NXY.GT.MAXR) WRITE(IOUT,4200) MAXR,NXX,NYY,NXY
        IF(NXY.GT.MAXR.and..not.batch)write(6,4200)MAXR,NXX,NYY,NXY
        IODMIN=32767
        IODMAX=0
        DO 310 J=2,NXY+1
        IODMIN=MIN0(IODMIN,OD(J))
  310   IODMAX=MAX0(IODMAX,OD(J))
        IODMIN=IODMIN
        SCALE=255./(IODMAX-IODMIN)
        IF(SCALE.GT.1.0) SCALE=1.0
        DO 320 J=2,NXY+1
  320   OD(J)=(OD(J)-IODMIN)*SCALE+0.5
        CALL SETMASK(MASK(1,1),LRAS)
        CALL SETSUMS(MASK(1,1),LRAS,SUMPQ(1,0))
        FULL=.TRUE.
        CALL INTEG(OD(2),LRAS,MASK(1,1),SUMPQ(1,0),ASPOT,IDR,FULL)
        DELX=DELX/FACT
        DELY=DELY/FACT
        WRITE(IOUT,3000) NXX,NYY,RMSBG,DELX,DELY
        if(.not.batch)write(6,3000)NXX,NYY,RMSBG,DELX,DELY
 3000   FORMAT(2I5,3F6.1)
        IF(XTEK) CALL NEWPG
        CALL RASPLOT(OD(2),NXX,NYY,MASK(1,1),-1)
        If(Ltek) CALL SCPAUS
        GOTO 300
  350   CLOSE (UNIT=4)
        CLOSE (UNIT=3)
        ifail=0
        call ccpdpn(-3,'MOSTEMP','OLD','U',0,ifail)
c         OPEN(UNIT=3,FILE='Mostemp',STATUS='OLD',
c      1  FORM='UNFORMATTED')
      RETURN
      END
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE AWRITE ****       *
C                      *                                           *
C                      *********************************************
C
C
C
C
        SUBROUTINE AWRITE(IBUF,ILEN,ICHAN)
        INTEGER*2 IBUF(ILEN)
        WRITE (ICHAN) IBUF
        RETURN
        END
C
      LOGICAL FUNCTION BATCH_MODE()
C
C PARAMETERS
C
C NONE
C
C SPECIFICATION STATEMENTS AND CODE
C ---------------------------------
C
      LOGICAL CCPONL
      BATCH_MODE=.NOT.CCPONL()
      RETURN
      END
C
      SUBROUTINE BEXPAN(A,IA,NXY)
C
C ***CONVEX VERSION***
C
      INTEGER*2 A(*)
      INTEGER IA(*)
      CALL CCPBYI(IA,A,NXY)
      RETURN
      END
C
      SUBROUTINE BPACK(N,IB,IV)
C
C ***CONVEX VERSION***
C
      INTEGER*2 IB(*)
      CALL CCPSTB(IV,IB,N)
      RETURN
      END
C
      BLOCK DATA BREPRT
c added by pdc 27/6/91 to solve compilation error in process.f and report.f
c which had illegal data statements for irange
      COMMON /REPRT/ NREF,NOFR,NOLO,MAXBSI,MINBSI,
     +IRANGE,IANAL,RATIO,AVSD,NEDGE,NBOX,NBZERO,
     +IBOXS,ITOTS,IPROS,NBAD,iprange
      INTEGER NREF,NOFR,MAXBSI,MINBSI
      INTEGER IRANGE(9),IANAL(10),iprange(9)
      REAL RATIO(10),AVSD(10)
      DATA IRANGE /-30,0,50,100,200,400,800,1600,3200/
      data iprange/-30,0,800,1600,3200,6400,12800,25600,51200/
c  For Cornell ips - 12 bits doesnt seem to need higher numbers
c      DATA IpRANGE /-30,0,50,100,200,400,800,1600,3200/
      END
C
      SUBROUTINE BSWAP(K1,K2,IP)
C
C ***CONVEX VERSION***
C
      COMMON/CPEL/CIB(14000)
      INTEGER*2 CIB
      COMMON/PEL/IBA,IB(5120),NEXTR
      INTEGER*2 IB, IBA
      CALL CCPMVB(CIB,IP,IB,K1,K2-K1+1)
      IP = IP + K2 - K1 + 1
      RETURN
      END
C
      SUBROUTINE CBPACK(I)
C
C ***CONVEX VERSION***
C
      COMMON/PEL/IBA,IB(5120),NEXTR
      INTEGER*2 IB, IBA
      COMMON/CPEL/CIB(14000)
      INTEGER*2 CIB
      II = IBA
      CALL CCPSTB(II,CIB,I)
      RETURN
      END
C
      SUBROUTINE CBYTE(I)
C
C ***CONVEX VERSION***
C
      COMMON/PEL/ IBA,IB(5120),NEXTR
      INTEGER*2 IB, IBA
      CALL CCPGTB(II,IB,I)
      IBA = II
      RETURN
      END
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE CENTRS ****       *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C       ****  SUBROUTINE CENTRS  ****
C
C FINDS SPOTS SUITABLE FOR REFINEMENT OF FILM CENTRE,
C CRYSTAL TO FILM DISTANCE AND FILM ORIENTATION
C THE OBSERVED C. OF G. OF THESE SPOTS IS PASSED TO
C OVERLAY REFS
C    IM test changed to use Multiplets (IM = 0) in refinement
C Sept 17th 1986 T.J.G.
c
c##### Laue mods - many changes to base refinement on nodals
c     (with nodal index less than nodlim1)
C=============================================================
C
C
        SUBROUTINE CENTRS(REPEAT)
c
cRCD ...Image Plate logical
        common/iplate/image,mar,moldyn
        logical image,mar,moldyn
c
        COMMON/SW/ISWTCH(10),oneref,noref,tworef
        COMMON/IOO/IOUT
        COMMON/PEL/IBA,IJBA(5120),NREC
        COMMON/CPEL/IARR(14000)
        INTEGER*2 iba,ijba,iarr
        COMMON/THRESH/IAVOD,IMTHR,IMREF
C
      INCLUDE 'comori.finc'
      INCLUDE 'comras.finc'
      INCLUDE 'comscn.finc'
      INCLUDE 'comrfs.finc'
      INCLUDE 'commcs.finc'
      INCLUDE 'comuab.finc'
      INCLUDE 'comlaue.finc'
      INCLUDE 'commask.finc'
      INCLUDE 'commaskl.finc'
C
        LOGICAL REPEAT,AGAIN,YES,oneref,noref,tworef
      integer lras(5)
      INTEGER NREF,WTX,VLIMIT
c      integer buf(10)
c        EQUIVALENCE (BUF(1),IHMR),(BUF(3),JX),(BUF(4),JY)
        INTEGER HWX,HWY,INDF,INDL
        LOGICAL FULL
        INTEGER X(500),Y(500),IREC(500),SHX,SHY,PNTR(100)
        INTEGER IWX(500)
        DATA MAXBUF/5000/,LIMIT/2500/,VLIMIT/4000/
        MAXREF=60
C
C------------ IAVOD FROM 18 TO 28
C------------ AND 12 TO 20.  OCT 3 1984
C------------ INCLUDE CALL TO SIZRAS FOR NPK
C------------ OK SINCE ALL UNEXPANDED
C------------ AND CGFIND ATTEMPTS BG CORRECTION
C
C#####  LAUE MODS. THRESH TO 30
C
        CALL SIZRAS(IRAS,NPK,NBG)
        IF(REPEAT) GOTO 1
        IAVOD=30
      if(image) IAVOD = 60
        GOTO 2
1       IF(.NOT.VEE) GOTO 4
        IF(VEE.AND.REPEAT) IAVOD=20
C
2       CONTINUE
        IF(IMTHR.NE.0) GOTO 4
        IF(.not.tworef.and..not.batch)write(6,999)
999     FORMAT(/1X,'  ......wait for it.......',/)
        IF(tworef) WRITE(IOUT,1000)IAVOD
        If (Batch) goto 3
        IF(tworef.and..not.batch)write(6,1000)IAVOD
1000    FORMAT(/' INNER REF THRESHOLD SET AT [',I3,
     1    '] (ADD 20? FOR STRONG IMAGES): ',$)
3       IF(tworef) read(5,*) IAVOD
1001    FORMAT(I4)
        IF(IAVOD.EQ.0) then
            IAVOD = 30
            if(image) IAVOD = 60
      endif
        IF(tworef) WRITE(IOUT,1002) IAVOD
1002    FORMAT(1H+,I5)
4       CONTINUE
        IMTHR = 1
C
        AGAIN=.TRUE.
        ISEP=200/SCNSZ
        LASTY=0
        IDOV2=CTOFD*XTOFRA/2.0
        IF(.NOT.VEE.AND.REPEAT) GOTO 10
C
C--------------------- START MEASURING
C------------ READ FULL SPOTS FROM GENERATE FILE AND SORT
C
c##### laue mods
        JADD=0
c        J1=PCKIDX+2
c        J2=J1+TOSPT-1
5       JADD=JADD+1
        NREF=0
C
C------------------------------- START LOOP
C
c##### laue mods
c        DO 9 JREC=J1,J2,JADD
c        READ(1,REC=JREC) BUF
c
c        IR=IAND(IHMR,'17'O)
c        IF(IR.NE.0) GOTO 9
c        IM=IAND(IHMR,'360'O)
C
C******************************
C
C                 Changed Sept 17th 1986 for Laues.
C                Select those flagged as multiplets
C
C******************************
C
c##### laue mods - select nodal spots
      do 9 jj = 1,numnod,jadd
      jrec = nodpt(jj)
      ir = nodal(jrec)/8
      if(ir .gt. nodlim1) go to 9
      jx = xyge(1,jrec)
      jy = xyge(2,jrec)
c        IF(IM.eq.0) GOTO 9
        IF(IABS(JY).GT.LIMIT) GOTO 9
C
C-------- TEST FOR AREA CONTAINING UNEXPANDED SPOTS FOR VEE-FILMS.
C
        IF(VEE) GOTO 6
        IF(IABS(JX).GT.LIMIT) GOTO 9
        GOTO 8
6       IF(REPEAT) GOTO 7
        IF(IABS(JX).GT.VLIMIT) GOTO 9
        GOTO 8
7       IF(IABS(IABS(JX)-IDOV2).GT.LIMIT) GOTO 9
8       NREF=NREF+1
        IREC(NREF)=JREC
        XC=JX
        YC=JY
        CALL XYCALC(XCAL,YCAL,XC,YC)
        X(NREF)=ANINT(XCAL*FACT)
        Y(NREF)=ANINT(YCAL*FACT)
        IF(NREF.EQ.500) GOTO 5
9       CONTINUE
C
C----------------------------- END LOOP
C
        GOTO 12
C
C-----------------  USE EXISTING LIST OF REFINEMENT SPOTS
C
10      DO 11 I=1,NRS
        JREC=RRS(I)
        IREC(I)=JREC
c##### laue mods - take x,y values direct from common/laue
c        READ(1,REC=JREC) BUF
c        XC=JX
c        YC=JY
      xc = xyge(1,jrec)
      yc = xyge(2,jrec)
        CALL XYCALC(XCAL,YCAL,XC,YC)
        X(I)=ANINT(XCAL*FACT)
        Y(I)=ANINT(YCAL*FACT)
11      CONTINUE
        NREF=NRS
12      IF(tworef) WRITE(IOUT,1003) NREF
        IF(tworef.and..not.batch)write(6,1003)NREF
1003    FORMAT(/I6,' NODAL SPOTS TO BE MEASURED')
        CALL SORTDN3(NREF,X,Y,IREC)
        HWX=NXS/2
        HWY=NYS/2
        NXY=NXS*NYS
        MAXB=NXY+2
        if(.not.image)then
        MAXW=(MAXB+1)/2
        MAXB=2*MAXW
        else
        maxw=maxb
        endif
        MAXN=MAXBUF/MAXW
        IF(MAXN.GT.100) MAXN=100
        NRSOLD=NRS
13      NRS=0
        IF(VEE.AND.REPEAT) NRS=NRSOLD
        IHALF=(60+NRS)/2+2
        DO 14 I=1,NREF
14      IWX(I)=NXS
        ifail=0
        call ccpdpn(-24,'TEMPSPOT','UNKNOWN','U',0,ifail)
        INDF=1
        INDL=1
        IFRST=1
        ILAST=0
        FULL=.FALSE.
C
C---------- GET THE START OF THE RASTER FOR THE FIRST SPOT OR
C--------------------- FOR A SPOT AFTER A GAP
C
15      CONTINUE
        WTX=X(INDF)+HWX
        IBLK=WTX
C
C------------ TEST IF SORTED SPOT HAS TO BE INCLUDED IN
C-------------------------- THIS SCAN
C
16      IER=-1
        CALL RDBLK(2,IBLK,IJBA,NWORD,IER)
17      IF(INDL.EQ.NREF) GOTO 18
        IF(FULL) GOTO 18
        IF(WTX.GT.X(INDL+1)+HWX) GOTO 18
        INDL=INDL+1
        IF(INDL-INDF.EQ.MAXN-1) FULL=.TRUE.
        GOTO 17
C
C----------- GET RESULTS FROM THIS SCAN AND START THE NEXT ONE
C
18      WTX=WTX-1
        IBLK=IBLK-1
C
C------ STORE OPTICAL DENSITIES FOR ALL SPOTS INCLUDED IN THIS SCAN
C
        IF(INDL.LT.INDF) GOTO 21
        DO 20 J=INDF,INDL
        MJ=MOD(J-1,MAXN)
        NJ=MJ*MAXB
        MJ=MJ+1
        IY=Y(J)
        KMN=IY-HWY
        KMX=IY+HWY
        IF(J.NE.IFRST) GOTO 19
        MFRST=MOD(IFRST-1,MAXN)+1
c
c    *****************************************************
c    I THINK IREC(j) IS UNIMPORTANT HERE. CAUSES TROUBLE. SET TO 0
c    *****************************************************
c
        IARR((MFRST-1)*MAXW+1)=0
        IADDR=(MFRST-1)*MAXB
        PNTR(MFRST)=3
        IFRST=IFRST+1
19      IZZ=PNTR(MJ)+NJ
        if(.not.image)then
        CALL BSWAP(KMN,KMX,IZZ)
        else
        call ip_bswap(kmn,kmx,izz)
        endif
        PNTR(MJ)=IZZ-NJ
        IWX(J)=IWX(J)-1
20      CONTINUE
C
C------------------ CHECK IF ONE OR MORE SPOTS ARE FINISHED
C
21      CONTINUE
        IF(IWX(INDF).EQ.0) GOTO 22
        IF(INDF.LE.INDL) GOTO 16
        FULL=.FALSE.
        GOTO 15
22      IND=ILAST*MAXW+1
        if(.not.image)then
           ijunk=ind+1
        else
           ijunk=ind+2
        endif
      if(streak) then
        lras(1) = nxs
        lras(2) = nys
          do ir=3,5
        lras(ir) = iras(ir)
        end do
        call cgmask(iarr(ijunk),lras,-1,delx,dely,
     1                  x(indf),y(indf),sod)
        avod = sod
      else
          CALL CGFIND(IARR(ijunk),-1,NXS,NYS,DELX,DELY,SOD)
          AVOD=SOD/NPK
C
C------------REPLACE NXY BY NPK.  OCT 3 1984.
C------------AVOD=SOD/NXY
C
      end if
        IF(.NOT.VEE.AND.REPEAT) GOTO 23
C
C---------------UPPER THRESH FROM 60 TO 150.  OCT 3 1984
C
        if(.not.image)then
        IF((AVOD.LT.IAVOD).OR.(AVOD.GT.150)) GOTO 24
        else
        IF((AVOD.LT.IAVOD).OR.(AVOD.GT.40000)) GOTO 24
        endif
C
C------------------ SPOTS SEPERATED BY > 5 MM
C
        IF(IABS(Y(INDF)-LASTY).LT.ISEP) GOTO 24
C
C------------ LIMIT NUMBER OF SPOTS ON RHS OF FILM
C
        IF(NRS.GT.IHALF.AND.X(INDF).GT.XCEN*FACT) GOTO 24
        IF(NRS.EQ.MAXREF) GOTO 25
        IF(ISWTCH(1).EQ.0) GOTO 23
        CALL ODPLOT(IARR(ijunk),NXS,NYS,-1)
c @@   array iarr has irec on the front, read by chkras.
23      CALL AWRITE(IARR(ijunk-1),MAXW,24)
        SHX=DELX/FACT+0.5
        SHY=DELY/FACT+0.5
        NRS=NRS+1
        LASTY=Y(INDF)
        XRS(NRS)=(X(INDF)+DELX)/FACT+0.5
        YRS(NRS)=(Y(INDF)+DELY)/FACT+0.5
        RRS(NRS)=IREC(INDF)
24      ILAST=ILAST+1
        IF(ILAST.EQ.MAXN) ILAST=0
        INDF=INDF+1
        IF(INDF.LE.NREF) GOTO 21
25      CLOSE (UNIT=24)
        IF(VEE.AND.REPEAT)GO TO 26
        IF(NRS.GT.10) GOTO 28
        GO TO 27
26      IF(NRS-NRSOLD.GT.10)GO TO 28
27      CONTINUE
        IF(.NOT.AGAIN) GOTO 28
C
C--- INSUFFICIENT REFINEMENT SPOTS FOUND. REPEAT WITH LOWER THRESHOLD
C--- THRESHOLD REDUCED BY 0.2 TO COPE WITH WEAK DATA ON CEA FILMS
C------------------------ ALAN / GARRY 16/8/82
C
C--------------------  REPLACED AT 0.8.  OCT 3 1984
C-------------------      SPECIAL WIGGLER CASE
C
        IAVOD=0.8*IAVOD
        IF(tworef) WRITE(IOUT,1004) NRS
        IF(tworef.and..not.batch)write(6,1004)NRS
1004    FORMAT(' ONLY',I5,' SPOTS FOUND-REPEATING SEARCH')
        AGAIN=.FALSE.
        GOTO 13
28      CONTINUE
        RETURN
        END
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE CGFIND ****       *
C                      *                                           *
C                      *********************************************
C
C
C
        SUBROUTINE CGFIND(A,IDR,NX,NY,DELX,DELY,SOD)
c
cRCD ...Image Plate logical
        common/iplate/image,mar,moldyn
        logical image,mar,moldyn
c
        INTEGER P,Q,HX,HY
        INTEGER*2 A(*)
        dimension ia(5000)
        MINOD=65000
        NXY=NX*NY
        HX=NX/2
        HY=NY/2
        if(.not.image)then
        call bexpan(a,ia,nxy)
        else
        call ip_bexpan(a,ia,nxy)
        endif
        DO 1 I=1,NXY
1       IF(MINOD.GT.ia(i)) MINOD=ia(i)
        SX=0.0
        SY=0.0
        SOD=0.0
        IJ=1
        HX=HX*IDR
        DO 2 P=-HX,HX,IDR
        DO 2 Q=-HY,HY
        IBA=IA(IJ)-MINOD
        SX=SX+P*IBA
        SY=SY+Q*IBA
        SOD=SOD+IBA
        IJ=IJ+1
2       CONTINUE
        if(sod.ne.0) goto 3
        delx = 10
        dely = 10
        return
3       DELX=SX/SOD
        DELY=SY/SOD
        RETURN
        END
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE CGFIT   ****      *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
        SUBROUTINE CGFIT(RAS,IRAS,IDR,DELX,DELY,SPOT)
c
cRCD ...Image Plate logical
        common/iplate/image,mar,moldyn
        logical image,mar,moldyn
c
        INTEGER P,Q,HX,HY,IRAS(5),S,T
        INTEGER*2 RAS(*)
        DIMENSION IA(5000)
        REAL A(10)
        EQUIVALENCE (A(1),SPP),(A(2),SQQ),(A(3),SPOD)
        EQUIVALENCE (A(4),SQOD),(A(5),SOD),(A(6),TPP),(A(7),TQQ)
        EQUIVALENCE (A(8),TOD),(A(9),TPOD),(A(10),TQOD)
        LOGICAL P1,P2
        HX=IRAS(1)/2
        HY=IRAS(2)/2
        NEXP=(2*HX+1)*(2*HY+1)
        if(.not.image)then
        CALL BEXPAN(RAS,IA,NEXP)
        else
        call ip_bexpan(ras,ia,nexp)
        endif
        IC=HX+HY-IRAS(3)
        IRX=HX-IRAS(4)
        IRY=HY-IRAS(5)
        DO 1 P=1,10
1       A(P)=0.0
        S=0
        T=0
        IJ=0
C
C------------------------ start loop
C
      hx = hx * idr
        DO 3 P=-HX,HX,idr
        IP=IABS(P)
        P1=(IP.GT.IRX)
        P2=(IP.EQ.IRX)
        DO 3 Q=-HY,HY
        IJ=IJ+1
        IQ=IABS(Q)
        IOD=IA(IJ)
        IPQ=IP+IQ
        IF(P1.OR.IPQ.GT.IC.OR.IQ.GT.IRY) GOTO 2
        IF(P2.OR.IPQ.EQ.IC.OR.IQ.EQ.IRY) GOTO 3
        T=T+1
        TPP=TPP+P*P
        TQQ=TQQ+Q*Q
        TPOD=TPOD+P*IOD
        TQOD=TQOD+Q*IOD
        TOD=TOD+IOD
        GOTO 3
2       S=S+1
        SPP=SPP+P*P
        SQQ=SQQ+Q*Q
        SPOD=SPOD+P*IOD
        SQOD=SQOD+Q*IOD
        SOD=SOD+IOD
3       CONTINUE
C
C-------------------------------  end loop
C
        B=SPOD/SPP
        C=SQOD/SQQ
        D=SOD/S
        SPOT=TOD-D*T
C
C-----------------------------CATCH ZERO SPOTS
C
        IF(SPOT.NE.0.)GO TO 4
        DELX=0.
        DELY=0.
        RETURN
4       CONTINUE
c        DELX=(TPOD-B*TPP)/SPOT*IDR
        DELX=(TPOD-B*TPP)/SPOT
        DELY=(TQOD-C*TQQ)/SPOT
C
C---------------------------  SPOT=SPOT/T IE AVERAGE IN SPOT!
C                                OCT 3 1984
C
        SPOT=SPOT/T
        RETURN
        END
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE CGFIT   ****      *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
        SUBROUTINE CGmask(RAS,IRAS,IDR,DELX,DELY,ix,iy,SPOT)
        common/iplate/image,mar,moldyn
        logical image,mar,moldyn
C
      INCLUDE 'commaskl.finc'
      INCLUDE 'commask.finc'
C
        INTEGER P,Q,HX,HY,IRAS(5),S,T
        INTEGER*2 RAS(1)
        DIMENSION IA(5000)
        REAL A(6)
        EQUIVALENCE (A(1),SPod),(A(2),SQod),(A(3),SoD)
        EQUIVALENCE (A(4),toD),(A(5),tpOD),(A(6),Tqod)
        HX=IRAS(1)/2
        HY=IRAS(2)/2
        NEXP=(2*HX+1)*(2*HY+1)
        if(.not.image)then
        CALL BEXPAN(RAS,IA,NEXP)
        else
        call ip_bexpan(ras,ia,nexp)
        endif

        DO 1 P=1,6
      rsums(p) = 0
1       A(P)=0.0
      do 11 p=1,5000
11      rmask(p) = 0
c
c      note that seekrs has idr = 1 which means that ods are
c      stored forwards in x while rmask is always back in x
c
      if(idr.eq.0) ispotr=9111
      call radmask(1,ix,iy,iras(1),iras(2),ispotr,ia)
      idir = idr
      if(idr.eq.0) idir=1
        IJ=0
C
C------------------------ start loop
C
      hx = hx * idir
        DO 3 P=-HX,HX,idir
        DO 3 Q=-HY,HY
        IJ=IJ+1
        IOD=IA(ij)
        if(rmask(ij)) 2,3,12
12      TPOD=TPOD+P*IOD
        TQOD=TQOD+Q*IOD
        TOD=TOD+IOD
        GOTO 3
2       SPOD=SPOD+P*IOD
        SQOD=SQOD+Q*IOD
        SOD=SOD+IOD
3       CONTINUE
C
C-------------------------------  end loop
C
        B=SPOD/rsums(2)
        C=SQOD/rsums(4)
        D=SOD/rsums(6)
        SPOT=TOD-D*rsums(5)
C
C-----------------------------CATCH ZERO SPOTS
C
        IF(SPOT.NE.0.)GO TO 4
        DELX=0.
        DELY=0.
        RETURN
4       CONTINUE
        DELX=(TPOD-B*rsums(1))/SPOT
c        DELX=(TPOD-B*rsums(1))/SPOT*idir
        DELY=(TQOD-C*rsums(3))/SPOT
C
C---------------------------  SPOT=SPOT/T IE AVERAGE IN SPOT!
C                                OCT 3 1984
C
        SPOT=SPOT/rsums(5)
c      write(8,69) npeak,nbg,(rsums(j),j=1,6)
c      write(8,70) b,c,d
c      if(.not.batch)write(6,69) npeak,nbg,(rsums(j),j=1,6)
c      if(.not.batch)write(6,70) b,c,d
c69      format(1x,2i4,6f6.1)
c70      format(1x,3f8.3)
c71      format(1x,8f8.3)
c      write(8,71) spod,sqod,sod,tpod,tqod,tod,delx,dely
c      if(.not.batch)write(6,71) spod,sqod,sod,tpod,tqod,tod,delx,dely
        RETURN
        END
C
      SUBROUTINE  check_file (file_name, facility, exists)
C
C Purpose: To check if specified file exists and if not then print error message
C
      implicit none
C
      character*(*)  file_name, facility
      logical        exists
C
C
C
      common /io_streams/ in_term, out_term, in_data, out_data
      integer*4           in_term, out_term, in_data, out_data
C
C in_term : main input stream (usually the terminal =5)
C out_term : main output (printer) stream (usually =6)
C in_data : input stream for data (to be written to files)
C out_data : output stream for data (to be written to files)
C
C
C-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
C Local Variables:
C
C
      integer*4  i
C
C------------------------------------------------------------------------------
C
      inquire (file=file_name, exist=exists)
C
      if  (.not.exists)  then
         do i=len(file_name),1,-1
            if  (file_name(i:i).ne.' ' .and.
     1          file_name(i:i).ne.char(0))  goto 1
         end do
         i = 4
         file_name = 'file'
 1       write (out_term, 1000) facility, file_name(1:i), 7
      end if
C
      return
 1000 format(1x,a,'-E-FILNOTEX, ',a,' does not exist',a1)
      end
C
C
C             SUBROUTINE TO CHECK THE BACKGROUND VALUE B
C      AT THE POINT X,Y AND EITHER INCLUDE IT IN A NEW
C              EVALUATION OF THE BG PLANE OR THROW
C                            IT OUT.
C
C
      SUBROUTINE CHKBCK(J, JBIN, B, XX, YY, SIGN)
C
      INCLUDE 'comhat.finc'
      INCLUDE 'combck.finc'
C
      INTEGER XX, YY, SIGN
C
C--------EVALUATE BG AT THIS POINT FROM CURRENT PLANE
C
      XN = FLOAT(XX)
      YN = FLOAT(YY)
      DEV = B - A(J)*XN - BZ(J)*YN - C(J)
      NDEV = NINT(DEV)
      if(.not.endbin) goto 10
      if(endbin.and.ovlap.and.correct) then
            if(dev.le.cutu.and.dev.ge.2.5*cutl) goto 10
            REJ(2) = REJ(2) + 1
         else
            IF(DEV.LE.CUTU.AND.DEV.GE.CUTL) GOTO 10
            REJ(J) = REJ(J) + 1
      end if
C
C---------SET TO REJECT THIS POINT
C
      SIGN = -1
C
C------------ RESET AM ELEMENTS... CHANGE COMING
C
        IF(.NOT.CHANGE(J)) CALL CONBCK(0.,0.,-999.,J)
      CHANGE(J) = .TRUE.
      CALL OFFBCK(J,B,XX,YY,SIGN)
      SIGN = 1
      RETURN
C
C----------THIS POINT OK.
C
10      SIGN = 1
      SIGMA(J)=SIGMA(J)+NDEV*NDEV
      RETURN
      END
C
C
C   SUBROUTINE TO REJECT VALUES OF H(I) FOR AN
C        OVERLAPPED REFLECTION.
C
C
      SUBROUTINE CHKPHAT(ODPR,PPR,QPR,PR,PR2,nobin)
C
      INCLUDE 'combck.finc'
      INCLUDE 'comhat.finc'
C
      real shft(5000),rratio(5000)
C
      nav = hpobs(nobin)
      avj = 0.0
C
      DO 100 I=1,HPOBS(nobin)
      rratio(i) = h(i)/hsp(i,nobin)
          if(rratio(i).LT.HOVLAP) then
            avj = avj + hatp(i,1,nobin)
            goto 100
          else
            ratrej = ratrej + 1
            nav = nav - 1
        end if
100       continue
c
      avj = avj/float(nav)
      shftmn = 0.0
c
      do 150 i=1,hpobs(nobin)
      shft(i) = hatp(i,1,nobin) - avj
          if(rratio(i).lt.HOVLAP) then
            shftmn = shftmn + abs(shft(i))
        end if
150      continue
      shftmn = shftmn/float(nav)
c
      icant = 0
      prorej = 0
c
      do 200 i=1,hpobs(nobin)
      if(shft(i).le.(HCUT*shftmn)) goto 200
C
C-----------SET TO REJECT POINT
C
      icant = icant + 1
      OREJ=OREJ+1
C
      CALL OFFHAT(I,ODPR,PPR,QPR,PR,PR2,nobin)
200   CONTINUE
      if((3*icant).gt.hpobs(nobin)) prorej = 1
      if(prorej.eq.0) then
            rgdrej = rgdrej + (hpobs(nobin) - nav)
            ogdrej = ogdrej + icant
      end if
      RETURN
      END
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE CHKRAS  ****      *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C
C       ***  SUBROUTINE CHKRAS  ***
C
C  CALCULATES AVERAGE SPOT PROFILE FROM
C  MEASUREMENTS MADE BY CENTRS.
C  DISPLAYS PROFILE AND MEASUREMENT BOX ON
C  TEKTRONIX AND ALLOWS BOX PARAMETERS TO
C  BE CHANGED.
C
C=============================================================
C
C
        SUBROUTINE CHKRAS
c
cRCD ...Image Plate logical
        common/iplate/image,mar,moldyn
        logical image,mar,moldyn
c
        COMMON/SW/ISWTCH(10),oneref,noref,tworef
        COMMON/IOO/IOUT
        COMMON/THRESH/IJNK1,IMTHR,IMREF
C
      INCLUDE 'comscn.finc'
      INCLUDE 'comras.finc'
      INCLUDE 'comuab.finc'
      INCLUDE 'commaskl.finc'
      INCLUDE 'commask.finc'
C
        INTEGER IOD(5000),KRAS(5),MASK(5000)
        INTEGER*2 BUF(2500)
        INTEGER MINOD,MAXOD
        REAL ASPOT(5),PQVAL(6)
        LOGICAL FULL,oneref,noref,tworef
        EQUIVALENCE (ASPOT(1),SPOT),(ASPOT(2),BGND),(ASPOT(3),RMSBG)
        EQUIVALENCE (ASPOT(4),DELX),(ASPOT(5),DELY)
        INTEGER*2 irec
        CHARACTER*1 JUNK
        ifail=0
        call ccpdpn(-25,'TEMPSPOT','OLD','U',0,ifail)
        IF(XTEK) CALL NEWPG
        NXY=NXS*NYS
        IF(NXY.LE.5000)GO TO 1
        if(.not.batch)write(6,1000)
        WRITE(IOUT,1000)
 1000   FORMAT(/,' MEASUREMENT BOX DIMENSIONS TOO LARGE TO DISPLAY',
     1         ' AVERAGE SPOT PROFILE',
     1         /,' NXS*NYS MUST BE LESS THAN OR EQUAL TO 5000')
        If (Batch) stop
        GO TO 9
1       CONTINUE
      if(.not.image) then
        NSIZ=(NXY+1)/2
      else
      nsiz = nxy
      endif
        DO 2 I=1,NXY
2       IOD(I)=0
C
C-------------------------- SUM ODS IN TEMSPOT
C
3       read(25,end=5)irec,(buf(i),i=1,nsiz)
        DO 4 I=1,NXY
        if(.not.image)then
        IOD(I)=IOD(I)+IBYTE(BUF,I)
        else
        iod(i)=iod(i)+ip_ibyte(buf,i)
        endif
4       CONTINUE
        GOTO 3
C
C--------------------- FIND MAX OD, SCALE RASTER SUM
C
5       MAXOD=0
        MINOD=65000
        DO 6 I=1,NXY
        MAXOD=MAX0(MAXOD,IOD(I))
6       MINOD=MIN0(MINOD,IOD(I))
        SCALE=255./(MAXOD-MINOD)
        DO 7 I=1,NXY
7       IOD(I)=(IOD(I)-MINOD)*SCALE+0.5
C
C--------------------- FIND AVERAGE SPOT SHAPE AND PLOT
C
        If (.not.ltek) goto 8
        IF(.NOT.XTEK) CALL TKCLR
8       if(.not.batch)write(6,1001)
        WRITE(IOUT,1001)
1001    FORMAT(/20X,'AVERAGE SPOT PROFILE FOR CENTRE OF IMAGE'
     +                /,20X,13('---')/)
        CALL SETMASK(MASK,IRAS)
        CALL SETSUMS(MASK,IRAS,PQVAL)
        FULL=.TRUE.
C
C--------------------------save for Batch
C
        Delxs = Delx
        Delys = Dely
        Rmsbgs = Rmsbg
        CALL INTEG(IOD,IRAS,MASK,PQVAL,ASPOT,-1,FULL)
        DELX=DELX/FACT
        DELY=DELY/FACT
        WRITE(IOUT,1002) DELX,DELY,RMSBG
        if(.not.batch)write(6,1002)DELX,DELY,RMSBG
1002    FORMAT(/' C. OF G. SHIFTS =',2F5.1,/' BACKGROUND RESIDUAL=',
     1             F5.1)
        CALL RASPLOT(IOD,NXS,NYS,MASK,-1)
C
C ----------- CHECK SHIFTS FOR CONVERGENCE
C
        Imref = Imref + 1
        Iswtch(2) = 0
        If(delx.eq.delxs.and.dely.eq.delys.and.rmsbg.eq.rmsbgs) goto 11
C
C------------ Stop at 10 cycles in Auto mode
C
        If (Imref.le.10) Iswtch(2) = 1
        If (Batch) goto 11
C
        if(.not.batch)write(6,1003)
1003    FORMAT(/' REPEAT?(Y/N) [Yes]',$)
        read(5,1004)JUNK
1004    FORMAT(A1)
        Iswtch(2) = 1
        CALL NOYES(JUNK,ISWTCH(2))
        IF(ISWTCH(2).EQ.0)GO TO 11
9       WRITE(IOUT,1005)
        if(.not.batch)write(6,1005)
1005    FORMAT(' RASTER PARAMETERS :-'/'  NXS  NYS   NC  NRX  NRY')
        if(.not.batch)write(6,1006) IRAS
        WRITE(IOUT,1006)IRAS
1006    FORMAT(5I5,'      ?')
        read(5,1007) KRAS
1007    FORMAT(5I5)
        WRITE(IOUT,1008)KRAS
1008    FORMAT(1X,5I5)
        DO 10 I=1,5
        IF(KRAS(I).EQ.0) GOTO 10
      if(kras(i).lt.0) kras(i) = 0
        IRAS(I)=KRAS(I)
10      CONTINUE
11      CONTINUE
        IF((MOD(NXS,2).NE.1).OR.(NXS.GT.69)) GOTO 12
        IF((MOD(NYS,2).NE.1).OR.(NYS.GT.69)) GOTO 12
        CLOSE (UNIT=25)
        If (Ltek) CALL TKCLR
        RETURN
12      if(.not.batch)write(6,1009)
1009    FORMAT('***NXS AND NYS MUST BE ODD INTEGERS < OR = TO 69')
        GOTO 9
        END
C
C
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE COMPR   ****      *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C                          SUBROUTINE COMPR
C                         ------------------
C
C  COMPARES CURRENT SPOT IN 'PROCESS' WITH REFLECTIONS
C  SPECIFIED IN 'SPOTDUMP' INPUT FILE, AND
C  SETS LOGICAL VARIABLE LDUMP IF THE CURRENT REFLECTION
C  IS TO BE DUMPED THROUGH ' BADSPOTS' OUTPUT.
C
C   AUG 82
C
C============================================================
C
C
        SUBROUTINE COMPR(KREC,IHD,LDUMP,NSPT)
        LOGICAL LDUMP
        INTEGER IHD(3,50),IH(6)
        JREC=IABS(KREC)
        CALL GETHKL(KREC,IH)
        DO 1 I=1,NSPT
        LDUMP=((IH(1).EQ.IHD(1,I)).AND.
     1       (IH(2).EQ.IHD(2,I)).AND.
     1       (IH(3).EQ.IHD(3,I)))
        IF(LDUMP) return
1       CONTINUE
        RETURN
        END
C
C
C           SUBROUTINE TO GET A,B,C FOR THIS SPOT FROM COMMON
C
      SUBROUTINE CONBCK(APL,BPL,CPL,IBIN)
C
      INCLUDE 'comuab.finc'
      INCLUDE 'combck.finc'
      INCLUDE 'comprf.finc'
      INCLUDE 'comhat.finc'
C
C------------- GET CONSTANTS FOR PRINTOUT WHEN CPL NE -999
C------------- COME FOR REJ WHEN CPL = -9999
C
      IF(CPL.EQ.-9999) GOTO 2
        IF(CPL.EQ.-999.) GOTO 1
      APL = A(IBIN)
      BPL = BZ(IBIN)
      CPL = C(IBIN)
      IF(.NOT.CHANGE(IBIN)) RETURN
C
C------------RESET AM MATRIX (NON-INVERTED) FOR LATER USE
C
1       BPP(ibin) = AMST(1,1)
      BQQ(ibin) = AMST(2,2)
      BN(ibin) = AMST(3,3)
      BPQ(ibin) = AMST(1,2)
      BP(ibin) = AMST(1,3)
      BQ(ibin) = AMST(2,3)
      RETURN
2      APL = FLOAT(REJ(IBIN))
      if(correct) then
      write(8,100) rej(1)
      if(.not.batch)write(6,100)rej(1)
100      format(/1x,I7,' Background replacements for overlapped spots')
      write(8,101) rej(2)
      if(.not.batch)write(6,101)rej(2)
101      format(/1x,I7,' Background rejections for overlapped spots')
      end if
      RETURN
      END
C
C
C    Subroutine to keep track of elapsed CPU time
C
C
C
      SUBROUTINE CPUTIME(ICODE)
C
      INCLUDE 'comuab.finc'
C
      INTEGER *4 ICODE,IMSECS,IMINS,ISECS
C
        CALL CCPTIM (ICODE,CPU,ELAPS)
        Isecs = CPU
      Imins = Isecs/60
      Isecs = Isecs - 60*Imins
C
      if(icode.eq.1) then
      if (.not.batch)Write(6,1000)imins,isecs
      Write(8,1000)imins,isecs
1000  Format(/1x,' ****** Elapsed cpu: ',I3,' mins ',I3,' secs *****')
      end if
      RETURN
      END
C
      SUBROUTINE CUREND
      RETURN
      END
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE CURREF ****       *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C
C       SUBROUTINE CURREF
C
C  REFINE FILM PARAMETERS FROM COORDINATES INPUT
C  FROM TEKTRONIX
C  PARAMETERS DERIVED FROM EQUATIONS
C       XOBS = DELX + RCOS*XCALC - RSIN*YCALC
C       YOBS = DELY + RCOS*YCALC + RSIN*XCALC
C       R    = SQRT(RCOS**2 + RSIN**2)
C
C=============================================================
C
C
C
        SUBROUTINE CURREF(CDX,CDY)
        DIMENSION IX(4),IY(4)
C
      INCLUDE 'comuab.finc'
      INCLUDE 'comscn.finc'
      INCLUDE 'comori.finc'
C
        COMMON/SW/ISWTCH(10),oneref,noref,tworef
        COMMON/IOO/IOUT
        LOGICAL oneref,noref,tworef
C
1       CONTINUE
C
C -------------  POSITION AT TOP OF PAGE
C
        CALL TKPLOT(0,780,1,IER)
        if(.not.batch)write(6,1000)
1000    FORMAT(' SET CURSER TO CALCULATED POSITION OF SPOT',
     1            ' (L = NODAL SPOT)',
     +           /' THEN PRESS ANY KEY ON TEKTRONIX')
        CALL CURSIS(ICHAR,IX(1),IY(1),1)
        CALL TKPLOT(0,720,1,IER)
        if(.not.batch)write(6,1001)
1001    FORMAT(' NOW REPEAT FOR OBSERVED POSITION...')
        CALL CURSIS(ICHAR,IX(2),IY(2),1)
        CALL TKPLOT(0,680,1,IER)
        if(.not.batch)write(6,1002)
1002    FORMAT(' ...AND FOR ANOTHER PAIR')
        CALL CURSIS(ICHAR,IX(3),IY(3),1)
        CALL CURSIS(ICHAR,IX(4),IY(4),1)
        DO 2 I=1,4
        IX(I)=IX(I)-512
2       IY(I)=IY(I)-390
        DXOBS=IX(2)-IX(4)
        DYOBS=IY(2)-IY(4)
        DXCAL=IX(1)-IX(3)
        DYCAL=IY(1)-IY(3)
        IF(DXOBS**2+DYOBS**2.GT.10000) GOTO 3
        CALL TKPLOT(0,0,1,IER)
        if(.not.batch)write(6,1003)
1003    FORMAT(' SEPARATION BETWEEN POSITIONS TOO SMALL-TRY AGAIN')
        GOTO 1
3       RCOS=(DXOBS*DXCAL+DYOBS*DYCAL)/(DXCAL**2+DYCAL**2)
        RSIN=(DYOBS*DXCAL-DXOBS*DYCAL)/(DXCAL**2+DYCAL**2)
        R=SQRT(RCOS**2+RSIN**2)
        XTOFRA=XTOFRA*R
        COSA=RCOS/R
        SINA=RSIN/R
        ANGLE=ATAN2(RSIN,RCOS)*180./3.14159
        COSOM=COSOM0*COSA-SINOM0*SINA
        SINOM=COSOM0*SINA+SINOM0*COSA
        SINOM0=SINOM
        COSOM0=COSOM
        OMEGA0=ATAN2(SINOM0,COSOM0)*180./3.14159
        DELX=(IX(2)+IY(1)*RSIN-IX(1)*RCOS)*5.0
        XCEN=XCEN+DELX
        CDX=CDX+DELX
        DELY=(IY(2)-IY(1)*RCOS-IX(1)*RSIN)*5.0
        YCEN=YCEN+DELY
        CDY=CDY+DELY
        CALL TKCLR
        WRITE(IOUT,1004) R,ANGLE,DELX,DELY
        if(.not.batch)write(6,1004)R,ANGLE,DELX,DELY
1004  FORMAT(/' SCALE FACTOR =',F6.3,' ROTATION OF CALCULATED PATTERN ='
     1           ,F6.2,' DEG.'/
     +          '  SHIFT IN CENTRE =',2F6.1,' (10 MICRON UNITS)'/)
        WRITE(IOUT,1005) XCEN,YCEN,OMEGA0,XTOFRA
        if(.not.batch)write(6,1005)XCEN,YCEN,OMEGA0,XTOFRA
1005    FORMAT(10X,'   XCEN    YCEN   OMEGA   XTOFRA'/
     +                10X,2I8,2F8.4/)
        CALL CUREND
        RETURN
        END
C
      SUBROUTINE CURSIS(ICH,IX,IY,IFLAG)
C
C Altered to qgcurs for intlaue.lib pdc 6/6/91
C
      character*1 ch
      call qgcurs(x,y,ch)
      ich = ichar(ch)
      ix=ifix(x)
      iy=ifix(y)
      RETURN
      END
C
C           SUBROUTINE TO SET BG CUTOFF AND IRAD IN COMMON
C
      SUBROUTINE CUTBCK(CUT,ISRAD,ISRAD2,NPWK1,NMBIN1)
C
      INCLUDE 'combck.finc'
      INCLUDE 'comprf.finc'
      INCLUDE 'comuab.finc'
C
C---------------WHILE HERE SET REJ(J) TO 0
C
      DO 100 IREJJ = 1,18
100      REJ(IREJJ) = 0
      IF(ISRAD.EQ.-1) GOTO 1
      FCUT = CUT
      CUTU = 10.0
      CUTL = - CUTU*0.8
      IRAD = ISRAD
      IRAD2 = ISRAD2
C
C-------- SET INTEN CUT OFF FOR INCLUSION IN PROFILES
C         STORED IN COMPRF
C         AND SET NMBIN (IN COMBCK) AND NUMBIN (IN COMPRF)
C
      NMBIN = NMBIN1
      NUMBIN = NMBIN1
      NPWK = NPWK1
      RETURN
C
C-------------------- SET CUTOFFS BASED ON AVEBG (CUT FROM PROCUAB)
C
1       IF(BGPLAN) GOTO 2
      CUTU = CUT * FCUT
      CUTL = - CUT * FCUT * 0.8
      WRITE(8,10) CUTU,CUTL
      if(.not.batch)write(6,10) CUTU,CUTL
10      FORMAT(/1X,' POINTS MORE THAN +',F6.1,F7.1,' FROM THE ',
     1   'BACKGROUND PLANE ARE REJECTED')
      RETURN
c
c---------- for lower films, reduce bg constants in NOB
c---------- to (c-36)*1/1.42 + 36 where 36 is reckoned to
c---------- be the base (14) + (Abg - 14)/4 (22 for ppb)
c---------- and the film factor is 1.42.
c---------- Then work out new cutoffs still
c---------- based on original input fraction.
c******** for rhino looks like 24 more appropriate
c
2      sumxob = 0.0
      do 50 i=1,numbin
      xob = (float(nob(i) - 24))/1.42 +24
      nob(i) = nint(xob)
50      sumxob = sumxob + xob
      xob = sumxob/float(numbin)
        CUTU = fcut * xob
      CUTL = - fcut * xob * 0.8
      WRITE(8,10) CUTU,CUTL
      if(.not.batch)write(6,10) CUTU,CUTL
      RETURN
      END
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE DSCAN ****       *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C
C            ****  SUBROUTINE DSCAN  ****
C
C CHECKS REQUIRED X-LIMITS ON FILM, SCANS AREA OF FILM AND
C TRANSFERS IMAGE TO DISK FILE 'FILM' IN DIRECTORY UD4.
C
C
C=============================================================
C
C
        SUBROUTINE DSCAN
        COMMON/PEL/JDUN,IDUM(5120),NREC
        INTEGER*2 IDUM,jdun
C
C------------------------------ COMMON VARIABLES
C
      INCLUDE 'comscn.finc'
      INCLUDE 'comfid.finc'
      INCLUDE 'comori.finc'
      INCLUDE 'commcs.finc'
      INCLUDE 'comras.finc'
      INCLUDE 'comuab.finc'
      INCLUDE 'comlaue.finc'
C
        COMMON/IOO/IOUT
C
C-------------------------- LOCAL VARIABLES
C
c        INTEGER IBUF(10)
        BYTE SCLINE(4800)
c        EQUIVALENCE(IBUF(1),IHMR),(IBUF(3),JX),(IBUF(4),JY)
        I16MM=16*40/SCNSZ
        IBG=I16MM/2+1
        IYO=IYOFF-I16MM
        IYL=IYLEN+I16MM
C
C------------------------- FIND LIMITS OF SCAN
C
      ymin = 18000
      ymax = 0
        XMIN=18000
        XMAX=0
c##### laue mods - change choice of spots
c        ICOL=3+2*(FILM-1)
c        JREC=PCKIDX+1
C
C------------------------------------- start loop
C
        DO 1 I=1,TOSPT
c##### laue mods
c     get x y for measurable spots
        im = nodal(i)/8
      IMULT = (NODAL(I)-8*IM)/4
      IMEAS = (NODAL(I)-8*IM-4*IMULT)/2
      IOV = (NODAL(I)-8*IM-4*IMULT-2*IMEAS)
        if(imult.gt.0.and.iov.ne.0) goto 1
        if(film.gt.1. and .imeas.ne.0) go to 1
c
        XC= xyge(1,i)
        YC= xyge(2,i)
        CALL XYCALC(XCAL,YCAL,XC,YC)
c      write(8,333) xc,yc,xcal,ycal
c333      format(1x,'dscan: in,out: ',4(f16.2,1x))
        IX=ANINT(XCAL)
        IF(IX.LT.XMIN) XMIN=IX
        IF(IX.GT.XMAX) XMAX=IX
        Iy=ANINT(yCAL)
        IF(Iy.LT.yMIN) yMIN=Iy
        IF(Iy.GT.yMAX) yMAX=Iy
1       CONTINUE
C
C--------------------------------------end loop
C
        XMIN=XMIN*FACT-NXS
        XMIN=MAX0(1,XMIN)
        XMAX=XMAX*FACT+NXS
        XMAX=MIN0(NREC,XMAX)
c
        yMIN=yMIN*FACT-NyS
        yMIN=MAX0(1,yMIN)
        yMAX=yMAX*FACT+NyS
        yMAX=MIN0(iylen,yMAX)
C
C------------------------ POSITION CHARACTER OUTPUT
C
        IF(Ltek) call tkclr
        If (Ltek) Call TKPLOT(0,700,1,IER)
        if(.not.batch)write(6,1000) XMIN,XMAX,ymin,ymax
        WRITE(IOUT,1000)XMIN,XMAX,ymin,ymax
1000    FORMAT(/' SCAN X-LIMITS ARE ',I5,' AND ',I5,
     1          ' SCAN Y-LIMITS ARE ',I5,' AND ',I5)
C
        IF(.NOT.ONLINE) GOTO 2
C
C--------------------  SCAN BACKWARDS AS JUST FOUND FIDUCIAL 3
C
c         CALL FSCAN(XMAX,XMIN,NREC,SCLINE,IYLEN,SCNSZ,4)
2       CONTINUE
        RETURN
        END
C
      SUBROUTINE ENDTX
      CALL QGTMOD
      RETURN
      END
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE FIDUS ****        *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C     LOCATES THE FIDUCIAL SPOTS, REFINES THEIR
C     POSITION AND CALCULATES THE CENTRE OF THE FILM
C     AND THE FILM TILTING ANGLE ON THE DRUM
C
C=============================================================
C
C
C
        SUBROUTINE FIDUS
C
cRCD ...Image Plate logical
        common/iplate/image,mar,moldyn
        logical image,mar,moldyn
        COMMON/PEL/IBA,IJBA(5120),NREC
        INTEGER*2 IJBA,iba
        COMMON/IOO/IOUT
        COMMON/SW/ISWTCH(10),oneref,noref,tworef
C
      INCLUDE 'comori.finc'
      INCLUDE 'comrfs.finc'
      INCLUDE 'comscn.finc'
      INCLUDE 'comfid.finc'
      INCLUDE 'commcs.finc'
      INCLUDE 'comuab.finc'
C
C---- LOCAL VARIABLES
        INTEGER DLT(4,2)
        INTEGER HFWX,HFWY,FSTX,LSTX,FSTY,LSTY
        INTEGER CTX,CTY,XF,YF,FPOS(5,2)
        LOGICAL LINE,SPOT,oneref,noref,tworef
        DATA THICK/22./
        DIMENSION NODD(5)
        DIMENSION ITHOD(6),NFIDTH(3)
        DATA ITHOD/230,200,160,128,90,40/
c
c##### laue mods
c
        dimension x_c(6),y_c(6),w_c(6),x_to_fra(6)
        dimension twists(6),tilts(6),bulges(6),y_scal(6)
      character*6 aword
C
C
        BYTE SCLINE(2400)
        if(.not.batch)write(6,1000)
        WRITE(IOUT,1000)
 1000   FORMAT(' ENTER FIDUS')
C
C------  READ CAMERA CONSTANTS FROM GENERATE FILE FOR B AND C FILMS
c
c##### laue mods - read new ge file
C
        igo = 0
        DO 99 I=1,3
99      NFIDTH(I) = 0
        IF(.not.bgplan) GOTO 1
      read(1, rec=2) aword,x_cen_fs,y_cen_fs,(w_fs(i),i=1,5)
      read(1, rec=3) w_fs(6)
      cent_in = .true.
      if(x_cen_fs(film).eq.0.and.y_cen_fs(film).eq.0) then
      cent_in = .false.
      endif
        READ(1,REC=5) x_c,y_c,w_c
        READ(1,REC=6) twists,tilts,bulges
        READ(1,rec=7) y_scal,x_to_fra
        cbar = 0
        yscal = y_scal(film)
        ccx   = nint(x_c(film)*100.)
        ccy   = nint(y_c(film)*100.*yscal)
        ccom  = w_c(film)
c
        TILT  = tilts(film)  * FDIST 
        TWIST = twists(film) * FDIST
        BULGE = bulges(film) * FDIST
        IF(VEE) BULGE=BULGE*3.14159/18000.
        VTILT=0.0
        VVERT=0.0
        XTOFRA = x_to_fra(film)
        IF(CCX.EQ.-999) STOP 31
1       continue
c
      if(cent_in) then
      xcenf = nint(x_cen_fs(film)*100.)
      ycenf = nint(y_cen_fs(film)*100.*yscal)
      omegaf = w_fs(film) * 4.0*atan(1.0)/180.
      xcen   = xcenf + ccx
      ycen   = ycenf + ccy
      omega0 = omegaf + ccom * 4.0*atan(1.0)/180.
      sinom0 = sin(omega0)
      cosom0 = cos(omega0)
        ICTF=CTOFD*XTOFRA
c
      write(iout,211)
      write(iout,102) xcenf,ycenf,yscal,w_fs(film)
      write(iout,103) ccx,ccy,yscal,ccom
      write(iout,104) ictf,xtofra
      if(.not.batch) write(6,211)
      if(.not.batch) write(6,102) xcenf,ycenf,yscal,w_fs(film)
      if(.not.batch) write(6,103) ccx,ccy,yscal,ccom
      if(.not.batch) write(6,104) ictf,xtofra
211      format(/1x,' Input Parameters from GE file: ')
102      format(/' x_cen_f: ',i8,/' y_cen_f: ',i8,
     1            ' (GE parameter * ',f6.4,')',/,' w_f    : ',
     1          f8.4,' (degrees)')
103      format(/' ccx    : ',i8,/' ccy    : ',i8,
     1            ' (GE parameter * ',f6.4,')',/,' ccom   : ',
     1          f8.4,' (degrees)')
104      format(/' xtofd  : ',i8,' (',f8.4,' )'/)
c
      if(ltek) call newpg
      return
      endif
      if(image) return
C
C----------------------------- FIDUCIAL BOX SIZE
C
        If (LTEK) CALL TKCLR
        HFWY=MM*FACT+0.5
        HFWX=HFWY
C
C----------------------------- RECORDING OF FIDUCIAL SPOTS
C
        TXFAC=0.14*SCNSZ
        DO 12 J=1,NFID
111     IGO = IGO + 1
        ITHRESH = ITHOD(IGO)
        CW=COS(OMEGAF)
        SW=SIN(OMEGAF)
        XF=(XCENF+FSPOS(J,1)*CW-FSPOS(J,2)*SW)*FACT+0.5
        YF=(YCENF+FSPOS(J,2)*CW+FSPOS(J,1)*SW)*FACT+0.5
C
C------------------- BOX IS ALWAYS SCANNED TOWARDS CENTRE OF FILM
C
        ISX=-ISIGN(1,FSPOS(J,1))
        FSTX=XF-HFWX*ISX
        LSTX=XF+HFWX*ISX
        ISY=-ISIGN(1,FSPOS(J,2))
        FSTY=YF-HFWY*ISY
        LSTY=YF+HFWY*ISY
C
C----------------------- DRAW FIDUCIAL BOXES
C
        IFX=FSTX*TXFAC
        ILX=LSTX*TXFAC
        IFY=FSTY*TXFAC
        ILY=LSTY*TXFAC
        If (Ltek) Then
                Call MOVTX(IFX,IFY)
                Call VECTX(ILX,IFY)
                Call VECTX(ILX,ILY)
                Call VECTX(IFX,ILY)
                Call VECTX(IFX,IFY)
        End If
        ACTX=FSTX
        SOD=0.0
        SXOD=0.0
        SYOD=0.0
        NOD=0
        SPOT=.FALSE.
        IF(.NOT.ONLINE) GOTO 2
C
C-----------------Scan fid area for 1+2  then 3
C                    carry info as arguments.
C
c         if(j.ne.2) call fscan(fstx,lstx,nrec,SCLINE,iylen,scnsz,J)
C
C----------------- SEARCH FOR LINE OF DENSITIES ABOVE THRESHOLD
C
2       CONTINUE
        DO 10 K=FSTX,LSTX,ISX
        IER=ISX
        CALL RDBLK(2,K,IJBA,NWORD,IER)
        ACTX=ACTX+ISX
        LINE=.FALSE.
        LB=FSTY
        LF=LSTY
        DO 5 I=FSTY,LSTY,ISY
        CALL CBYTE(I)
        IF(IBA.LT.ITHRESH) GOTO 3
        IF(LINE) GOTO 5
C
C---------------------  START OF LINE
C
        LINE=.TRUE.
        LB=I
        IB=LB*TXFAC
        JX=ACTX*TXFAC
        If (Ltek) Call MOVTX(JX,IB)
        GOTO 5
C
C----------------------  END OF LINE
C
3       IF(.NOT.LINE) GOTO 5
        LF=I-1
        IE=LF*TXFAC
        If (Ltek) Call VECTX(JX,IE)
        If (Ltek) Call ALPHA
C
C--------------------- TESTS FOR ACCEPTABLE LINE
C------------------- FID MUST LIE COMPLETELY WITHIN BOX
C
        IF(LB.EQ.FSTY) GOTO 4
C
C------------------------ MAX. LINE LENGTH IS 1.5MM
C
        ILFLB=IABS(LF-LB)
        IF(ILFLB.GT.60/SCNSZ) GOTO 4
C
C------------------------ MIN. LINE LENGTH IS 2 PIXELS
C
        IF(ILFLB.LT.2) GOTO 4
        IF(.NOT.SPOT) GOTO 7
C
C---------------------- TEST FOR OVERLAP WITH PREVIOUS LINE
C
        IF(IABS((LBP+LFP)-(LB+LF)).LT.IABS((LF-LB)+(LFP-LBP))) GOTO 7
4       LINE=.FALSE.
5       CONTINUE
        LINE=.FALSE.
C
C------------------------------ NO LINE
C
        IF(.NOT.SPOT) GOTO 10
C
C------------------------------  TESTS FOR ACCEPTABLE FID
C------------------------------  FID MUST LIE WITHIN BOX
        IF(IFSTX.EQ.FSTX) GOTO 6
C
C-----  FID MUST HAVE AREA GREATER THAN .02MM*2  IE 8*50 MICRON PIXELS
C
        IF(NOD.LT.(32/(SCNSZ*SCNSZ))) GOTO 6
        GOTO 11
C
C--------------------------------- FID REJECTED
C
6       SPOT=.FALSE.
        SOD=0.0
        SXOD=0.0
        SYOD=0.0
        NOD=0
        GOTO 10
C
C--------------------------------- LINE
C
7       LFP=LF
        LBP=LB
        IF(SPOT) GOTO 8
        IFSTX=K
        SPOT=.TRUE.
8       CONTINUE
        DO 9 I=LB,LF,ISY
        OD=IBYTE(IJBA,I)
        NOD=NOD+1
        SOD=SOD+OD
        SXOD=SXOD+OD*(ACTX-1)
9       SYOD=SYOD+OD*I
10      CONTINUE
        IF(IGO.LT.6) GOTO 111
        If (Ltek) Call TKPLOT(0,780,1,IER)
        if(.not.batch)write(6,1001) J
        WRITE(IOUT,1001)J
1001    format(' FIDUCIAL',I4,' NOT FOUND')
           If(Batch) Stop
        if(.not.batch)write(6,1002)
        WRITE(IOUT,1002)
1002    FORMAT(' ENTER NEW ESTIMATE OF POSITION OF FIDUCIAL 1 ',$)
        read(5,*)IXF,IYF
        WRITE(IOUT,*)IXF,IYF
        XCENF=-FSPOS(1,1)+IXF/FACT
        YCENF=-FSPOS(1,2)+IYF/FACT
        IGO = 0
        DO 101 KKK = 1,3
101     NFIDTH(KKK) = 0
        GO TO 1
11      IF(NOD.GT.1000/(SCNSZ*SCNSZ).and..not.batch)write(6,1003) J
        IF(NOD.GT.1000/(SCNSZ*SCNSZ))WRITE(IOUT,1003)J
1003    FORMAT( ' FIDUCIAL',I4,'  TOO LARGE')
        If (Ltek) Call ALPHA
        CTX=SXOD/SOD
        CTY=SYOD/SOD
        SOD=SOD/NOD
C
C--------------------------- CENTRE OF FIDUCIAL SPOT
C
        FPOS(J,1)=CTX/FACT
        FPOS(J,2)=CTY/FACT
C
C------------------------- SHIFTS FROM EXPECTED POSITIONS
C
        DLT(J,1)=(CTX-XF)/FACT
        DLT(J,2)=(CTY-YF)/FACT
        NODD(J)=NOD
        NFIDTH(J) = ITHOD(IGO)
        IGO = 0
12      CONTINUE
C
C------------------------ POSITION CHARACTER OUTPUT
C
        If (Ltek) Call TKPLOT(300,700,1,IER)
        WRITE(IOUT,1005)
        IF(XTEK.and..not.batch)write(6,1004)
1004    FORMAT(///////)
        if(.not.batch)write(6,1005)
        DO 13 II=1,NFID
        WRITE(IOUT,1006)II,FPOS(II,1),FPOS(II,2),DLT(II,1),
     1                 DLT(II,2),NODD(II),NFIDTH(II)
        if(.not.batch)write(6,1006)II,FPOS(II,1),FPOS(II,2),DLT(II,1)
     1                  ,DLT(II,2),NODD(II),NFIDTH(II)
13      CONTINUE
C
C---------------- CALCULATE FILM CENTRE AND TILTING ANGLE
C ----------------BOTH AS DETERMINED FROM THE MEASUREMENT AND
C-----------------AS CORRECTED USING THE CAMERA CONSTANTS
C
C ******************* mods for new fids
C
      NFP = 1
      NFC = 2
      if(fpos(1,1).gt.fpos(1,2)) then
            NFP = 2
            NFC = 1
      end if
C
C *******************
C
        XCENF=(FPOS(NFP,1)+FPOS(3,1))/2.0+0.5
        YCENF=(FPOS(NFP,2)+FPOS(3,2))/2.0+0.5
        OMEGAF=FLOAT(FPOS(3,2)-FPOS(NFC,2))/(FPOS(3,1)-FPOS(NFC,1))
      omegaf = atan(omegaf)
        XCEN=XCENF+CCX
        YCEN=YCENF+CCY
        OMEGA0=OMEGAF+CCOM*3.14159/180.
        COSOM0=COS(OMEGA0)
        SINOM0=SIN(OMEGA0)
C
C----------------------- REPORT RESULTS
C
1005    FORMAT(/' FIDUS NO.  X       Y     DX    DY  NO.POINTS THRESH')
1006    FORMAT(I6,2I8,2I6,2I8)
        ANGLE=OMEGA0*57.296
        ICTF=CTOFD*XTOFRA
        WRITE(IOUT,1007) ICTF,XTOFRA,ANGLE,XCEN,YCEN
        if(.not.batch)write(6,1007)ICTF,XTOFRA,ANGLE,XCEN,YCEN
1007    FORMAT(' POST-FIDUCIAL TRANSFORMATION.'/' XTOFD: ',I8,
     1        '   ( ',F8.4,' )'/' OMEGA: ',F8.4/' XCEN:  ',I8/
     2           ' YCEN:  ',I8/)
         RETURN
         END
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE FLMPLOT****       *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C       SUBROUTINE FLMPLOT
C
C  SUPERIMPOSES IMAGE OF FILM WITH CALCULATED
C  SPOT POSITIONS.  ALLOWS REFINEMENT OF
C  FILM/DRUM TRANSFORMATION BY CURSOR INPUT
C  FROM TEKTRONIX.
C=============================================================
C
C

        SUBROUTINE FLMPLOT
        COMMON/IOO/IOUT
        COMMON/SW/ISWTCH(10),oneref,noref,tworef
C
      INCLUDE 'commcs.finc'
      INCLUDE 'comscn.finc'
      INCLUDE 'comori.finc'
      INCLUDE 'comras.finc'
      INCLUDE 'comuab.finc'
C
        LOGICAL oneref,noref,tworef
        CHARACTER*1 JUNK
        IF(XTEK)CALL NEWPG
        IF(.NOT.XTEK) CALL TKCLR
C
C ------------------- PLOT REFLEXION POSITIONS
C
        CDX=0.0
        CDY=0.0
        CALL REFPLOT(CDX,CDY)
C
C------------------  SCAN FILM AND PLOT IMAGE OF FILM
C
        CALL SPOTPLOT
        CALL TKPLOT(0,50,1,IER)
        if(.not.batch)write(6,1000)
1000    FORMAT(' DO YOU WANT TO ADJUST THIS ?(Y/N)' ,$)
        read(5,1004)JUNK
        IF(JUNK.EQ.'N')GO TO 2
        if(junk.eq.'n') goto 2
C
C------------  REFINE TRANSFORMATION USING CURSOR INPUT DATA
C
1       CALL CURREF(CDX,CDY)
        if(.not.batch)write(6,1001)
1001    format(' DO YOU WISH TO REPLOT ? (Y or N)',$)
        read(5,1004)junk
        if(junk.eq.'n') goto 2
        if(junk.eq.'N') goto 2
        CALL TKCLR
C
C------------------------  REPLOT REFLEXIONS
C
        CALL REFPLOT(CDX,CDY)
C
C--------------------------  REPLOT IMAGE
C
        CALL VECPLOT
        WRITE(IOUT,1002)
        if(.not.batch)write(6,1002)
1002    FORMAT(/'    REPEAT CURSOR INPUT?(Y/N) [No]',$)
        read(5,1004)JUNK
        WRITE(IOUT,1003)JUNK
1003    FORMAT(1X,A1)
1004    FORMAT(A1)
        Iswtch(3) = 0
        CALL NOYES(JUNK,ISWTCH(3))
        IF(ISWTCH(3).EQ.1) GOTO 1
2       CONTINUE
        CALL TKCLR
        RETURN
        END
C
C
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE FNDFLM ****       *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C  LOCATES CORNERS OF FILM, CALCULATES Y-OFFSET
C  AND Y-LENGTH FOR SCANNER.
C  DETERMINES APPROX. FILM CENTRE.
C
C=============================================================
C
C
C
        SUBROUTINE FNDFLM
c
cRCD ...Image Plate logical
        common/iplate/image,mar,moldyn
        logical image,mar,moldyn
c
C  ONLY SINGLE BUFFER USED TO ENABLE MAXIMUM YLENGTH TO BE
C  READ IN FINDING FILM POSITION
C  Y-OFFSET AND Y-LENGTH ARE RESET AFTER FILM FOUND
        COMMON/PEL/IBA,IJBA(5120),NREC
        INTEGER*2 IJBA,iba
C
      INCLUDE 'comori.finc'
      INCLUDE 'commcs.finc'
      INCLUDE 'comscn.finc'
      INCLUDE 'comfid.finc'
      INCLUDE 'comuab.finc'
      INCLUDE 'comhis.finc'
C
        COMMON /EXTRAS/ JUMPAX
      common /io_streams/ in_term, out_term, in_data, out_data
      integer*4           in_term, out_term, in_data, out_data
      LOGICAL ERR
        LOGICAL TEST,LFOUND,HFOUND
        INTEGER SFX,SVX,BUF(10)
        REAL DUMMY(15)
c
c##### laue mods - allow 6 films
        DIMENSION IFDAT(6)
        DATA IFDAT/'A ','B ','C ', 'D ','E ','F '/
        DATA SFX/6250/,SVX/8750/
        CHARACTER*60 SFILE
      character*6 aword
        COMMON/IOO/IOUT
        CHARACTER*1 JUNK
C
C
        if(.not.batch)write(6,1000)
        WRITE(IOUT,1000)
1000    FORMAT(' ENTER FNDFLM')
        if(ltek) CALL ALPHA
        JJ=IFDAT(FILM)
        READ(1,REC=10) NPCK
        WRITE(IOUT,1001) NPCK,JJ
        if(.not.batch)write(6,1001)NPCK,JJ
1001    FORMAT(/10X,10('**')/10X,'IMAGE',I5,A1,
     1            /10X,10('**')/)
        CALL TDATE
C
C--------------------- CLEAR COLUMNS FOR B AND C FILMS
C##### laue mods - special subroutine
        IF(FILM.EQ.1) GOTO 2
c        ICOL=3+2*FILM
c        JREC=PCKIDX+1
c        DO 1 I=1,TOSPT
c        JREC=JREC+1
c        READ(1,REC=JREC) BUF
c        BUF(ICOL)=-9999
c        BUF(ICOL+1)=-9999
c        WRITE(1,REC=JREC) BUF
c1        CONTINUE
c     for laues call a subroutine
      iclear = 1
      if(.not.batch) then
       if(.not.batch)write(6,1002)
       WRITE(IOUT,1002)
1002   FORMAT( ' Clear Intensities for this film ? (Y/N) ',$)
       read(5,1003) JUNK
       WRITE(IOUT,1004) JUNK
1003   FORMAT(A1)
1004   FORMAT(1X,A1)
       CALL NOYES(JUNK,Iclear)
      end if
       IF(iclear.ne.0) call setge (tospt, film)
2        CONTINUE
C
C----  READ IN DATA FROM 'FILM' FILE WHEN SKIPPING FILM SCANNING
C----  NORMALLY:
C----  FIDUS SETS XCENF,YCENF,OMEGAF,XCEN,YCEN,COSOM0,SINOM0,XTOFRA
C----  DSCAN SETS XMIN,XMAX
C
C       SKIP ALL THIS FOR LAUES
C
        ONLINE = .false.
C       If (Batch) goto 3
C       if(.not.batch)write(6,1002)
C       WRITE(IOUT,1002)
C1002   FORMAT( ' PROCESS PREVIOUSLY SCANNED FILM IMAGE? (Y/N) ',$)
C       read(5,1003) JUNK
C       WRITE(IOUT,1004) JUNK
C1003   FORMAT(A1)
C1004   FORMAT(1X,A1)
C       CALL NOYES(JUNK,IONL)
C       IF(IONL.EQ.0) ONLINE = .TRUE.
3       if(.not.batch)write(6,1005)
        WRITE(IOUT,1005)
1005    FORMAT(' FILENAME OF DIGITIZED DATA?',$)
        read(5,1007)SFILE
        WRITE(IOUT,1006)SFILE
1006    FORMAT(1X,A60)
1007    FORMAT(A60)
        IF(.NOT.ONLINE) GOTO 4
c         OPEN(UNIT=2,FILE=SFILE,STATUS='UNKNOWN',
c      1         ACCESS='DIRECT',
c      1         FORM='UNFORMATTED',
c      1         ORGANIZATION='RELATIVE',RECL=560)
C
C-------- write last record (nrec) to speed up transfer
C
c         WRITE(2,REC=NREC) 0
        GOTO 5
4       lrec=2*nword
        call flmrcl(lrec,lw,6)
        ifail=1
        call ccpdpn(-2,sfile,'OLD','DU',lw,ifail)
        if(ifail.eq.1)goto 5
c         OPEN(UNIT=2,FILE=SFILE,STATUS='OLD',READONLY,
c      1             ACCESS='DIRECT',
c      1             FORM='UNFORMATTED',iostat=imok)
c         If(imok.eq.0) goto 5
        if(.not.batch)write(6,108)sfile
        write(iout,108)sfile
108     format(1x,' ** File ',A60,' not found **')
        goto 3
C
C----------------------------SET CONSTANTS
C
5       ITHRESH=N1OD*BASEOD
        IYOFF=0
cRCD ...Skip FID stuff for IP
        if(image)goto 999
C
      cent_in = .true.
      read(1, rec=2) aword,x_cen_fs,y_cen_fs,(w_fs(i),i=1,5)
      if(x_cen_fs(film).eq.0.and.y_cen_fs(film).eq.0) then
      cent_in = .false.
      endif
      if(cent_in) then
            if(batch) then
              read(5,1011)ItemX,ItemY
            endif
      return
        endif
C
C-------------------INPUT SCANNER PARAMETERS
C
        IF(ONLINE) GOTO 6
        IXFS = Nint(nrec/2 + fspos(1,1)*fact)
        IYFS = Nint(nword  + fspos(1,2)*fact)
C
C********************** mods for new fids
C
c      if(fspos(1,2).gt.0) then
c              IyFS = Nint(11000.*fact)
c              IxFS = Nint(2000.*fact)
c      end if
      IXF = IXFS
      IYF = IYFS
C
C *********************
C
        if(.not.batch)write(6,1008)IXF,IYF
        WRITE(IOUT,1008)IXF,IYF
1008    FORMAT(' FIRST FIDUCIAL AT (X,Y IN PIXELS) [',
     1       I4,',',I4,']:',$)
        read(5,*)IXF,IYF
1011    FORMAT(2I4)
        IF(IXF.EQ.0) IXF = IXFS
        IF(IYF.EQ.0) IYF = IYFS
        WRITE(IOUT,1012) IXF,IYF
1012    FORMAT(1H+,2I5)
        GOTO 7
C
C-------------------- ONLINE
C
6       ixf=130
        iyf=380
        WRITE(IOUT,1009)IXF,IYF
        if(.not.batch)write(6,1009)IXF,IYF
1009    FORMAT(/' FIRST FIDUCIAL AT (X,Y IN PIXELS)',2I5,/)
7       XCENF=-FSPOS(1,1)+IXF/FACT
        YCENF=-FSPOS(1,2)+IYF/FACT
        OMEGAF=0.
        IYOFF=0
        ACTX=0
        WRITE(IOUT,1010)XCENF,YCENF
        if(.not.batch)write(6,1010)XCENF,YCENF
1010    FORMAT(' FNDFLM: XCENF,YCENF',2I8)
999     RETURN
        END
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE GENSORT ****      *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C
C       MAIN PROGRAM GENSORT
C
C
C  READS REFLEXIONS FROM GENERATE FILE,TRANSFORMS COORDINATES
C  TO SCANNER FRAME AND SORTS RECORD NUMBERS ON X-COORDINATE,
C  WRITES FILE OF SORTED RECORD NUMBERS, X AND Y-COORDINATES
C     MAXIMUM NUMBER OF REFLEXIONS = 100000/300000 (CHANGED FROM 5800
C  SEPT 1st  1986 T.J.G.
C     Value of IM changed to use Multiplets in refinement
C  SEPT 17th 1986 T.J.G.
c
c##### laue mods
c     max number of refs = 100000/300000 for 25 micron!
c     for mode = 0 for seekrs loop over nodals and check against
c                      nodlim2
c     else loop over all spots but in box integration omit spatial
c                       overlaps
C=============================================================
C
C
        SUBROUTINE GENSORT
        COMMON/TREV/NXMAX,NYMAX
        COMMON/SW/ISWTCH(10),oneref,noref,tworef
        COMMON/IOO/IOUT
C
      INCLUDE 'dimgns.finc'
      INCLUDE 'comlaue.finc'
      INCLUDE 'comuab.finc'
      INCLUDE 'commcs.finc'
      INCLUDE 'comori.finc'
      INCLUDE 'comfid.finc'
      INCLUDE 'comhat.finc'
C
C----------- Include RAS, SCN, RFS, REP for old send file.
C----------------------NEW COMSND for FNAME, MODE
C
      INCLUDE 'comsnd.finc'
      INCLUDE 'comras.finc'
      INCLUDE 'comscn.finc'
      INCLUDE 'comrep.finc'
      INCLUDE 'comrfs.finc'
C
        LOGICAL oneref,noref,tworef
        INTEGER VLIMIT,XLOW,XHIGH,YLOW,YHIGH
        DATA LIMIT/2500/,VLIMIT/4000/
      DIMENSION ISTAT(8),NREJGE(8),nusing(8)
      do 99 i=1,8
      nusing(i) = 0
      NREJGE(i)=0
99      istat(i) = 0
      nboxit = 0
      nproit = 0
C
C--------------------- OPEN SORT AND GE FILES
C
       ifail=0
       call ccpdpn(-9,'SORTTEMP','UNKNOWN','U',0,ifail)
c        OPEN(UNIT=9,FILE='SORTTEMP',STATUS='UNKNOWN',FORM='UNFORMATTED')
c########
c        OPEN(UNIT=1,FILE=FNAME,STATUS='OLD',ACCESS='DIRECT',
c     1  FORM='UNFORMATTED')
C
        if(.not.batch)write(6, 1010)
        write(iout, 1010)
 1010   format(' Preparing spot list...')
        XLOW=XMIN+(NXMAX/2+1)
        XHIGH=XMAX-(NXMAX/2+1)
        YLOW=ymin +(NYMAX/2+1)
        YHIGH=ymax -(NYMAX/2+1)
      if(.not.batch)write(6,1011)XLOW,XHIGH,YLOW,YHIGH
      WRITE(iout,1011)XLOW,XHIGH,YLOW,YHIGH
1011      FORMAT(/1X,' Spot X Limits: ',2I8,
     1             ' Spot Y Limits: ',2I8,/)
c##### laue mods
c        ICOL=3+2*(FILM-1)
c        JREC=PCKIDX+1
        NFAILY=0
        MAXREF = MAXRFL
        NFAIL=0
        NREF=0
      infail = 0
      imfail = 0
      ndfail = 0
      nofail = 0
      iifail = 0
      mofail = 0
c
c##### laue mods
      nloop = tospt
      if(mode .eq.0)nloop = numnod
C
C------------------------------------------------- START LOOP---
      do 4 ii=1,nloop
      i = ii
      if ( mode.eq.0) i =nodpt(ii)
      jrec = i
      idec(jrec)=0
      xc = xyge(1,i)
      yc = xyge(2,i)
      jx = xc
      jy = yc
C ---------------------------unpack flags
      nodali = nodal(i)
C      ir = iand(nodali, '01'x)
      im = nodal(i)/8
      IMULT = (NODAL(I)-8*IM)/4
      IMEAS = (NODAL(I)-8*IM-4*IMULT)/2
      IR = (NODAL(I)-8*IM-4*IMULT-2*IMEAS)
C      imult = iand(nodali, '04'x)
C      imeas = iand(nodali, '02'x)
C ----------------------------  statistics histogram
      Nbit = 4*imult + 2*Imeas + ir +1
      ISTAT(nbit) = ISTAT(nbit) + 1
C  --------------------------- omit overlaps if box integration
      if ((. not. profil).or.(. not. correct)) then
         if(ir.ne.0) then
            nofail = nofail + 1
            go to 4
       end if
      endif
C --------------------------- Jump to 2 for mode = 0 (refinement)
        IF(MODE.NE.0) GOTO 2
          if(im.eq.0) then
            imfail = imfail + 1
            go to 4
        end if
        IF(VEE) GOTO 1
C --------------------------- check nodal value is less than nodlim2
      if(im.gt.nodlim2) then
            ndfail = ndfail + 1
            go to 4
      end if
C --------------------------- check x and y limits
        IF(IABS(JX).LT.LIMIT.AND.IABS(JY).LT.LIMIT) THEN
         infail = infail + 1
         GOTO 4
      end if
        GOTO 2
1       IF(IABS(JX).LT.VLIMIT.AND.IABS(JY).LT.LIMIT) GOTO 4
2       CONTINUE
c
C ------ FOR BOTH MODES - imeas set for film 1 based on mint
c
        if(film.gt.1.and.imeas.ne.0) then
            iifail = iifail + 1
            go to 4
      end if
c -------------- Reject overlapped multiplets
        if((mode.ne.0).and.(imult.gt.0).and.(ir.ne.0)) then
            mofail = mofail + 1
            goto 4
      end if
        NREF=NREF+1
        IF(NREF.GT.MAXREF) GOTO 8
        IREC(NREF)=JREC
        if (mode.ne.0)then
           if(imult.gt.0) then
             irec(nref) = -jrec
           end if
             if(ir.ne.0)idec(jrec)=1
        end if
21      continue
        CALL XYCALC(XCAL,YCAL,XC,YC)
        IX(NREF)=ANINT(XCAL*FACT)
        IF(IX(NREF).GT.XLOW.AND.IX(NREF).LT.XHIGH) GOTO 3
        NFAIL=NFAIL+1
        NREF=NREF-1
        GOTO 4
3       IY(NREF)=ANINT(YCAL*FACT)
        IF(IY(NREF).GT.YLOW.AND.IY(NREF).LT.YHIGH)GO TO 31
        NFAILY=NFAILY+1
        NREF=NREF-1
      goto 4
C --------------------------- check flags for refinement nodals
31      nbit = 4*imult + 2*imeas + ir + 1
      nusing(nbit) = nusing(nbit) + 1
4       CONTINUE
C
C---------------------------------------------------------- END LOOP--
        IF(NREF.GT.0)GO TO 5
        WRITE(IOUT,1000)
1000    FORMAT(' SPOT RANGE TOO SMALL TO FIND ANY SPOTS FOR',
     1           1X,'SEEKRS REFINEMENT')
        if(.not.batch)write(6,1000)
        STOP
5       continue
      if(xtek) call tkclr
c
c      write statistics
c
      nsing = 0
      nnod = 0
      nrefnd = 0
      irejge = 0
      do 40 i=1,4
      nsing = nsing + istat(i)
      nrefnd = nrefnd + nusing(i)
      nrejge(i) = istat(i) - nusing(i)
      irejge = irejge + nrejge(i)
      j = i+4
      nrefnd = nrefnd + nusing(j)
      nrejge(j) = istat(j) - nusing(j)
      irejge = irejge + nrejge(j)
40      nnod = nnod + istat(j)
      NNref = nnod + nsing
c------------------------------WRITE STATS. BOTH MODES
      if(.not.batch)write(6,111)
      WRITE(IOUT,111)
111      format(/1x,'        Summary of input reflection data',/)
      if(.not.batch)write(6,112)nnref
      WRITE(IOUT,112)NnREF
112      format(1x,' Total number of reflections           : ',i6,/)
      if(.not.batch)write(6,113) Nsing
      WRITE(IOUT,113)NSING
113     format(1x,'                   ','  SINGLETS          : ',i6)
      if(.not.batch)write(6,117)Nnod
      WRITE(IOUT,117)NNOD
117     format(1x,'                   ',' MULTIPLETS         : ',i6,/)
c
      if(.not.batch)write(6,1211)
      write(iout,1211)
      if(.not.batch)write(6,122)
      write(iout,122)
      if(.not.batch)write(6,123)   istat(1),istat(3),istat(5),istat(7)
      if(.not.batch)write(6,124)   istat(2),istat(4),istat(6),istat(8)
      write(iout,123) istat(1),istat(3),istat(5),istat(7)
      write(iout,124) istat(2),istat(4),istat(6),istat(8)
c
      if(.not.batch)write(6,1171) irejge
      write(iout,1171) irejge
      IF(INFAIL.GT.0.and..not.batch)write(6,1172) infail
      IF(INFAIL.GT.0) write(iout,1172) infail
      IF(NFAIL.GT.0.and..not.batch)write(6,1173) nfail
      IF(NFAIL.GT.0) write(iout,1173) nfail
      IF(NFAILY.GT.0.and..not.batch)write(6,1174) nfaily
      IF(NFAILY.GT.0) write(iout,1174) nfaily
c
      IF(nofail.GT.0.and..not.batch)write(6,1181) nofail
      IF(nofail.GT.0) write(iout,1181) nofail
      IF(imfail.GT.0.and..not.batch)write(6,1182) imfail
      IF(imfail.GT.0) write(iout,1182) imfail
      IF(ndfail.GT.0.and..not.batch)write(6,1183) ndfail
      IF(ndfail.GT.0) write(iout,1183) ndfail
      IF(iifail.GT.0.and..not.batch)write(6,1184) iifail
      IF(iifail.GT.0) write(iout,1184) iifail
      IF(mofail.GT.0.and..not.batch)write(6,1185) mofail
      IF(mofail.GT.0) write(iout,1185) mofail
1171      format(1x,' Total number of rejections            : ',i6)
1181      format(1x,'                    On overlap flag    : ',i6)
1182      format(1x,'                    On nodal flag      : ',i6)
1183      format(1x,'                    On nodal index     : ',i6)
1184      format(1x,'                    On Intensity       : ',i6)
1185      format(1x,'                    On overlap/mult    : ',i6)
1172      format(1x,'                    On Inner Limits    : ',i6)
1173      format(1x,'                    On Outer X Limits  : ',i6)
1174      format(1x,'                    On Outer Y Limits  : ',i6,/)
c
      if(.not.batch)write(6,1211)
      write(iout,1211)
      if(.not.batch)write(6,122)
      write(IOUT,122)
      if(.not.batch)write(6,123)   nrejge(1),nrejge(3),nrejge(5),
     +                               nrejge(7)
      if(.not.batch)write(6,124)   nrejge(2),nrejge(4),nrejge(6),
     +                               nrejge(8)
      write(IOUT,123) nrejge(1),nrejge(3),nrejge(5),nrejge(7)
      write(IOUT,124) nrejge(2),nrejge(4),nrejge(6),nrejge(8)
c
      if(mode.eq.0) then
      if(.not.batch)write(6,121)Nrefnd
      write(iout,121)Nrefnd
121     format(1x,' Total number of refinement nodals     : ',i6,/)
            else
      if(.not.batch)write(6,1175)Nrefnd
      write(iout,1175)Nrefnd
1175    format(1x,' Total number of usable reflections    : ',i6,/)
      END IF
      if(.not.batch)write(6,1211)
      write(iout,1211)
      if(.not.batch)write(6,122)
      write(IOUT,122)
      if(.not.batch)write(6,123)   nusing(1),nusing(3),nusing(5),
     +                             nusing(7)
      if(.not.batch)write(6,124)   nusing(2),nusing(4),nusing(6),
     +                             nusing(8)
      write(IOUT,123) nusing(1),nusing(3),nusing(5),nusing(7)
      write(IOUT,124) nusing(2),nusing(4),nusing(6),nusing(8)
c
1211    format(1x,'                     SINGLET             MULTIPLET')
122     format(1x,'            ','Measurable    Weak',
     1                       '    Measurable    Weak')
123      format(1x,' Separated   ',i6,   3x,1x,i6,6x,i6,4x,i6)
124      format(1x,' Overlapped  ',i6,   3x,1x,i6,6x,i6,4x,i6/)

C -----------------------WRITE STATS FINAL MODE PRE SORT
      if(.not.batch)write(6,1111)
1111      format(1x,'Sorting reflections.......')
      CALL SORTUP3(NREF,IX,IY,IREC)
        IS=1
        IE=NREF
        ID=1
        IF(MODE.EQ.0) GOTO 6
        IS=NREF
        IE=1
        ID=-1
6       DO 7 I=IS,IE,ID
        IRXY(1)=IREC(I)
        if(irec(i).lt.0) then
            if (PROPT) then
                  Nproit = Nproit + 1
            else
                       Nboxit = Nboxit + 1
            end if
       else
           Nproit = Nproit + 1
        end if
        IRXY(2)=IX(I)
        IRXY(3)=IY(I)
        WRITE(9) IRXY
7       CONTINUE
C
C -------------------------------WRITE STATS FINAL MODE POST SORT
      if(.not.batch)write(6,1100) nref
      write(iout,1100) nref
      if(mode.ne.0) then
      if(.not.batch)write(6,1101) Nproit
      if(.not.batch)write(6,1102) Nboxit
      write(iout,1101) Nproit
      write(iout,1102) Nboxit
      end if
1100      format(1x,' Number of reflections sorted   : ',i6)
1101      format(1x,' Number to be profile fitted    : ',I6)
1102      format(1x,' Number to be box summed        : ',I6)
      if(xtek) call newpg
        CLOSE (UNIT=9)
        RETURN
8       WRITE(IOUT,1003)
        if(.not.batch)write(6,1003)
1003    FORMAT('BUFFERS TOO SMALL FOR SORT')
        STOP
        END
C
c     routine for extracting x,y and flags from Laue
c     data file to store in core
c     input is from Version 2 ge1/ge2 files
c
c     Pella Machin       Sep 1986
c
c
      subroutine getgexy(inunit,nref,ifilm,mint,iprof, nodmax)
c
      integer*1  mhkl(4), nodhkl(4)
C
      INCLUDE 'comlaue.finc'
      INCLUDE 'comuab.finc'
C
      integer*2 intens(24), mflags2
      dimension ibuf(19),buf(19)
      DIMENSION ISTAT(8),nodref(8)
      DIMENSION NCHECK(7,2000)
c
      equivalence (ibuf(1), buf(1))
      equivalence (ibuf(3), intens(1))
      equivalence (ibuf(15),mhkl(1))
      equivalence (ibuf(17),nodhkl(1))
      common/ioo/iout
c
      integer*2 mask31
      data mask31/'001f'x/
c
c     byte array nodal contains nodal n value (5 bits),
c     wavelength ov flag as 1 bit (1=overlap, 0 else)
c     measurable flag as one bit (0=yes,1=no)
c     spatial overlap as one bit (1=overlap, 0 else)
c
      NBAD = 0
      NN = 0
      do 99 i=1,8
      nodref(i)=0
99      istat(i) = 0
      j=0
      write (6, 1010)
 1010 format(' Reading reflections . . .')
c
c     main loop over all reflections
c
      do 100 i=1,nref
      jrec=10+i
      read(inunit, rec=jrec, err=990) ibuf
      xyge(1,i) = buf(1)*100.
      xyge(2,i) = buf(2)*100.
c
      imeas = 0
c     check intensity threshold if not first film IN A RUN!
c         Sept 5th., 1992. decided not necessary - stops
c       lower films being refined if one above not integrated!
c
c     iprof is 0/1 for box/profiles
c      if (ifilm .gt .1 ) then
c         jj=iprof*12  + (ifilm-1)
c         ilast = intens(jj)
c         if(ilast.le.mint) imeas = 1
c      endif
c
      mflags2 = mhkl(1)
      iov = iand (mflags2, '0040'x)
      imult = iand (mflags2,mask31)
C      imult = iand (mflags2, '001F'x)
      nod = iand (mflags2, '0020'x)
c      IF(IMULT.EQ.1.AND.NODHKL(1).EQ.0) GOTO 75
c      IF(IMULT.GT.1.AND.NODHKL(1).GT.0) GOTO 75
c      NBAD = NBAD + 1
c      IF(NBAD.GT.75) GOTO 75
c      IF(NBAD.EQ.1) THEN
c          IF(XTEK) THEN
c            CALL NEWPG
c            if(.not.batch)write(6,71)
c            WRITE(IOUT,71)
c          END IF
c      END IF
c71    FORMAT(/1X,' FLAG ERROR: NO,Rec,X,Y,NOD,Nodhkl,Imult,Imeas,Iov')
c      NX = NINT(XYGE(1,I))
c      NY = NINT(XYGE(2,I))
c     if(.not.batch)write(6,72)NBAD,I,NX,NY,Nod,NODHKL(1),IMULT,IMEAS,IOV
c      WRITE(IOUT,72)NBAD,I,NX,NY,Nod,NODHKL(1),IMULT,IMEAS,IOV
c72      format(4x,9I8)
c75      continue
      IMULT = IMULT - 1
      if (imult.gt.0) imult = 1
      if(iov.ne.0) iov = 1
      if(nod. eq. 0. and. nodhkl(1). eq.0) go to 30
      if(nod. ne. 0. and. nodhkl(1). ne.0) go to 30
c
c     error with nodals
  900 if(.not.batch)write(6,902) i, nod, nodhkl(1)
      write(iout, 902) i, nod, nodhkl(1)
  902 format(' ** error in nodal flags for spot',i5,
     1 ' flag=',i4,' number=',i5)
      nod = 0
      nodhkl(1)=0
      go to 30
c
c     error on file input
  990 if(.not.batch)write(6,992)i, jrec
      write(iout,992) i, jrec
  992 format(' ** error on input in getgexy, spot=',i5,
     1 ' record=',i6)
      stop
c
c     nodal flags OK so store info (packed) and pointer
c      (after checking nodal value does not exceed max)
   30 if(nodhkl(1). gt. nodmax) then
        nod = 0
        nodhkl(1) = 0
      endif
      nodal(i)= nodhkl(1)*8 + imult*4 +imeas*2 +iov
      if(nod. ne. 0) then
         j=j+1
         if(j.le.2000) then
            nodpt(j)= i
c check flags for refinement nodals
      nbit = 4*imult + 2*imeas + iov + 1
      nodref(nbit) = nodref(nbit) + 1
         endif
      endif
c
      NN = NN + 1
      Nbit = 4*imult + 2*Imeas + iov +1
      ISTAT(nbit) = ISTAT(nbit) + 1
c
  100 continue
c
      IF(XTEK) CALL TKCLR
      if(j.gt.2000) then
         if(.not.batch)write(6,110) j
  110    format(' over 2000 nodals (',i5,'), rest lost')
         j=2000
      endif
      numnod = j
c
c      write statistics
c
      nsingle = 0
      nnodal = 0
      do 40 i=1,4
      nsingle = nsingle + istat(i)
      j = i+4
40      nnodal = nnodal + istat(j)
c
      if(.not.batch)write(6,111)
      write(IOUT,111)
111      format(/1x,'            Summary of reflection data',/)
      if(.not.batch)write(6,112)NN
      write(IOUT,112)NN
112      format(1x,' Total number of reflections           : ',i6,/)
c
      if(.not.batch)write(6,113) Nsingle
      if(.not.batch)write(6,114)
      if(.not.batch)write(6,115) istat(1),istat(3)
      if(.not.batch)write(6,116) istat(2),istat(4)
      write(IOUT,113) Nsingle
      write(IOUT,114)
      write(IOUT,115) istat(1),istat(3)
      write(IOUT,116) istat(2),istat(4)
113     format(1x,'                   ','  SINGLETS          : ',i6)
114     format(1x,'              ','Measurable      Weak')
115      format(1x,' Separated     ',i6,   3x,3x,i6)
116      format(1x,' Overlapped    ',i6,   3x,3x,i6,/)
c
      if(.not.batch)write(6,117)Nnodal
      if(.not.batch)write(6,118)
      if(.not.batch)write(6,119) istat(5),istat(7)
      if(.not.batch)write(6,120) istat(6),istat(8)
      write(IOUT,117)Nnodal
      write(IOUT,118)
      write(IOUT,119) istat(5),istat(7)
      write(IOUT,120) istat(6),istat(8)
117     format(1x,'                   ',' MULTIPLETS         : ',i6)
118     format(1x,'              ','Measurable      Weak')
119      format(1x,' Separated     ',i6,   3x,3x,i6)
120      format(1x,' Overlapped    ',i6,   3x,3x,i6,/)
c
      if(.not.batch)write(6,121)Numnod
      write(IOUT,121)Numnod
      if(.not.batch)write(6,1211)
      write(iout,1211)
      if(.not.batch)write(6,122)
      if(.not.batch)write(6,123) nodref(1),nodref(3),nodref(5),
     1  nodref(7)
      if(.not.batch)write(6,124) nodref(2),nodref(4),nodref(6),
     1  nodref(8)
      write(IOUT,122)
      write(IOUT,123) nodref(1),nodref(3),nodref(5),nodref(7)
      write(IOUT,124) nodref(2),nodref(4),nodref(6),nodref(8)
121     format(1x,'               ','REFINEMENT NODALS       : ',i6,/)
1211    format(1x,'                     SINGLET             MULTIPLET')
122     format(1x,'            ','Measurable    Weak',
     1                       '    Measurable    Weak')
123      format(1x,' Separated   ',i6,   3x,1x,i6,6x,i6,4x,i6)
124      format(1x,' Overlapped  ',i6,   3x,1x,i6,6x,i6,4x,i6/)
      IF(XTEK) CALL NEWPG
C -----------------------DEBUG - WRITE NODPT ARRAY
c      if(.not.batch)write(6,56)
c      WRITE(IOUT,56)
c56      FORMAT(/1X,  ' NODAL ARRAY: Rec,X,Y,Nodhkl,Imult,Imeas,Iov',/)
c      DO 60 J=1,NUMNOD
c      I = NODPT(J)
c      IM = NODAL(I)/8
c      nodali = nodal(i)
c      IMULT = IAND(NODALi, '04'X)
c      IOV   = IAND(NODALi, '01'X)
c      IMEAS = IAND(NODALi, '02'X)
c      NCHECK(1,J) = I
c      NCHECK(2,J) = XYGE(1,I)
c      NCHECK(3,J) = XYGE(2,I)
c      NCHECK(4,J) = IM
c      NCHECK(5,J) = IMULT
c      NCHECK(6,J) = IMEAS
c60      NCHECK(7,J) = IOV
c          if(.not.batch)write(6,61) ((ncheck(i,j),i=1,7),j=1,numnod)
c          write(IOUT,61) ((ncheck(i,j),i=1,7),j=1,numnod)
c61      format(1x,7i5,4x,7i5)
c          if(xtek)call newpg
      return
      end
C
C
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE GETHKL  ****      *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C
C       SUBROUTINE GETHKL
C
C  READS AND UNPACKS H,K,L,M,X,Y
C  FROM THE GENERATE FILE.
C
c##### laue mods
c     get from version 2 ge1/ge2 file
C=============================================================
C
C
C
        SUBROUTINE GETHKL(JREC,IH)
        INTEGER IH(6)
c##### laue mods throughout
c       integer JBUF(10)
      integer ibuf(19)
      real*4  rbuf(19)
      integer*1 mhkl(4)
      equivalence (ibuf(15),mhkl(1))
      equivalence (ibuf(1), rbuf(1))
c        READ(1,REC=JREC) JBUF
c        IH(1)=IAND(JBUF(1),'177400'O)/256
c        IF(IH(1).GE.128)IH(1)=-256+IH(1)
c        IH(2)=IAND(JBUF(2),'377'O)
c        IF(IH(2).GE.128)IH(2)=-256+IH(2)
c        IH(3)=IAND(JBUF(2),'177400'O)/256
c        IF(IH(3).GE.128)IH(3)=-256+IH(3)
c        IH(4)=IAND(JBUF(1),'360'O)/16
c        IH(5)=JBUF(3)
c        IH(6)=JBUF(4)
      read(1, rec=jrec+10) ibuf
      ih(1) = mhkl(2)
      ih(2) = mhkl(3)
      ih(3) = mhkl(4)
      ih(5) = rbuf(1)*100. + sign(.5, rbuf(1))
      ih(6) = rbuf(2)*100. + sign(.5, rbuf(2))
c     ???????
      ih(4) = 0
        RETURN
        END
C
C
C     SUBROUTINE TO SET UP THE HAT-MATRIX.
C
C
      SUBROUTINE HATHAT(nobin)
C
      INCLUDE 'comprf.finc'
      INCLUDE 'comhat.finc'
C
      REAL XTX(3,3),BUF1(3),BUF2(3),
     *     XXINV(5000,3),HATT(3,5000)
C
C-----------SET UP HAT MATRIX
C
      IF(PORB.EQ.1)THEN
        NN=HPOBS(nobin)
      ELSE
        NN=HNOBS
      END IF
      DO 100 I=1,NN
         DO 110 J=1,3
            IF(PORB.EQ.1)THEN
              HAT(I,J)=HATP(I,J,nobin)
            ELSE
              HAT(I,J)=HATB(I,J)
            END IF
            HATT(J,I)=HAT(I,J)
110      CONTINUE
100   CONTINUE
C
      DO 200 I=1,3
      DO 200 J=1,3
         XTX(I,J)=0.0
         DO 210 K=1,NN
            PROD=HATT(I,K)*HAT(K,J)
            XTX(I,J)=XTX(I,J)+PROD
210      CONTINUE
200   CONTINUE
C
C-----------------INVERT XTX
C
      CALL MINV(XTX,3,DET,BUF1,BUF2)
C
      DO 300 I=1,NN
         DO 310 J=1,3
            XXINV(I,J)=0.0
            DO 320 K=1,3
               PROD=HAT(I,K)*XTX(K,J)
               XXINV(I,J)=XXINV(I,J)+PROD
320         CONTINUE
310      CONTINUE
300   CONTINUE
C
C----------CALCULATE H(I)'S-----------
C
      DO 400 I=1,NN
         H(I)=0.0
         DO 410 K=1,3
            PROD=XXINV(I,K)*HATT(K,I)
            H(I)=H(I)+PROD
410      CONTINUE
400   CONTINUE
      RETURN
      END
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE HISBCK  ****      *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
      SUBROUTINE HISBCK(IXIN,IYIN,IBGIN,NBGIN,NCODE)
C
      INCLUDE 'comhis.finc'
      INCLUDE 'commcs.finc'
      INCLUDE 'comuab.finc'
      INCLUDE 'combck.finc'
      INCLUDE 'comfid.finc'
C
      common/iplate/image,mar,moldyn
      logical image,mar,moldyn
      LOGICAL    ERR
      INTEGER    INTENS, ISTRIP(4800), FILM_UNIT
      COMMON /IO_STREAMS/ IN_TERM, OUT_TERM, IN_DATA, OUT_DATA
      INTEGER             IN_TERM, OUT_TERM, IN_DATA, OUT_DATA
      character*60 sfile
      goto (100,200,300,100,400), ncode
c
c    store x,y coordinates of histogram bin
c    or retrieve bg for integration
c
100      if(.not.BGPLAN) then
      ixhbg = (((ixin-xmin) * 12)/(xmax-xmin)) + 1
      iyhbg = (((iyin-ymin) * 12)/(ymax-ymin)) + 1
        else
      ixhbg = (((ixin-xhmin) * 12)/(xhmax-xhmin)) + 1
      iyhbg = (((iyin-yhmin) * 12)/(yhmax-yhmin)) + 1
      end if
      if(ixhbg.lt.1) ixhbg = 1
      if(iyhbg.lt.1) iyhbg = 1
      if(ixhbg.gt.12) ixhbg = 12
      if(iyhbg.gt.12) iyhbg = 12
      if(ncode.eq.1) return
c
c   retrieve correct bg for this spot
c   note reduction for lower already achieved
c   below (ncode = 5)
c
      Ibgin = histbg(ixhbg,iyhbg)
c
c   set correct cuts for this spot
c
      cutu = fcut * float(Ibgin)
      cutl = -0.8 * fcut * float(Ibgin)
      return
c
c    add av bg for this spot into histogram
c
200      histbg(ixhbg,iyhbg) = histbg(ixhbg,iyhbg) +
     1                        nint(float(Ibgin)/float(nbgin))
      histn(ixhbg,iyhbg) = histn(ixhbg,iyhbg) + 1
      return
c
c    finally get average in each bin and print out
c    check for zeros and replace by av of neighbours
c
300     itemp = 0
         do ii = 1,12
      do jj = 1,12
            if(histn(ii,jj).eq.0) then
               if(ii.eq.1.and.jj.eq.1) then
                  itemp = 1
               else
               ist = max0(1,ii - 1)
               iend = min0(12,ii + 1)
               jst = max0(1,jj - 1)
               jend = min0(12,jj + 1)
                sub = 0
               hsum = 0
                 do ik = ist,iend
                  do jk = jst,jend
                  hsum = hsum + float(histbg(ik,jk))
                  sub = sub + float(histn(ik,jk))
                  end do
                 end do
               if(sub.eq.0) sub=1
               histn(ii,jj) = 1
               histbg(ii,jj) = nint(hsum/sub)
               end if
            end if
      end do
      end do
            if(itemp.eq.1) then
                sub = 0
               hsum = 0
                 do ik = 1,2
                  do jk = 1,2
                  if(.not.(ik.eq.1.and.jk.eq.1)) then
                  hsum = hsum + float(histbg(ik,jk))
                  sub = sub + float(histn(ik,jk))
                  end if
                  end do
                 end do
               if(sub.eq.0) sub=1
               histn(1,1) = 1
               histbg(1,1) = nint(hsum/sub)
            end if
c
c  save the histogram in lastbg
c
      do ii = 1,12
      do jj = 1,12
            if(histn(ii,jj).eq.0) histn(ii,jj) = 1
            histbg(ii,jj) = histbg(ii,jj) / histn(ii,jj)
            lastbg(ii,jj) = histbg(ii,jj)
      end do
      end do
c
c now make sure we store 'A' film x,y limits in common (his)
c as its these that determine the array position for lower
c films!
c
      xhmin = xmin
      xhmax = xmax
      yhmin = ymin
      yhmax = ymax
c
c
c    print out
c
999      if(.not.batch) write(6,1000)
      write(8,1000)
1000      format(/1x,' Background histogram ',/)
CCC      do ii = 1,12
      do ii = 12,1,-1
        if(.not.BGPLAN) then
CCC        if(.not.batch)write(6,1001) (histbg(jj,ii),jj=12,1,-1)
CCC                      write(8,1001) (histbg(jj,ii),jj=12,1,-1)
        if(.not.batch)write(6,1001) (histbg(jj,ii),jj=1,12)
                      write(8,1001) (histbg(jj,ii),jj=1,12)
        else
CCC        if(.not.batch)write(6,1001) (histbg1(jj,ii),jj=12,1,-1)
CCC                      write(8,1001) (histbg1(jj,ii),jj=12,1,-1)
        if(.not.batch)write(6,1001) (histbg1(jj,ii),jj=1,12)
                      write(8,1001) (histbg1(jj,ii),jj=1,12)
        end if
      end do
1001      format(1x,12i4)
c
      if(BGPLAN) then
      if(.not.batch) write(6,1004)
      write(8,1004)
1004      format(/1x,' Corrected Background histogram ',/)
CCC      do ii = 1,12
      do ii = 12,1,-1
CCC        if(.not.batch)write(6,1001) (histbg(jj,ii),jj=12,1,-1)
CCC                      write(8,1001) (histbg(jj,ii),jj=12,1,-1)
        if(.not.batch)write(6,1001) (histbg(jj,ii),jj=1,12)
                      write(8,1001) (histbg(jj,ii),jj=1,12)
      end do
      end if
      if(ltek) call newpg
      return
c
c ****** >>>>>>>>>>>>    entry for lower films
c
400      do i=1,12
      do j=1,12
      histbg(i,j) = 0
      histbg1(i,j) = 0
      histn(i,j) = 0
      histn1(i,j) = 0
      end do
      end do
      nbadbg = 0
      if(.not.batch) write(6,1002)
      write(8,1002)
1002      format(/1x,' . . . forming Background histogram ',/)
c
c  Base histogram area on same limits as A film
c
      FILM_UNIT = 2
      call flmopn(-2,sfile,iylen,1,OUT_TERM,ERR)
      IF (Image) THEN
         CALL IMPPOS(FILM_UNIT,xhmin,OUT_TERM,ERR)
      ELSE
         CALL FLMPOS(FILM_UNIT,xhmin,OUT_TERM,ERR)
      END IF
c
      do ixstrp = xhmin,xhmax

         IF (Image) THEN
            CALL IMPLRD(FILM_UNIT,ISTRIP,iylen,OUT_TERM,ERR)
         ELSE
            CALL FILMRD(FILM_UNIT,ISTRIP,iylen,OUT_TERM,ERR)
         END IF
c
      ixhbg = (((ixstrp-xhmin) * 12)/(xhmax-xhmin)) + 1
      if(ixhbg.lt.1) ixhbg = 1
      if(ixhbg.gt.12) ixhbg = 12
c
      do j= yhmin,yhmax
        iyhbg = (((j-yhmin) * 12)/(yhmax-yhmin)) + 1
        if(iyhbg.lt.1) iyhbg = 1
        if(iyhbg.gt.12) iyhbg = 12
c
c    add  od's for this stripe into histogram
c    if intensity is much bigger than expected (ie a spot?)
c    dont add in!
c
        intens = istrip(j)
        if(image) intens = intens/1000 + 1
          histbg1(ixhbg,iyhbg) = histbg1(ixhbg,iyhbg) + intens
          histn1(ixhbg,iyhbg) = histn1(ixhbg,iyhbg) + 1
        if(istrip(j).gt.(lastbg(ixhbg,iyhbg)+10)) then
          goto 450
        end if
        histbg(ixhbg,iyhbg) = histbg(ixhbg,iyhbg) + intens
        histn(ixhbg,iyhbg) = histn(ixhbg,iyhbg) + 1
450        continue
      end do
      end do
c
      close (unit=2)
c
c    finally get average in each bin and print out
c    check for zeros and replace by formula and
c    warn!
c
      do ii = 1,12
      do jj = 1,12
            if(histbg(ii,jj).eq.0) then
        histbg(ii,jj) = (float(lastbg(ii,jj) - 36))/1.42 +36
       histn(ii,jj) = 1
       nbadbg = 1
            end if
      end do
      end do
c
      do ii = 1,12
      do jj = 1,12
            histbg(ii,jj) = histbg(ii,jj) / histn(ii,jj)
            if(image) histbg(ii,jj) = histbg(ii,jj)*1000.
            histbg1(ii,jj) = histbg1(ii,jj) / histn1(ii,jj)
            if(image) histbg1(ii,jj) = histbg1(ii,jj)*1000.
            lastbg(ii,jj) = histbg(ii,jj)
      end do
      end do
c
      if(nbadbg.eq.1) then
        if(.not.batch) write(6,1003)
        write(8,1003)
1003      format(/1x,' **** WARNING - abnormal Background ****',/)
      end if
c
c now print out
c
      goto 999
c
c    entry for lower films. reduce values in histogram
c    according to:
c---------- for lower films, reduce bg constants in hist
c---------- to (c-36)*1/1.42 + 36 where 36 is reckoned to
c---------- be the base (14) + (Abg - 14)/4 (22 for ppb)
c---------- and the film factor is 1.42.
c **** for rhino, looks like 24 more appropriate
c
c400      do ii = 1,12
c      do jj = 1,12
c      histbg(ii,jj) = (float(histbg(ii,jj) - 24))/1.42 +24
c      end do
c      end do
c
c now print out
c
c      goto 999
      end
C
C ***BYTE PACKING, NON-STANDARD COMMONS ETC.***
C
C     Unpack bytes
C
      INTEGER FUNCTION IBYTE(IJBA,N)
C
C ***CONVEX VERSION***
C
      INTEGER*2 IJBA(*)
      CALL CCPGTB(IBYTE,IJBA,N)
      RETURN
      END
C
      SUBROUTINE INITT
c   initialize graphics and vdu
      CHARACTER*20 TRM
      ITTYP = 0
      CALL UGTENV('TERM',TRM)
      IF(TRM(1:5).EQ.'xterm') ITTYP = 1
      CALL QGINIT(0,6,ITTYP,LUNSTI(0),LUNSTO(0))
      RETURN
      END
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE INTEG  ****       *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C
        SUBROUTINE INTEG(RAS,IRAS,MASK,PQVAL,ASPOT,IDR,FULL)
        INTEGER RAS(1),IRAS(5),MASK(1),IDR,P,Q,HX,HY
        REAL ASPOT(5),PQVAL(6)
        LOGICAL FULL
        INTEGER SPOD,SQOD,TPOD,TQOD,SOD,TOD
        INTEGER SPD(-35:35),SQD(-35:35),TPD(-35:35),TQD(-35:35)
        HX=IRAS(1)/2
        HY=IRAS(2)/2
        DO 1 Q=-HY,HY
        SQD(Q)=0
1       TQD(Q)=0
        IJ=0
C
C-------------------------- start sum loop
C
        DO 4 P=-HX,HX
        SPD(P)=0
        TPD(P)=0
        DO 4 Q=-HY,HY
        IJ=IJ+1
        IOD=RAS(IJ)
        IF(MASK(IJ)) 3,4,2
C
C-------------------------  SUMS FOR PEAK
C
2       TPD(P)=TPD(P)+IOD
        IF(.NOT.FULL) GOTO 4
        TQD(Q)=TQD(Q)+IOD
        GOTO 4
C
C----------------------  SUMS FOR BACKGROUND
C
3       SPD(P)=SPD(P)+IOD
        SQD(Q)=SQD(Q)+IOD
4       CONTINUE
C
C--------------------------- end sum loop
C
        SOD=(SPD(0)+SPD(1)+SPD(-1))
        TOD=(TPD(0)+TPD(1)+TPD(-1))
        SPOD=(SPD(1)-SPD(-1))
        DO 5 P=2,HX
        SOD=SOD+(SPD(P)+SPD(-P))
        TOD=TOD+(TPD(P)+TPD(-P))
5       SPOD=SPOD+P*(SPD(P)-SPD(-P))
        SQOD=(SQD(1)-SQD(-1))
        DO 6 Q=2,HY
6       SQOD=SQOD+Q*(SQD(Q)-SQD(-Q))
        IF(.NOT.FULL) GOTO 9
        TPOD=(TPD(1)-TPD(-1))
        DO 7 P=2,HX
7       TPOD=TPOD+P*(TPD(P)-TPD(-P))
        IF(.NOT.FULL) GOTO 9
        TQOD=(TQD(1)-TQD(-1))
        DO 8 Q=2,HY
8       TQOD=TQOD+Q*(TQD(Q)-TQD(-Q))
C
C-------------------------  BACKGROUND CONSTANTS
C
9       A=SPOD/PQVAL(2)
        B=SQOD/PQVAL(4)
        C=SOD/PQVAL(6)
        ASPOT(2)=C*PQVAL(5)
        ASPOT(1)=TOD-ASPOT(2)
C
C---------- CALCULATE RMS. BACKGROUND RESIDUAL AFTER PLANE FITTING
C
        SD=0.0
        IJ=0
        DO 10 P=-HX,HX
        APC=A*P+C
        DO 10 Q=-HY,HY
        IJ=IJ+1
        IF(MASK(IJ).GE.0) GOTO 10
        DIFF=RAS(IJ)-(APC+B*Q)
        SD=SD+DIFF*DIFF
10      CONTINUE
        ASPOT(3)=SQRT(SD/PQVAL(6))
        IF(.NOT.FULL) RETURN
C
C-------------------------- CATCH ZERO SPOTS
C
        IF(ASPOT(1).NE.0.0)GO TO 11
        ASPOT(4)=0.
        ASPOT(5)=0.
        RETURN
11      CONTINUE
        ASPOT(4)=(TPOD-A*PQVAL(1))/ASPOT(1)*IDR
        ASPOT(5)=(TQOD-B*PQVAL(3))/ASPOT(1)
        RETURN
        END
C
      SUBROUTINE IP_BEXPAN(A,IA,NXY)
      INTEGER*2 A(*)
      DIMENSION IA(*)
      CALL CCPI2I(IA,A,NXY,.FALSE.,.FALSE.)
      RETURN
      END
C
      SUBROUTINE IP_BPACK(N,IB,IV)
      INTEGER*2 IB(*),JV
      JV=IV
      IB(N)=JV
      RETURN
      END
C
      SUBROUTINE IP_BSWAP(K1,K2,IP)
      COMMON/PEL/IOD,IBUF(5120),NEXTR
      COMMON/CPEL/IARR(14000)
      INTEGER*2 IOD,IBUF,IARR
      DO 110 K=K1,K2
       IARR(IP)=IBUF(K)
       IP=IP+1
110   CONTINUE
      RETURN
      END
C
      SUBROUTINE IP_CBPACK(I)
      COMMON/PEL/IOD,IBUF(5120),NEXTR
      COMMON/CPEL/IARR(14000)
      INTEGER*2 IOD,IBUF,IARR
      IARR(I)=IOD
      RETURN
      END
C
      SUBROUTINE IP_CBYTE(I)
      COMMON/PEL/IOD,IBUF(5120),NEXTR
      INTEGER*2 IOD,IBUF
      IOD=IBUF(I)
      RETURN
      END
C
      INTEGER FUNCTION IP_IBYTE(IJBA,N)
      INTEGER*2 IJBA(*)
      CALL CCPGI2(IP_IBYTE,IJBA,N)
      RETURN
      END
C
C
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE MEAS    ****      *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C
C                PROGRAMME MEAS.
C                ----------------
C
C READS STRIPES FROM DISK FILE 'FILM' INTO ARRAY BA.
C SELECTS DATA FOR INDIVIDUAL SPOTS INTO ARRAY BB.
C AS SPOTS ARE COMPLETED, THEY ARE WRITTEN ONTO DISK FILE MOSCO.SP
C EACH RECORD CONTAINS THE DATA FOR ONE SPOT.
C IT STARTS WITH 3 WORDS CONTAINING RECORD NO.,X,Y AND THEN
C A WORD CONTAINING 2*HFWX+1 IN ONE BYTE AND 2*HFWY+1 IN THE OTHER.
C THE OPTICAL DENSITIES THEN FOLLOW,PACKED INTO BYTES.
C
C==============================================================
C
C
C
        SUBROUTINE MEAS
c
cRCD ...Image Plate logical
        common/iplate/image,mar,moldyn
        logical image,mar,moldyn
c
      INCLUDE 'comscn.finc'
      INCLUDE 'comlaue.finc'
C
C----------- Include ORI, MCS, RFS, REP for old SEND file
C
      INCLUDE 'comuab.finc'
      INCLUDE 'comori.finc'
      INCLUDE 'commcs.finc'
      INCLUDE 'comrfs.finc'
      INCLUDE 'comrep.finc'
      INCLUDE 'comras.finc'
C
        COMMON/IOO/IOUT
        COMMON/SW/ISWTCH(10),oneref,noref,tworef
        COMMON/PEL/KJBA,IJBA(5120),nextr
        INTEGER*2 IJBA,kjba
        COMMON/CPEL/BB(14000)
        INTEGER*2 BB
c
c*****************************************
c NEW COMMON TO HANG ON TO RECORD NUMBER > 32767
c*****************************************
c      ADDED TO COMLAUE
C      common/lauebb/NBBREC(300000 OR 100000)
      INTEGER NBBTEM(14000)
C
        LOGICAL oneref,noref,tworef
        INTEGER TWORD,NSP,PNTR,ACTREF
        INTEGER XC,YC,RC,WTX,PNTB(120),PNTC(120),PNTD
        INTEGER HFWX,HFWY,BUF(3),NSIZ,FWX
        INTEGER X(120),Y(120),REC(120),IWORD(120)
        INTEGER HW(240),WX(240)
        LOGICAL FULL,REND,YFULL
        EQUIVALENCE (BUF(1),RC),(BUF(2),XC),(BUF(3),YC)
        DATA TWORD/14000/,NSP/120/
C
        if(.not.batch)write(6,1000)
        WRITE(IOUT,1000)
1000    FORMAT(' COLLECTING DENSITY DATA FOR EACH SPOT ')
C
C---------------- OPEN INPUT AND OUTPUT FILES
C
        ifail=0
        call ccpdpn(-4,'SORTTEMP','OLD','U',0,ifail)
        ifail=0
        call ccpdpn(-26,'MOSTEMP','UNKNOWN','U',0,ifail)
C
C--------------- GET INFORMATION ON 200 SPOTS FROM SORT FILE
C
        IHTOFD=CTOFD*XTOFRA*FACT*0.5
        NECX=XCEN*FACT+0.5
        NECY=YCEN*FACT+0.5
        NR=0
      NDONE = 1
        IWORD(NSP)=0
        PNTB(NSP)=1
        DO 1 M=1,NSP
        ASSIGN 1 TO IRETN
        GOTO 16
1       CONTINUE
C
C---------  PNTB,PNTC
C           THESE ARE MARKERS IN BB TO SHOW WHERE IT IS FILLED
C---------- INDF,INDL:-
C           SHOW WHICH REFLECTIONS ARE ACTIVE IN SORTED FILE
C
        INDF=1
        LINDF=INDF
        INDL=1
        MINDF=1
        ACTREF=0
        YFULL=.FALSE.
        FULL=.FALSE.
        REND=.FALSE.
C
C------------ START SCAN FOR THE FIRST SPOT OR SPOT AFTER A GAP
C
2       WTX=X(MINDF)+HW(2*MINDF-1)
        IBLK=WTX
C
C------------------- INCLUDE THE SPOT IN THIS SCAN
C
3       IER=-1
        CALL RDBLK(2,IBLK,IJBA,NWORD,IER)
        IF(IER.EQ.1) GOTO 4
        WRITE(IOUT,1001) IER
        if(.not.batch)write(6,1001)IER
1001    FORMAT(/'ERROR ON DISK BLOCK READ : IER=',I4/)
        STOP
4       IF(INDL.EQ.NREF) GOTO 7
        IF(FULL) GOTO 7
        MINDL=MOD(INDL,NSP) + 1
        IF(WTX.GT.X(MINDL)+HW(2*MINDL-1)) GOTO 7
        IF (INDL-INDF.GE.NSP-1) GO TO 5
        IF (MINDL.EQ.MINDF) GO TO 6
        PNTD=PNTB(MINDL)-PNTB(MINDF)
        IF(PNTD.GT.0.OR.PNTD.LT.-IWORD(MINDL)) GOTO 6
5       YFULL=.TRUE.
        FULL=.TRUE.
        GOTO 4
6       INDL=INDL+1
        GOTO 4
C
C------------ GET THE INFORMATION FOR THIS SCAN AND START A NEW ONE
C
7       CONTINUE
        IBLK=IBLK-1
C
C---------------- WRITE SPOTS COLLECTED IN THIS STRIPE TO BB
C
        DO 9 J=INDF,INDL
        MJ=MOD(J-1,NSP)+1
        YC=Y(MJ)
        KMN=YC-HW(2*MJ)
        KMX=YC+HW(2*MJ)
        PNTR=PNTC(MJ)
C
C--------- PNTR IS USED AS WORD POINTER THEN CHANGED TO BYTE POINTER
C
        IF(PNTR.NE.PNTB(MJ)) GOTO 8
c
c*************************************************************
c
c  PUT RECORD NUMBER INTO  NBBTEM
c
c*************************************************************
c
      NBBTEM(PNTR) = REC(MJ)
      IF(IABS(REC(MJ)).LE.32000) THEN
              BB(PNTR)=REC(MJ)
           ELSE
            BB(PNTR)= 0
      END IF
        BB(PNTR+1)=X(MJ)
        BB(PNTR+2)=YC
C
C----------------------- MULTIPLY BOTH BYTES BY 2 AFTER ADDING 1
C
        IFULLX=2*HW(2*MJ-1)+1
        IFULLY=2*HW(2*MJ)+1
C@@@@        BB(PNTR+3)=256*IFULLY+IFULLX
      CALL BPACK(7,BB(PNTR),IFULLX)
      CALL BPACK(8,BB(PNTR),IFULLY)
        if(.not.image)then
        PNTR=2*PNTR+7
        else
        pntr=pntr+4
        endif
8       CONTINUE
        if(.not.image)then
        CALL BSWAP(KMN,KMX,PNTR)
        else
        call ip_bswap(kmn,kmx,pntr)
        endif
        PNTC(MJ)=PNTR
        WX(MJ)=WX(MJ)-1
9       CONTINUE
        IF(INDL-INDF+1.GT.ACTREF) ACTREF=INDL-INDF +1
        WTX=WTX-1
C
C--------------------- SEE IF SPOT IS FINISHED
C
10      IF(WX(MINDF).EQ.0) GOTO 14
C
C-----------  READ NEW SPOTS FOR THOSE COMPLETED ON THIS STRIPE
C
11      IF(LINDF.EQ.INDF) GOTO 13
        M=MOD(LINDF-1,NSP)+1
        IF(REND) GOTO 12
        ASSIGN 12 TO IRETN
        GOTO 16
12      LINDF=LINDF+1
        GOTO 11
13      IF(INDF.LE.INDL) GOTO 3
        FULL=.FALSE.
        GOTO 2
C
C-------------------- WRITE OUT COMPLETED SPOTS AND READ IN NEW ONES
C                              hopefully one at once!
14      LEN=IWORD(MINDF)-4
        IST=PNTB(MINDF)
        CALL PWRITE(nbbtem(ist),BB(IST),BB(IST+4),LEN,26)
C
C**************************************************************
C
C MAKE SURE WEVE GOT REC. NO IN NBBREC
C
C**************************************************************
      NBBREC(NDONE) = NBBTEM(IST)
      IF(IABS(NBBREC(NDONE)).GT.32000) GOTO 141
      IF(NBBREC(NDONE).EQ.BB(IST)) THEN
            GOTO 141
          ELSE
              if(.not.batch)write(6,142) NDONE,NBBREC(NDONE),BB(IST)
142              FORMAT(1X,'ERROR. NUM, NBBREC, BB(IST) :',3I8)
            STOP
      END IF
141      NDONE = NDONE + 1
        IF(ISWTCH(1).EQ.0) GOTO 15
        NXX=2*HW(2*MINDF-1)+1
        NYY=2*HW(2*MINDF)+1
        CALL ODPLOT(BB(PNTB(MINDF)+4),NXX,NYY,-1)
15      INDF=INDF+1
        MINDF=MOD(INDF-1,NSP)+1
        IF(INDF.LE.NREF) GOTO 10
        IF(YFULL) WRITE(IOUT,1002)
        IF(YFULL.and..not.batch)write(6,1002)
1002    FORMAT('  BUFFER WAS FULL')
        WRITE(IOUT,1003) ACTREF
        if(.not.batch)write(6,1003)ACTREF
1003    FORMAT('  MAX. NO. OF ACTIVE REFLEXIONS=',I6)
        WRITE(IOUT,1004) NREF
        if(.not.batch)write(6,1004)NREF
1004    FORMAT(I6,' REFLEXIONS RECORDED IN MEAS')
        CLOSE (UNIT=2)
        CLOSE(UNIT=26)
        RETURN
16      READ(4,END=17) BUF
        NR=NR+1
        X(M)=XC
        Y(M)=YC
        REC(M)=RC
        FX=IABS(XC-NECX)
        IFX=FX
        IF(VEE) FX=IABS(IFX-IHTOFD)
        FY=IABS(YC-NECY)
        N1=FX*(FX*VARAS(2)+VARAS(1))+0.5
        N2=FY*(FY*VARAS(3)+VARAS(1))+0.5
        HFWX=(NXS+N1)/2
        HFWY=(NYS+N2)/2
        FWX=2*HFWX+1
        HW(2*M-1)=HFWX
        HW(2*M)=HFWY
        WX(M)=FWX
        if(.not.image)then
        IWORD(M)=(FWX*(2*HFWY+1)+1)/2+4
        else
        iword(m)=fwx*(2*hfwy+1)+4
        endif
        M1=M-1
        IF(M.EQ.1) M1=NSP
        PNTB(M)=PNTB(M1)+IWORD(M1)
        IF(PNTB(M)+IWORD(M).GT.TWORD) PNTB(M)=1
        PNTC(M)=PNTB(M)
        GOTO IRETN
17      REND=.TRUE.
        IF(NR.EQ.NREF) GOTO IRETN
        WRITE(IOUT,1005) NREF,NR
        if(.not.batch)write(6,1005)NREF,NR
1005    FORMAT(/'SHOULD BE',I6,'RECORDS'/'EOF AFTER',I6,'RECORDS'/)
        STOP
        END
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE MINV   ****       *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C       SUBROUTINE MINV SEE SSP PAGE 118
C
C=============================================================
C
C
C
        SUBROUTINE MINV(A,N,D,RL,RM)
        REAL A(*),RL(*),RM(*),D
        INTEGER L(N),M(N)
        D=1.0
        NK=-N
        DO 18 K=1,N
        NK=NK+N
        L(K)=K
        M(K)=K
        KK=NK+K
        BIGA=A(KK)
        DO 2 J=K,N
        IZ=N*(J-1)
        DO 2 I=K,N
        IJ=IZ+I
        IF(ABS(BIGA)-ABS(A(IJ))) 1,2,2
1       BIGA=A(IJ)
        L(K)=I
        M(K)=J
2       CONTINUE
C
C  ------------------------  INTERCHANGE ROWS
C
        J=L(K)
        IF(J-K) 5,5,3
3       KI=K-N
        DO 4 I=1,N
        KI=KI+N
        HOLD=-A(KI)
        JI=KI-K+J
        A(KI)=A(JI)
4       A(JI)=HOLD
C
C  ------------------------- INTERCHANGE COLUMNS
C
5       I=M(K)
        IF(I-K)8,8,6
6       JP=N*(I-1)
        DO 7 J=1,N
        JK=NK+J
        JI=JP+J
        HOLD=-A(JK)
        A(JK)=A(JI)
7       A(JI)=HOLD
C
C  ----------------------  DIVIDE COLUMN BY MINUS PIVOT
C
8       IF(BIGA)10,9,10
9       D=0.0
        RETURN
10      DO 12 I=1,N
        IF(I-K) 11,12,11
11      IK=NK+I
        A(IK)=A(IK)/(-BIGA)
12      CONTINUE
C
C --------------------------  REDUCE MATRIX
C
        DO 15 I=1,N
        IK=NK+I
        HOLD=A(IK)
        IJ=I-N
        DO 15 J=1,N
        IJ=IJ+N
        IF(I-K)13,15,13
13      IF(J-K) 14,15,14
14      KJ=IJ-I+K
        A(IJ)=HOLD*A(KJ)+A(IJ)
15      CONTINUE
C
C ----------------------------  DIVIDE ROW BY PIVOT
C
        KJ=K-N
        DO 17 J=1,N
        KJ=KJ+N
        IF(J-K) 16,17,16
16      A(KJ)=A(KJ)/BIGA
17      CONTINUE
C
C ----------------------- PRODUCT OF PIVOTS
C
C       D=D*BIGA
C
C --------------------REPLACE PIVOT BY RECIPROCAL
C
        A(KK)=1.0/BIGA
18      CONTINUE
C
C ------------------FINAL ROW AND COLUMN INTERCHANGE
C
        K=N
19      K=(K-1)
        IF(K) 26,26,20
20      I=L(K)
        IF(I-K) 23,23,21
21      JQ=N*(K-1)
        JR=N*(I-1)
        DO 22 J=1,N
        JK=JQ+J
        HOLD=A(JK)
        JI=JR+J
        A(JK)=-A(JI)
22      A(JI)=HOLD
23      J=M(K)
        IF(J-K) 19,19,24
24      KI=K-N
        DO 25 I=1,N
        KI=KI+N
        HOLD=A(KI)
        JI=KI-K+J
        A(KI)=-A(JI)
25      A(JI)=HOLD
        GOTO 19
26      RETURN
        END
C      *****************************************************************
C       *                                                *
C       *  This version of INTLAUE prepared for export 12/10/92.           *
C      *  The history is somewhat chequered but runs something         *
C      *  like this:                                          *
C      *            The basic framework still resembles the            *
C      * original Alan Wonacott MOSFLM which was implemented on the      *
C      * Daresbury VAX by Pella Machin and Trevor Greenhough in 1982   *
C      * (ref DL/CSE/TM23). This suite of programs was exported to UAB *
C      * (Alabama) by TJG in 1984 where a major rewrite to include       *
C      * profile fitting was performed. (J.A.C., 19,400-409, 1986).      *
C      * A further major rewrite for Laue films, begun by PAM and TJG  *
C      * in 1986, has been completed by TJG and Annette Shrive in 1988.*
C      * Further modifications for new BG determination and Overlap    *
C      * deconvolution and radial masks by AKS and TJG are included in *
C      * this version, as are image plate modifications by  TJG, AKS   *
C       * and Richard Denny (Keele). To the best of our knowledge      *
C      * accreditation and ownership now reside with T.J. Greenhough   *
C      * this program being maintained and distributed from Keele       *
C      * University to both individuals and Daresbury to then form a       *
C      * part of the Daresbury Laboratory Laue Software Suite.            *
C      *                                                *
C      *****************************************************************
C
c
c
        PROGRAM MOSUAB
C
C       COMMON VARIABLES
c
cRCD ...Image Plate logical
        common/iplate/image,mar,moldyn
        logical image,mar,moldyn
C
      INCLUDE 'comsnd.finc'
      INCLUDE 'comori.finc'
      INCLUDE 'comras.finc'
      INCLUDE 'comscn.finc'
      INCLUDE 'comfid.finc'
      INCLUDE 'comrfs.finc'
      INCLUDE 'commcs.finc'
      INCLUDE 'comuab.finc'
      INCLUDE 'comhat.finc'
      INCLUDE 'comtem.finc'
      INCLUDE 'commask.finc'
      INCLUDE 'commaskl.finc'
C
        COMMON /EXTRAS/ JUMPAX
        COMMON/THRESH/IJNK1,IMTHR,IMREF
        COMMON/IOO/IOUT
        common/PEL/IBA,IDUM(5120),NEXTR
      integer*2 idum,iba
        COMMON/SW/ISWTCH(10),oneref,noref,tworef
        Character*4 stat,toggle(2)
        Data toggle(1)/'OFF '/,toggle(2)/' ON '/
      character*6 aword
      integer intdm(9)
      real realdm(6)
        DATA NWORD/2560/,ELIMIT/8./
        LOGICAL REPEAT,MORE,Batch_Mode,oneref,noref,tworef
      EXTERNAL BREPRT
c##### laue mods - insert extra common for open_ge and initialise
      common/io_streams/ in_term, out_term, in_data, out_data
      integer*4          in_term, out_term, in_data, out_data
      dimension x_c(6), y_c(6), w_c(6), y_scal(6), x_to_fra(6)
      real*4 twists(6), tilts(6), bulges(6)
      common/fgname/ge_file,iendpos
      character*40 ge_file
        character*1 junk
c
      ge_file = 'interactive'
      in_term = 5
      out_term = 6
      in_data = 5
      out_data = 6
c
C --- CONFUSING. BGPLAN IS ACTUALLY TO TELL US IF WE ARE
C---  DOING B AND/OR C FILMS IN SAME RUN AS A
C
C---------------------- initialize Timer
C
        iflag=0
        call ccptim(iflag,cpu,elaps)
        IBA=0
C
C       open unit 11 for reading. **Use unit 5 now instead**
C
c         open(unit=11,file='SYS$INPUT:',status='UNKNOWN')
C
c##### laue mods Temp mod while debugging
c
c        OPEN(UNIT=8,file='intlaue.log',STATUS='NEW',
c    1          DISP='PRINT/DELETE')
        ifail=0
        call ccpdpn(-8,'LOGFILE','NEW','F',0,ifail)
c         OPEN(UNIT=8,file='intlaue.log',STATUS='NEW')
        IOUT=8
C
C
C-----------------------------FIND OUT IF BATCH
C
        Iswtch(1) = 0
        Iswtch(2) = 1
        Iswtch(3) = 0
        Iswtch(4) = 0
        Iswtch(5) = 0
        Iswtch(9) = 0
        Ltek = .False.
        Xtek = .False.
        Batch = Batch_Mode()
        If (Batch) Isw2 = Iswtch(2)
C
      mar = .false.
      moldyn = .false.
      image = .false.
        CORRECT=.FALSE.
      cent_in = .false.
      streak =.false.
      imaxod = 230
C
C------ open unit 10 for interactive mode ***Use unit 6 now***
C
c         If(Batch) open(unit=10,file='NL:',status='unknown')
c         If(.Not.Batch) open(unit=10,file='SYS$OUTPUT:',status='unknown')
        If (Batch) goto 10
C
C---------------------------CHECK ON TERMINAL STATUS
C
C=====================================================
C       CALL TKCLR
C       CALL MOVTX(0,0)  ****** FOR DL ****
C       CALL TKCLR
        LTEK = .TRUE.
        XTEK = .TRUE.
C=====================================================
C       if(.not.batch)write(6,1010)
C       WRITE(IOUT,1010)
C1010    FORMAT(/1X,' IS THE TEKTRONIX AVAILABLE? (Y/N)',$)
C       read(5,1020) JUNK
1020    FORMAT(A1)
C        WRITE(IOUT,1030)JUNK
1030    FORMAT(1X,A1)
C        CALL NOYES(JUNK,ISTEK)
C       LTEK=.FALSE.
C       IF(ISTEK.EQ.1) LTEK=.TRUE.
C       if(.not.batch)write(6,1040)
C       WRITE(IOUT,1040)
C1040   FORMAT(/1X,' IS THIS A TEKTRONIX TERMINAL? (Y/N)',$)
C       read(5,1020) JUNK
C       WRITE(IOUT,1030)JUNK
C       CALL NOYES(JUNK,ISTEK)
C       XTEK=.FALSE.
C       IF(ISTEK.EQ.1) XTEK=.TRUE.
c          IF(LTEK) OPEN (UNIT=12,FILE='SYS$SCOPE:',STATUS='UNKNOWN',
c      1      CARRIAGECONTROL='NONE',SHARED)
        call initt
      call qgentr
C
C---------------------- NEXT 2 LINES NOT FOR DL
C
        IF(LTEK) CALL MOVTX(0,0)
        IF(LTEK) CALL TKCLR
C
C------------------------------ WRITE HEADER
C
10      if(.not.batch)write(6,1050)
        WRITE(iout,1050)
1050    FORMAT( /,15X,'  INTLAUE: Laue Image Integration Software',
     1          /,15X,'  ----------------------------------------',
     2          /,15X,'      T.J. Greenhough and A.K. Shrive',
     3          /,15X,'University of Keele and Daresbury Laboratory',
     4          /,15X,'(with thanks to P.A. Machin & A.J. Wonacott)',
     5          /,15x,'      Latest Update March 2nd, 1994.',/)
       CALL CPUTIME(0)
C
C------------------  GET GENERATE FILENAME AND OPEN IT
C
c##### laue mods - open ge file
c
20       call open_ge(1, 13, ge_file, iendpos, iendrec)
c
C
C-------------------------------- SWITCH OPTIONS
C---------------------------- CHECK FOR AV SPOT PROFILE
C
c------------------------ remove Iprp from end for Laues
c
        If(Batch) then
            read(5,*) Iswtch(2),Iswtch(4),Iswtch(5),Ipro,Iprw,
     1                       Ibth,Icorr,Istreak,Ibinv,IPorF,iouter
            if(IPorF.ge.1) image = .true.
            dstor = 1
            if(IPorF.eq.1) then
                  mar = .true.
                  dstor =2 
            endif
            if(IPorF.eq.2) then
                  moldyn = .true.
                  dstor = 3
                  IPorF = 1
            endif
      else
            write(6,907)
              write(iout,907)
907     format(' IMAGE PLATE DATA (Y/N) [N]:',$)
              read(5,1020) junk
              write(iout,1030)junk
            IPorF = 0
              if(junk.eq.'Y'.or.junk.eq.'y') then
                  image=.true.
                  IPorF = 1
                  Imaxod = 65000
            endif
            write(6,9071)
              write(iout,9071)
9071    format(' MAR (Spiral) IMAGE PLATE DATA (Y/N) [N]:',$)
              read(5,1020) junk
              write(iout,1030)junk
            dstor =3 
            moldyn = .true.
              if(junk.eq.'Y'.or.junk.eq.'y') then
                  mar=.true.
                  moldyn = .false.
                  dstor =2 
            endif
            dstor = 1
        endif
c
        Isw2 = Iswtch(2)
        If(Batch) Iswtch(5) = 1 - Iswtch(5)
        If(Batch) goto 110
        ISWTCH(1)=0
        ISWTCH(9)=0
c
        if(.not.batch)write(6,1090)
        WRITE(IOUT,1090)
1090    FORMAT(' INNER REF AND AVERAGE SPOT PROFILE AFTER IMAGE',
     1              1X,'CENTERING?(Y/N) [No]',$)
        read(5,1020)JUNK
        WRITE(IOUT,1030)JUNK
        iswtch(2) = 0
        CALL NOYES(JUNK,ISWTCH(2))
      ISW2=ISWTCH(2)
c
      if(.not.moldyn) then
        if(.not.batch)write(6,1091)
        WRITE(IOUT,1091)
1091    FORMAT(' OUTER REFINEMENT (Y/N) [Yes]',$)
      iouter = 1
      else
        if(.not.batch)write(6,1092)
        WRITE(IOUT,1092)
1092    FORMAT(' OUTER REFINEMENT (Y/N) [No]',$)
      iouter = 0
      endif
        read(5,1020)JUNK
        WRITE(IOUT,1030)JUNK
        CALL NOYES(JUNK,iouter)
C
C-------------------------CHECK FOR SPOT PROFILES IN PROCESS
C
        if(.not.batch)write(6,1100)
        WRITE(IOUT,1100)
1100    FORMAT(' CALC AND DISPLAY AV SPOT PROFILES ',
     1           'IN PROCESS?(Y/N) [No]',$)
        read(5,1020)JUNK
        WRITE(IOUT,1030)JUNK
        iswtch(4) = 0
        CALL NOYES(JUNK,ISWTCH(4))
C
C------------------------- CHECK ON B AND C FILM STRATEGY
C
      if(.not.image) then
        if(.not.batch)write(6,1110)
        WRITE(IOUT,1110)
1110    FORMAT(' FORCE LOWER FILMS TO BE MEASURED ',
     1           'AS A ?(Y/N) [Yes]',$)
        read(5,1020)JUNK
        WRITE(IOUT,1030)JUNK
        iswtch(5) = 1
        CALL NOYES(JUNK,ISWTCH(5))
        ISWTCH(5)=1-ISWTCH(5)
      endif
C
C-------------------RETURN HERE FOR FURTHER FILMS NOT B,C
C
  100   CONTINUE
C
C---------------------------------- NOW GET PROFILE STRATEGY
C
        PROFIL = .FALSE.
        If(Batch) goto 110
        if(.not.batch)write(6,1120)
        WRITE(IOUT,1120)
1120    FORMAT(' DO YOU REQUIRE PROFILE FITTING?(Y/N) [Yes]',$)
        read(5,1020)JUNK
        WRITE(IOUT,1030)JUNK
        ipro = 1
        CALL NOYES(JUNK,IPRO)
110     IF(IPRO.NE.0) PROFIL=.TRUE.
        IBOTH = .FALSE.
        IF(.NOT.PROFIL) GOTO 140
        If(Batch) goto 113
        if(.not.batch)write(6,112)
        WRITE(IOUT,112)
112     FORMAT(' PROFILES AND BOX INTEGRATION?(Y/N) [No]',$)
        read(5,1020)JUNK
        WRITE(IOUT,1030)JUNK
        ibth = 0
        CALL NOYES(JUNK,Ibth)
113     IF(Ibth.ne.0) IBOTH=.TRUE.
        IF(BATCH) GOTO 1131
        if(.not.batch)write(6,1121)
        WRITE(IOUT,1121)
1121    FORMAT(' DO YOU WANT TO CORRECT FOR OVERLAPS? (Y/N) [No]',$)
        read(5,1020)JUNK
        WRITE(IOUT,1030)JUNK
        ICORR=0
        CALL NOYES(JUNK,ICORR)
1131    IF(ICORR.EQ.1)THEN
          CORRECT=.TRUE.
          HOVLAP=1.1
      hcut = 5.0
          IF(.NOT.BATCH)THEN
          if(.not.batch)write(6,1122)
          WRITE(IOUT,1122)
          read(5,*)HOVLAP,hcut
          IF(HOVLAP.EQ.0.0)HOVLAP=1.1
      if(hcut.eq.0.0) hcut = 5.0
          WRITE(IOUT,1126)HOVLAP,hcut
          END IF
        END IF
1122    FORMAT(' ENTER REJECTION LEVELS FOR OVERLAPS [1.1,5.0]:',$)
1123    FORMAT(2F8.4)
1126    FORMAT(1H+,2X,2F8.4,/)
C
C---------------------------------  GET BG CONSTANTS
C
        CUT = 0.2
        NUMBIN = 17
        inrad = 0
        inrad2 = 0
        If(Batch) THEN
         read(5,*) numbin, cut, hovlap, hcut, npwk1,
     1                 IMAXOD, INRAD, INRAD2
1125     format(I4,3F8.4,4I4)
         goto 115
        END IF
        if(.not.batch)write(6,1130)cut,numbin
        WRITE(IOUT,1130)cut,numbin
1130    FORMAT(' ENTER BG FRAC ,NUMBER OF BINS (5,9,17):[',
     1            f7.5,',',i3,']:',$)
        read(5,*)CUT,nUMBIN
1135    FORMAT(F8.4,I4)
        IF(CUT.EQ.0) CUT = 0.2
        IF(NUMBIN.EQ.0) NUMBIN = 17
        WRITE(IOUT,1140)CUT,NUMBIN
1140    FORMAT(1H+,2x,F8.4,I5)
C
C----------------- FIND BIN RADII BASED ON RESOLUTION
C
115     NOUTER = (NUMBIN-1)
        NINOUT = (NUMBIN/16) +1
        NEQUAL = NOUTER/NINOUT + NOUTER
          If(Batch.and.Inrad.ne.0) goto 118
        INRAD  = (1200**3)/NEQUAL
        CUBERT = 1.0/3.0
c***********************
c   multiply by 2 here for 25micron (cube root of 2 cubed)
C***********************
        INRAD  = 2*(FLOAT(INRAD)**(CUBERT))
          If(Batch) goto 118
116     write(iout,1145)inrad
        if(.not.batch)write(6,1145) inrad
1145    format(/1x,'INNER BIN RADIUS IN RASTERS [',I4,'] :',$)
        read(5,*) INRADI
1146    FORMAT(I4)
        IF(INRADI.NE.0) INRAD = INRADI
        write(iout,1147)inrad
1147    format(1H+,I5)
118     IF(NINOUT.LE.1) GOTO 119
          if(Batch.and.Inrad2.ne.0) goto 120
c***********************
c   multiply by 2 here for 25micron (cube root of 2 cubed)
C***********************
        RAD2 = ((1200**3) + (FLOAT(INRAD)**3)/(2.0**3))/2.0
        INRAD2 = 2*(RAD2**CUBERT)
          If(Batch) goto 120
        WRITE(IOUT,1144) INRAD2
        if(.not.batch)write(6,1144) INRAD2
1144    FORMAT(' OUTER BIN RADIUS IN RASTERS [',I4,'] :',$)
        read(5,1146) INRADI
        IF(INRADI.NE.0) INRAD2 = INRADI
        write(iout,1147)inrad2
119          If(Batch) goto 120
C
C----------------- GET MIN INT FOR INCLUSION IN PROFILES
C                 AND MAXIMUM ALLOWABLE OD FOR THIS MEDIUM
C  Note that input imaxod is uncorrected for film characteristics
C
      if(image) then
        inod = 65000
        innpwk = 150
      else
        inod = 230
        innpwk = 40
      endif
        if(.not.batch)write(6,1150)innpwk,inod
        WRITE(IOUT,1150)innpwk,inod
1150    FORMAT(/1x,'ENTER MIN I FOR PROFILES AND MAX OD [',
     1       i5,i6,']:',$)
        read(5,*)NPWK1,IMAXOD
1148    FORMAT(I4,I4)
        if(npwk1.eq.0) npwk1 = innpwk
      if(imaxod.eq.0)  imaxod = inod
        WRITE(IOUT,1160)NPWK1,IMAXOD
1160    FORMAT(1h+,2x,2I5)
C
C---------------- CHECK FOR VARIABLE WEIGHTED PROFILES
C
        if(.not.batch)write(6,1170)
        WRITE(IOUT,1170)
1170    FORMAT(/,' DO YOU REQUIRE VARIABLE ',
     1          'WEIGHTED PROFILES?(Y/N) [Yes]',$)
        read(5,1020)JUNK
        WRITE(IOUT,1030)JUNK
        iprw = 1
        CALL NOYES(JUNK,IPRW)
120     WTPRO=.FALSE.
        IF(IPRW.NE.0) WTPRO=.TRUE.
C
C---------------- CHECK FOR PARTIAL PROFILES
C                multiplets for laues. assume always yes
C
        PROPT =.true.
c       If(Batch) goto 130
c        if(.not.batch)write(6,1180)
c        WRITE(IOUT,1180)
c1180    FORMAT(' APPLY PROFILES TO PARTIALS?(Y/N)',$)
c        read(5,1020)JUNK
c        WRITE(IOUT,1030)JUNK
c        CALL NOYES(JUNK,IPRP)
c130     PROPT=.FALSE.
c        IF(IPRP.NE.0) PROPT=.TRUE.
        CALL CUTBCK(CUT,INRAD,INRAD2,NPWK1,NUMBIN)
140     streak = .false.
      binvar = .false.
      If(Batch) then
            if(istreak.ne.0) streak=.true.
            if(streak) then
                      read(5,*)cmask,dmask,kmask,bminor,border
                    if (ibinv.ne.0) binvar=.true.
                    if (binvar) then
                       read(5,*)ammax,ammin,themaxx
                       themax = themaxx*atan(1.0)/45.
                    end if
                    goto 145
            end if
            goto 150
      end if
c
        if(.not.batch)write(6,1119)
        WRITE(IOUT,1119)
1119    FORMAT(' DO YOU REQUIRE VARIABLE RADIAL MASKS?(Y/N) [Yes]',$)
        read(5,1020)JUNK
        WRITE(IOUT,1030)JUNK
        istreak = 1
        CALL NOYES(JUNK,Istreak)
      IF(Istreak.ne.0) streak=.true.
      if(.not.streak) goto 141
        if(.not.batch)write(6,1118)
        WRITE(IOUT,1118)
1118    FORMAT(/1x,'ENTER Amajor,Dmask,Kmask,Bminor,Border',
     1              '[0.8,100.0,0.0,0.4,1.0]:',$)
        read(5,1117)cmask,dmask,kmask,bminor,border
1117    FORMAT(5f8.4)
145     if(cmask.eq.0.0) cmask= 0.8
       if(dmask.eq.0.0) dmask=100.0
      if(bminor.eq.0.0) bminor = 0.4
      if(border.eq.0.0) border = 1.0
      if(batch) goto 146
        WRITE(IOUT,1116)cmask,dmask,kmask,bminor,border
1116    FORMAT(1h+,2x,5f8.4)
146     if(streak)then
          if(.not.batch) then
            write(6,2119)
            WRITE(IOUT,2119)
2119        FORMAT(' DO YOU REQUIRE VARIABLE BINNING?(Y/N) [No]',$)
            read(5,1020)JUNK
            WRITE(IOUT,1030)JUNK
            ibinv = 0
            CALL NOYES(JUNK,Ibinv)
            IF(Ibinv.ne.0) binvar=.true.
            if(.not.binvar) goto 141
            write(iout,2118)cmask,cmask
2118        format(/1x,'ENTER Amax,Amin,Themax',
     1                '[',2f5.2,',0.0]:',$)
            read(5,2117)ammax,ammin,themaxx
2117        format(3f8.4)
          end if
          if(ammax.eq.0.0) ammax = cmask
          if(ammin.eq.0.0) ammin = cmask
          if(themaxx.eq.0.0) themax = 0.0
          if(batch) goto 150
          themax = themaxx*atan(1.0)/45.
          WRITE(IOUT,2116)ammax,ammin,themaxx
2116      FORMAT(1h+,2x,3f8.4)
        end if
141      if(batch) goto 150
        If(.not.LTEK) goto 150
C
C----------------- CHECK FOR FILMPLOT
C
        if(.not.image) then
            if(.not.batch) write(6,1185)
              WRITE(IOUT,1185)
        else
            if(.not.batch) write(6,1186)
              WRITE(IOUT,1186)
      endif
1185    FORMAT(' FILMPLOT AFTER LOADING FIDUCIALS?(Y/N) [No]',$)
1186    FORMAT(' PLOT IMAGE AND CALCULATED PATTERN?(Y/N) [No]',$)
        read(5,1020)JUNK
        WRITE(IOUT,1030)JUNK
        Iswtch(3) = 0
        CALL NOYES(JUNK,ISWTCH(3))
C
C------------------  READ GENERATE FILE AND POSITION
C------------------------- FOR CORRECT FILM
C
150     BGPLAN = .FALSE.
      if(streak) then
            wtpro = .false.
            iprw = 0
      end if
c
c---sort out ref options
c
      noref  = .false.
        oneref = .false.
      tworef = .true.
      if(iswtch(2).gt.iouter) then
            iswtch(2) = iswtch(2) - 1
            isw2 = iswtch(2)
      endif
      itemp = iswtch(2) + iouter
        IF(itemp.EQ.0) noref = .true.
        IF(itemp.EQ.1) ONEREF = .true.
      if(itemp.ne.2) tworef = .false.
c
c------------------------ output for batch
c
c         If(.not.batch) goto 155
        Write(iout,1190)
      Write(iout,1138)toggle(IPorF+1)
        Write(iout,1191)toggle(ISW2+1)
        Write(iout,1157)toggle(Iouter+1)
        Write(iout,1192)toggle(Iswtch(4) +1)
        if(.not.image) Write(iout,1193)toggle(2-Iswtch(5))
        Write(iout,1139)toggle(Ipro+1)
      Write(iout,1155)toggle(Istreak+1)
      Write(iout,1156)cmask,dmask,kmask,bminor,border
      Write(iout,2155)toggle(Ibinv+1)
      Write(iout,2156)ammax,ammin,themaxx
        if(.not.profil) goto 155
        Write(iout,1149)toggle(icorr+1)
        Write(iout,1195)toggle(iprw+1)
        Write(iout,1159)toggle(Ibth+1)
        Write(iout,1196)numbin
        Write(iout,1197)inrad
        If(NINOUT.gt.1)Write(iout,1198)inrad2
        Write(iout,1189)cut,hovlap,hcut
        Write(iout,1199)npwk1,imaxod
1190    format(/1x,' Control data:')
1138    format(2x ,'     Image plate processing         ',A)
1191    format(2x ,'     Inner ref and average spot     ',A)
1157    format(2x ,'     Outer refinement               ',A)
1192    format(2x ,'     Average expanded spot display  ',A)
1193    format(2x ,'     Measure lower films as A film  ',A)
1139    format(2x ,'     Profile fitting                ',A)
1155      format(2x ,'     Radial Masking                 ',A)
1156    format(2x ,'     Mask Definition (a,d,k,b,bg)  ',
     1         5F7.3)
2155      format(2x ,'     Variable binning              ',A)
2156      format(2x ,'     Bin variation (amax,amin,themax)  ',
     1         3F8.3)
1149    format(2x ,'     Overlap deconvolution          ',A)
1195    format(2x ,'     Variable weighted profiles     ',A)
1159    format(2x ,'     Profile and Box integration    ',A)
1196    format(2x ,'     Number of profile bins        ',I4)
1197    format(2x ,'     Inner bin radius (rasters)    ',I4)
1198    format(2x ,'     Outer bin radius (rasters)    ',I4)
1189    format(2x ,'     Bg cutoff, Pk cutoffs (H,J)   ',
     1         3F8.3)
1199    format(2x ,'     Profile min and max OD        ',2I6)
C
C---------- Now get going!
C
155     CALL START
C
C----------------------------  SEARCH FOR FILM CORNERS
C
C------------------------------RETURN HERE FOR B AND C
C
160     ISWTCH(2)=ISW2
        if(film.gt.1) imaxod = 230
        CALL FNDFLM
        REPEAT=.FALSE.
C
C----------------------  LOCATE FIDUCIAL MARKS
C
         CALL FIDUS
c        if(.not.image)CALL FIDUS
C
C----------------------  TRANSFER FILM IMAGE TO DISK
C
        CALL DSCAN
        CALL CPUTIME(1)
C
C
        MORE=.TRUE.
        Imthr = 0
        Imref = 1
        Irep = 0
        I2REF = .FALSE.
        IF(FILM.GT.1.AND.ISWTCH(5).EQ.0) GOTO 290
        IF(ISWTCH(3).EQ.1) GOTO 200
C
C-----------------  FIND REFINEMENT SPOTS IN CENTRAL PORTION OF FILM
C
180     CALL CENTRS(REPEAT)
      if(oneref) goto 240
      if(noref) goto 240
        IF(NRS.GT.10) GOTO 190
        WRITE(IOUT,1200) NRS
        if(.not.batch)write(6,1200)NRS
1200    FORMAT(' ONLY',I6,' REFINEMENT SPOTS FOUND')
        GOTO 200
C
C------------------------- REFINEMENT OF FILM TRANSFORMATION
C
190     If(oneref) goto 240
      If(noref) goto 240
        If(vee) then
                CALL RVDIST
        Else
                CALL RDIST
        End If
        IF(RMSRES.LT.6) GOTO 210
C
C-------------------------  PLOT CALCULATED SPOT POSITIONS TOGETHER
C-------------------------    WITH DISPLAYED SPOTS FROM FILM SCAN
C
200     If(.not.Ltek) stop
        CALL FLMPLOT
        GOTO 180
210     IF(.NOT.MORE) GOTO 220
        MORE=.FALSE.
        IF(ESTART.GT.ELIMIT) GOTO 180
220     IF(ISWTCH(2).EQ.0) GOTO 240
        IF(REPEAT) GOTO 230
        IF(VEE) CALL SELECT
        REPEAT=.TRUE.
        NRSOLD=NRS
        GOTO 180
C
C------------------------- CHECK ON MEASUREMENT BOX DIMENSIONS
C
230     CALL CHKRAS
        NRS=NRSOLD
        IF(ISWTCH(2).EQ.1) GOTO 180

C
C------------------------- CALCULATE VARIABLE BOX PARAMETERS,
C------------------------- MAXX,MAXY AND MAXR
C
240     CALL CPUTIME(1)
        CALL RMAXR
C
c ---- use this for no entry into spot finding
c
c      if(noref) then
c            call rdist
c            goto 280
c      endif
C
C----------------- REPLACE WRSEND BY MODE = 0 HERE
C
        MODE = 0
C
C------------------------- SELECT 20 REFINEMENT SPOTS FROM CENTRS
C
        IF(.NOT.(VEE.AND.REPEAT)) CALL SELECT
C
C------------------------- SORT FULLY RECORDED SPOTS FROM GENERATE FILE
C
        CALL GENSORT
C
C------------------------- SEARCH FOR REFINEMENT SPOTS OVER WHOLE OF FILM
C
c####################################
        if(Oneref) nrs=0
      if(Noref) nrs=0
c####################################
        NRSOLD=NRS
        MORE=.TRUE.
250     NRS=NRSOLD
        CALL CPUTIME(1)
        CALL SEEKRS
C
C------------------------- REFINEMENT OF FILM TRANSFORMATION
C
        If(vee) then
                CALL RVDIST
        Else
                CALL RDIST
        End IF
c
c---- jump out now for noref (noref!)
c
      if(noref) goto 280
c
        If (Irep.eq.1) goto 270
        If (Batch) Irep = 1
        If (Batch) goto 260
        if(.not.batch)write(6,1210)
1210    FORMAT(' DO YOU WANT TO REPEAT THIS REFINEMENT? [Yes]',$)
        read(5,1020)JUNK
        Irep = 1
        CALL NOYES(JUNK,IREP)
        IF(IREP.EQ.0) GOTO 270
C
C---------------------------- REPLACE WRSEND BY MODE = 0 HERE
C
260     MODE = 0
        CALL GENSORT
        GOTO 250
270     IF(.NOT.MORE) GOTO 280
        MORE=.FALSE.
        IF(ESTART.GT.ELIMIT) GOTO 250

C
C------------------------- WRITE REFINED FILM PARAMETERS BACK
C------------------------- TO GENERATE FILE
C
280     ITILT=TILT/FDIST
        ITWIST=TWIST/FDIST
        IBULGE=BULGE/FDIST
        IF(VEE) IBULGE=BULGE/FDIST*18000./3.14159
c
c##### laue mods
c
c      NOW READ IN GE FILE PARAMETERS, CORRECT FOR THIS RUN
c      AND WRITE BACK OUT. NOTE THAT THE CENTRES READ IN
c      ARE NOT CHANGED BY THE PROGRAM!
c      *** WRITE OUT EVEN IF CENT_IN IS NOT TRUE ***
c      Lower films NOT_AS_A are done below
c
c********** READ IN GE PARAMETERS
c
      if (ltek) call newpg
      read(1,rec=2) aword,x_cen_fs,y_cen_fs,(w_fs(i),i=1,5)
      read(1,rec=3) w_fs(6),intdm,realdm,intg1,intg2,intg3
      read(1,rec=5) x_c,y_c, w_c, delta
      read(1,rec=6) twists, tilts, bulges, dump
      read(1,rec=7) y_scal, x_to_fra, mint, epsi, roff_in, toff_in
c
c********** PRINT OUT GE PARAMETERS
c
      write(8,1770)
      if(.not.batch) write(6,1770)
      write(8,1771) x_cen_fs(film),y_cen_fs(film),
     1                w_fs(film),x_c(film),y_c(film),w_c(film),
     1                y_scal(film),x_to_fra(film)
      if(.not.batch) write(6,1771) x_cen_fs(film),y_cen_fs(film),
     1                 w_fs(film),x_c(film),y_c(film),w_c(film),
     1                 y_scal(film),x_to_fra(film)
      if(.not.image) then
        write(8,1772) twists(film),tilts(film),bulges(film)
           if(.not.batch) then 
               write(6,1772) twists(film),tilts(film),bulges(film)
           endif
      else if (mar) then
        write(8,1773) twists(film),tilts(film),roff_in,toff_in
           if(.not.batch) then 
               write(6,1773) twists(film),tilts(film),roff_in,toff_in
           endif
      else if(moldyn) then
        write(8,1774) twists(film),tilts(film)
           if(.not.batch) then 
               write(6,1774) twists(film),tilts(film)
           endif
      endif
1770  format(/1x,' Input  x_cen  y_cen   w_f    x_c    y_c    w_c   ',
     1                        'y_scale xtofra')
1771      format(1x,7x,2f7.2,6f7.4,/)
1772      format(2x,'Input  twist,tilt,bulge: ',3f9.4)
1773      format(2x,'Input  twist,tilt,roff,toff: ',4f9.4)
1774      format(2x,'Input  twist,tilt: ',2f9.4)
c
c********** UPDATE GE PARAMETERS
c
      x_cen_fs(film)=float(xcenf)/100.
      y_cen_fs(film)=float(ycenf)/(100.*yscal)
      w_fs(film) = omegaf*180./(4.0*atan(1.0))      
      x_c(film) = float(ccx)/100.
      y_c(film) = float(ccy)/(100.*yscal)
      w_c(film) = ccom
      twists(film) = twist/fdist
      tilts(film) = tilt/fdist
      bulges(film) = bulge/fdist
      y_scal(film)=yscal
      x_to_fra(film)=xtofra
c
c********** WRITE OUT NEW GE PARAMETERS
c
      write(1,rec=2) aword,x_cen_fs,y_cen_fs,(w_fs(i),i=1,5)
      write(1,rec=3) w_fs(6),intdm,realdm,intg1,intg2,intg3
        write(1,rec=5) x_c, y_c, w_c, delta
        write(1,rec=6) twists, tilts, bulges, dump
        write(1,rec=7) y_scal, x_to_fra, mint, epsi, roff, toff
c
c********** PRINT OUT NEW GE PARAMETERS
c
      write(8,1775)
      if(.not.batch) write(6,1775)
      write(8,1776) x_cen_fs(film),y_cen_fs(film),
     1                w_fs(film),x_c(film),y_c(film),w_c(film),
     1                y_scal(film),x_to_fra(film)
      if(.not.batch) write(6,1776) x_cen_fs(film),y_cen_fs(film),
     1                 w_fs(film),x_c(film),y_c(film),w_c(film),
     1                 y_scal(film),x_to_fra(film)
      if(.not.image) then
        write(8,1777) twists(film),tilts(film),bulges(film)
           if(.not.batch) then 
               write(6,1777) twists(film),tilts(film),bulges(film)
           endif
      else if (mar) then
        write(8,1778) twists(film),tilts(film),roff,toff
           if(.not.batch) then 
               write(6,1778) twists(film),tilts(film),roff,toff
           endif
      else if(moldyn) then
        write(8,1779) twists(film),tilts(film)
           if(.not.batch) then 
               write(6,1779) twists(film),tilts(film)
           endif
      endif
1775  format(1x,' Output x_cen  y_cen   w_f    x_c    y_c',
     1                        '    w_c   y_scale xtofra')
1776      format(1x,7x,2f7.2,6f7.4,/)
1777      format(2x,'Output twist,tilt,bulge: ',3f9.4)
1778      format(2x,'Output twist,tilt,roff,toff: ',4f9.4)
1779      format(2x,'Output twist,tilt: ',2f9.4)
c
c ************************************************************
c
        if(.not.batch) then
            write(6,1786)
1786    FORMAT(' Terminate processing now? (Y/N) [Yes]',$)
        read(5,1020)JUNK
      if(junk.eq.'N'.or.junk.eq.'n') goto 310
      close unit=1
      stop
      endif
c
c ************************************************************
c
        GOTO 310
C
C---------------- MEASURE REFINEMENT SPOTS ON 'B' OR 'C' FILM IN A PACK
C
290     CALL RMAXR
        IF(LTEK) CALL TKCLR
c
c----- use this to skip spot finding
c
c      if(noref) then
c            call rdist
c            goto 310
c      endif
        CALL NEXT
C
C------------------------- REFINEMENT OF FILM TRANSFORMATION
C
        If(vee) then
                CALL RVDIST
        Else
                CALL RDIST
        End IF
c
c---- jump out now for noref 
c
      if(noref) goto 310
c
300     IF(.NOT.MORE) GOTO 310
        MORE=.FALSE.
        GOTO 290
C
C------------------------- REPLACE WRSEND BY MODE = 1 HERE
C
310     MODE = 1
c
c------------------------ write back new xtofra,yscal if lower film
c                         measured same as A. That for A and those
c                         lower films not as A is done above.
c
        IF(FILM.eq.1.OR.ISWTCH(5).ne.0) GOTO 312
c
c********** READ IN GE PARAMETERS
c
      if (ltek) call newpg
      read(1,rec=2) aword,x_cen_fs,y_cen_fs,(w_fs(i),i=1,5)
      read(1,rec=3) w_fs(6),intdm,realdm,intg1,intg2,intg3
      read(1,rec=5) x_c,y_c, w_c, delta
      read(1,rec=6) twists, tilts, bulges, dump
      read(1,rec=7) y_scal, x_to_fra, mint
c
c********** PRINT OUT GE PARAMETERS
c
      write(8,1770)
      if(.not.batch) write(6,1770)
      write(8,1771) x_cen_fs(film),y_cen_fs(film),
     1                w_fs(film),x_c(film),y_c(film),w_c(film),
     1                y_scal(film),x_to_fra(film)
      if(.not.batch) write(6,1771) x_cen_fs(film),y_cen_fs(film),
     1                 w_fs(film),x_c(film),y_c(film),w_c(film),
     1                 y_scal(film),x_to_fra(film)
      write(8,1772) twists(film),tilts(film),bulges(film)
      if(.not.batch) then 
               write(6,1772) twists(film),tilts(film),bulges(film)
      endif
c
c********** UPDATE GE PARAMETERS
c
      x_cen_fs(film)=float(xcenf)/100.
      y_cen_fs(film)=float(ycenf)/(100.*yscal)
      w_fs(film) = omegaf*180./(4.0*atan(1.0))      
      x_c(film) = float(ccx)/100.
      y_c(film) = float(ccy)/(100.*yscal)
      w_c(film) = ccom
      twists(film) = twist/fdist
      tilts(film) = tilt/fdist
      bulges(film) = bulge/fdist
      y_scal(film)=yscal
      x_to_fra(film)=xtofra
c
c********** WRITE OUT NEW GE PARAMETERS
c
      write(1,rec=2) aword,x_cen_fs,y_cen_fs,(w_fs(i),i=1,5)
      write(1,rec=3) w_fs(6),intdm,realdm,intg1,intg2,intg3
        write(1,rec=5) x_c, y_c, w_c, delta
        write(1,rec=6) twists, tilts, bulges, dump
        write(1,rec=7) y_scal, x_to_fra, mint
c
c********** PRINT OUT NEW GE PARAMETERS
c
      write(8,1775)
      if(.not.batch) write(6,1775)
      write(8,1776) x_cen_fs(film),y_cen_fs(film),
     1                w_fs(film),x_c(film),y_c(film),w_c(film),
     1                y_scal(film),x_to_fra(film)
      if(.not.batch) write(6,1776) x_cen_fs(film),y_cen_fs(film),
     1                 w_fs(film),x_c(film),y_c(film),w_c(film),
     1                 y_scal(film),x_to_fra(film)
        write(8,1777) twists(film),tilts(film),bulges(film)
        if(.not.batch) then 
               write(6,1777) twists(film),tilts(film),bulges(film)
        endif
c
c ************************************************************
c
        if(.not.batch) then
            write(6,1786)
        read(5,1020)JUNK
      if(junk.eq.'N'.or.junk.eq.'n') goto 312
      close unit=1
      stop
      endif
c
c ************************************************************
c
312     continue
c
c##### laue mods
c
C
C------------------------- CLOSE TEKTRONIX CHANNEL
C
        IF(XTEK) CALL ENDTX
C
C------------------------- SORT ALL REFLEXIONS IN GENERATE FILE
C
        CALL GENSORT
C
C------------------------- MEASURE ALL REFLEXIONS IN FILE
C
        Call CPUTIME(1)
        CALL MEAS
C
C------------------------- PROCESS THE MEASUREMENTS FOR EACH SPOT
C
        Call CPUTIME(1)
        CALL PROCESS
        Call CPUTIME(1)
        FILM=FILM+1
        BGPLAN = .TRUE.
        If(Batch) read(5,1220) Stat
1220    format(A)
        If(stat.eq.'STOP') goto 330
        If(stat.eq.'stop') goto 330
        If(stat.eq.'END ') goto 20
        If(stat.eq.'end ') goto 20
c
c##### laue mods
c
      if(image) goto 330
        If (Film.gt.Films) goto 320
        if(batch) goto 1226
        IMORE = 1
        if(.not.batch)write(6,1225)
1225    FORMAT(' PROCESS NEXT FILM IN PACK ?(Y/N) [YES]',$)
        read(5,1020)JUNK
        CALL NOYES(JUNK,IMORE)
        IF(IMORE.EQ.0) GOTO 330
1226    If(Ltek) Call NEWPG
        GO TO 160
C
C-------------------- END OF PACK
C
320     continue
        imore = 0
        if(.not.batch)write(6,1230)
1230    FORMAT(' ANY MORE FILMS TO PROCESS?(Y/N) [NO]',$)
        read(5,1020)JUNK
        CALL NOYES(JUNK,IMORE)
        If(Ltek) CALL TKCLR
        IF(IMORE.EQ.1) GOTO 100
330     continue
        STOP
        END
c
      SUBROUTINE MOVTX(IX,IY)
c   replace movetx with calls to qgmove pdc 6/6/91
      X = FLOAT(IX)
      Y = FLOAT(IY)
      CALL QGMOVE(X,Y)
      RETURN
      END
C
      SUBROUTINE NEWPG
      CALL SCPAUS
      CALL TKCLR
      RETURN
      END
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE NEXT   ****       *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C
C       ****  SUBROUTINE NEXT  ****
C
C FINDS SPOTS SUITABLE FOR REFINEMENT OF FILM CENTRE,
C CRYSTAL TO FILM DISTANCE AND FILM ORIENTATION
C USING SPOTS CHOSEN FROM THOSE MEASURED ON
C A PREVIOUS FILM PACK.
C THE OBSERVED C. OF G. OF THESE SPOTS IS PASSED TO
C OVERLAY REFS
C
C      Changed to select Multiplets (IM = 1) for refinement
C Sept 17th 1986 T.J.G.
c
c##### laue mods - changed to read from core and to select
c               nodals lt nodlim1
C=============================================================
C
C
        SUBROUTINE NEXT
        COMMON/SW/ISWTCH(10),oneref,noref,tworef
        COMMON/IOO/IOUT
        COMMON/PEL/IBA,IJBA(5120),nextr
        COMMON/CPEL/IARR(14000)
        INTEGER*2 IJBA,iba
        INTEGER*2 IARR
        LOGICAL oneref,noref,tworef
C
      INCLUDE 'comori.finc'
      INCLUDE 'comras.finc'
      INCLUDE 'comscn.finc'
      INCLUDE 'comrfs.finc'
      INCLUDE 'commcs.finc'
      INCLUDE 'commask.finc'
      INCLUDE 'commaskl.finc'
      INCLUDE 'comlaue.finc'
      INCLUDE 'comuab.finc'
C
        INTEGER WTX
      integer*2 intens(24)
      integer ibuf(19)
      equivalence (ibuf(3),intens(1))
        LOGICAL FULL
        INTEGER INDF,INDL,LRAS(5)
        INTEGER X(256),Y(256),IREC(256),ISPOT(256),SHX,SHY,PNTR(256)
        INTEGER IWX(256),HWX(256),HWY(256)
        integer nsd(6),numnsd(6)
        EQUIVALENCE (LRAS(1),NXX),(LRAS(2),NYY)
        DATA MAXBUF/5000/
        data nsd /1,5,10,15,20,25/
        CHARACTER*1 JUNK
C
C-----------------------------------------
C
      nzeros = 0
      nbigxy = 0
        DO 1 I=3,5
1       LRAS(I)=IRAS(I)
        IHTOFD=CTOFD*XTOFRA*FACT*0.5
        NECX=XCEN*FACT+0.5
        NECY=YCEN*FACT+0.5
        NRS=0
        DO 2 I=1,256
        ISPOT(I)=0
2       CONTINUE
        do 21 i=1,6
21      numnsd(i) = 0
c        JREC=PCKIDX+1
c        XSIZBIN=(MAXX/8.)
c        YSIZBIN=(MAXY/8.)
      xsizbin = float(xmax - xmin)/16
      ysizbin = float(ymax - ymin)/16
c       IA=3+2*(FILM-1)
      ia= 0
      if(profil) ia = 12
      ia=ia + film-1
C
C------------------  SEARCH FOR STRONGEST FULL SPOTS IN
C----------------------  SUBDIVISIONS OF FILM
C
c##### laue mods
c        DO 3 I=1,TOSPT
c        JREC=JREC+1
c        READ(1,REC=JREC) BUF
c        IR=IAND(IHMR,'17'O)
c        IF(IR.NE.0) GOTO 3
c        IM=IAND(IHMR,'360'O)
C
C************************
C                          FROM IM.NE.0 TO IM.EQ.0
C************************
C
c        IF(IM.EQ.0) GOTO 3
      do 3 ii= 1,numnod
      i = nodpt(Ii)
      im = nodal(i)/8
      if(im.eq.0) goto 3
      if(im.gt.nodlim2) goto 3
      IMULT = (NODAL(I)-8*IM)/4
      IMEAS = (NODAL(I)-8*IM-4*IMULT)/2
      IOV = (NODAL(I)-8*IM-4*IMULT-2*IMEAS)
c        if(imult.gt.0.and.iov.ne.0) goto 3
c        if(film.gt.1. and .imeas.ne.0) go to 3
      jrec = i
c
c        IF(BUF(IA).EQ.9999.OR.BUF(IA).LE.0) GOTO 3
c        XC=JX
c        YC=JY
      read(1,rec=10+i) ibuf
      if(intens(ia).ge.9999.or.intens(ia).le.0) go to 3
      if(intens(ia+6).ge.9999.or.intens(ia+6).le.0) go to 3
      xc = xyge(1,i)
      yc = xyge(2,i)
        CALL XYCALC(XCAL,YCAL,XC,YC)
      k = nint(  (xcal*fact - float(xmin))   / xsizbin)
c        K=(MAXX+XC)/XSIZBIN
        IF(K.GT.15) K=15
        if(k.lt.0) k=0
      j = nint(  (ycal*fact - float(ymin))   / ysizbin) + 1
c        J=(MAXY+YC)/YSIZBIN + 1
        IF(J.LT.1) J=1
        IF(J.GT.16) J=16
        INDX=(k*16)+j
      if(indx.lt.1.or.indx.gt.256) goto 3
c####
c        ISDR=BUF(IA)/BUF(IA+1)
      isdr = intens(ia)/intens(ia+6)
      if(isdr.le.0) goto 3
        IF(ISDR.LT.ISPOT(INDX)) GOTO 3
        ISPOT(INDX)=ISDR
        IREC(INDX)=JREC
c        CALL XYCALC(XCAL,YCAL,XC,YC)
        X(INDX)=ANINT(XCAL*FACT)
        Y(INDX)=ANINT(YCAL*FACT)
3       CONTINUE
c
c------------------------- reduce by sd test but not below 30
c
        do 31 i=1,256
      if(ispot(i).eq.0) goto 31
        nbinsd = (Ispot(i)/5) + 1
        if(nbinsd.gt.6) nbinsd = 6
        if(nbinsd.lt.1) nbinsd = 1
        numnsd(nbinsd) = numnsd(nbinsd) + 1
31      continue
        if(.not.batch)write(6,310) nsd
        write(8,310)  nsd
        if(.not.batch)write(6,311) numnsd
        write(8,311)  numnsd
310     format(/1x,' I/sdI    ',6i6)
311     format( 1x,' Number   ',6i6)
        ntotsd = 0
        do 32 i=1,6
        ii = 7-i
        ntotsd = ntotsd + numnsd(ii)
        nscut = nsd(ii)
        if(ntotsd.ge.30) goto 33
32      continue
33      if(.not.batch)write(6,312) ntotsd,nscut
        write(8,312)  ntotsd,nscut
312     format(/5x,i4,' spots selected with I/sdI above ',i3,/)
C
C-------------------------  COUNT NUMBER OF SPOTS FOUND
C
        NREF=0
        DO 4 I=1,256
        IF(ISPOT(I).LT.nscut) GOTO 4
        NREF=NREF+1
        Y(NREF)=Y(I)
        IREC(NREF)=IREC(I)
        X(NREF)=X(I)
4       CONTINUE
        WRITE(IOUT,1000) NREF
1000    FORMAT(I6,' SPOTS TO BE MEASURED')
        if(.not.batch)write(6,1000)NREF
        CALL SORTDN3(NREF,X,Y,IREC)
C
C------------------------------  SET UP VARIABLE RASTERS
C
        DO 5 I=1,NREF
        FX=IABS(X(I)-NECX)
        IFX=FX
        IF(VEE) FX=IABS(IFX-IHTOFD)
        FY=IABS(Y(I)-NECY)
        N1=FX*(FX*VARAS(2)+VARAS(1))+0.5
        N2=FY*(FY*VARAS(3)+VARAS(1))+0.5
        IHWX=(NXS+N1)/2
        HWX(I)=IHWX
        HWY(I)=(NYS+N2)/2
        IWX(I)=IHWX*2+1
5       CONTINUE
        MAXB=MAXR
        MAXW=(MAXB+1)/2
        MAXB=2*MAXW
        MAXN=MAXBUF/MAXW
        INDF=1
        INDL=1
        IFRST=1
        ILAST=0
        FULL=.FALSE.
C
C------------ GET THE START OF THE RASTER FOR THE FIRST SPOT OR
C------------ FOR A SPOT AFTER A GAP
C
6       CONTINUE
        WTX=X(INDF)+HWX(INDF)
        IBLK=WTX
C
C--------------- TEST IF SORTED SPOT HAS TO BE INCLUDED IN
C----------------- THIS SCAN
C
7       IER=-1
        CALL RDBLK(2,IBLK,IJBA,NWORD,IER)
8       IF(INDL.EQ.NREF) GOTO 9
        IF(FULL) GOTO 9
        IF(WTX.GT.X(INDL+1)+HWX(INDL+1)) GOTO 9
        INDL=INDL+1
        IF(INDL-INDF.EQ.MAXN-1) FULL=.TRUE.
        GOTO 8
C
C----------- GET RESULTS FROM THIS SCAN AND START THE NEXT ONE
C
9       WTX=WTX-1
        IBLK=IBLK-1
C
C----- STORE OPTICAL DENSITIES FOR ALL SPOTS INCLUDED IN THIS SCAN
C
        IF(INDL.LT.INDF) GOTO 13
        DO 12 J=INDF,INDL
        MJ=MOD(J-1,MAXN)
        NJ=MJ*MAXB
        MJ=MJ+1
        IY=Y(J)
        KMN=IY-HWY(J)
        KMX=IY+HWY(J)
        DO 11 K=KMN,KMX
        CALL CBYTE(K)
        IF(J.NE.IFRST) GOTO 10
        PNTR(IFRST)=1
        IFRST=IFRST+1
10      IZZ=PNTR(J)+NJ
        CALL CBPACK(IZZ)
        PNTR(J)=PNTR(J)+1
11      CONTINUE
        IWX(J)=IWX(J)-1
12      CONTINUE
C
C--------------------- CHECK IF ONE OR MORE SPOTS ARE FINISHED
C
13      CONTINUE
        IF(IWX(INDF).EQ.0) GOTO 14
        IF(INDF.LE.INDL) GOTO 7
        FULL=.FALSE.
        GOTO 6
14      IND=ILAST*MAXW+1
        ILAST=ILAST+1
        NXX=HWX(INDF)*2+1
        NYY=HWY(INDF)*2+1
      if(streak) then
        call cgmask(iarr(ind),lras,-1,delx,dely,
     1                  x(indf),y(indf),sod)
      else
          CALL CGFIT(IARR(IND),LRAS,-1,DELX,DELY,SOD)
      end if
      if(sod.le.0) then
            nzeros = nzeros + 1
            goto 151
      end if
      if(abs(delx).lt.float(nxx).and.abs(dely).lt.float(nyy))
     1  then
              IF(ISWTCH(1).EQ.0) GOTO 15
              CALL ODPLOT(IARR(IND),NXX,NYY,-1)
            goto 15
      end if
      nbigxy = nbigxy + 1
      goto 151
15      SHX=DELX/FACT+0.5
        SHY=DELY/FACT+0.5
        NRS=NRS+1
        XRS(NRS)=(X(INDF)+DELX)/FACT+0.5
        YRS(NRS)=(Y(INDF)+DELY)/FACT+0.5
        RRS(NRS)=IREC(INDF)
151      continue
        IF(ILAST.EQ.MAXN) ILAST=0
        INDF=INDF+1
        IF(INDF.LE.NREF) GOTO 13
      if(nzeros.gt.0) then
            if(.not.batch) write(6,141)nzeros
            write(8,141)nzeros      
141      format(1x,i4,' zero intensity spots have been rejected ')
      end if
      if(nbigxy.gt.0) then
            if(.not.batch) write(6,142)nbigxy
            write(8,142)nbigxy
142      format(1x,i4,' large error spots have been rejected ')
      end if
        RETURN
        END
C
C
      SUBROUTINE NOYES(I1,I2)
      CHARACTER*(*) I1
      IF(I1.EQ.'Y')I2=1
      IF(I1.EQ.'y')I2=1
      IF(I1.EQ.'N')I2=0
      IF(I1.EQ.'n')I2=0
      RETURN
      END
C
C   SUBROUTINE TO PUT OD'S IN HAT ARRAY.
C
      SUBROUTINE ODPHAT(HX,HY,MASK,IBIN,nobin)
C
      INCLUDE 'comprf.finc'
      INCLUDE 'combck.finc'
      INCLUDE 'comhat.finc'
      INCLUDE 'comuab.finc'
C
      INTEGER MASK(1),P,Q,HX,HY
C
      N=0
      IJ=0
      idir = -1
      DO 2 P=-HX,HX,idir
      DO 2 Q=-HY,HY
      IJ=IJ+1
                  IF (MASK(IJ)) 2,2,1
1     IK=IJ+1
      N=N+1
      IOD=IP(IK,IBIN)
      IPROB=IP(IK,NOBIN)
      IF(WTPRO) IPROB=IPW(IK)
      PR=FLOAT(IPROB)
      IF(PR.EQ.0.0)PR=1.0
      HATP(N,1,nobin)=(FLOAT(IOD)-A(IBIN)*FLOAT(P)
     1           -BZ(IBIN)*FLOAT(Q)-C(IBIN))/PR
c      if(float(iprob).eq.0.0) then
c            if(abs(hatp(n,1,nobin)).gt.0.5) then
c              hatp(n,1,nobin) = (hatp(n,1,nobin) /
c    1                   abs(hatp(n,1,nobin))) * 0.5
c            end if
c      end if
      HATP(N,4,nobin)=float(iprob)
      HATP(N,5,nobin)=FLOAT(IOD)
2     CONTINUE
      RETURN
      END
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE ODPLOT  ****      *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C
C       SUBROUTINE ODPLOT
C
C  PLOTS ARRAY OF OPTICAL DENSITIES FOR SPOT ON
C  TEKTRONIX.OD.S ARE STORED IN A BYTE ARRAY AND
C  ARE PLOTTED AS OD/3.
C  CALLED BY CENTRS,SEEKRS,MESS,NEXT FOR OPTIONAL
C  OUTPUT OF SPOTS
C
C=============================================================
C
        SUBROUTINE ODPLOT(BB,IWX,IWY,IDR)
c
cRCD ...Image Plate logical
        common/iplate/image,mar,moldyn
        logical image,mar,moldyn
c
        INTEGER IA(30)
        INTEGER*2 BB(1)
C        OPEN(UNIT=7,FILE='TT:',STATUS='UNKNOWN')
        NXY=IWX*IWY
        N=NXY
        IF(IDR.EQ.1) N=IWY
        DO 2 J=1,IWY
        IJ=N-J+1
        DO 1 I=1,IWX
        if(.not.image)then
        IA(I)=IBYTE(BB,IJ)/3
        else
        ia(i)=ip_ibyte(bb,ij)/3
        endif
        IJ=IJ+IDR*IWY
1       CONTINUE
        WRITE(6,1000) (IA(K),K=1,IWX)
1000    FORMAT(24I3)
2       CONTINUE
c         CLOSE (UNIT=7)
        RETURN
        END
C
C
C           SUBROUTINE TO TAKE OUT A BG POINT OF VALUE
C         B AT X,Y (RELATIVE TO CENTRE) FROM THE PLANE SUMS .
C                    NEED TO DO ALL ELEMENTS SINCE
C                   AM WILL NEED REEVALUATING
C
      SUBROUTINE OFFBCK(J, B, P, Q, SIGN)
C
      INCLUDE 'combck.finc'
      INCLUDE 'comprf.finc'
C
      INTEGER P, Q, SIGN
C
C
      RP = FLOAT(P)
      RQ = FLOAT(Q)
      BN(J) = BN(J) - 1
      BP(J) = BP(J) - P
      BQ(J) = BQ(J) - Q
      BPQ(J) = BPQ(J) -  RP * RQ
      BPP(J) = BPP(J) -  RP * RP
      BQQ(J) = BQQ(J) -  RQ * RQ
C
C------------------  TAKE OFF BM
C
      BOP(J) = BOP(J) -  B * RP
      BOQ(J) = BOQ(J) -  B * RQ
      BO(J) = BO(J) -  B
      IF(bn(j).ge.4) RETURN
c      if(.not.batch)write(6,311)bn(j)
c      WRITE(8,311)bn(j)
c311      FORMAT(1X,' ** BG POINTS REDUCED TO',I3,' IN OFFBCK **')
      intpro = 99999
      RETURN
      END
C
C
C           SUBROUTINE TO TAKE OUT A J VALUE WHEN
C             CALCULATING THE INTENSITY FOR AN
C                 OVERLAPPED PROFILE.
C
      SUBROUTINE OFFHAT(I,ODPR,PPR,QPR,PR,PR2,nobin)
C
      INCLUDE 'combck.finc'
      INCLUDE 'comprf.finc'
      INCLUDE 'comhat.finc'
C
C--------------SET TO TAKE OFF VALUES
C
        ODPR=ODPR-HATP(I,5,nobin)*HATP(I,4,nobin)
        PPR=PPR-HATP(I,2,nobin)*HATP(I,4,nobin)
        QPR=QPR-HATP(I,3,nobin)*HATP(I,4,nobin)
        PR=PR-HATP(I,4,nobin)
        PR2=PR2-HATP(I,4,nobin)*HATP(I,4,nobin)
C
      RETURN
      END
C
C+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
C
      SUBROUTINE  open_ge (in_data1, in_data2, file_name, end_pos,
     1                     end_rec)
C
C Purpose: To open .ge files using the given units and filename
C          Note that if the filename 'interactive' is supplied
C          then the user is prompted for a filename
C          Also note that filenames can have extensions, but these are removed
C
      implicit none
C
      integer*4      in_data1, in_data2, end_rec
      character*(*)  file_name
      integer        end_pos
C
C
      common /io_streams/ in_term, out_term, in_data, out_data
      integer*4           in_term, out_term, in_data, out_data
C
C in_term : main input stream (usually the terminal =5)
C out_term : main output (printer) stream (usually =6)
C in_data : input stream for data (to be written to files)
C out_data : output stream for data (to be written to files)
C
C CALLS   0:
C
C-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
C Local Variables:
C
      integer*4     i, IFAIL
      character*30  fname, fname1, fname2
      logical       exists1, exists2
C
C fname : filename of .ge input file
C fname1 : filename with .ge1 appended
C fname2 : filename with .ge2 appended
C
C--------------------------------------------------------------------------
C
 1    if  (file_name.eq.'interactive')  then
         call  parse_file ('?_ge_file: ', 'ge1', fname, i)
       else
         call  parse_file (file_name, 'ge1', fname, i)
      end if
      file_name = fname
      end_pos = i
C
      fname1 = fname(1:i) // '.ge1'
      fname2 = fname(1:i) // '.ge2'
C
      call  check_file (fname1, '%OPEN_GE', exists1)
      call  check_file (fname2, '%OPEN_GE', exists2)
C
      if  (.not.(exists1.and.exists2))  then
         file_name = 'interactive'
         goto 1
      end if
C
      IFAIL=0
      CALL CCPDPN(-in_data1,fname1,'OLD','DU',19,IFAIL)
C      open(unit=in_data1, file=fname1, status='old', recl=19,
C     1     form='unformatted', access='direct')
C
      IFAIL=0
      CALL CCPDPN(-in_data2,fname2,'OLD','DU',2,IFAIL)
C      open(unit=in_data2, file=fname2, status='old', recl=2,
C     1     form='unformatted', access='direct')
C
      write (8, 1000) fname(1:i)
C
      read (unit=in_data1,rec=10) i,end_rec
      if  (end_rec.lt.1)  then
         read (unit=in_data1,rec=9) i,i,end_rec
      end if
      end_rec = end_rec + 10
C
      return
 1000 format(1x,'input ge file is ',a)
      end
C
      SUBROUTINE  parse_file (filename, ext, parsed_name, end_pos)
C
C Purpose: To parse filename and add .ext if it has no extension
C
      implicit none
C
      character*(*)  filename, parsed_name
      character*3    ext
      integer*4      end_pos
C
C
      common /io_streams/ in_term, out_term, in_data, out_data
      integer*4           in_term, out_term, in_data, out_data
C
C in_term : main input stream (usually the terminal =5)
C out_term : main output (printer) stream (usually =6)
C in_data : input stream for data (to be written to files)
C out_data : output stream for data (to be written to files)
C
C CALLS   0:
C
C-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
C Local Variables:
C
      integer*4     i, lenfname
      character*30  fname
C
C fname : filename of .ge input file
C
C--------------------------------------------------------------------------
C
      fname = filename
C
      if  (filename(1:1).eq.'?')  then
C
 1       write (out_term, 1000) filename(2:)
         read (in_term, 1010) fname
         if (fname(1:1).eq.' ') goto 1
         write (8, 1011) fname
C
      end if
C
      lenfname = len(fname)
      i = lenfname
      do while (fname(i:i).ne.']' .and. fname(i:i).ne.'.' .and. i.gt.1)
         i = i - 1
      end do
C
C========  Check if extension is present
C
      if  (fname(i:i).eq.'.') then
         end_pos = i - 1
         parsed_name = fname
       else
         i = 1
         do while (fname(i:i).ne.' ' .and. i.lt.lenfname)
            i = i + 1
         end do
         if  (i.eq.lenfname) then
            stop ' parsefile: Fatal error, filename too long'
         end if
         end_pos = i - 1
         parsed_name = fname(:end_pos) // '.' // ext
      end if
C
      return
C
 1000 format(1x,a,$)
 1010 format(a30)
 1011 format(1x,a30)
      end
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE PLTRAS ****       *
C                      *                                           *
C                      *********************************************
C
C
C
C
C
        SUBROUTINE PLTRAS(IRAS)
        INTEGER IRAS(5),P,Q
C
      INCLUDE 'comuab.finc'
C
        COMMON/IOO/IOUT
        If (Batch) goto 1
        If (.not.Ltek) goto 1
        IF(.NOT.XTEK)CALL TKCLR
1       IF(XTEK) CALL NEWPG
C
C ------------------
C                    Introduce scale = 15 for X direction
C                               sept 8. trevor
C
        J=15
        K=20
        N=0
        IHX=IRAS(1)/2
        IHY=IRAS(2)/2
        IX=J*IHX
        IY=K*IHY
        if(.not.ltek) goto 3
2     CALL MOVTX(-IX+512,IY+300)
      CALL VECTX(IX+512,IY+300)
      CALL VECTX(IX+512,-IY+300)
      CALL VECTX(-IX+512,-IY+300)
      CALL VECTX(-IX+512,IY+300)
        IX=J*(IHX-IRAS(4))
        IY=K*(IHY-IRAS(5))
        N=N+1
        IF(N.EQ.1) GOTO 2
      CALL ALPHA
        IX=J*IHX
        IY=K*IHY
        ICX=IX-J*IRAS(3)
        ICY=IY-K*IRAS(3)
C
        KX1=ICX+512
        KX2=IX+512
        KX3=-ICX+512
        KX4=-IX+512
        KY1=IY+300
        KY2=ICY+300
        KY3=-IY+300
        KY4=-ICY+300
      CALL MOVTX(KX1,KY1)
      CALL VECTX(KX2,KY2)
      CALL MOVTX(KX1,KY3)
      CALL VECTX(KX2,KY4)
      CALL MOVTX(KX3,KY1)
      CALL VECTX(KX4,KY2)
      CALL MOVTX(KX3,KY3)
      CALL VECTX(KX4,KY4)
      CALL ALPHA
3      IC=IHX+IHY-IRAS(3)
        IRX=IHX-IRAS(4)
        IRY=IHY-IRAS(5)
        NPEAK=0
        NBG=0
        NMISS=0
        DO 6 P=-IHX,IHX
        IP=IABS(P)
        DO 6 Q=-IHY,IHY
        IQ=IABS(Q)
        IPQ=IP+IQ
        IF((IPQ.GT.IC).OR.(IP.GT.IRX).OR.(IQ.GT.IRY)) GOTO 4
        IF((IPQ.LT.IC).AND.(IP.LT.IRX).AND.(IQ.LT.IRY)) GOTO 5
        NMISS=NMISS+1
        GOTO 6
4       NBG=NBG+1
        GOTO 6
5       NPEAK=NPEAK+1
6       CONTINUE
C
C -------------------POSITION TEXT AT TOP OF PAGE
C
        if(ltek) CALL TKPLOT(0,760,1,IER)
        WRITE(IOUT,1000)
        if(.not.batch)write(6,1000)
1000    FORMAT(/,' MEASUREMENT BOX PARAMETERS.')
        WRITE(IOUT,1001) NPEAK,NBG,NMISS
        if(.not.batch)write(6,1001)NPEAK,NBG,NMISS
1001    FORMAT(' FOR THIS BOX THERE ARE',I5,' POINTS IN THE PEAK',/,
     1'                       ',I5,' POINTS IN THE BACKGROUND',/,
     1'                    AND',I5,' POINTS OMITTED')
        RETURN
        END
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE PREAD   ****      *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
      SUBROUTINE PREAD(IB,L,IU)
      INTEGER*2 IB(L)
      READ(IU)IB
      RETURN
      END
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE PROBIN ****       *
C                      *                                           *
C                      *********************************************
C
C         IBIN IS THE SPOT BIN (NUMBIN+1 FOR INTEGRATION PASS)
C         NOBIN IS THE STANDARD PROFILE BIN (=IBIN FOR STANDARDS)
C
C=============================================================
C
C
        SUBROUTINE PROBIN(IRAS,MASK,IBIN,NOBIN)
C
      INCLUDE 'comuab.finc'
      INCLUDE 'comprf.finc'
      INCLUDE 'combck.finc'
      INCLUDE 'comhat.finc'
      INCLUDE 'commaskl.finc'
C
        INTEGER IRAS(5),MASK(1),P,Q,HX,HY,SIGN,BJN
cRCD ...Image Plate logical
        common/iplate/image,mar,moldyn
        logical image,mar,moldyn
      IOUT = 8
        HX=IRAS(1)/2
        HY=IRAS(2)/2
      idir = -1
      hx = hx * idir
C
C-------------------------ZERO ARRAYS FOR THIS BIN
C
      CHANGE(IBIN) = .FALSE.
      CALL SETBCK(IBIN)
      IPSC(IBIN) = 999.0
      SIGN = 1
C
C--------------------START AT 2ND POINT IN IP ARRAY
C
      if(endbin) then
      nobi = nob(ibin)
      if(nobin.eq.nobi) goto 7007
      if(.not.batch) write(6,7006) ibin, nobi, nobin
      write(8,7006) ibin, nobi, nobin
7006      format(1x,' problem in probin: ibin, nobi, nobin',
     1        3i5)
      end if
7007      continue
c
      REPB = 2.0
C ----------------- Cornell ips were poor - needed REPB = 3.0
C      if(image) REPB = 3.0
        BJN=0
        IJ=0
        PORB=0
        DO 2 P=-HX,HX,idir
        DO 2 Q=-HY,HY
      IJ=IJ+1
      IF (MASK(IJ)) 1,2,2
1       IK=IJ+1
        XOD= XIP(IK,IBIN)
      IF(ENDBIN) then
      XOD = FLOAT(IP(IK,IBIN))
         if(ovlap.and.correct) then
c            nobi = nob(ibin)
            xot = float(nob(nobin)) + repb*cutu
               if(xod.gt.xot) then
                  ip(ik,ibin) = nob(nobin)
                  XOD = FLOAT(IP(IK,IBIN))
                  rej(1) = rej(1) + 1
               end if
         end if
      end if
C
C----------------------- SET UP STANDARD AM FIRST TIME
C------------------------- SET UP P & Q IN HAT ARRAY
C
      if (endbin) then
            goto 11
      end if
      if(.not.streak) then
          IF(IBIN.NE.1) GOTO 11
        end if
        CALL AMBCK(ibin,P,Q)
c        CALL SETHAT(BJN,P,Q)
C
C-----------------------  SUMS FOR BACKGROUND
C
11      CALL SUMBCK(IBIN,XOD,P,Q,SIGN)
2       CONTINUE
C
C-----------------------  SET STANDARD AM
C
      nobbi = ibin
      if(endbin) nobbi = nob(ibin)
c
      if (streak) then
                  call am1bck(nobbi)
            else
                  IF(IBIN.EQ.1) THEN
                    CALL AM1BCK(ibin)
                  end if
      end if
C
C----------------------- EVALUATE BG PLANE
C
      CALL ABCBCK(IBIN,AMPRO)
C
C----------------------- CHECK BG PLANE
C
      intpro=9999
      IJ=0
        DO 4 P=-HX,HX,idir
        DO 4 Q=-HY,HY
        IJ=IJ+1
      IF (MASK(IJ)) 3,4,4
3      IK=IJ+1
        XOD= XIP(IK,IBIN)
      IF(ENDBIN) XOD = FLOAT(IP(IK,IBIN))
        CALL CHKBCK(IBIN,nobin,XOD,P,Q,SIGN)
4       CONTINUE
c        END IF
C
C------------------------  REDO BG PLANE IF NECCESSARY
C
      IF(.NOT.CHANGE(IBIN)) GOTO 5
      if(intpro.eq.99999) return
      CALL SLVBCK(IBIN)      
      if(intpro.eq.99999) return
C
C-------------------------MAKE PROFILE-------------------
C
C
C--------------------------  WHOLE SPOT FOR STANDARDS
C
5      IF(ENDBIN) GOTO 9
      IJ=0
        DO 8 P=-HX,HX,idir
        DO 8 Q=-HY,HY
        IJ=IJ+1
      IK=IJ+1
        XOD= XIP(IK,IBIN)
      SIGN = 1
      IF (MASK(IJ)) 6,6,7
6      SIGN = -1
7       CONTINUE
        CALL PROSET(IBIN,XOD,P,Q,SIGN)
      IP(IK,IBIN) = NINT(XOD/10000.)
      XIP(IK,IBIN)= XOD/10000.
8      CONTINUE
C
C------------------------WRITE OUT INFO ON THIS BIN
C
      if(.not.batch)write(6,1000)IBIN
      WRITE(IOUT,1000)IBIN
1000      FORMAT(1X,' *** BIN N0. ',I3,' FINISHED ***')      
C
C---------------------  SCALE THE PROFILE
C
      CALL PROSCL (IBIN,IRAS(1),IRAS(2),HX,HY)
C
C--------------------   CHECK PROFILE FITTED INTENSITY
C
9      Call PROCAL (IBIN,NOBIN,IRAS(1),IRAS(2),HX,HY,MASK)
C
        RETURN
        END
C
C
C                EVALUATE PROFILE FITTED INTENSITY
C
C
C
C=============================================================
C
C
        SUBROUTINE PROCAL(IBIN,NOBIN,IRAS1,IRAS2,HX,HY,MASK)
C
      INCLUDE 'comprf.finc'
      INCLUDE 'combck.finc'
      INCLUDE 'comuab.finc'
      INCLUDE 'comhat.finc'
      INCLUDE 'comtem.finc'
      INCLUDE 'commaskl.finc'
C
        INTEGER MASK(1),P,Q,HX,HY,BKN
      REAL ODPR,PPR,QPR,PR,PR2,PRT,podsav,qodsav
      DATA IOVER/0/
      IOUT = 8
      INTPRO = 9999
      INTP = 9999
      PODSUM = 0.0
      QODSUM = 0.0
      idir = -1
        HX=IRAS1/2
        HY=IRAS2/2
      hx = hx * idir
      NPS = 1
      NMBN = NUMBIN + 1
c      if(ibin.eq.nmbn.and.BN(nmbn).eq.0) then
c       write(iout,500)
c       if(.not.batch)write(6,500)
c500       format(1x,' ** NO POINTS IN BG. REJECT THIS SPOT** ')
c       RETURN
c      END IF
C
C--------------------START AT 2ND POINT IN IP ARRAY
C
1      ODPR = 0.0
      QPR = 0.0
      PPR = 0.0
      PR  = 0.0
      PR2 = 0.0
        IJ=0
      nsat = 0
        BKN =0
        PORB = 1
        DO 4 P=-HX,HX,idir
        DO 4 Q=-HY,HY
      IJ=IJ+1
            IF (MASK(IJ)) 4,4,2
2       IK=IJ+1
c
      if(nps.eq.1) then
       if(correct) then
          if(.not.endbin) then
            call sethat(ibin,bkn,p,q)
          end if
       end if
      end if
c
        IOD= IP(IK,NMBN)
      IF(IOD.GT.Isan) NSAT = NSAT + 1
      IF(NSAT.GT.IOVER) RETURN
      IPROB = IP(IK,NOBIN)
      IF(.NOT.ENDBIN) GOTO 3
      IF(WTPRO) IPROB = IPW(IK)
3      IF(NPS.EQ.2) IPROB=1
C
C----------------------------SUMS FOR PEAK
C
      ODPR = ODPR + FLOAT(IOD * IPROB)
      PPR = PPR + FLOAT(P * IPROB)
      QPR = QPR + FLOAT(Q * IPROB)
      PR = PR + FLOAT(IPROB)
      PR2 = PR2 + FLOAT(IPROB * IPROB)
        PRT = PR
      PODSUM = PODSUM + FLOAT(IOD*P)
      QODSUM = QODSUM + FLOAT(IOD*Q)
4       CONTINUE
C------------------- SET UP HAT-MATRIX & H
C
      if(nps.eq.1) then
          if(correct) then
c
             if(.not.endbin) then
             if(xtek) call newpg
             CALL STPHAT(ibin,HX,HY)
             if(xtek) call newpg
             end if
c
               IF(ENDBIN.AND.OVLAP)THEN
               CALL ODPHAT(HX,HY,MASK,NMBN,nobin)
                 CALL HATHAT(nobin)
                 CALL CHKPHAT(ODPR,PPR,QPR,PR,PR2,nobin)
               if(prorej.eq.1) then
                nopuse = nopuse + 1
                return
               end if
               END IF
          end if
      end if
C
C-----------------------  GET INTENSITY
C
      INTPRO=(ODPR-A(IBIN)*PPR-BZ(IBIN)*QPR-C(IBIN)*PR)*PRT/PR2 + 0.5
C
C---------------------  PRINT OUT
C
      IF(.not.endbin) goto 5
      If(.not.iboth) return
5      IF(NPS.EQ.1) then
       INSAV = INTPRO
       podsav = podsum
       qodsav = qodsum
      end if
      NPS = NPS + 1
      IF(NPS.EQ.2) GOTO 1
      INTP = INTPRO
      INTPRO = INSAV
      podsum = podsav
      qodsum = qodsav
      if(endbin) return
      WRITE(8,1000) IBIN, INTPRO, intp
      if(.not.batch)write(6,1000) IBIN,INTPRO, intp
1000      FORMAT(1h1,1X,'BIN ',I2,' PROFILE FIT I = ',I7,
     1        '   INTEGRATED  I = ',I7)
c      WRITE(8,1001) IBIN, INTP
c      if(.not.batch)write(6,1001) IBIN,INTP
c1001      FORMAT(2X,'BIN ',I2,' INTEGRATED  I = ',I5)
        RETURN
        END
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE PROCESS ****      *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C                      ******      PROCESS     ******
C
c##### laue mods
C   Mods for specific Laue processing. Back from Pella 25 Sept 1986
C
C=============================================================
C
C
C
        SUBROUTINE PROCESS
c
cRCD ...Image Plate logical
        common/iplate/image,mar,moldyn
        logical image,mar,moldyn
C
      INCLUDE 'comhis.finc'
      INCLUDE 'comori.finc'
      INCLUDE 'compro.finc'
      INCLUDE 'comras.finc'
      INCLUDE 'comrep.finc'
      INCLUDE 'comuab.finc'
      INCLUDE 'comprf.finc'
      INCLUDE 'comhat.finc'
      INCLUDE 'comtem.finc'
C
C---------  Include SCN, RFS, MCS, SND for old SEND file
C
      INCLUDE 'comscn.finc'
      INCLUDE 'comrfs.finc'
      INCLUDE 'commcs.finc'
      INCLUDE 'comsnd.finc'
      INCLUDE 'commask.finc'
      INCLUDE 'commaskl.finc'
C
        COMMON/IOO/IOUT
        COMMON/SW/ISWTCH(10)
        REAL ASPOT(5),XSH(16),YSH(16),SUMPQ(6,0:15)
        INTEGER GENBUF(10),BUFSIZ,SVMASK(5000)
        INTEGER OD(5000)
        INTEGER*2 IBUF(4),KKREC,IPXX,IPYY
        INTEGER NSH(16),TABLE(0:255),IH(6),IHD(3,50)
        INTEGER LRAS(5)
        LOGICAL FULL,YES,LDUMP,END4
        COMMON/PEL/IOD,BUF(5120),nextr
      integer*2 buf,iod
        DATA FATNO/0/
c         DATA IRANGE /-30,0,50,100,200,400,800,1600,3200/
        EQUIVALENCE (LRAS(1),NXX),(LRAS(2),NYY)
        EQUIVALENCE (IBUF(1),KKREC),(IBUF(2),IPXX),(IBUF(3),IPYY)
        EQUIVALENCE (ASPOT(1),SPOT),(ASPOT(2),BGND),(ASPOT(3),RMSBG)
        EQUIVALENCE (ASPOT(4),DELX),(ASPOT(5),DELY)
        CHARACTER*1 JUNK
        SAVE
C
        ENDBIN=.FALSE.
C
C-----------  FACTOR USED TO ADJUST SD. OF STRONG SPOTS
C
        DATA EFAC/4.0/
        if(.not.batch)write(6,1000)
        WRITE(IOUT,1000)
1000    FORMAT(' ENTER PROCESS')
        IF(XTEK) CALL NEWPG
        If(.not.ltek) goto 10
        IF(.NOT.XTEK) CALL TKCLR
10      continue
c
c
c
c cmask input is constant eta*D or FULL spot size (mm) for k=0
c bminor input is full width in mm
c dmask is cf in mm
c kmask is scale: 0 for constant, 1 for MMH
c all read in in mosuab and converted
c in SEEKRS for a,b ellipse axes
c
c      if(streak) then
c            if(.not.bgplan) then
c                    bminor = bminor/2.0*100.*fact
c                    cmask = cmask/2.0*100.*fact
c                    dmask = dmask*100.*fact
c            end if
c      end if
C
C----------------------- CHECK BOX SIZE
C
        IF(MAXR.LE.5000)GO TO 20
        STOP ' MEASUREMENT BOX TOO LARGE'
20      CONTINUE
C
C-------------- OPEN FILES FOR INPUT.
C
        ifail=0
        call ccpdpn(-3,'MOSTEMP','OLD','U',0,ifail)
c         OPEN(UNIT=3,FILE='Mostemp',STATUS='OLD',
c      1            FORM='UNFORMATTED')
C
C------ OPEN FILE FOR DUMPING SPECIFIED REFLECTIONS TO BADSPOTS FILE
C
        IF(ISWTCH(9).EQ.0)GO TO 40
        if(.not.batch)write(6,1010)
        WRITE(IOUT,1010)
1010    FORMAT(/,' BADSPOTS OUTPUT WILL CONTAIN SPECIFIED REFLECTIONS',
     1         1X,'READ IN FROM SPOTDUMP INPUT FILE ')
        ifail=0
        call ccpdpn(-21,'SPOTDUMP','UNKNOWN','F',0,ifail)
c         OPEN(UNIT=5,FILE='SPOTDU.DAT',STATUS='UNKNOWN')
        READ (21,*)NSPT
        IF(NSPT.GT.50)NSPT=50
        DO 30 I=1,NSPT
        READ(21,*) (IHD(K,I),K=1,3)
30      CONTINUE
40      CONTINUE
C
C------------------  COLUMN FOR OUTPUT INTENSITY
C
c##### laue mods
c
      iprof = 0
      if(profil) iprof = 12
      ifill = 4 + iprof +film
        IDR=-1
        BUFSIZ=1000
C
C------------------  GENERATE LOOK-UP TABLE
C
        if(.not.image)then
        CFAC=CURV/N1OD
        BASE=BASEOD*N1OD
        DO 50 I=0,255
        TRUOD=I-BASE
        TABLE(I)=TRUOD*(1.0+CFAC*TRUOD)+0.5
50      TABLE(I)=AMAX0(0,TABLE(I))
        ISAN = Table(imaxod)
        else
        isan = imaxod
        endif
C
C---------------------  CLEAR FOR STATISTICS
C
        itots = 0
        iboxs = 0
        ipros = 0
        NOLO=0
        NEDGE=0
        NBOX=0
        NBZERO=0
        NOFR=0
        MAXBSI=0
        MINBSI=200
      nopuse = 0
        OREJ=0
      ratrej = 0
      ogdrej = 0
      rgdrej = 0
        DO 60 I=1,10
        IANAL(I)=0
        AVSD(I)=0.0
60      RATIO(I)=0.0
        DO 70 I=1,16
        XSH(I)=0.
        YSH(I)=0.
70      NSH(I)=0
C
C-----------------  DETERMINE SCALE FACTOR FOR INTENSITIES
C
        CALL SETMASK(MASK(1,1),IRAS)
        CALL SETSUMS(MASK(1,1),IRAS,SUMPQ(1,1))
        SCAI=50./SUMPQ(5,1)
      if(streak) then
            scai=50./(4.0*bminor*cmask)
      end if
      if(image) scai = scai/10.0
        if(.not.batch)write(6,701) scai
        WRITE(IOUT,701) scai
701     FORMAT(1x,' Scale factor for Intensities:  ',f12.4)
        DO 80 I=3,5
80      LRAS(I)=IRAS(I)
C
C ------------- SET SWITCH (4) ACCORDING TO PROFILES
C
        IF(PROFIL) ISWTCH(4)=1
        IF(PROFIL) GOTO 90
        IF(ISWTCH(4).EQ.0) GOTO 170
C
C------------------- AVERAGE SPOT IN BINS
C
        CALL AVSPOT
C
C--------------------- PROFILE AVERAGES IN BINS
C-------------------  SKIP FORMING PROFILES IF B OR C FILM
C                               IN SAME RUN.
C      But set up the background histogram for lower films
C
90      Call cputime(1)
      IF(BGPLAN) then
            call hisbck(0,0,0,0,5)
            GOTO 170
      end if
      if(.not.batch)write(6,1020)
        WRITE(IOUT,1020)
1020    FORMAT(' FORM PROFILES')
C
C
        NECX=XCEN*FACT+0.5
        NECY=YCEN*FACT+0.5
C
C---------------SET ARRAYS TO ZERO
C
      do 62 i=1,12
      do 61 j=1,12
      histbg(i,j) = 0
      histn(i,j) = 0
61      continue
62      continue
        DO 100 NOBIN=1,NUMBIN+1
        NOB(NOBIN) = 0
        IXBIN(NOBIN) = 0
        IYBIN(NOBIN) = 0
        DO 100 K=1,5000
        XIP(K,NOBIN) = 0.0
100     IP(K,NOBIN)=0
C
C-------------------- UNEXPANDED SPOT
C
        NPY = NYS
        NPX = NXS
        NPXY = NPX * NPY
        NPACK = NPX*256+NPY
C
C------------------------
C------------------------  LOOP OVER ALL REFLECTIONS
C------------------------
C
        DO 110 I=1,NREF
C
C--------------  READ ARRAY OF OD.S FOR EACH SPOT
C
        READ(3)klrec,LEN,IBUF
        CALL PREAD(BUF(1),LEN,3)
C
C---------------  NOT FOR PARTIALS
C
        IF(KlREC.LT.0) GOTO 110
        IF(IDEC(KLREC).EQ.1) GOTO 110
        KREC=Klrec
        IX=IPXX
        IY=IPYY
        NXX=IBYTE(IBUF,7)
        NYY=IBYTE(IBUF,8)
        NXY=NXX*NYY
        IF(NXY.GT.MAXR) WRITE(IOUT,1230) MAXR,NXX,NYY,NXY
        IF(NXY.GT.MAXR.and..not.batch)write(6,1230)MAXR,NXX,NYY,NXY
        NSIZ=(NXY+1)/2
C
C----------------SET UP BIN NUMBER NOBIN
C
        IF(NUMBIN.EQ.5) THEN
                CALL PRONUM5(NOBIN,IX,IY,NECX,NECY,I)
          ELSE
        if (streak) then
                call pronum17(nobin,ix,iy,necx,necy,i)
          else
                CALL PRONUM(NOBIN,IX,IY,NECX,NECY,I)
        end if
        END IF
c
c    store bg histogram array address
c    prosel will put bg into this slot
c
      call hisbck(IX,IY,0,0,1)
C
C-----------------SELECT ONLY THE OD'S IN THE UNEXPANDED SPOT
C------------------            PUT INTO IP
C
        CALL PROSEL(NOBIN,NXX,NYY,TABLE)
C
C----------------- RETURNING NOBIN = -1 MEANS WEAK OR OVLOAD
C----------------- PROSEL HAS NOT ADDED THIS SPOT IN
C
        IF(NOBIN.EQ.-1) GOTO 110
C
C------------------SET UP AVERAGE COORDS IN EACH BIN
C
        IXBIN(NOBIN) = IXBIN(NOBIN) + (IX - NECX)
        IYBIN(NOBIN) = IYBIN(NOBIN) + (IY - NECY)
C
C----------------- ADD UP NUMBER IN BIN
C
        NOB(NOBIN) = NOB(NOBIN) + 1
C
C--------------------END OF LOOP
C
110     CONTINUE
c
c    form and print background histogram
c
      call hisbck(1,1,0,0,3)
C
C---------------------- NOW SET NXX, NYY TO NPX, NPY
C
        NXX = NPX
        NYY = NPY
        NXY = NPXY
C
C--------------------- GET MASK READY. CONSTANT BOX
C
        CALL SETMASK(MASK(1,1),LRAS)
        CALL SETSUMS(MASK(1,1),LRAS,SUMPQ(1,0))
C
C------------------------LOOP OVER BINS
C
        DO 140 IBIN = 1, NUMBIN
C
C----------------PACK SPOT SIZE INTO WORD 1 OF IP
C
        IP(1,IBIN)=NPACK
C
C----------------GET BIN CENTRE
C
        IF(NOB(IBIN).EQ.0) GOTO 140
        IXBIN(IBIN) = IXBIN(IBIN) / NOB(IBIN)
        IYBIN(IBIN) = IYBIN(IBIN) / NOB(IBIN)
C
C------------------FORM AVGE. SPOT FOR EACH BIN.
C
        DO 130 INEED = 2,NPXY+1
        XIP(INEED,IBIN) = FLOAT (IP(INEED,IBIN))/FLOAT(NOB(IBIN))
        IP (INEED,IBIN) = NINT (XIP(INEED,IBIN))
        OD (INEED)      =        IP(INEED,IBIN)
C
C---------------- SAVE RAW SPOT IN BIN NUMBIN+1 FOR EVALUATION
C
130     IP(INEED,NUMBIN+1) = IP(INEED,IBIN)
        FULL=.TRUE.
c
      if(streak) then
      call radmask(-1,ixbin(ibin),iybin(ibin),nxx,nyy,isp,od(2))
        CALL INTEG(OD(2),LRAS,RMASK,rsums,ASPOT,IDR,FULL)
            else
        CALL INTEG(OD(2),LRAS,MASK(1,1),SUMPQ(1,0),ASPOT,IDR,FULL)
      end if
c
        DELX=DELX/FACT
        DELY=DELY/FACT
C
C-------------------SAVE INTEGRATED INTENSITY FOR PROBIN
C
        IPJ(IBIN) =  SPOT + 0.5
        WRITE(IOUT,1040)
        if(.not.batch)write(6,1045)
1040    FORMAT(1h1,'   BIN NOBIN XAV  YAV  ',
     1      'NX   NY   INT   RMSBG  DELX  DELY')
1045    FORMAT('   BIN NOBIN XAV  YAV  ',
     1      'NX   NY   INT   RMSBG  DELX  DELY')
        WRITE(IOUT,1050)IBIN,NOB(IBIN),IXBIN(IBIN),IYBIN(IBIN),
     1                  NXX,NYY,IPJ(IBIN),RMSBG,DELX,DELY
        if(.not.batch)write(6,1060)IBIN,NOB(IBIN),IXBIN(IBIN),
     1             IYBIN(IBIN),NXX,NYY,IPJ(IBIN),RMSBG,DELX,DELY
1050    FORMAT(1x,6I5,i7,3F6.1)
1060    FORMAT(2X,6I5,i7,3F6.1)
C
C----------------------SET UP STANDARD PROFILES
C--------------------- CALL PROFILE MAIN ROUTINE
C
        NOBINN = IBIN
      if(streak) then
        CALL PROBIN(LRAS,rmask,IBIN,NOBINN)
         else
        CALL PROBIN(LRAS,MASK(1,1),IBIN,NOBINN)
      end if
        IF(XTEK) CALL NEWPG
c
      if(streak) then
        CALL RASPLOT(IP(2,IBIN),NXX,NYY,rmask,-1)
         else
        CALL RASPLOT(IP(2,IBIN),NXX,NYY,MASK(1,1),-1)
      end if
c
        IF(XTEK) CALL SCPAUS
        IF(XTEK) CALL TKCLR
c
      if(streak) then
      do 138 irmask = 1,5000
138      rmaskb(ibin,irmask) = rmask(irmask)
      do 139 irsum = 1,6
139      rsumsb(ibin,irsum) = rsums(irsum)
      end if
c      
140     CONTINUE
        If(.not.Ltek) goto 150
        IF(.NOT.XTEK) CALL TKCLR
C
C--------------------- GET WEIGHTING CONSTANT FOR VARIABLE
C--------------------- PROFILE ANALYSIS IF REQUIRED.
C
150     IF(WTPRO) CALL PROVAR
C
C------------------------SAVE MASK FOR PROFILE FITTING
C
        DO 160 IMS = 1,5000
160      SVMASK(IMS) = MASK(IMS,1)
        CLOSE (UNIT=4)
        CLOSE (UNIT=3)
        ifail=0
        call ccpdpn(-3,'MOSTEMP','OLD','U',0,ifail)
c         OPEN(UNIT=3,FILE='Mostemp',STATUS='OLD',
c      1  FORM='UNFORMATTED')
C
C                                    ********************
C
C------------------------START CALCULATION OF INTEGRATED INTENSITIES
C
C                                    ********************
C
170     CONTINUE
        Write(6,1070)
        Write(Iout,1070)
1070    format(' BEGIN INTEGRATION')
        Call cputime(1)
C
C=====================================
C
        IF(.NOT.PROFIL) GOTO 200
C
C----------------------- GET AVEBG
C----------------------- FIRST CHECK FOR B &/OR C FILM
C
        IF(BGPLAN) GOTO 190
        AVEBG = 0.0
        DO 180 IBN = 1,NUMBIN
        CALL CONBCK(APL,BPL,CPL,IBN)
      nob(ibn) = nint(cpl)
180     AVEBG = AVEBG + CPL
        CUT = AVEBG/FLOAT(NUMBIN)
190     CALL CUTBCK(CUT,-1,-1,-1,-1)
200     ifail=0
        call ccpdpn(-4,'BADSPOTS','UNKNOWN','U',0,ifail)
c 200     OPEN(UNIT=4,FILE='BADSPO.DAT',STATUS='UNKNOWN',
c      1            FORM='UNFORMATTED')
        NBAD=0
        QFAC=G1OD*G1OD*N1OD/(2.*(10./FACT)**2)
        NXCUR=0
        J=0
C
C-------------------------------  BIN NUMBER NUMBIN+1 FOR INTEG
C
        IBIN = NUMBIN+1
        nobin = numbin+1
        ENDBIN=.TRUE.
        IF(XTEK) CALL NEWPG
C
C ---------------------------  LOOP OVER REFLEXIONS
C
C
C  ******************************   BOTH METHODS  ********************
C
C
C
        DO 500 I=1,NREF
        J=J+1
        READ(3)klrec,LEN,IBUF
        CALL PREAD(BUF,LEN,3)
        KREC=Klrec
C
        OVLAP=.FALSE.
C
        IX=IPXX
        IY=IPYY
        IREC(J)=IABS(KREC)
        FULL=.FALSE.
        IF(KREC.LT.0) GOTO 210
        IF(IDEC(KLREC).EQ.1) OVLAP=.TRUE.
        FULL=.TRUE.
        NOFR=NOFR+1
210     NXX=IBYTE(IBUF,7)
C
C-------------------------BRANCH FOR PROFILES
C
        IF(PROFIL.AND.FULL) GOTO 270
        IF(PROPT) GOTO 270
C
C
C *********************************  NON - PROFILES ***************
C
C ------------------  TEST FOR CHANGE OF NXX FOR SETTING MASKS
C
        iboxs = iboxs + 1
        IF(NXX.EQ.NXCUR) GOTO 230
        NXCUR=NXX
        NYY=NYS
        DO 220 K=0,15
        CALL SETMASK(MASK(1,K),LRAS)
        CALL SETSUMS(MASK(1,K),LRAS,SUMPQ(1,K))
        NYY=NYY+2
        NXY=NXX*NYY
        IF(NXY.GT.MAXR) GOTO 230
        IF(NYY.GT.70)GO TO 230
220     CONTINUE
230     NYY=IBYTE(IBUF,8)
C
C---------------------------------
C
        IF(NXX.GT.70.OR.NYY.GT.70)GO TO 390
        NXY=NXX*NYY
        IF(NXY.GT.MAXR) WRITE(IOUT,1230) MAXR,NXX,NYY,NXY
        IF(NXY.GT.MAXR.and..not.batch)write(6,1230)MAXR,NXX,NYY,NXY
        NSIZ=(NXY+1)/2
        NN=(NYY-NYS)/2
        IF(NN.LE.15)GO TO 240
        if(.not.batch)write(6,1080)NYY,NYS
        WRITE(IOUT,1080)NYY,NYS
1080    FORMAT(' BOX IS OVEREXPANDED IN Y, NYY=',I5,' NYS=',I5)
        STOP ' OVEREXPANDED BOX'
240     CONTINUE
        TPEAK=SUMPQ(5,NN)
        TBGND=SUMPQ(6,NN)
C
C-------------------  READ DENSITIES FOR ONE SPOT
C-------------------  CORRECT DENSITIES
C
        N=0
        DO 250 K=1,NXY
        if(.not.image)then
        CALL CBYTE(K)
        else
        call ip_cbyte(k)
        endif
        IF(IOD.EQ.0)GO TO 400
c********************* ovload od from CUTOFF to IMAXOD (in comtem)
        if(.not.image)then
        IF(IOD.GT.IMAXOD) N=N+1
        OD(K)=TABLE(IOD)
        else
        if(abs(int(iod)).gt.imaxod) n=n+1
        od(k)=abs(int(iod))
        endif
250     CONTINUE
        IF(N.GT.FATNO) GOTO 410
        IF(ISWTCH(1).EQ.0) GOTO 260
        IF(XTEK) CALL NEWPG
        CALL RASPLOT(OD,NXX,NYY,MASK(1,NN),-1)
        if (Ltek) CALL SCPAUS
260     continue
      CALL INTEG(OD,LRAS,MASK(1,NN),SUMPQ(1,NN),ASPOT,IDR,FULL)
        ISPOT=SCAI*SPOT+0.5
C ******************************************************************
c
      if(streak) then
c      if(ispot.gt.600) then
      call radmask(1,ix,iy,nxx,nyy,ispot,od)
        TPEAK=SUMPQ(5,NN)
        TBGND=SUMPQ(6,NN)
        CALL INTEG(OD,LRAS,RMASK,rsums,ASPOT,IDR,FULL)
      tpeak = rsums(5)
      tbgnd = rsums(6)
      ispot = scai*spot+0.5
c      irspot = scai *spot+0.5
c      if(.not.batch)write(6,2601) irspot
c      write(8,2601) irspot
c2601      format(/1x,' Radial mask I = ',I8)
c      end if
      end if
        GOTO 300
C
C
C       FOR NON PROFILES NOW GOT SPOT, SHIFTS, BGND, RMSBG,
C                                TPEAK, TBGND,
C
C   ************************* PROFILE ONLY ********************
C
C
C-------------------NOW EVALUATE PROFILE FITTED INTENSITY
C-------------------FOR FULLYS AND PARTIALS IF NEEDED
C
C-------------------------------ZERO IP(.,NUMBIN+1)
C
270     CONTINUE
        ipros = ipros + 1
        DO 280 IMN = 1,5000
280     IP(IMN,NUMBIN+1) = 0
        NYY=IBYTE(IBUF,8)
C
C-------------------FIRST SELECT STANDARD PROFILE BIN
C
        IF(NUMBIN.EQ.5) THEN
                CALL PRONUM5(NOBIN,IX,IY,NECX,NECY,I)
          ELSE
        if (streak) then
                call pronum17(nobin,ix,iy,necx,necy,i)
          else
                CALL PRONUM(NOBIN,IX,IY,NECX,NECY,I)
        end if
        END IF
            nob(ibin) = nobin
c
      if(nobin.eq.numbin) goto 400
c
c     now select background from histogram and
c     put cutu, cutl into combck and bg into nob(nobin)
c     as usual
c
      call hisbck(ix,iy,nob(nobin),0,4)
c
      if(streak) then
      do mbin = 1,5000
      rmask(mbin) = rmaskb(nob(ibin),mbin)      
      end do
      do mbin = 1,6
      rsums(mbin) = rsumsb(nob(ibin),mbin)
      end do
      end if
C
C------------------- GET WEIGHTED PROFILE IF REQUIRED
C
        IF(WTPRO) CALL PROWT(IX,IY,NECX,NECY,I,SVMASK)
C
C------------------- NOW SELECT REQUIRED OD'S
C------------------- PUT INTO IP BIN NUMBIN+1. i.e. IP(N,NUMBIN+1)
C
        CALL PROSEL(IBIN,NXX,NYY,TABLE)
C
C------------------- NOW GET INTENSITY
C
      if(streak) then
              CALL PROBIN(IRAS,rmask,IBIN,NOBIN)
      else
            CALL PROBIN(IRAS,svmask,IBIN,NOBIN)
      end if
C
C------------------  CHECK FOR OVERLOADS
C
c------ problem with bg - to many rejects or too tilted.
c
        if(intpro.eq.99999) then
        nbzero = nbzero + 1
        goto 420
      end if
        IF(INTPRO.NE.9999) GOTO 290
        GOTO 410
C
C------------------- GET PARAMS FOR SHIFTS,SD AND STATS
C
290     CALL PROSIG(DELX,DELY,TPEAK,TBGND,RMSBG,BGND)
        SPOT = FLOAT(INTPRO)
        ISPOT = SCAI*INTPRO+0.5
        intbox = -9999
        if(iboth) intbox = scai*intp+0.5
        DELX = DELX * IDR
C
C
C  ***************************** BOTH *************************
C
C ----------- CALCULATE STANDARD DEVIATION OF INTENSITY
C
300     VSPOT=QFAC*SPOT
        VBG=TPEAK*RMSBG
        if(image) vbg=tpeak*rmsbg*rmsbg
        if(.not.image)then
        ISDBSI=SCAI*SQRT(2*VBG+EFAC*ABS(VSPOT))+0.5
        else
      isdbsi=scai*sqrt(vbg + vbg/tpeak)+0.5
c        isdbsi=scai*sqrt(vbg/tbgnd+vbg/tpeak)+0.5
        endif
        INTEN(J)=ISPOT
        intenb(j) = intbox
        ISD(J)=ISDBSI
c
c***************** for now ...
c
        isdb(j) = isd(j)
c
        SIGPRO = ISDBSI
        IBGND=SCAI*BGND+0.5
        IF(BGND.GT.0.0)GO TO 310
      WRITE(IOUT,1090)IX,IY,IREC(j),intpro
        if(.not.batch)write(6,1090)IX,IY,irec(j),intpro
1090    FORMAT('** ZERO BACKGROUND SPOT, X,Y,REC,intpro **',4I8)
        BGND=0.01
310     CONTINUE
        if(.not.image)then
        BGRATIO=RMSBG/SQRT(QFAC*BGND/TPEAK)
        else
        bgratio=1.0
        endif
c
c **************** skip plot and stats for baddies ***********
c                           5th March 1987
c
        IF((BGRATIO.ge.3.).or.(ISPOT.le.-5*ISDBSI)) GOTO 360
c
        IF(.NOT.FULL) GOTO 320
        if(.not.image)then
        IF(ISPOT.LT.IRANGE(3)) GOTO 320
        else
        if(ispot.lt.iprange(3)) goto 320
        endif
        XX=IX/FACT
        YY=IY/FACT
        DELX=DELX/FACT
        DELY=DELY/FACT
        if ((ltek).AND.(.not.batch)) CALL RESIDPL(XX,YY,DELX,DELY)
        IXX=XX-XCEN
        IYY=YY-YCEN
        CALL XYSHIFT(DELX,DELY,IXX,IYY,XSH,YSH,NSH,1)
320     MAXBSI=MAX0(MAXBSI,ISPOT)
        MINBSI=MIN0(MINBSI,ISPOT)
C
C------------------------- GET STATISTICS
C
        DO 330 K=1,9
        if(.not.image)then
        IF(ISPOT.LT.IRANGE(K)) GOTO 340
        else
        if(ispot.lt.iprange(k)) goto 340
        endif
330     CONTINUE
        K=10
340     IANAL(K)=IANAL(K)+1
        RATIO(K)=RATIO(K)+BGRATIO
        AVSD(K)=AVSD(K)+ISDBSI
        IF(ISWTCH(9).EQ.0)GO TO 350
        CALL COMPR(KREC,IHD,LDUMP,NSPT)
        IF(.NOT.LDUMP)GO TO 430
        GO TO 360
350     IF((BGRATIO.LT.3.).AND.(ISPOT.GT.-5*ISDBSI)) GOTO 430
360     IF(PROFIL.AND.FULL) GOTO 370
        IF(PROPT) GOTO 370
        WRITE(4) KREC,ISPOT,ISDBSI,IBGND,RMSBG,BGRATIO,NXY,NXX,NYY
        WRITE(4)(OD(II),II=1,NXY)
        GOTO 380
370     WRITE(4) KREC,ISPOT,ISDBSI,IBGND,RMSBG,BGRATIO,NPXY,NPX,NPY
        WRITE(4)(IP(II,NUMBIN+1),II=2,NPXY+1)
380     CONTINUE
        NBAD=NBAD+1
        GOTO 430
390     NBOX=NBOX+1
        GO TO 420
400     NEDGE=NEDGE+1
        GO TO 420
410     NOLO=NOLO+1
420     CONTINUE
        INTEN(J)=-9999
        intenb(j) = -9999
        ISD(J)=-9999
        SIGPRO = -9999
        GOTO 460
430     CONTINUE
C
C
C************************************ PROFILE ONLY
C
C------------------- CHECK RESULTS
C
        itots = itots + 1
        if(.not.image)then
        IF(INTEN(J).LT.IRANGE(8)) GOTO 460
        else
        if(inten(j).lt.iprange(8)) goto 460
        endif
        IF(.NOT.PROFIL) GOTO 450
        IF ( ( .NOT. PROPT ) .AND. ( .NOT. FULL) )  GOTO 450
        IF(INTPRO.EQ.9999) GOTO 440
        INTPRO = INTPRO * SCAI + 0.5
440     Call Conbck(Ap,Bp,Cp,Ibin)
        IDUMX = NINT(DELX/5.0)
        IDUMY = NINT(DELY/5.0)
        WRITE(IOUT,1100)INTEN(J),ISD(J),Intenb(j),Isdb(j),
     1            IBIN,NOB(ibin),NPX,
     2            NPY,AP,BP,CP,rsums(5),rsums(6),IDUMX,IDUMY
1100    FORMAT(1X,4I6,3X,4I3,3F8.3,2f8.1,3X,2I4)
        GOTO 460
C
C------------------------ NON PROFILE PARTIAL IN PROFILE RUN
C
450     CONTINUE
        if(.not.image)then
        IF(INTEN(J).LT.IRANGE(8)) GOTO 460
        else
        if(inten(j).lt.iprange(8)) goto 460
        endif
        WRITE(IOUT,1110)INTEN(J),ISD(J)
1110    FORMAT(1X,3I6)
C
C
C   ************************  BOTH HERE *******************
C
C-------------- WRITE OUT IF BUFFER FULL
C
460     IF(J.LT.BUFSIZ) GOTO 500
        CALL WRGEN(J,1)
        J=0
C
C
500     CONTINUE
C
C------------------------------END OF INTEGRATION LOOP
C
        if(ltek) CALL TKPLOT(0,760,1,IER)
        Write(6,1120)
        Write(Iout,1120)
1120    format(1x,//,' END INTEGRATION')
        Call cputime(1)
        If (.not.Ltek) goto 510
        IF(XTEK) CALL NEWPG
C
C---------------------------- WRITE OUT REJECTIONS
C
510     CONT = -9999.
        CALL CONBCK(REJI,AJUNK,CONT,IBIN)
        IREJI = REJI
        IF(IREJI.EQ.1)THEN
              if(.not.batch)write(6,1130) IREJI
              WRITE(8,1130) IREJI
            ELSE
              if(.not.batch)write(6,1131)IREJI
              WRITE(8,1131)IREJI
        END IF
1130    FORMAT(/1X,I12,' Background point was rejected')
1131    FORMAT(/1X,I12,' Background points were rejected')
c
      if(correct) then
      if(.not.batch)write(6,1134) nopuse
      write(8,1134) nopuse
1134      format(/1x,i12,' Spots could not be deconvoluted')
      if(.not.batch)write(6,1135) ratrej
      write(8,1135) ratrej
1135      format(/1x,i12,' H change rejections in total')
      if(.not.batch)write(6,1136) rgdrej
      write(8,1136) rgdrej
1136      format(/1x,i12,' H change rejections in good spots')
          IF(OREJ.EQ.1)THEN
              if(.not.batch)write(6,1132)OREJ
              WRITE(8,1132)OREJ
          ELSE
              if(.not.batch)write(6,1133)OREJ
              WRITE(8,1133)OREJ
          END IF
1132    FORMAT(/1X,I12,' J value rejection in total')
1133    FORMAT(/1X,I12,' J value rejections in total')
      if(.not.batch)write(6,1137) ogdrej
      write(8,1137) ogdrej
1137      format(/1x,i12,' J value rejections in good spots',/)
      end if
c
        IF(J.GT.0)CALL WRGEN(J,1)
        DO 520 I=1,10
        IF (IANAL(I).EQ.0) GO TO 520
        RATIO(I)=RATIO(I)/IANAL(I)
        AVSD(I)=AVSD(I)/IANAL(I)
520     CONTINUE
      nolo = nolo - nopuse
        CALL REPORT
        CALL XYSHIFT(0.0,0.0,0,0,XSH,YSH,NSH,2)
        CLOSE (UNIT=3)
        endfile (unit=4)
        REWIND (unit=4)
        if (ltek) CALL ALPHA
        if(.not.batch)write(6,1140) NBAD
        WRITE(IOUT,1140)NBAD
1140    FORMAT(' ',i8,'BAD SPOTS BASED ON BGRATIO or I/SIGI')
        IF (NBAD.EQ.0) GO TO 600
C
        END4 = .FALSE.
        If (Batch) goto 525
        if(.not.batch)write(6,1150)
        WRITE(IOUT,1150)
1150    FORMAT(' DO YOU WANT TO CHECK BAD SPOTS(Y/N)? [No]',$)
        read(5,1153)JUNK
        WRITE(IOUT,1154)JUNK
1153    format(a1)
1154    format(1x,a1)
        icheck = 0
        CALL NOYES(JUNK,Icheck)
        Yes = .false.
        If(Icheck.eq.1) Yes = .true.
        NCHK = NBAD
        If(.not.YES) goto 525
        if(.not.batch)write(6,1156)NCHK
        WRITE(IOUT,1156)NCHK
1156    FORMAT(' NUMBER OF SPOTS TO CHECK ? [',I4,'] ',$)
        read(5,*)JNK
1157    FORMAT(I4)
        WRITE(IOUT,1158)JNK
1158    FORMAT(1H+,37X,I4)
        IF(JNK.NE.0) NCHK = JNK
        IF(YES)GO TO 540
C
C----- FLAG NBAD SPOTS AS REJECTED IN GENVEE FILE
C
525     N = 0
        DO 530 I=1,1000
        READ(4,end=531)KREC,ISPOT
        READ(4,end=531)JNK
        INTEN(I)=ISPOT
        ISD(I)=-9999
        IREC(I)=IABS(KREC)
        N = N+1
530     CONTINUE
        GOTO 590
531     END4 = .TRUE.
        GO TO 590
540     CONTINUE
        IMAP=0
        if(batch) goto 550
        if(.not.batch)write(6,1160)
1160    FORMAT(' DO YOU WANT A PLOT OF EACH SPOT?(Y/N) [No]',$)
        read(5,1153)JUNK
        WRITE(IOUT,1154)JUNK
        iMAP = 0
        CALL NOYES(JUNK,IMAP)
        If(Ltek) CALL TKCLR
550     N=0
C
C
        DO 580 I=1,1000
        READ(4,END=581) KREC,ISPOT,ISDBSI,IBGND,BGRMS,BGRATIO,NXY,NXX,NYY
        READ(4,END=581)(OD(II),II=1,NXY)
        IF(I.GT.NCHK) GOTO 570
        JREC=IABS(KREC)
        CALL GETHKL(JREC,IH)
        if(.not.batch)write(6,1170)
        WRITE(IOUT,1170)
1170    FORMAT(/'  H   K   L    M   X     Y   INTEN   S.D. BGRATIO')
        WRITE(IOUT,1180) IH,ISPOT,ISDBSI,BGRATIO
        if(.not.batch)write(6,1180)IH,ISPOT,ISDBSI,BGRATIO
1180    FORMAT(4I4,4I6,F8.1/)
        IF(IMAP.EQ.1)GO TO 560
        CALL SETMASK(MASK(1,0),LRAS)
        CALL RASPLOT(OD,NXX,NYY,MASK(1,0),-1)
560     CONTINUE
        yes = .true.
        if(batch) goto 570
        if(.not.batch)write(6,1190)
        WRITE(IOUT,1190)
1190    FORMAT(' REJECT (Y OR N)? [Yes]',$)
        read(5,1153)JUNK
        WRITE(IOUT,1154)JUNK
        ijunK = 1
        CALL NOYES(JUNK,ijunK)
        If(ijunk.eq.0) Yes = .false.
        if(ltek)CALL TKCLR
        IF(.NOT.YES) GOTO 580
570      N=N+1
        INTEN(N)=ISPOT
        ISD(N)=-9999
        IREC(N)=IABS(KREC)
580     CONTINUE
581     END4 = .TRUE.
        IF(N.EQ.0)GO TO 600
590     CALL WRGEN(N,1)
C
        If(END4) then
                goto 600
        else
                if(batch.or.icheck.eq.0) then
                        goto 525
                else
                        goto 550
                end if
        END IF
C
600     CONTINUE
        CLOSE (UNIT=4)
        WRITE(IOUT,1220)
        if(.not.batch)write(6,1220)
1220    FORMAT(1X,/,15('@@@@')/)
1230    FORMAT(' MEASUREMENT BOX LARGER THAN EXPECTED; MAXR=',I6/
     +         'NX=',I4,'NY=',I4,'NXY=',I4)
        RETURN
        END
C
C
C                     PROFILE BIN SELECTION ROUTINE
C                 RETURN BIN NUMBER IN NOBIN
C
      SUBROUTINE PRONUM(NOBIN,IX,IY,NECX,NECY,I)
C
      INCLUDE 'comprf.finc'
C
      IXRES = IX - NECX
      IYRES = IY - NECY
      IRESN = IXRES*IXRES + IYRES*IYRES
      IRESN = SQRT(FLOAT(IRESN)) + 0.5
      NOBIN = NUMBIN
      NADD =0
      IF(IRESN.LE.IRAD) RETURN
      IF(IRESN.LE.IRAD2) NADD = 8
      NADD1 = 0
      ANGL =  FLOAT(IXRES)/FLOAT(ABS(IYRES) + 1)
      IF(ABS(ANGL).GT.1.0) NADD1 = 1
      IF(IX-NECX) 1,3,3
1      NOBIN = 1 + NADD + NADD1
      IF(IY-NECY) 5,2,2
2      NOBIN = 3 + NADD + NADD1
      GOTO 5
3      NOBIN = 5 + NADD + NADD1
      IF(IY-NECY) 4,5,5
4      NOBIN = 7 + NADD + NADD1
5      CONTINUE
C
C
      RETURN
      END
C
C
C                     PROFILE BIN SELECTION ROUTINE
C                 RETURN BIN NUMBER IN NOBIN
C
      SUBROUTINE PRONUM17(NOBIN,IX,IY,NECX,NECY,I)
C
      INCLUDE 'comprf.finc'
C
      IXRES = IX - NECX
      IYRES = IY - NECY
      IRESN = IXRES*IXRES + IYRES*IYRES
      IRESN = SQRT(FLOAT(IRESN)) + 0.5
      NOBIN = NUMBIN
      NADD =0
      IF(IRESN.LE.IRAD) RETURN
c      IF(IRESN.LE.IRAD2) NADD = 8
      NADD1 = 0
      ANGL =  FLOAT(IXRES)/FLOAT(ABS(IYRES) + 1)
      IF(ABS(ANGL).GT.1.0) NADD1 = 1
        if((abs(angl).gt.2.0).or.(abs(angl).lt.0.5)) then
            if(numbin.eq.17) nadd = 8
      end if
      IF(IX-NECX) 1,3,3
1      NOBIN = 1 + NADD + NADD1
      IF(IY-NECY) 5,2,2
2      NOBIN = 3 + NADD + NADD1
      GOTO 5
3      NOBIN = 5 + NADD + NADD1
      IF(IY-NECY) 4,5,5
4      NOBIN = 7 + NADD + NADD1
5      CONTINUE
C
C
      RETURN
      END
C
C
C                PROFILE BIN SELECTION ROUTINE
C                 RETURN BIN NUMBER IN NOBIN
C       ***** PRONUM5 SPECIFIC FOR 5 BINS ONLY *****
C
      SUBROUTINE PRONUM5(NOBIN,IX,IY,NECX,NECY,I)
C
      INCLUDE 'comprf.finc'
C
      IXRES = IX - NECX
      IYRES = IY - NECY
      IRESN = IXRES*IXRES + IYRES*IYRES
      IRESN = SQRT(FLOAT(IRESN)) + 0.5
      NOBIN = 5
      IF(IRESN.LE.IRAD) RETURN
      IF(IX-NECX) 1,3,3
1      NOBIN = 1
      IF(IY-NECY) 5,2,2
2      NOBIN = 2
      GOTO 5
3      NOBIN = 3
      IF(IY-NECY) 4,5,5
4      NOBIN = 4
5      CONTINUE
C
C
      RETURN
      END
C
C
C                        SUBROUTINE TO SCALE PROFILES
C
      SUBROUTINE PROSCL(J, IRAS1 ,IRAS2 ,HX,HY)
      INTEGER  HX, HY
C
      INCLUDE 'comuab.finc'
      INCLUDE 'comprf.finc'
      INCLUDE 'combck.finc'
C
C-------------  SCALE IPSC(J)
C
      NXY = IRAS1 * IRAS2
      DO 1 N = 2,NXY+1
      IP(N,J) = NINT((IPSC(J) * XIP(N,J)))
C
C--------------   SET NEGATIVES TO ZERO HERE
C
1      IF(IP(N,J).LT.0) IP(N,J)=0
c      if(.not.batch)write(6,1102)J
c      WRITE(8,1102)J
c1102      FORMAT(1X,' *** BIN N0. ',I3,' SCALED PROFILE ***')      
      WRITE(8,100) IPSC(J)
      if(.not.batch)write(6,100) IPSC(J)
100      FORMAT(1X,'      BIN SCALE FACTOR IS ',F8.4)
C
C---------------  PRINT OUT BACKWARDS AS ON FILM
C
      IST= 2
      IEND = NXY + 1
      IDIR = -IRAS2
      DO 600 IO = -HY,HY
      WRITE(8,601) (IP(IJ,J),IJ=IEND,IST,IDIR)
      if(.not.batch)write(6,602) (IP(IJ,J),IJ=IEND,IST,IDIR)
602      FORMAT(2X,40I3)
601      FORMAT(/1X,40I3)
      IEND = IEND - 1
600      CONTINUE
        RETURN
      END
C
C
C                  SUBROUTINE TO SELECT OD'S IN THE UNEXPANDED AREA
C                               OF THE SPOT FOR PROFILE FITTING.
C
C--------------- ARRAY IP, RAW UNSCALED SPOT, RETURNED IN IP
C                           FOR STANDARDS PROFILE IS
C                  SO FAR UNSCALED AND NOT AVERAGED.
C
C
      SUBROUTINE PROSEL(IBIN,NXX,NYY,TABLE)
C
cRCD Image Plate logical...
        common/iplate/image,mar,moldyn
        logical image,mar,moldyn
C
      INCLUDE 'comhis.finc'
      INCLUDE 'comprf.finc'
      INCLUDE 'comras.finc'
      INCLUDE 'comtem.finc'
C
C       EXTERNAL PEL
      COMMON /PEL/IOD,BUF(5120),nextr
        integer*2 buf,iod
      INTEGER TABLE(0:255),PSUM,BSUM
      dimension nip(5000)
C
      integer i4od
C
C-----------------SELECT ONLY THE OD'S IN THE UNEXPANDED SPOT
C
      NXY   = NXX * NYY
      ISTX  = (NXX - NPX)/2 + 1
      ISTY  = (NYY - NPY)/2 + 1
      IENDX = ISTX + NPX -1
      IENDY = ISTY + NPY -1
      N1ROW = (ISTX-1)*NYY
      N2ROW = (IENDX * NYY)+1
      INY1  = ISTY+NRY-1
      INY2  = IENDY-NRY+1
      NXY1  = N1ROW+(NRX*NYY)
      NXY2  = N2ROW-(NRX*NYY)
C
C----------------ROUGH BG/PEAK POINTS FOR WEAK SPOTS
C----------------WHEN FORMING STANDARDS. UNEXPANDED
C----------------AREA ONLY
C
      NPK = (NPX-2*NRX)*(NPY-2*NRY)
      NBG = 0
C
C----------------------  SUM OD.S FOR GIVEN RASTER SIZE
C                             PUT INTO STANDARD PROFILE
C                                  ARRAY IP(5000,IBIN)
      NMBN = NUMBIN + 1
      PSUM = 0
      BSUM = 0
      KIP = 0
      J = 0
        DO 898 K=1,NXY
        if(.not.image)then
        CALL CBYTE(K)
        else
        call ip_cbyte(k)
        endif
C
C--------------------SELECT REQUIRED DENSITIES
C                   WITHIN UNEXPANDED AREA ONLY FOR PROFILES
C
      IF((K.LE.N1ROW).or.(K.GE.N2ROW)) GOTO 898
      J = J + 1
      IF (J.LT.ISTY) GOTO 898
      IF (J.GT.IENDY) GOTO 897
      KIP = KIP + 1
C
C--------------------  MAKE IOD TABLE(IOD) FOR REAL PROCESSING
C--------------------  check for overloads when forming standards
C
        i4od = abs(int(iod))
c
      IF(IBIN.NE.NMBN) Then
C
            if(.not.image)then
            if(TABLE(iod).ge.IMAXOD) Then
                  ibin = -1
                  RETURN
            End If
            else
                if(i4od.ge.imaxod)then
                        ibin = -1
                        RETURN
                endif
            endif
C
C            ---- PSUM IS TOTAL COUNT IN SPOT AREA
C            ---- BSUM IS TOTAL COUNTS OVER BG AREA
C            ---- BOTH FOR UNEXPANDED SPOT ONLY
C            ---- NEEDED IN SPOT SELECTION FOR STANDARDS
C
C            -----  PUT IOD INTO NIP TEMPORARILY FOR STANDARDS
C            -----  IN CASE WE THROW SPOT OUT FROM STANDARD SUM
C
C
            if(.not.image)then
            PSUM = PSUM + TABLE(IOD)
            IF( (J.LE.INY1).OR.(J.GE.INY2).OR.
     1          (K.LE.NXY1).OR.(K.GE.NXY2) ) then
                     BSUM = BSUM + TABLE(IOD)
                   nbg = nbg + 1
            end if
            NIP(KIP) = TABLE(IOD)
            else
            PSUM = PSUM + i4od
            IF( (J.LE.INY1).OR.(J.GE.INY2).OR.
     1          (K.LE.NXY1).OR.(K.GE.NXY2) ) then
                     BSUM = BSUM + i4od
                   nbg = nbg + 1
            end if
            NIP(KIP) = i4od
            endif
C
C
      Else
            if(.not.image)then
            IP(KIP+1,IBIN) = IP(KIP+1,IBIN) + TABLE(IOD)
            else
                ip(kip+1,ibin) = ip(kip+1,ibin) + i4od
            endif
      End If
C
  897      IF (J.EQ.NYY) J = 0
  898      CONTINUE
      IF(KIP.NE.NPXY) STOP 'STOP AT 898 IN PROSEL: KIP NE NPXY'
C
C------------------ CHECK FOR WEAK SPOT
C------------------ WHEN FORMING STANDARDS
C
            if(ibin.eq.NMBN) Return
          BAV = FLOAT(BSUM) / FLOAT(NBG)
          IAPX = PSUM - FLOAT(NPXY) * BAV      
c
c      include in background histogram
c
      call hisbck(1,1,BSUM,nbg,2)
c
          IF(IAPX.LT.NPWK) GOTO 910
C
C------------------ GOOD SPOT, INCLUDE IN STANDARD
C
      DO 900 K = 1,NPXY
900      IP(K+1,IBIN) = IP(K+1,IBIN) + NIP(K)
      RETURN
C
C------------------- FLAG OVERLOADS AND WEAKSPOTS BY IBIN=-1
C------------------- ONLY WHEN FORMING STANDARDS
C
910      ibin = -1
      RETURN
      END
C
C
C                        SUBROUTINE TO SET UP PROFILES
C                       FOR STANDARDS. READING B AT P,Q
C
C                     IPROB(P,Q) = J*(B - Pa - Qb - c)/J*J
C
      SUBROUTINE PROSET(J, B,XX, YY, SIGN)
      INTEGER XX, YY, SIGN
C
      INCLUDE 'comprf.finc'
      INCLUDE 'combck.finc'
C
C-------------  PUT IPJ(J) INTO THE SCALING
C
      XP = FLOAT(XX)
      YP = FLOAT(YY)
      B = B - XP*A(J) - YP * BZ(J) - C(J)
C
C-------------SCALE BY 10000. TO RETURN B
C
      B =  B * 10000.
      IF(B.LE.0) RETURN
C
C---------------   DONT USE BG POINTS FOR SCALING
C
      IF(SIGN.LE.0) RETURN
      BSCAL = 1000000./B
      IF(BSCAL. LT. IPSC(J)) IPSC(J) = BSCAL
C
C--------------  GET NUMBER OF POINTS IN PEAK FOR LATER
C
      IF(J.NE.1) RETURN
      PN = PN + 1
      PPSUM = PPSUM + XP * XP
      QQSUM = QQSUM + YP * YP
      RETURN
      END
C
C
C           SUBROUTINE TO GET CONSTANTS FOR THIS SPOT
C
      SUBROUTINE PROSIG(DELX,DELY,TP,TB,RMSBG,BGND)
C
      INCLUDE 'comuab.finc'
      INCLUDE 'combck.finc'
      INCLUDE 'comprf.finc'
      INCLUDE 'commask.finc'
      INCLUDE 'commaskl.finc'
C
      NMBN = NUMBIN + 1
      TP = FLOAT(PN)
      if (streak) then
            tp = rsums(5)
      end if
      IF(TP.gt.0) GOTO 3112
      if(.not.batch)write(6,3111) tp
      WRITE(8,3111) tp
3111      FORMAT(/1X,' Problem in prosig: TP is ', f10.3,/)
3112      IF(.NOT.CHANGE(NMBN)) BN(NMBN) = AMST(3,3)
      TB = FLOAT(BN(NMBN))
      IF(TB.NE.0) GOTO 312
      if(.not.batch)write(6,311)
      WRITE(8,311)
311      FORMAT(/1X,' STOP AT RMSBG .. /TB IN PROSIG',
     1               ' TB IS ZERO',/)
      STOP
312      CONTINUE
      RMSBG = SQRT(FLOAT(SIGMA(NMBN))/TB)
      IF(rmsbg.GT.0.0) then
        if(streak) then
        if(tb.gt.5.and.tb.le.rsums(6)) GOTO 3122
        else
        if(tb.gt.5.and.tb.le.amst(3,3)) GOTO 3122
        end if
      end if
      if(.not.batch)write(6,3121) rmsbg,tb,tp
      WRITE(8,3121) rmsbg,tb,tp
3121      FORMAT(1X,' Problem in prosig: RMSBG,TB,TP are: ',
     1              3f8.3)
3122      BGND = BO(NMBN)+20000.0
      IF(INTPRO.LE.0) RETURN
      pps = ppsum
      qqs = qqsum
      if (streak) then
       pps = rsums(1)
       qqs = rsums(3)
      end if
      DELX = (PODSUM - A(NMBN)*PPS)/FLOAT(INTPRO)
      DELY = (QODSUM - BZ(NMBN)*QQs)/FLOAT(INTPRO)
      RETURN
      END
C
C
C            PROFILE VARIABLE WEIGHTING CONSTANT
C
      SUBROUTINE PROVAR
C
      INCLUDE 'comuab.finc'
      INCLUDE 'comprf.finc'
C
      DIMENSION AVAR(18)
      REAL WT
C
C------------------ LOOP OVER BINS
C
      NMBN = NUMBIN - 1
      if(numbin.eq.17) nmbn = 8
      DO 1 IK=1,NMBN      
      if(numbin.eq.17) then
            ixres = ixbin(ik) - ixbin (ik+8)
            iyres = iybin(ik) - iybin (ik+8)
            ixres = ixres * ixres
            iyres = iyres * iyres
        else
            IXRES = IXBIN(IK) * IXBIN(IK)
            IYRES = IYBIN(IK) * IYBIN(IK)
      end if
      WT =  FLOAT(IXRES + IYRES)
C
C---------------- GET AVAR = .05 AT OUTER BIN CENTRES
C
      AVAR(IK) = 3.0/WT
1      CONTINUE
C
C-------------------  LOOK AT SOME RESULTS
C
      SUMW = 0
      DO 2 IK = 1,NMBN
      SUMW = SUMW + AVAR(IK)
      WRITE(8,3)IK, AVAR(IK)
      if(.not.batch)write(6,3)IK, AVAR(IK)
2      CONTINUE
      ACONS = SUMW/FLOAT(NMBN)
      WRITE(8,4) ACONS
      if(.not.batch)write(6,4) ACONS
3      FORMAT(2X,' BIN ',I3,' Weighting Constant ACONS = ',F12.8)
4      FORMAT(/1X,'                    Average ACONS = ',F12.8)
      RETURN
      END
C
C
C                     PROFILE VARIABLE WEIGHTING
C
      SUBROUTINE PROWT(IX,IY,NECX,NECY,I,MASK)
C
      INCLUDE 'comprf.finc'
C
      INTEGER MASK(1)
      REAL SUMW,SUMWP,WT,MAX,EXWT(18)
      IPW(1) = NPX * 256 + NPY
        IJ=0
      MAX = -999.
      IXSH = IX - NECX
      IYSH = IY - NECY
      NMBN = NUMBIN + 1
C
C------------------ SET UP WTS
C
      SUMW = 0.0
      ibin2 = numbin
      if(numbin.eq.17) then
            if(nob(nmbn).ne.17) ibin2 = 16
      end if
      DO 10 IK = 1,ibin2
      IXRES = IXBIN(IK) - IXSH
      IYRES = IYBIN(IK) - IYSH
      WT = - ACONS * FLOAT(IXRES*IXRES + IYRES*IYRES)      
      EXWT(IK) = EXP(WT)
      SUMW = SUMW + EXWT(IK)
10      continue
C
C------------------ LOOP OVER PROFILE
C
      DO 40 IPRO = 2,NPXY+1
      SUMWP = 0.0
      IJ = IJ + 1
C
C----------------  ONLY FOR PEAK
C
      IF (MASK(IJ)) 40,40,20
C
C-----------------  LOOP OVER BINS
C
20      CONTINUE
      DO 30 IK = 1,ibin2
      SUMWP = SUMWP + EXWT(IK) * FLOAT(IP(IPRO,IK))
30      CONTINUE
C
C------------------ USE XIP(..,NUMBIN+1) FOR SCALING
C
      XIP(IPRO,NMBN) = SUMWP/SUMW
      IF(XIP(IPRO,NMBN).GE.MAX) MAX = XIP(IPRO,NMBN)
40      CONTINUE
C
C------------------- NOW SCALE
C
      DO 50 IPRO = 2,NPXY+1
      IPW(IPRO) = NINT(100. * XIP(IPRO,NMBN)/MAX)
50      CONTINUE
C
      RETURN
      END
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE PWRITE  ****      *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
      SUBROUTINE PWRITE(IREC,IA,IB,L,IU)
      INTEGER*2 IA(4),IB(L)
      WRITE(IU)irec,L,IA
      WRITE(IU)IB
      RETURN
      END
c
c  Program designed to create a mask for radial streaks
c
      subroutine radmask(irecm,ixmin,iymin,nxm2,nym2,ispotr,odm)
C
      INCLUDE 'comras.finc'
      INCLUDE 'comscn.finc'
      INCLUDE 'comori.finc'
      INCLUDE 'commcs.finc'
      INCLUDE 'commask.finc'
      INCLUDE 'commaskl.finc'
C
      integer odm(5000)
      integer tem(5000)
c
      logical output
c
c  Get xcen,ycen from common ORI, both in 10micron units positive
c  incoming ixmin,iymin are in rasters, positive for irecm=1
c                           in rasters about centre for irecm=-1
c  convert to rasters about centre using fact (0.2 for 50micron)
c
c      eg, xcen,ycen 6000,6000
c           ixmin,iymin   200,1700
c           fact = 0.2
c
      ixm = ixmin
      iym = iymin
      if(irecm.eq.1) then
            ixm = anint(  (float(ixmin) - xcen*fact)  )
            iym = anint(  (float(iymin) - ycen*fact)  )
      end if
c
c LATER ? get NXS and NYS (box size) from common RAS
c
       nxm = nxm2/2
       nym = nym2/2
c
c cmask is constant eta*D or FULL spot size (mm) for k=0
c bminor is full width in mm
c dmask is cf in mm
c kmask is scale: 0 for constant, 1 for MMH
c all read in in mosuab and converted in procuab
c Set mask array to zero initially
c
      do 1 j=1,5000
1     rmask(j) = 0
      do 11 j=1,6
11      rsums(j) = 0.0
c
      pi = 4.0*atan(1.0)
c
c seekrs finds values for a & b as follows
c
c       bminor = bminor/2.0*100.*fact
c       cmask = cmask/2.0*100.*fact
c       dmask = dmask*100.*fact
c
         jxstart = ixm - nxm
         jxend   = ixm + nxm
         kystart = iym - nym
         kyend   = iym + nym
      if (ixm.eq.0) then
         theta = pi/2.0
         if(iym.lt.0)then
         theta = 1.5*pi
         end if
         goto 1001
      end if
c
      theta = atan(float(iym)/float(ixm))
      if (ixm.lt.0) then
           theta = theta + pi
      else
         if(iym.lt.0)then
           theta = theta + 2.0*pi
         end if
      end if
1001      continue
      radius = sqrt(float(ixm**2) + float(iym**2))
      if (binvar) then
         angle = theta - themax
         anew = ammin**2*sin(angle)**2+ammax**2*cos(angle)**2
         amajor = sqrt(anew) *(1. + kmask*radius/dmask)
      else
         amajor = cmask *(1. + kmask*radius/dmask)
      end if
      imark = 0
c
c ods are stored as if film scanned backwards (IDIR = -1)
c An old throwback from interactive work.
c RMASK must therefore be stored the same way.
c Printing of masks, spots, profiles etc by RASPLOT, PROSCL
c and here are now the same. Eventually need to use IDIR since
c not sure which way spot refinement works.
c
            do j=jxend,jxstart,-1
      do k=kystart,kyend
      imark = imark + 1
         xprime = (float(j)-ixm)*cos(theta)
         xprime = xprime + (float(k)-iym)*sin(theta)
         yprime = (float(k)-iym)*cos(theta)
         yprime = yprime - (float(j)-ixm)*sin(theta)
         abg    = amajor + 1.0 + border
         bbg    = bminor + 1.0 + border
         ellipb = (xprime**2/abg**2) + (yprime**2/bbg**2)
         if (ellipb.le.1.0) then
           aig    = amajor + 1.0
           big    = bminor + 1.0
           ellipi = (xprime**2/aig**2) + (yprime**2/big**2)
           if(ellipi.le.1.0)then
             ellip  = (xprime**2/amajor**2) + (yprime**2/bminor**2)
             if(ellip.le.1.0)then
c PEAK
      rsums(1) = rsums(1) + (jxend-j-nxm)*(jxend-j-nxm)
      rsums(3) = rsums(3) + (k-kystart-nym)*(k-kystart-nym)
      rsums(5) = rsums(5) + 1
               rmask(imark) = 1
               goto 1002
             end if
             goto 1002
           end if
C BACKGROUND
      rsums(2) = rsums(2) + (jxend-j-nxm)*(jxend-j-nxm)
      rsums(4) = rsums(4) + (k-kystart-nym)*(k-kystart-nym)
      rsums(6) = rsums(6) + 1
           rmask(imark) = -1
         end if
1002  end do
      end do
c
      if(ispotr.eq.9111) then
            ii = 0 
            do 990 i=1,nxm2
            kst = nxm2*nym2-(i*nym2)+1
            kend = kst + nym2 - 1
            do 989 j=kst,kend
            ii = ii + 1
989            tem(ii) = rmask(j)
990            continue
            nxy2 = (nxm2*nym2) + 1
            do 991 j=1,nxy2
991            rmask(j) = tem(j)
      end if
c
c      call cputime(1)
c
c Write mask array to output file
c
c      output = .true.
c       if(output)then
c      if(irecm.ge.0.and.ispotr.lt.1000) goto 964
c      write(8,975)nxm2,nym2,ixm,iym
c975      format(1h1,' box size ',2i8,6x,' spot at ',2i8)
c      if(.not.batch)write(6,976)nxm2,nym2,ixm,iym
c976      format(1x,' box size ',2i8,6x,' spot at ',2i8)
c
c      write(8,9751) rsums
c      if(.not.batch)write(6,9751) rsums
c9751      format(1x,' Rsums = ',6f10.0)
c
c      do 990 i=1,nym2
c      kst = nxm2*nym2-i+1
c      kend = nym2-i+1
c      write(8,981) (rmask(k),k=kst,kend,-nym2)
c      if(.not.batch)write(6,981) (rmask(k),k=kst,kend,-nym2)
c981      format(1x,34i3)
c990      continue
c
c      if(.not.batch)write(6,982)
c      write(8,982)
c982      format(1x,//)
c
c      do 991 i=1,nym2
c      kst = nxm2*nym2-i+1
c      kend = nym2-i+1
c      write(8,981) (odm(k),k=kst,kend,-nym2)
c      if(.not.batch)write(6,981) (odm(k),k=kst,kend,-nym2)
c991      continue
c
c        end if
c      if(.not.batch)write(6,2601) ispotr
c      write(8,2601) ispotr
c2601      format(/1x,' Square  mask I = ',I8)
c964      continue
c
c      do 990 i=1,nym2
c      kst = nxm2*nym2-i+1
c      kend = kst + (nxm2-1)*nym2
c      write(8,981) (rmask(k),k=kst,kend,nym2)
c      if(.not.batch)write(6,981) (rmask(k),k=kst,kend,nym2)
c990      continue
c
c      do 991 i=1,nym2
c      kst = i
c      kend = kst + (nxm2 - 1)*nym2
c      write(8,981) (odm(k),k=kst,kend,nym2)
c      if(.not.batch)write(6,981) (odm(k),k=kst,kend,nym2)
c991      continue
      return
      end
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE RASPLOT ****      *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
        SUBROUTINE RASPLOT(BB,IWX,IWY,MASK,IDR)
C
      INCLUDE 'comuab.finc'
      INCLUDE 'comsnd.finc'
      INCLUDE 'commaskl.finc'
C
        INTEGER BB(1),MASK(1)
        CHARACTER*1 IA(72),ISYMB(-1:1),KSYMB
        COMMON/IOO/IOUT
        DATA ISYMB/'-','*',' '/
        DATA KSYMB/' '/
        CHARACTER*1 ICHAR(0:36)
        DATA ICHAR/'0','1','2','3','4','5','6','7','8','9',
     1    'A','B','C','D','E','F','G','H','I','J','K','L',
     1    'M','N','O','P','Q','R','S','T','U','V','W','X',
     1    'Y','Z',']'/
C
C---------------------------- SET SYMBOLS
C
        DO 1 I=1,72
1       IA(I)=KSYMB
        If (Batch) goto 2
C        OPEN(UNIT=7,FILE='TT:',STATUS='UNKNOWN',DISPOSE='DELETE')
2       NXY=IWX*IWY
        N=NXY
        IF(IDR.EQ.1) N=IWY
        If (.not.Ltek) goto 3
        IF(.NOT.XTEK) CALL TKCLR
3       DO 6 J=1,IWY
        IJ=N-J+1
        JJ=1
        DO 4 I=1,IWX
        NUM=(BB(IJ)*36)/255
        IA(JJ)=ISYMB(MASK(IJ))
        IA(JJ+1)=ICHAR(NUM)
      if (streak) then
            if(mode.eq.1) then
                  if(ia(jj).eq.isymb(0)) then
                         ia(jj+1)=isymb(0)
                  end if
                  ia(jj) = ksymb
            end if
      end if
c
        JJ=JJ+2
        IJ=IJ+IDR*IWY
4       CONTINUE
        IF(XTEK) WRITE(6,1000) IA
        IF((.NOT.ltek).AND.(.NOT.xtek).AND.(.NOT.batch)) WRITE(6,1000)IA
1000    FORMAT(1X,72A1)
        If (.not.Ltek) goto 5
        IF(.NOT.XTEK) WRITE(6,1001) IA
1001    FORMAT(9X,72A1)
5       WRITE(IOUT,1002)IA
1002    FORMAT(20X,72A1)
6       CONTINUE
C        CLOSE (UNIT=7)
        RETURN
        END
C
      SUBROUTINE RDBLK(IUNIT,IBLK,IBA,LEN,IER)
C     READ A RECORD FROM THE DA DISK FILE OF DIGITIZED DATA
cRCD ...Image Plate logical
      common/iplate/image,mar,moldyn
      logical image,mar,moldyn
c
cRCD ...Set up temps and masks for byte swapping and reflection of stripe
      INTEGER*2 IBA(LEN),temp(5120),temp1,temp2,maska,maskb
      data maska,maskb/'377'o,'177400'o/
c
C   C   JUNK=IBLK+IER
      IER=1
cRCD ...Standard film read
      if(.not.image)then
      READ(IUNIT,REC=IBLK,ERR=90)IBA
C      FIND(IUNIT,REC=JUNK)
      RETURN
      else
cRCD ...Image Plate read - swap bytes if necessary
      read(iunit,rec=iblk,err=90)IBA
      do 10 i=1,len
c          temp1 = ishft(iand(iba(i),maska),8)
c          temp2 = ishft(iand(iba(i),maskb),-8)
c          temp(i) = ior(temp1,temp2)
         temp(i) = iba(i)
10    continue
cRCD ...Reverse order of stripe if necessary
      do 20 i=1,len
c         ii=len-i+1
         ii=i
         iba(i) = temp(ii)
20    continue
      return
      endif
   90 IER=-1
      write(6,*)' ***** ERROR IN RDBLK *****'
      write(6,91) IBLK,LEN
91      format(1x,' IBLK = ',i7,'  LEN = ',i7)
      STOP
      END
C
C
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE RDIST  ****       *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C
C            ****  SUBROUTINE RDIST  ****
C
C REFINES FILM CENTRE, FILM ORIENTATION, CRYSTAL TO FILM DISTANCE, YSCAL
C AND FILM DISTORTIONS FOR FLAT FILMS USING A NON-LINEAR LEAST SQUARES
C METHOD AND REPORTS NEW FIDUCIAL CONSTANTS
C
C
c##### laue mods - spot information taken from common/laue
C=============================================================
C
C
        SUBROUTINE RDIST
c
cRCD ...Image Plate Logical
        common/iplate/image,mar,moldyn
        logical image,mar,moldyn
c
        COMMON/IOO/IOUT
        COMMON/SW/ISWTCH(10),oneref,noref,tworef
C
      INCLUDE 'comori.finc'
      INCLUDE 'comfid.finc'
      INCLUDE 'comrfs.finc'
      INCLUDE 'commcs.finc'
      INCLUDE 'comuab.finc'
      INCLUDE 'comlaue.finc'
C
        INTEGER XGEN(512),YGEN(512)
        REAL CLCX(512),CLCY(512)
        REAL RF(100),R(10),RR(10),W(10),DDX(10),DDY(10)
        LOGICAL BAD(512),oneref,noref,tworef
c##### laue mods
c      integer buf(10)
c      EQUIVALENCE (BUF(3),IX),(BUF(4),IY)
C
cRCD ...Define variables for AGWL's MOSFLM IP refinement
        real cpsi,spsi
C
C----------------  COORDINATES USED IN THIS OVERLAY
C
C---- (CLCX,CLCY) ARE FROM THE GENERATE-FILE AND THEREFORE
C                 IN 10 MICRON UNITS RELATIVE TO THE CENTRE
C                 OF THE DIFFRACTION PATTERN
C--- (XRS,YRS)   ARE IN 10 MICRON UNITS (FROM OVERLAY SEEKRS OR NEXT)
C
C--- LEAST SQUARES REFINEMENT OF XTOFRA,XCEN,YCEN AND OMEGA0
C--- DONE BY FITTING OBSERVED POSITIONS (XRS,YRS) TO THOSE
C--- IN THE GENERATE-FILE (CLCX,CLCY)
C
C--- NO. OF PARAMETERS TO REFINE
        NP=8
cRCD ...Allow for 9 variables of MAR IP (AGWL's MOSFLM)
        if(mar)np=9
      if(moldyn) np=7
        NP2=NP*NP
        OMEGA0=ASIN(SINOM0)
        NOTRY=0
      if(.not.noref) then
        WRITE(IOUT,1000) NRS
        if (.not.batch)WRITE(6,1000)NRS
 1000   FORMAT(/' FILM DISTORTION REFINEMENT USING',I3,' SPOTS'/)
C
      else
        WRITE(IOUT,2003) NRS
        if (.not.batch)WRITE(6,2003)NRS
 2003   FORMAT(/' CALCULATE RESIDUALS FOR ',I3,' UNREFINED SPOTS'/)
      endif
        NXD=ANINT(CTOFD*XTOFRA)
        ITILT=TILT/FDIST
        ITWIST=TWIST/FDIST
        if(.not.image)then
        IBULGE=BULGE/FDIST
        WRITE(IOUT,2005)XCEN,YCEN,XTOFRA,NXD,YSCAL,ITILT,ITWIST,
     1      IBULGE
        if (.not.batch)
     1  WRITE(6,2005)XCEN,YCEN,XTOFRA,NXD,YSCAL,ITILT,ITWIST,
     1      IBULGE
        else if (mar) then
        WRITE(IOUT,2007)XCEN,YCEN,XTOFRA,NXD,YSCAL,ITILT,ITWIST,
     1      roff,toff
        if (.not.batch)
     1  WRITE(6,2007)XCEN,YCEN,XTOFRA,NXD,YSCAL,ITILT,ITWIST,
     1      roff,toff
      else if (moldyn) then
        WRITE(IOUT,2004)XCEN,YCEN,XTOFRA,NXD,YSCAL,ITILT,ITWIST
      write(iout,2002)spdx_min,spdx_max,(spdx(ni),ni=1,nspdx)
      write(iout,2012)spdy_min,spdy_max,(spdy(ni),ni=1,nspdy)
      write(iout,2013)spdxy
        if (.not.batch) then
        WRITE(6,2004)XCEN,YCEN,XTOFRA,NXD,YSCAL,ITILT,ITWIST
      write(6,2002)spdx_min,spdx_max,(spdx(ni),ni=1,nspdx)
      write(6,2012)spdy_min,spdy_max,(spdy(ni),ni=1,nspdy)
      write(6,2013)spdxy
      endif
        endif
2007    FORMAT(/' STARTING PARAMETERS:  ',/
     1    2X,'    XCEN    YCEN  XTOFRA   XTOFD  YSCALE    TILT   TWIST'
     2        ,'   Roff    Toff',/2X,2I8,F8.4,I8,F8.4,2I8,2f7.1/)
2005    FORMAT(/' STARTING PARAMETERS:  ',/
     1    10X,'    XCEN    YCEN  XTOFRA   XTOFD  YSCALE    TILT   TWIST'
     2        ,' BULGE',/10X,2I8,F8.4,I8,F8.4,3I8/)
2004    FORMAT(/' STARTING PARAMETERS:  ',/
     1    10X,'    XCEN    YCEN  XTOFRA   XTOFD  YSCALE    TILT   TWIST'
     2        ,/10X,2I8,F8.4,I8,F8.4,2I8/)
2002      format(1x,' IP x  distortion: ',2f12.2,/(8f10.7))
2012      format(1x,' IP y  distortion: ',2f12.2,/(8f10.7))
2013      format(1x,' IP xy distortion: ',f10.7,/)
c
      ang0 = omega0 * 57.296
      angf = omegaf * 57.296
        CCOM=(OMEGA0-OMEGAF)*57.296
        WRITE(IOUT,2006) CCX,CCY,CCOM,
     1                 angf,ang0,sinom0,cosom0
        if (.not.batch)WRITE(6,2006)CCX,CCY,CCOM,
     1                 angf,ang0,sinom0,cosom0
2006    FORMAT(' STARTING CAMERA CONSTANTS.'/' CCX: ',I8/
     1      ' CCY: ',I8/' CCOM:',F8.4/
     1      ' wF : ',f8.4/' w0  :',F8.4/
     1      ' sinw:',f8.4/' cosw:',F8.4/)
C
C------------------  READ COORDS FROM GENERATE FILE
c##### laue mods - coords from core
c
C
        DO 1 J=1,NRS
        IR=RRS(J)
c        READ(1,REC=IR) BUF
c        XGEN(J)=IX
c        YGEN(J)=IY
      xgen(j) = xyge(1,ir)
      ygen(j) = xyge(2,ir)
        BAD(J)=.FALSE.
1       CONTINUE
        GOTO 10
2       WRITE(IOUT,1001) RMSRES
        if (.not.batch)WRITE(6,1001)RMSRES
1001    FORMAT('  STARTING RESIDUAL=',F5.1)
        ESTART=RMSRES
      if(noref) return
3       CONTINUE
C
C--------------------- START LOOP FOR NON-LINEAR REFINEMENT
C
        DO 9 ICYC=1,2
C
C------------------------ INITIALIZE SUMS
C
        DO 4 K=1,NP
4       W(K)=0.0
        DO 5 K=1,NP2
5       RF(K)=0.0
C
C------------------------- SET UP NORMAL EQUATIONS
C
        DO 7 J=1,NRS
        IF(BAD(J)) GOTO 7
        XG=float(XGEN(J))
        YG=float(YGEN(J))
        RG=SQRT(XG*XG+YG*YG)
        XM=XG*COSOM0-YG*SINOM0
        YM=YG*COSOM0+XG*SINOM0
        if(rg.eq.0.0)then
        cpsi = 1.0
        spsi = 0.0
        else
        cpsi = xm/rg
        spsi = ym/rg
        endif
        dxtof = xtofra+tilt*xg+twist*yg+bulge*rg
        if(image) dxtof = xtofra+tilt*xg+twist*yg
c xcen
        DDX(1)=1.0
        DDY(1)=0.0
c ycen
        DDX(2)=0.0
        DDY(2)=1.0
c omega
        DDX(3)=-ym*dxtof
        DDY(3)=xm*yscal*dxtof
c yscal
        DDX(4)=0.0
        DDY(4)=ym*dxtof
c xtofra
        DDX(5)=xm
        DDY(5)=ym*yscal
c tilt
        DDX(6)=xg*xm
        DDY(6)=xg*ym*yscal
c twist
        DDX(7)=yg*xm
        DDY(7)=yg*ym*yscal
c bulge
        if(.not.image)then
        DDX(8)=rg*xm
        DDY(8)=rg*ym*yscal
c roff,toff
        else if (mar) then
        ddx(8)=cpsi
        ddy(8)=spsi
        ddx(9)=-spsi
        ddy(9)=cpsi
        endif
C
      if(moldyn) then
            call xymd (xc1,yc1,xg,yg)
      else
        CALL XYCALC(XC1,YC1,XG,YG)
      endif
        XO=float(XRS(J))-XC1
        YO=float(YRS(J))-YC1
        IK=0
        DO 6 I=1,NP
        W(I)=W(I)+DDX(I)*XO+DDY(I)*YO
        DO 6 K=1,NP
        IK=IK+1
        RF(IK)=RF(IK)+DDX(I)*DDX(K)+DDY(I)*DDY(K)
6       CONTINUE
7       CONTINUE
C
C-----------------------------  SOLVE
C
        CALL MINV(RF,NP,DET,R,RR)
        IK=0
        DO 8 I=1,NP
        R(I)=0.0
        DO 8 K=1,NP
        IK=IK+1
        R(I)=R(I)+RF(IK)*W(K)
8       CONTINUE
C
C------------------------------ UPDATE PARAMETERS
C
        XCEN=XCEN+nint(R(1)+.5)
        YCEN=YCEN+nint(R(2)+.5)
        OMEGA0=OMEGA0+R(3)
        YSCAL=YSCAL+R(4)
        XTOFRA=XTOFRA+R(5)
        TILT=TILT+R(6)
        TWIST=TWIST+R(7)
        if(.not.image) then
          BULGE=BULGE+R(8)
        else if (mar) then
          roff = roff+R(8)
          toff = toff+r(9)
      end if
        SINOM0=SIN(OMEGA0)
        COSOM0=COS(OMEGA0)
9       CONTINUE
C
C----------------------- END OF NON-LINEAR REFINEMENT LOOP
C
C--------------------- CALCULATE STD.DEVS. AND TEST RESIDUALS
C
        NOTRY=NOTRY+1
10      RESMX=0
        RESD=0.0
C
C---------------------------START LOOP
C
        DO 11 J=1,NRS
        XG=float(XGEN(J))
        YG=float(YGEN(J))
        CALL XYCALC(CLCX(J),CLCY(J),XG,YG)
        IF(BAD(J)) GOTO 11
        DX=CLCX(J)-float(XRS(J))
        DY=CLCY(J)-float(YRS(J))
        RES=DX*DX+DY*DY
        RESD=RESD+RES
        IF(RES.LT.RESMX) GOTO 11
        RESMX=RES
        MXJ=J
11      CONTINUE
C
C-----------------------  END LOOP. TEST RESIDUALS
C
        RESMX=SQRT(RESMX)
        RMSRES=SQRT(RESD/(NRS-NOTRY+1))
        RESLIM=2.*RMSRES
        IF(NOTRY.EQ.0) GOTO 2
        IF(NOTRY.EQ.1) WRITE(IOUT,1002) RMSRES
        IF(NOTRY.EQ.1.AND..NOT.BATCH)WRITE(6,1002)RMSRES
1002    FORMAT('  RESIDUAL AFTER 1 CYCLE=',F5.1)
        IF(NOTRY.GT.NRS/10) GOTO 12
        IF(RESMX.LT.RESLIM) GOTO 12
        BAD(MXJ)=.TRUE.
        GOTO 3
12      CONTINUE
C
C---------------------------- PRINT OUT
C
C       IF(NOTRY.GT.1) WRITE(IOUT,1003)
        IF(XTEK) CALL NEWPG
1003    FORMAT(/'  REJECTS (ERROR>2*RMS RESID.)'
     1  /'     XOBS    XCAL    DELX    YOBS    YCAL    DELY ')
        If(.Not.Ltek) goto 13
        IF(.NOT.XTEK) CALL TKCLR
13      CONTINUE
        DO 14 J=1,NRS
        DX=float(XRS(J))-CLCX(J)
        DY=float(YRS(J))-CLCY(J)
        If (Ltek) CALL RESIDPL(CLCX(J),CLCY(J),DX,DY)
14      CONTINUE
        If (Ltek) CALL TKPLOT(0,0,1,IER)
        IF(XTEK) CALL NEWPG
        IF(NOTRY.GT.1)WRITE(IOUT,1003)
        IF(NOTRY.GT.1.AND..NOT.BATCH)WRITE(6,1003)
        DO 15 J=1,NRS
        IF(.NOT.BAD(J))GO TO 15
        DX=float(XRS(J))-CLCX(J)
        DY=float(YRS(J))-CLCY(J)
      nxobs = xrs(j) - xcen
      nyobs = yrs(j) - ycen
      nxcal = nint(clcx(j)) - xcen
      nycal = nint(clcy(j)) - ycen
        if (.not.batch)WRITE(6,1004)nxobs,nxcal,dx,nyobs,nycal,dy
        WRITE(IOUT,1004) nxobs,nxcal,dx,nyobs,nycal,dy
1004    FORMAT(' ',2(1x,i7,1x,i7,2x,F6.1))
15      CONTINUE
        NXD=ANINT(CTOFD*XTOFRA)
        ITILT=TILT/FDIST
        ITWIST=TWIST/FDIST

        if(.not.image)then
        IBULGE=BULGE/FDIST
        WRITE(IOUT,1005)RMSRES,XCEN,YCEN,XTOFRA,NXD,YSCAL,ITILT,ITWIST,
     1      IBULGE
        if (.not.batch)
     1  WRITE(6,1005)RMSRES,XCEN,YCEN,XTOFRA,NXD,YSCAL,ITILT,ITWIST,
     1      IBULGE
        else if (mar) then
        WRITE(IOUT,1007)RMSRES,XCEN,YCEN,XTOFRA,NXD,YSCAL,ITILT,ITWIST,
     1      roff,toff
        if (.not.batch)
     1  WRITE(6,1007)RMSRES,XCEN,YCEN,XTOFRA,NXD,YSCAL,ITILT,ITWIST,
     1      roff,toff
        else 
        WRITE(IOUT,1008)RMSRES,XCEN,YCEN,XTOFRA,NXD,YSCAL,ITILT,ITWIST
        if (.not.batch)
     1  WRITE(6,1008)RMSRES,XCEN,YCEN,XTOFRA,NXD,YSCAL,ITILT,ITWIST
        endif

1008    FORMAT(/' FINAL RMS RESIDUAL:  ',F8.1/
     1    2X,'    XCEN    YCEN  XTOFRA   XTOFD  YSCALE    TILT   TWIST'
     2        ,/2X,2I8,F8.4,I8,F8.4,2I8/)
1007    FORMAT(/' FINAL RMS RESIDUAL:  ',F8.1/
     1    2X,'    XCEN    YCEN  XTOFRA   XTOFD  YSCALE    TILT   TWIST'
     2        ,'   Roff    Toff',/2X,2I8,F8.4,I8,F8.4,2I8,2f7.1/)
1005    FORMAT(/' FINAL RMS RESIDUAL:  ',F8.1/
     1    10X,'    XCEN    YCEN  XTOFRA   XTOFD  YSCALE    TILT   TWIST'
     2        ,' BULGE',/10X,2I8,F8.4,I8,F8.4,3I8/)
C
C------------- PRINT REFINED CAMERA CONSTANTS
C
        CCXs=float(XCEN-XCENF)
        CCYs=float(YCEN-YCENF)
c
        ccx = nint(ccxs*cos(omegaf)+ccys*sin(omegaf))
        ccy = nint(-ccxs*sin(omegaf)+ccys*cos(omegaf))
c
      ang0 = omega0 * 57.296
      angf = omegaf * 57.296
c
        CCOM=(OMEGA0-OMEGAF)*57.296
        WRITE(IOUT,1006) CCX,CCY,CCOM,
     1                 angf,ang0,sinom0,cosom0
        if (.not.batch)WRITE(6,1006)CCX,CCY,CCOM,
     1                 angf,ang0,sinom0,cosom0
1006    FORMAT(' REFINED CAMERA CONSTANTS.'/' CCX: ',I8/
     1      ' CCY: ',I8/' CCOM:',F8.4/
     1      ' wF : ',f8.4/' w0  :',F8.4/
     1      ' sinw:',f8.4/' cosw:',F8.4/)
        RETURN
        END
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE REFPLOT****       *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C       SUBROUTINE REFPLOT
C
C  PLOTS REFLEXIONS COORDINATES FOR CENTRAL
C  AREA OF FILM ( L -FULLY RECORDED;
C  INVERTED L -PARTIALLY RECORDED;ELONGATED L -
C  ZERO LEVEL REFLEXIONS)
C       Changed to plot only multiplets (IM NE 0)
C       and limits chenged
C Sept 17th 1986 T.J.G.
c
c##### laue mods - plot only nodals
C============================================================
C
C

        SUBROUTINE REFPLOT(CDX,CDY)
C
      INCLUDE 'commcs.finc'
      INCLUDE 'comscn.finc'
      INCLUDE 'comori.finc'
      INCLUDE 'comras.finc'
      INCLUDE 'comlaue.finc'
C
        COMMON /EXTRAS/ JUMPAX
c##### laue mods
c        INTEGER ABUF(10),H(3)
C
c        IR=PCKIDX+1
c        DO 1 I=1,TOSPT
c        IR=IR+1
c        READ(1,REC=IR) ABUF
c        IF(IABS(ABUF(3)).GT.2500) GOTO 1
c        IF(IABS(ABUF(4)).GT.1900) GOTO 1
C
C---------------- UNPACK H,K,L,M FROM FIRST TWO WORDS
C
c        H(1)=IAND(ABUF(1),'177400'O)/256
c        IF(H(1).GE.128)H(1)=-256+H(1)
c        H(2)=IAND(ABUF(2),'377'O)
c        IF(H(2).GE.128)H(2)=-256+H(2)
c        H(3)=IAND(ABUF(2),'177400'O)/256
c        IF(H(3).GE.128)H(3)=-256+H(3)
c        IM=IAND(ABUF(1),'360'O)
C *********
C
C----------------- Remove partials from filmplot for LAUE's
C                  and change idelx,idely from 10,10 to 3,12
C                  REMEMBERING that jumpax is always set to 0
C                           Sept 8th 1986. Trevor
C
C       IDELX=10
C       IF(IM.NE.0) IDELX=-10
C       IDELY=IDELX
c       IF(IM.EQ.0) GOTO 1
c       IDELX = 3
c        IF(IM.NE.0) IDELX=-IDELX
c        IDELY=12*(3/idelx)
C
C *********

c        IF(H(JUMPAX).EQ.0) IDELX=3*IDELX
c        X=ABUF(3)
c        Y=ABUF(4)
c##### laue mods - code to replace that above
c     only plot nodals
c
      idelx = 10
      idely = 10
      do 1 ii=1,numnod
      i = nodpt(ii)
      x = xyge(1,i)
      y = xyge(2,i)
      if(abs(x) . gt. 5000) go to 1
      if(abs(y) . gt. 3800) go to 1
        CALL XYCALC(XC,YC,X,Y)
        IX=0.1*(XC-XCEN+CDX)+512
        IY=0.1*(YC-YCEN+CDY)+390
C
C ------------------------  CHECK FOR EDGE OF SCREEN
C
        IF(IX.GT.993.OR.IX.LT.31)GO TO 1
        CALL MOVTX(IX,IY)
        CALL VECTX(IX+IDELX,IY)
        CALL MOVTX(IX,IY)
        CALL VECTX(IX,IY+IDELY)
        CALL ALPHA
   1    CONTINUE
        RETURN
        END
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE REPORT  ****      *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
        SUBROUTINE REPORT
cRCD ...Image Plate logical
        common/iplate/image,mar,moldyn
        logical image,mar,moldyn
c
        COMMON/IOO/IOUT
        COMMON/SW/ISWTCH(10),oneref,noref,tworef
        LOGICAL oneref,noref,tworef
C
      INCLUDE 'comrep.finc'
      INCLUDE 'comuab.finc'
C
      ITOTS = ITOTS - NBAD
        WRITE(IOUT,1000) NREF,NOFR,IPROS,IBOXS,ITOTS,NOLO,NEDGE,NBZERO,
     1                   NBAD,NBOX
        if (.not.batch)
     1  WRITE(6,1000)   NREF,NOFR,IPROS,IBOXS,ITOTS,NOLO,NEDGE,NBZERO,
     1                   NBAD,NBOX
1000    FORMAT(' ',i8,' SPOTS MEASURED ON THIS FILM'/' '
     1            ,i8,' ARE SINGLETS'/' '
     1            ,i8,' PROFILE FITTED'/' '
     1            ,i8,' BOX SUMMED'/' '
     1            ,i8,' GOOD SPOTS'/' '
     1            ,i8,'     REJECTED AS OVERLOADS'/' '
     1            ,i8,'     REJECTED OVER SCAN LIMIT (Including ',
     1                      'central bin)'/' '
     1            ,i8,'     REJECTED ON BAD BACKGROUND'/' '
     1            ,i8,'     REJECTED ON BGRATIO OR I/SIGI'/' '
     1            ,i8,'     REJECTED ON OVERLARGE BOX')
      if(ltek) call newpg
C
cRCD ...Change ranges for IP data
        if(.not.image)then
        WRITE(IOUT,1003) IRANGE
        if (.not.batch)WRITE(6,1003)IRANGE
        else
        WRITE(IOUT,1003) iprange
        if (.not.batch)WRITE(6,1003)iprange
        endif
1003    FORMAT(/'    ANALYSIS OF INTENSITIES'/'   RANGES ',9I7)
        WRITE(IOUT,1004) IANAL
        if (.not.batch)WRITE(6,1004)IANAL
1004    FORMAT('    NO. ',10I7)
        WRITE(IOUT,1005) RATIO
        if (.not.batch)WRITE(6,1005)RATIO
1005    FORMAT(' BGRATIO',10F7.1)
        if (.not.batch)WRITE(6,1006)AVSD
        WRITE(IOUT,1006)AVSD
1006    FORMAT('   AVSD ',10F7.1)
        WRITE(IOUT,1007) MAXBSI,MINBSI
        if (.not.batch)WRITE(6,1007)MAXBSI,MINBSI
1007    FORMAT(/'  MAXIMUM INTENSITY=',I7/'  MINIMUM INTENSITY=',I7)
        RETURN
        END
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE RESIDPL ****      *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
        SUBROUTINE RESIDPL(XCAL,YCAL,DX,DY)
C
      INCLUDE 'comori.finc'
C
        INTEGER X,Y
        REAL DX,DY,XCAL,YCAL
C
C------------ REDUC CHANGED FROM 16 TO 32. (CORRECT SIZE TEKPLOT)
C                            OCT 3 1984.
C
        REDUC=32.
c
c --- change dx reduc to that in lauegen etc - 5x
c
      dreduc = 5./reduc
        X=(XCAL-XCEN)/REDUC+512
        Y=(YCAL-YCEN)/REDUC+380
        CALL MOVTX(X,Y)
C
C--------------- 2*DX TO DX (RE 16 TO 32 CHANGE). OCT 3 1984.
C
      if(abs(dx).gt.64.) dx = (dx/abs(dx))*64.
      if(abs(dy).gt.64.) dy = (dy/abs(dy))*64.
        IX=X+dreduc*DX
        IY=Y+dreduc*DY
        CALL VECTX(IX,IY)
        DXY=dreduc*SQRT(DX*DX+DY*DY)
C
C--------------- 2*DX TO DX (RE 16 TO 32 CHANGE). OCT 3 1984.
C
        IX=-dreduc*DY/DXY
        IY=dreduc*DX/DXY
        CALL MOVTX(X+IX,Y+IY)
        CALL VECTX(X-IX,Y-IY)
        CALL ALPHA
        RETURN
        END
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE RMAXR  ****       *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C
C       **** SUBROUTINE RMAXR ****
C
C  DETERMINES MAXIMUM X AND Y COORDINATES OF
C  REFLEXIONS IN GENERATE FILE, BOX EXPANSION
C  COEFFICIENTS AND CHECKS THAT DIMENSIONS OF
C  THE EXPANDED BOX ARE WITHIN LIMITS.
C
C
c##### laue mods - re selection of spots
c     only take measurable spots without spatial overlaps
C==============================================================
C
C
        SUBROUTINE RMAXR
        COMMON/TREV/NXMAX,NYMAX
        COMMON/IOO/IOUT
        COMMON/SW/ISWTCH(10),oneref,noref,tworef
c##### laue mods
c        INTEGER BUF(10)
         LOGICAL oneref,noref,tworef
C
      INCLUDE 'comuab.finc'
      INCLUDE 'comlaue.finc'
      INCLUDE 'comori.finc'
      INCLUDE 'comscn.finc'
      INCLUDE 'comras.finc'
      INCLUDE 'commcs.finc'
      INCLUDE 'comrfs.finc'
      INCLUDE 'comfid.finc'
C
c        EQUIVALENCE (BUF(1),IHMR),(BUF(3),JX),(BUF(4),JY)
        XTOFD=DTOFD*FACT
        VVAR=.5/(XTOFD*XTOFD*XTOFRA*XTOFRA)
        VARAS(1)=20./(DTOFD*XTOFRA)
        VARAS(2)=(NXS-MAX0(2*NRX+2,0))*VVAR
        VARAS(3)=(NYS-MAX0(2*NRY+2,0))*VVAR
        FACT1=FACT*1.01*XTOFRA
C
C------ INCREASED BY 1% TO ALLOW FOR XTOFRA, CCOM, DISTORTION, ETC.
C
        DOV2=CTOFD*.5*FACT1
        MAXX=0
        MAXY=0
        MAXR=0
        NXMAX=0
        NYMAX=0
c##### laue mods
c        JREC=PCKIDX+1
C
C-------  READ SPOTS FROM GENERATE FILE AND CALCULATE SIZE
C-------  OF EXPANDED MEASUREMENT BOX.
C
C---------------------------start loop
C
        DO 1 I=1,TOSPT
c##### laue mods
c        JREC=JREC+1
c        READ(1,REC=JREC) BUF
c        IR=IAND(IHMR,'17'O)
c        IF(IR.NE.0) GOTO 1
c        JX=IABS(JX)
c     ignore spots if unmeasurable or flagged overlaps
c ***** dont! TRevor
c      nodali = nodal(i)
c      ir = iand (nodali, '03'x)
c      if(ir.ne.0)go to 1
      jx = abs( xyge(1,i))
        MAXX=MAX0(MAXX,JX)
        XC=JX*FACT1
        IF(VEE) XC=ABS(XC-DOV2)
c        JY=IABS(JY)
      jy = abs(xyge(2,i))
        MAXY=MAX0(MAXY,JY)
        YC=JY*FACT1
        NX=XC*(XC*VARAS(2)+VARAS(1))+.5
        NY=YC*(YC*VARAS(3)+VARAS(1))+.5
        NX=((NX+1)/2)*2+NXS
        NXMAX=MAX0(NXMAX,NX)
        NY=((NY+1)/2)*2+NYS
        NYMAX=MAX0(NYMAX,NY)
        KRAS=(NX+1)*(NY+1)
        MAXR=MAX0(MAXR,KRAS)
1       CONTINUE
C
C----------------------------end loop
C
        IF(MAXR.GT.5000) WRITE(IOUT,1000) MAXR
        IF(MAXR.GT.5000.and..not.batch)write(6,1000)MAXR
1000    FORMAT(' EXPANDED RASTER LARGER THAN BUFFER (5000 POINTS)=',I6)
        IF(NXMAX.GT.69.OR.NYMAX.GT.69) WRITE(IOUT,1001) NXMAX,NYMAX
        IF(NXMAX.GT.69.OR.NYMAX.GT.69.and..not.batch)
     1      write(6,1001)NXMAX,NYMAX
1001    FORMAT(' EXPANDED BOX DIMENSION LARGER THAN 69 POINTS=',2I6)
        RETURN
        END
C
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE RVDIST ****       *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C
C            ****  SUBROUTINE RVDIST  ****
C
C REFINES FILM CENTRE, FILM ORIENTATION, CRYSTAL TO FILM DISTANCE,
C YSCAL, BAR CONSTANT, AND FILM DISTORTIONS FOR VEE-SHAPED FILMS
C USING A NON-LINEAR LEAST SQUARES METHOD
C AND REPORTS NEW FIDUCIAL CONSTANTS
C
C=============================================================
C
C
C
        SUBROUTINE RVDIST
        COMMON/SW/ISWTCH(10),oneref,noref,tworef
        COMMON/IOO/IOUT
C COMMON VARIABLES
C
      INCLUDE 'comori.finc'
      INCLUDE 'comfid.finc'
      INCLUDE 'comrfs.finc'
      INCLUDE 'commcs.finc'
      INCLUDE 'comuab.finc'
C
C LOCAL VARIABLES
        INTEGER XGEN(256),YGEN(256),BUF(10)
        REAL CLCX(256),CLCY(256)
        REAL RF(121),R(11),RR(11),W(11),DDX(11),DDY(11)
        LOGICAL BAD(256),oneref,noref,tworef
        EQUIVALENCE (RF(1),BUF(1))
        EQUIVALENCE (BUF(3),IX),(BUF(4),IY)
        DATA A0/2.86E-7/,B0/2.78E-8/
C
C--- A0 AND B0 DEFINE SHAPE OF BULGE
C---  COORDINATES USED IN THIS OVERLAY
C (CLCX,CLCY) ARE FROM THE GENERATE-FILE AND THEREFORE
C             IN 10 MICRON UNITS RELATIVE TO THE CENTRE
C             OF THE DIFFRACTION PATTERN
C-- (XRS,YRS)   ARE IN 10 MICRON UNITS (FROM OVERLAY SEEKRS OR NEXT)
C-- LEAST SQUARES REFINEMENT OF XTOFRA,XCEN,YCEN AND OMEGA0
C-- DONE BY FITTING OBSERVED POSITIONS (XRS,YRS) TO THOSE
C-- IN THE GENERATE-FILE (CLCX,CLCY)
C
        DOV2=CTOFD/2.*XTOFRA
        RADEG=18000./3.14159
C
C-------------------- NO. OF PARAMETERS TO REFINE
C
        NP=11
        NP2=NP*NP
        IF(XTEK) CALL NEWPG
        OMEGA0=ASIN(SINOM0)
        NOTRY=0
        WRITE(IOUT,1000) NRS
        if(.not.batch)write(6,1000)NRS
1000    FORMAT(/' FILM DISTORTION REFINEMENT USING',I3,' SPOTS'/)
C
C----------------- READ COORDS FROM GENERATE FILE
C
        DO 1 J=1,NRS
        IR=RRS(J)
        READ(1,REC=IR) BUF
        XGEN(J)=IX
        YGEN(J)=IY
        BAD(J)=.FALSE.
1       CONTINUE
        GOTO 12
2       WRITE(IOUT,1001) RMSRES
        if(.not.batch)write(6,1001)RMSRES
1001    FORMAT('  STARTING RESIDUAL=',F5.1)
        ESTART=RMSRES
C
C--------------------- START MAIN REFINEMENT LOOP
C
3       CONTINUE
C
C------------------ START LOOP FOR NON-LINEAR REFINEMENT
C
        DO 11 ICYC=1,2
C
C-------------------- INITIALIZE SUMS
C
        DO 4 K=1,NP
4       W(K)=0.0
        DO 5 K=1,NP2
5       RF(K)=0.0
C
C---------------------- SET UP NORMAL EQUATIONS
C
        DO 9 J=1,NRS
        IF(BAD(J)) GOTO 9
        XG=XGEN(J)
        YG=YGEN(J)
        XM=XG*COSOM0-YG*SINOM0
        YM=YG*COSOM0+XG*SINOM0
        XN=-YM*XTOFRA
        YN=XM*XTOFRA*YSCAL
        ABSX=ABS(XG)
        SNX=SIGN(1.0,XG)
        XDASH=ABSX-4300.
        XD=(ABSX-DOV2)
        XX=XDASH*XDASH*A0
        YY=YG*YG*B0
        FB=EXP(-XX-YY)
        FR=FB*XD
        BY=FB*YG
        DDX(1)=1.0
        DDX(2)=0.0
        DDX(3)=XM
        DDX(4)=0.0
        DDX(7)=-SNX*COSOM0
        DDX(8)=XN+SNX*CBAR*SINOM0
        DDX(9)=XG*YG
        DDX(10)=XG*XG
        DDX(11)=XG*SNX
        DDY(1)=0.0
        DDY(2)=1.0
        DDY(3)=0.0
        DDY(4)=YM
        DDY(7)=-SNX*SINOM0
        DDY(8)=YN-SNX*CBAR*COSOM0
        DDY(9)=YG*YG
        DDY(10)=YG*XG
        DDY(11)=YG*SNX
        IF(XG.LT.0) GOTO 6
C
C-------------------- POSITIVE XG
C
        DDX(5)=0.0
        DDX(6)=-FR
        DDY(5)=0.0
        DDY(6)=-BY
        GOTO 7
C
C------------------- NEGATIVE XG
C
6       DDX(5)=FR
        DDX(6)=0.0
        DDY(5)=-BY
        DDY(6)=0.0
7       CONTINUE
        CALL XYCALC(XC1,YC1,XG,YG)
        XO=XRS(J)-XC1
        YO=YRS(J)-YC1
        IK=0
        DO 8 I=1,NP
        W(I)=W(I)+DDX(I)*XO+DDY(I)*YO
        DO 8 K=1,NP
        IK=IK+1
        RF(IK)=RF(IK)+DDX(I)*DDX(K)+DDY(I)*DDY(K)
8       CONTINUE
9       CONTINUE
C
C--------------------------- SOLVE
C
        CALL MINV(RF,NP,DET,R,RR)
        IK=0
        DO 10 I=1,NP
        R(I)=0.0
        DO 10 K=1,NP
        IK=IK+1
        R(I)=R(I)+RF(IK)*W(K)
10      CONTINUE
        XCEN=XCEN+R(1)+.5
        YCEN=YCEN+R(2)+.5
        R(4)=R(4)+YSCAL*XTOFRA
        XTOFRA=XTOFRA+R(3)
        YSCAL=R(4)/XTOFRA
        VBNEG=VBNEG+R(5)
        VBPOS=VBPOS+R(6)
        CBAR=CBAR+R(7)+.5
        OMEGA0=OMEGA0+R(8)
        SINOM0=SIN(OMEGA0)
        COSOM0=COS(OMEGA0)
        VTWIST=VTWIST+R(9)
        VTILT=VTILT+R(10)
        VVERT=VVERT+R(11)
11      CONTINUE
C
C--------------- END OF INNER REFINEMENT LOOP
C
C-------------- CALCULATE STD.DEVS. AND TEST RESIDUALS
C
        NOTRY=NOTRY+1
12      RESMX=0
        RESD=0.0
        DO 13 J=1,NRS
        XG=XGEN(J)
        YG=YGEN(J)
        CALL XYCALC(CLCX(J),CLCY(J),XG,YG)
        IF(BAD(J)) GOTO 13
        DX=CLCX(J)-XRS(J)
        DY=CLCY(J)-YRS(J)
        RES=DX*DX+DY*DY
        RESD=RESD+RES
        IF(RES.LT.RESMX) GOTO 13
        RESMX=RES
        MXJ=J
13      CONTINUE
        RESMX=SQRT(RESMX)
        RMSRES=SQRT(RESD/(NRS-NOTRY+1))
        RESLIM=2.5*RMSRES
        IF(NOTRY.EQ.0) GOTO 2
        IF(NOTRY.EQ.1) WRITE(IOUT,1002) RMSRES
        IF(NOTRY.EQ.1.and..not.batch)write(6,1002)RMSRES
1002    FORMAT('  RESIDUAL AFTER 1 CYCLE=',F5.1)
        IF(NOTRY.GT.NRS/10) GOTO 14
        IF(RESMX.LT.RESLIM) GOTO 14
        BAD(MXJ)=.TRUE.
        GOTO 3
14      CONTINUE
C
C----------------------- PRINT OUT
C
        IF(XTEK) CALL NEWPG
        If (.not.Ltek) goto 15
        IF(.NOT.XTEK) CALL TKCLR
15      Continue
        DO 16 J=1,NRS
        DX=XRS(J)-CLCX(J)
        DY=YRS(J)-CLCY(J)
        If (Ltek) CALL RESIDPL(CLCX(J),CLCY(J),DX,DY)
16      CONTINUE
        If (Ltek) CALL TKPLOT(0,760,1,IER)
        IF(XTEK)CALL NEWPG
        IF(NOTRY.GT.1.and..not.batch)write(6,1003)
        IF(NOTRY.GT.1)WRITE(IOUT,1003)
1003    FORMAT(/'  REJECTED REFLEXIONS (ERROR>2.5*RMS RESID.)'
     1           /'     XOBS   DELX   YOBS  DELY ')
        DO 17 J=1,NRS
        IF(.NOT.BAD(J))GO TO 17
        DX=XRS(J)-CLCX(J)
        DY=YRS(J)-CLCY(J)
        if(.not.batch)write(6,1004)XRS(J)-XCEN,DX,YRS(J)-YCEN,DY
        WRITE(IOUT,1004)XRS(J)-XCEN,DX,YRS(J)-YCEN,DY
1004    FORMAT(' ',2(2X,I6,F5.1))
17      CONTINUE
        NXD=ANINT(CTOFD*XTOFRA)
        IBNEG=VBNEG*CTOFD
        IBPOS=VBPOS*CTOFD
        ITILT=VTILT*CTOFD*RADEG
        ITWIST=VTWIST*CTOFD*RADEG
        IVERT=VVERT*CTOFD
        WRITE(IOUT,1005) RMSRES,XCEN,YCEN,XTOFRA,NXD,YSCAL,IBNEG,
     1  IBPOS,CBAR,ITILT,ITWIST,IVERT
        if(.not.batch)write(6,1005)RMSRES,XCEN,YCEN,XTOFRA,NXD,YSCAL,IBNEG,
     1            IBPOS,CBAR,ITILT,ITWIST,IVERT
1005    FORMAT(/' FINAL RMS RESIDUAL:  ',F8.1/
     1     1X,' XCEN  YCEN  XTOFRA  XTOFD YSCALE  -BULGE +BULGE',
     2     ' CBAR XTILT ITWIST  IVERT'/1X,2I6,F8.4,I7,F8.4,6I6/)
C
C--------------- PRINT REFINED FIDUCIAL CONSTANTS
C
        CCX=XCEN-XCENF
        CCY=YCEN-YCENF
        CCOM=(OMEGA0-OMEGAF)*57.296
        WRITE(IOUT,1006) CCX,CCY,CCOM
        if(.not.batch)write(6,1006)CCX,CCY,CCOM
 1006   FORMAT(' REFINED CAMERA CONSTANTS.'/' CCX: ',I8/
     1         ' CCY: ',I8/' CCOM:',F8.4/)
        If (Ltek) CALL ALPHA
        RETURN
        END
      SUBROUTINE SCPAUS
      WRITE(6,111)
  111 FORMAT(' TYPE RETURN TO CONTINUE ',$)
      READ(5,112)JUNK
  112 FORMAT(A1)
      RETURN
      END
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE GENSORT ****      *
C                      *                                           *
C                      *********************************************
C
C                      ******** EXTRACTED: 17/2/88 TJG *********
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE SEEKRS ****       *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C SEARCHES OUTSIDE OF FILM FOR REFINEMENT SPOTS.
C FILM IS DIVIDED INTO SECTORS WITHIN WHICH
C AFIXED NO. OF SPOTS SHOULD BE FOUND.
C MEASURES STRIPES FROM SCANNER INTO ARRAY BA.
C SELECTS DATA FOR PARTICULAR SPOTS INTO ARRAY BB.
C COMPLETED SPOTS ARE TESTED FOR INTEGRATED INTENSITY
C AND SEPARATION.
C
C=============================================================
C
C
C
        SUBROUTINE SEEKRS
c
cRCD ...Image Plate logical
        common/iplate/image,mar,moldyn
        logical image,mar,moldyn
c
        COMMON/IOO/IOUT
        COMMON/THRESH/JNK1,IODS,ISIZS
        COMMON/SW/ISWTCH(10),oneref,noref,tworef
        LOGICAL oneref,noref,tworef
C
      INCLUDE 'comtem.finc'
      INCLUDE 'comori.finc'
      INCLUDE 'comras.finc'
      INCLUDE 'comscn.finc'
      INCLUDE 'comrfs.finc'
      INCLUDE 'commcs.finc'
      INCLUDE 'comuab.finc'
      INCLUDE 'commaskl.finc'
C
C--------------- Get COMREP for NREF
C
      INCLUDE 'comrep.finc'
C
        COMMON/PEL/IBA,IJBA(5120),NREC
        INTEGER*2 IJBA,iba
        COMMON/CPEL/BB(14000)
        INTEGER*2 BB
        INTEGER IBIN,JBIN,SBIN,LBIN,LRAS(5)
        INTEGER NOSPT,XC,YC,RC,PNTR(120)
        INTEGER HFWX,HFWY,BUF(3),NSIZ,WTX
        INTEGER X(200),Y(200),REC(200),SHX,SHY
        INTEGER INF1(200),INF2(200),INF3(200)
        LOGICAL FULL,YES
        EQUIVALENCE (BUF(1),RC),(BUF(2),XC),(BUF(3),YC)
        EQUIVALENCE (LRAS(1),NXX),(LRAS(2),NYY)
        DATA MAXBUF/7000/
C
C----------------------- START SEEKRS
C
        DO 1 I=3,5
1       LRAS(I)=IRAS(I)
C
C----------------- SET MAXIMUM RASTER SIZE IN WORDS
C
        MAXB=MAXR+4
        if(.not.image)then
        MAXW=(MAXB+1)/2
        MAXB=2*MAXW
        else
        maxw=maxb
        endif
        MAXN=MAXBUF/MAXW
        IF(MAXN.GT.100) MAXN=100
c
c-------------------- set max av od as 0.6*maxod
c
      maxi = nint(float(imaxod)*0.6)
C
C-------------------- OPEN INPUT SORT FILE
C
        ifail=0
        call ccpdpn(-4,'SORTTEMP','OLD','U',0,ifail)
c         OPEN(UNIT=4,FILE='Sorttemp',STATUS='OLD',FORM='UNFORMATTED')
C
C-------------- GET INFORMATION ON SPOTS FROM SORT FILE
C
        IHTOFD=CTOFD*XTOFRA*FACT*0.5
        NECX=XCEN*FACT+0.5
        NECY=YCEN*FACT+0.5
        LBIN=2*28
        NBIN=8
        SBIN=NREF/NBIN
        IF(SBIN.LE.200) GOTO 2
        NBIN=NREF/200+1
        NBIN=2*((NBIN+1)/2)
        SBIN=NREF/NBIN
        LBIN=2*230/NBIN
2       WRITE(IOUT,1000) NREF,NBIN
        if(.not.batch)write(6,1000)NREF,NBIN
 1000   FORMAT(I6,' REFLEXIONS TO BE MEASURED IN',I4,' BINS')
C
C----------------IOD FROM 6 TO 2.  TREVOR OCT 3 1984
C----------------ISIZ FROM 3 TO 1.
C
        IF(I2REF) GOTO 21
        IODS=4
        IF(NBIN.GT.10) IODS=4
        ISIZS=2
      if(image) then
            IODS = 10
            ISIZS = 10
      endif
C*******
21      if(.not.image) WRITE(IOUT,1001)IODS,ISIZS
        if(image) WRITE(IOUT,1234)IODS,ISIZS
        If (Batch.and.I2ref) goto 4
        If (Batch) goto 3
        if(.not.batch) then
            if(.not.image) write(6,1001)IODS,ISIZS
            if(image) write(6,1234)IODS,ISIZS
      endif
1001    FORMAT(/' THRESHOLDS (3,1 weak; 5,3 strong). [',
     1          I3,',',I3,']: ',$)
1234    FORMAT(/' THRESHOLDS (7,5 weak; 10,15 strong). [',
     1          I3,',',I3,']: ',$)
3       read(5,*) NIODS,NISIZS
1002    format(2I4)
        if(Niods.ne.0) Iods = iabs(Niods)
        if(Nisizs.ne.0) Isizs = Nisizs
        if(Niods.lt.0) Isizs = 0
4       WRITE(IOUT,1003) IODS,ISIZS
1003    FORMAT(1h+,2I5)
        I2REF = .TRUE.
C
C--------------- NOW FIND SPOTS
C
        IOD = IODS
        ISIZ = ISIZS
        ISEP=nint(0.5*160./SCNSZ)
        LASTX=0
        LASTY=0
C
C---------------------------- LOOP OVER ALL SPOTS
C
        DO 18 NB=1,NBIN
C
C--------------------------- LOOP OVER BINS
C
        DO 5 NOSPT=1,SBIN
        READ(4) BUF
      if(.not.profil) then
        FX=IABS(XC-NECX)
        IFX=FX
        IF(VEE) FX=IABS(IFX-IHTOFD)
        FY=IABS(YC-NECY)
        N1=FX*(FX*VARAS(2)+VARAS(1))+0.5
        N2=FY*(FY*VARAS(3)+VARAS(1))+0.5
      else
      n1 = 0
      n2 = 0
      endif
        HFWX=(NXS+N1)/2
        HFWY=(NYS+N2)/2
        X(NOSPT)=XC
        Y(NOSPT)=YC
        REC(NOSPT)=RC
        INF1(NOSPT)=2*HFWX+1
        INF2(NOSPT)=HFWX
        INF3(NOSPT)=HFWY
5       CONTINUE
C
C-------------- SET FLAGS TO INITIAL VALUES
C----------------------- IFRST,ILAST:-
C--------------THESE ARE MARKERS IN BB TO SHOW WHERE IT IS FILLED
C------------------------INDF,INDL:-
C--------------SHOW WHICH REFLECTIONS ARE ACTIVE IN SORTED FILE
C
        INDF=1
        INDL=1
        IBIN=0
        IFRST=1
        ILAST=0
C
C---------- START SCAN FOR THE FIRST SPOT OR SPOT AFTER A GAP
C
6       CONTINUE
        WTX=X(INDF)-INF2(INDF)
        IBLK=WTX
C
C------------ INCLUDE THE SPOT IN THIS SCAN
C
7       IER=1
        CALL RDBLK(2,IBLK,IJBA,NWORD,IER)
8       IF(INDL.EQ.SBIN) GOTO 9
        IF(FULL) GOTO 9
        MINDL=INDL+1
        IF(WTX.LT.X(MINDL)-INF2(MINDL)) GOTO 9
        INDL=INDL+1
        IF(INDL-INDF.EQ.MAXN-1) FULL=.TRUE.
        GOTO 8
C
C--------- GET THE INFORMATION FOR THIS SCAN AND START A NEW ONE
C
9       CONTINUE
C
C---------- WRITE SPOTS COLLECTED IN THIS STRIPE TO BB
C
        IF(INDL.LT.INDF) GOTO 12
        DO 11 J=INDF,INDL
        MJ=MOD(J-1,MAXN)
        YC=Y(J)
        KMN=YC-INF3(J)
        KMX=YC+INF3(J)
        NJ=MJ*MAXB
        MJ=MJ+1
        IF(J.NE.IFRST) GOTO 10
        MFRST=MOD(IFRST-1,MAXN)+1
c
c    *****************************************************
c    I THINK REC(j) IS UNIMPORTANT HERE. CAUSES TROUBLE. SET TO 0
c    *****************************************************
c
        BB((MFRST-1)*MAXW+1)=0
        IADDR=(MFRST-1)*MAXB
        IN=INF2(J)*2+1
        IZZ=IADDR+3
        if(.not.image)then
        CALL BPACK(IZZ,BB,IN)
        else
        call ip_bpack(izz,bb,in)
        endif
        IN=INF3(J)*2+1
        IZZ=IZZ+1
        if(.not.image)then
        CALL BPACK(IZZ,BB,IN)
        else
        call ip_bpack(izz,bb,in)
        endif
        PNTR(MFRST)=5
        IFRST=IFRST+1
10      IZZ=PNTR(MJ)+NJ
        if(.not.image)then
        CALL BSWAP(KMN,KMX,IZZ)
        else
        call ip_bswap(kmn,kmx,izz)
        endif
        PNTR(MJ)=IZZ-NJ
        INF1(J)=INF1(J)-1
11      CONTINUE
12      WTX=WTX+1
        IBLK=IBLK+1
C
C--------------- SEE IF SPOT IS FINISHED
C
13      IF(INF1(INDF).EQ.0) GOTO 14
        IF(INDF.LE.INDL) GOTO 7
        FULL=.FALSE.
        GOTO 6
C
C----------- WRITE OUT COMPLETED SPOTS AND READ IN NEW ONES
C
14      IND=ILAST*MAXW+1
        NXX=2*INF2(INDF)+1
        NYY=2*INF3(INDF)+1
        NXY=NXX*NYY
C
        if(.not.image)then
           ijunk=ind+2
        else
           ijunk=ind+4
        endif
      if(streak) then
        call cgmask(bb(ijunk),lras,0,delx,dely,
     1                  x(indf),y(indf),spot)
      else
          CALL CGFIT(BB(ijunk),LRAS,+1,DELX,DELY,SPOT)
      end if
C
C---------------UPPER THRESH FROM 80 TO 100.  OCT 3 1984
C          *****        AND CHANGED CGFIT TO GIVE SPOT/NPK
C                         IE IAVOD SINCE IN THIS VERSION
C                         NPEAK IS UNEXPANDED SIZE!!
C
C       IAVOD=SPOT/NPEAK
C
        IAVOD=SPOT
        if(.not.image)then
        IF((IAVOD.LT.IOD).OR.(IAVOD.GT.maxi)) GOTO 15
        else
        if((iavod.lt.iod).or.(iavod.gt.maxi)) goto 15
        endif
C
C------------  SPOT SEPARATION  > 4 MM
C
        IF(IABS(X(INDF))-LASTX.GE.ISEP) LASTY=0
        IF(IABS(Y(INDF)-LASTY).LT.ISEP) GOTO 15
        LASTX=X(INDF)
        IBIN=IBIN+1
      if(.not.image)then
        NSIZ=(PNTR(ILAST+1)+1)/2
      else
      nsiz=pntr(ilast+1)
      endif
        SHX=DELX/FACT+0.5
        SHY=DELY/FACT+0.5
        NRS=NRS+1
        LASTY=Y(INDF)
        XRS(NRS)=ANINT((X(INDF)+DELX)/FACT)
        YRS(NRS)=ANINT((Y(INDF)+DELY)/FACT)
        RRS(NRS)=REC(INDF)
        IF(ISWTCH(1).EQ.0) GOTO 15
        WRITE(IOUT,1004) NRS,SHX,SHY,IAVOD
        if(.not.batch)write(6,1004)NRS,SHX,SHY,IAVOD
1004    FORMAT('SPOT',I4,' C.OF G. SHIFTS',2F6.1,' AV.OD.',I6)
        CALL ODPLOT(BB(IND+2),NXX,NYY,1)
15      ILAST=ILAST+1
        IF(ILAST.EQ.MAXN) ILAST=0
        INDF=INDF+1
        IF(IBIN.EQ.LBIN) GOTO 16
        IF(INDF.LT.SBIN) GOTO 13
        WRITE(IOUT,1005) IBIN,NB
        if(.not.batch)write(6,1005)IBIN,NB
1005    FORMAT(' ONLY',I6,' SPOTS FOUND IN BIN',I4)
16      IF(NB.NE.NBIN/2) GOTO 17
        ISIZ=-ISIZ
        IOD=IOD-ISIZ
17      IOD=IOD+ISIZ
18      CONTINUE
        CLOSE (UNIT=4)
        RETURN
        END
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE SELECT ****       *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
        SUBROUTINE SELECT
C
      INCLUDE 'comrfs.finc'
C
        IF(NRS.LE.20) RETURN
        SEP=200
1       SEP=SEP+100
        DSEP=SEP*SEP
        I=1
2       J=I+1
3       DX=IABS(XRS(I)-XRS(J))
        IF(DX.GT.SEP) GOTO 7
        DY=IABS(YRS(I)-YRS(J))
        IF(DY.GT.SEP) GOTO 6
        DR=DX*DX+DY*DY
        IF(DR.GT.DSEP) GOTO 6
        NRS=NRS-1
        IF(J.GT.NRS) GOTO 5
        DO 4 K=J,NRS
        XRS(K)=XRS(K+1)
        YRS(K)=YRS(K+1)
4       RRS(K)=RRS(K+1)
5       IF(NRS.EQ.20) RETURN
6       IF(J.GE.NRS) GOTO 7
        J=J+1
        GOTO 3
7       IF(I.EQ.NRS-1) GOTO 1
        I=I+1
        GOTO 2
        END
C
C
C             SUBROUTINE TO INITIALIZE COMMON BCK DATA
C
C
      SUBROUTINE SETBCK(J)
C
      INCLUDE 'combck.finc'
C
      NNEG = 0
      NSAT = 0
      BN(J) = 0
      BP(J) = 0
      BQ(J) = 0
      BO(J) = -20000.
      PS(J) = 0
      QS(J) = 0
      MS(J) = 0
      SS(J) = 0.0
      SMS(J) = 0.0
      BPQ(J) = 0.0
      BPP(J) = 0.0
      BQQ(J) = 0.0
      BOP(J) = 0.0
      BOQ(J) = 0.0
C      REJ(J) = 0
      SIGMA(J) = 0
      IF(J.NE.1) RETURN
      PN = 0
      PPSUM = 0.0
      QQSUM = 0.0
      RETURN
      END
c     set intensity for  this film
      subroutine setge(nspot, ifilm)
C
      INCLUDE 'comuab.finc'
C
      integer*2 ibuf(38)
      iprof = 0
      if(profil)iprof=12
      if(.not.batch)write(6,100)
  100 format(' clearing intensities for this film ....')
c
      jrec = 10
      ii = 4 +iprof + ifilm
      do i = 1,nspot
      jrec = jrec +1
      read (1,rec=jrec) ibuf
      ibuf(ii) = -9999
      ibuf(ii+6) = -9999
      write(1,rec = jrec) ibuf
      enddo
      return
      end
C
C    SUBROUTINE TO SET-UP ARRAYS & CONSTANTS FOR
C           HAT-MATRIX.
C
      SUBROUTINE SETHAT(ibin,N,P,Q)
C
      INCLUDE 'combck.finc'
      INCLUDE 'comhat.finc'
C
      INTEGER P,Q
      nopuse = 0
C
      HN=N+1
      HAT(HN,2)=FLOAT(P)
      HAT(HN,3)=FLOAT(Q)
      N=N+1
C
      IF(PORB.EQ.1)THEN
      HATP(HN,2,ibin)=HAT(HN,2)
      HATP(HN,3,ibin)=HAT(HN,3)
      HPOBS(ibin)=HN
      ELSE
      HATB(HN,2)=HAT(HN,2)
      HATB(HN,3)=HAT(HN,3)
      HNOBS=HN
      END IF
C
      RETURN
      END
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE SETMASK ****      *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C
        SUBROUTINE SETMASK(MASK,IRAS)
        INTEGER MASK(1),IRAS(5),HX,HY,P,Q
        LOGICAL P1,P2
        HX=IRAS(1)/2
        HY=IRAS(2)/2
        IC=HX+HY-IRAS(3)
        IRX=HX-IRAS(4)
        IRY=HY-IRAS(5)
        IJ=0
C
C---------------------------------start loop
C
        DO 3 P=-HX,HX
        IP=IABS(P)
        P1=(IP.GT.IRX)
        P2=(IP.EQ.IRX)
        DO 3 Q=-HY,HY
        IJ=IJ+1
        IQ=IABS(Q)
        IPQ=IP+IQ
        IF(P1.OR.IPQ.GT.IC.OR.IQ.GT.IRY) GOTO 1
        IF(P2.OR.IPQ.EQ.IC.OR.IQ.EQ.IRY) GOTO 2
        MASK(IJ)=1
        GOTO 3
1       MASK(IJ)=-1
        GOTO 3
2       MASK(IJ)=0
3       CONTINUE
C
C-------------------------------------end loop
C
        RETURN
        END
C
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE SETSUMS ****      *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C
C
C       LATEST REVISION-  1 JUNE 1979
C
C CALLED BY PROCESS
C THIS ROUTINE CALCULATES SUMS USED IN THE ROUTINE INTEG
C
C
C============================================================
C
C
        SUBROUTINE SETSUMS(MASK,IRAS,PQVAL)
        INTEGER IRAS(*),MASK(*)
        REAL PQVAL(6)
        INTEGER S,T
        HX=IRAS(1)/2
        HY=IRAS(2)/2
        T=0
        S=0
        TPP=0.0
        SPP=0.0
        TQQ=0.0
        SQQ=0.0
        IJ=0
C
C---------------------------- start loop
C
        DO 3 P=-HX,HX
        PP=P*P
        DO 3 Q=-HY,HY
        IJ=IJ+1
        IF (MASK(IJ))2,3,1
C
C--------------------------- SUMS FOR THE PEAK
C
1       T=T+1
        QQ=Q*Q
        TPP=TPP+PP
        TQQ=TQQ+QQ
        GO TO 3
C
C------------------------- SUMS FOR THE BACKGROUND
C
2       S=S+1
        QQ=Q*Q
        SPP=SPP+PP
        SQQ=SQQ+QQ
3       CONTINUE
C
C--------------------------end loop
C
        PQVAL(1)=TPP
        PQVAL(2)=SPP
        PQVAL(3)=TQQ
        PQVAL(4)=SQQ
        PQVAL(5)=T
        PQVAL(6)=S
        RETURN
        END
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE SIZRAS ****       *
C                      *                                           *
C                      *********************************************
C
C
C
C
C
        SUBROUTINE SIZRAS(IRAS,NPEAK,NBG)
        INTEGER IRAS(5),P,Q
        IHX=IRAS(1)/2
        IHY=IRAS(2)/2
        IC=IHX+IHY-IRAS(3)
        IRX=IHX-IRAS(4)
        IRY=IHY-IRAS(5)
        NPEAK=0
        NBG=0
        NMISS=0
C
C---------------------------------start loop
C
        DO 3 P=-IHX,IHX
        IP=IABS(P)
        DO 3 Q=-IHY,IHY
        IQ=IABS(Q)
        IPQ=IP+IQ
        IF((IPQ.GT.IC).OR.(IP.GT.IRX).OR.(IQ.GT.IRY)) GOTO 1
        IF((IPQ.LT.IC).AND.(IP.LT.IRX).AND.(IQ.LT.IRY)) GOTO 2
        NMISS=NMISS+1
        GOTO 3
1       NBG=NBG+1
        GOTO 3
2       NPEAK=NPEAK+1
3       CONTINUE
C
C------------------------------- end loop
C
        RETURN
        END
C
C
C           SUBROUTINE TO SOLVE THE BG PLANES FOR THE
C                       CURRENT BIN OR SPOT.
C               IF CHANGE FROM STANDARD IS REQUIRED.
C
      SUBROUTINE SLVBCK( J )
C
      INCLUDE 'comuab.finc'
      INCLUDE 'combck.finc'
C
      REAL AM(3,3), BM(3), BUF1(3), BUF2(3)
C
C
      AM(1,1) = BPP(J)
      AM(1,2) = BPQ(J)
      AM(1,3) = BP(J)
      AM(2,1) = BPQ(J)
      AM(2,2) = BQQ(J)
      AM(2,3) = BQ(J)
      AM(3,1) = BP(J)
      AM(3,2) = BQ(J)
      AM(3,3) = BN(J)
      IF(J.EQ.NMBIN+1) GOTO 101
      if(.not.batch)write(6,98) J
      WRITE(8,98) J
 98   FORMAT(1X,'  MATRIX  AM  FOR  BIN  ',I5)
      DO 100 JM = 1,3
      WRITE(8,99) (AM(IM,JM),IM=1,3)
      if(.not.batch)write(6,99) (AM(IM,JM),IM=1,3)
 99   FORMAT(2X,3F10.4)
 100  CONTINUE
 101  CONTINUE
C
C--------------------------INVERT AM
C
      CALL MINV(AM, 3, DET, BUF1, BUF2)
C
C------------------------CALL ABCBCK
C
      CALL ABCBCK(J,AM)
C
      RETURN
      END
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE SORT  ****        *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C REF.
C COMM.ACM VOL.12 #3 MARCH 1969, R.C.SINGLETON
C
C=============================================================
C
C
        SUBROUTINE SORTDN3(N,A,B,C)
        DIMENSION A(N),B(N),C(N),IL(16),IU(16)
        INTEGER A,B,C,T,TT,X,Y
        M=1
        I=1
        J=N
1       IF(I.GE.J) GOTO 9
2       K=I
        IJ=(I+J)/2
        T=A(IJ)
        IF(A(I).GE.T) GOTO 3
        A(IJ)=A(I)
        A(I)=T
        T=A(IJ)
        X=B(I)
        B(I)=B(IJ)
        B(IJ)=X
        Y=C(I)
        C(I)=C(IJ)
        C(IJ)=Y
3       L=J
        IF(A(J).LE.T) GOTO 6
        IF(A(J).GT.A(I)) GOTO 4
        A(IJ)=A(J)
        A(J)=T
        T=A(IJ)
        X=B(IJ)
        B(IJ)=B(J)
        B(J)=X
        Y=C(IJ)
        C(IJ)=C(J)
        C(J)=Y
        GOTO 6
4       A(IJ)=A(I)
        A(I)=A(J)
        A(J)=T
        T=A(IJ)
        X=B(J)
        B(J)=B(IJ)
        B(IJ)=B(I)
        B(I)=X
        Y=C(J)
        C(J)=C(IJ)
        C(IJ)=C(I)
        C(I)=Y
        GOTO 6
5       A(L)=A(K)
        A(K)=TT
        X=B(L)
        B(L)=B(K)
        B(K)=X
        Y=C(L)
        C(L)=C(K)
        C(K)=Y
6       L=L-1
        IF(A(L).LT.T) GOTO 6
        TT=A(L)
7       K=K+1
        IF(A(K).GT.T) GOTO 7
        IF(K.LE.L) GOTO 5
        IF((L-I).LE.(J-K)) GOTO 8
        IL(M)=I
        IU(M)=L
        I=K
        M=M+1
        GOTO 10
8       IL(M)=K
        IU(M)=J
        J=L
        M=M+1
        GOTO 10
9       M=M-1
        IF(M.EQ.0) GOTO 13
        I=IL(M)
        J=IU(M)
10      IF((J-I).GE.11) GOTO 2
        IF(I.EQ.1) GOTO 1
        I=I-1
11      I=I+1
        IF(I.EQ.J) GOTO 9
        T=A(I+1)
        IF(A(I).GE.T) GOTO 11
        X=B(I+1)
        Y=C(I+1)
        K=I
12      A(K+1)=A(K)
        B(K+1)=B(K)
        C(K+1)=C(K)
        K=K-1
        IF(T.GT.A(K)) GOTO 12
        A(K+1)=T
        B(K+1)=X
        C(K+1)=Y
        GOTO 11
13      RETURN
        END
C
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE SORTUP3 ****      *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C       SUBROUTINE SORT
C
C REF.
C COMM.ACM VOL.12 #3 MARCH 1969, R.C.SINGLETON
C
C============================================================
C
C
        SUBROUTINE SORTUP3(N,A,B,C)
        DIMENSION A(N),B(N),C(N),IL(16),IU(16)
        INTEGER A,B,C,T,TT,X,Y
        M=1
        I=1
        J=N
1       IF(I.GE.J) GOTO 9
2       K=I
        IJ=(I+J)/2
        T=A(IJ)
        IF(A(I).LE.T) GOTO 3
        A(IJ)=A(I)
        A(I)=T
        T=A(IJ)
        X=B(I)
        B(I)=B(IJ)
        B(IJ)=X
        Y=C(I)
        C(I)=C(IJ)
        C(IJ)=Y
3       L=J
        IF(A(J).GE.T) GOTO 6
        IF(A(J).LT.A(I)) GOTO 4
        A(IJ)=A(J)
        A(J)=T
        T=A(IJ)
        X=B(IJ)
        B(IJ)=B(J)
        B(J)=X
        Y=C(IJ)
        C(IJ)=C(J)
        C(J)=Y
        GOTO 6
4       A(IJ)=A(I)
        A(I)=A(J)
        A(J)=T
        T=A(IJ)
        X=B(J)
        B(J)=B(IJ)
        B(IJ)=B(I)
        B(I)=X
        Y=C(J)
        C(J)=C(IJ)
        C(IJ)=C(I)
        C(I)=Y
        GOTO 6
5       A(L)=A(K)
        A(K)=TT
        X=B(L)
        B(L)=B(K)
        B(K)=X
        Y=C(L)
        C(L)=C(K)
        C(K)=Y
6       L=L-1
        IF(A(L).GT.T) GOTO 6
        TT=A(L)
7       K=K+1
        IF(A(K).LT.T) GOTO 7
        IF(K.LE.L) GOTO 5
        IF((L-I).LE.(J-K)) GOTO 8
        IL(M)=I
        IU(M)=L
        I=K
        M=M+1
        GOTO 10
8       IL(M)=K
        IU(M)=J
        J=L
        M=M+1
        GOTO 10
9       M=M-1
        IF(M.EQ.0) GOTO 13
        I=IL(M)
        J=IU(M)
10      IF((J-I).GE.11) GOTO 2
        IF(I.EQ.1) GOTO 1
        I=I-1
11      I=I+1
        IF(I.EQ.J) GOTO 9
        T=A(I+1)
        IF(A(I).LE.T) GOTO 11
        X=B(I+1)
        Y=C(I+1)
        K=I
12      A(K+1)=A(K)
        B(K+1)=B(K)
        C(K+1)=C(K)
        K=K-1
        IF(T.LT.A(K)) GOTO 12
        A(K+1)=T
        B(K+1)=X
        C(K+1)=Y
        GOTO 11
13      RETURN
        END
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE SPOTPLOT ****     *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C
C       SUBROUTINE SPOTPLOT
C
C  SCANS CENTRAL AREA OF FILM AND PLOTS IMAGE
C  OF FILM ON TEKTRONIX.
C
C=============================================================
C
C
C
C
        SUBROUTINE SPOTPLOT
c
cRCD ...Image Plate logical
        common/iplate/image,mar,moldyn
        logical image,mar,moldyn
C
      INCLUDE 'comori.finc'
      INCLUDE 'comras.finc'
      INCLUDE 'comscn.finc'
C
        LOGICAL VEC
        INTEGER NH(40)
        COMMON/PEL/IBA,IJBA(5120),NREC
        INTEGER*2 iba,IJBA
      integer i4ba,kpx,jpx
        COMMON/PLOTSC/DLSCAL
        DATA INTV/4/
C
C--------------  FILE USED FOR STORING IMAGE OF FILM
C
        ifail=0
        call ccpdpn(-22,'SPOTVEC','UNKNOWN','U',0,ifail)
        DO 1 J=1,40
1       NH(J)=0
        if(.not.image)then
        ITHRESH=70
        else
        ithresh=20000
        endif
c       **** NSCAN+1 is total number of scans required ****
        XNSCAN=2.0*25.6*100.0*fact
        NSCAN = XNSCAN
        IYCEN=YCEN*FACT
        IXCEN=XCEN*FACT
c       **** IBLK here is the starting stripe of data ****
        IBLK=IXCEN-2560*FACT
        NSTEP=4/SCNSZ
        if(nstep.eq.0)nstep=1
      stepn = 4.0/scnsz
        scal=0.1*stepn/fact
        jlim=int(float(1024)/scal)
        klim=int(float(780)/scal)
        NBLK=NSTEP
        DO 8 JPX=0,jlim,2
        j=jpx*scal
        IER=NBLK
        CALL RDBLK(2,IBLK,IJBA,NWORD,IER)
        IBLK=IBLK+NBLK
        VEC=.FALSE.
        IADDR=IYCEN-1950*FACT
        DO 4 KPX=0,klim,2
        k=kpx*scal
        if(.not.image)then
        CALL CBYTE(IADDR)
        else
        call ip_cbyte(iaddr)
        endif
        IADDR=IADDR+NSTEP
        i4ba=abs(int(iba))
C
C------------- HISTOGRAM FOR FIRST TEN SCAN LINES
C
        IF(J.GT.20) GOTO 2
        N=MIN0(I4BA/4+1,40)
        NH(N)=NH(N)+1
        GOTO 6
2       IF(I4BA.LT.ITHRESH) GOTO 3
        IF(VEC) GOTO 4
        IB=K
        VEC=.TRUE.
        CALL MOVTX(J,IB)
        WRITE(22) J,IB
        GOTO 4
3       IF(.NOT.VEC) GOTO 4
        IE=(K-1)
        VEC=.FALSE.
        CALL VECTX(J,IE)
        CALL ALPHA
        WRITE(22) J,IE
4       CONTINUE
        IF (.NOT.VEC) GO TO 5
        CALL VECTX(J,780)
        CALL ALPHA
        WRITE(22) J,780
5       IF(J.GT.20) GOTO 8
6       NMAX=0
        DO 7 K=1,40
        IF(NH(K).LT.NMAX) GOTO 7
        NMAX=NH(K)
        NK=K
7       CONTINUE
        ITHRESH=(INTV*NK)*DLSCAL
8       CONTINUE
        CLOSE (UNIT=22)
        RETURN
        END
c##### Laue mods for Version 2 ge1/ge2 file      Sep 1986
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE START ****        *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C READS INPUT PARTLY FROM GENERATE-FILE PARTLY FROM SCANNER
C AND PREPARES COMMON VARIABLES FOR LATER OVERLAYS
C  DISPLAYS MEASUREMENT BOX AND ALLOWS CHANGES
C  OF PARAMETERS.
C=============================================================
C
C
C
        SUBROUTINE START
c
C
C   ------------------------ COMMON VARIABLES
C
C
cRCD ...Image Plate logical
        common/iplate/image,mar,moldyn
        logical image,mar,moldyn
      INCLUDE 'comori.finc'
      INCLUDE 'comscn.finc'
      INCLUDE 'comras.finc'
      INCLUDE 'comfid.finc'
      INCLUDE 'commcs.finc'
      INCLUDE 'comrfs.finc'
      INCLUDE 'comuab.finc'
      INCLUDE 'commaskl.finc'
      INCLUDE 'commask.finc'
      INCLUDE 'comlaue.finc'
C
        COMMON /EXTRAS/ JUMPAX
C
C  ------------------------  LOCAL VARIABLES
C
c##### Laue mods - extra arrays for header info
c      original buffer array removed
C
        character*76 title
      character*6 crystal_id
        dimension x_c(6), y_c(6), w_c(6), y_scal(6), x_to_fra(6)
        dimension twists(6),tilts(6),bulges(6),dummy4(13)
        INTEGER ibuf(19), HXS,HYS,TST
        INTEGER    PACKID,PACK,DMM(5)
        INTEGER CORX(2),CORY(2),CORBAR(2)
        REAL CORANG(2),rast
        COMMON/SW/ISWTCH(10),oneref,noref,tworef
        COMMON/IOO/IOUT
        common/pel/iba,ijba(5120),nrec
        integer*2 ijba,iba
        LOGICAL oneref,noref,tworef
        common/plotsc/dlscal
      common/fgname/ge_file,iendpos
      character*40 ge_file,genfile
        character*1 junk
c
c##### laue mods - increase to 6 films per pack
        DIMENSION IFDAT(6),IFLDAT(6)
        DATA IFDAT/'A ','B ','C ','D ','E ','F '/
        DATA IFLDAT/'a ','b ','c ','d ','e ','f '/
C
C---------------------- ASK FOR PACKID AND FILM
C
c##### laue mods - remove idxfil = 1
1       continue
        if(.not.batch)write(6,900)
        if(.not.batch)WRITE(IOUT,900)
900     FORMAT(' PACK NUMBER (FREE FORMAT):',$)
        read(5,*) PACKID
        IF(.NOT.BATCH)write(iout,905) packid
905     format(1h+,I4)
c
        if(image) goto 21
      if(.not.batch)write(6,910)
        if(.not.batch)WRITE(IOUT,910)
910     FORMAT(' FILM NUMBER (A,B,C,D,E,F):',$)
        read(5,920) FILM
920     FORMAT(A1)
        IF(.NOT.BATCH)write(iout,930) film
930     format(1h+,a1)
c##### laue mods - increase to 6 films
        DO 2 I=1, 6
        IF(FILM.EQ.IFDAT(I))GO TO 3
        IF(FILM.EQ.IFLDAT(I))GO TO 3
2       CONTINUE
21      I=1
3       FILM=I
c
c##### laue mods - read title and crystal and general header info
c
      read(1, rec=1) title
      read(1, rec=2) crystal_id,x_cen_fs,y_cen_fs,(w_fs(i),i=1,5)
      read(1, rec=3) w_fs(6)
      cent_in = .true.
      if(x_cen_fs(film).eq.0.and.y_cen_fs(film).eq.0) then
      cent_in = .false.
      endif
c
c##### laue mods - read raster(mm) and box sizes
c
      read(1,rec=4) rastin,iras,dummy4
      rast = (rastin*1000.)
C
C------------ SCANSZ IS THE SCANNER UNIT IN MULTIPLES OF 25 MICRONS
C
        SCNSZ=RAST/25.
C
C--------------FACT IS THE FACTOR USED THROUGH-OUT THE PROGRAM TO
C------ TRANSFORM FROM GENERATE-FILE UNITS (10 MICRON) TO SCANNER UNITS.
C
        FACT=0.4/SCNSZ
      if(streak) then
            if(.not.bgplan) then
                    bminor = bminor/2.0*100.*fact
                    cmask = cmask/2.0*100.*fact
                    dmask = dmask*100.*fact
             if (binvar) then
                ammax = ammax/2.0*100.*fact
                ammin = ammin/2.0*100.*fact
             end if
            end if
      end if
c
c##### laue mods - read twists,tilts,bulges,c_to_fd (mm)
c
      read(1,rec=6) twists, tilts, bulges, c_to_fd
      itilt  = tilts(film)
      itwist = twists(film)
      ibulge = bulges(film)
      ctofd  = nint(c_to_fd * 100.0)
c
c##### laue mods - read x_c, y_c, w_c, delta (mm)
c
      read(1, rec=5) x_c, y_c, w_c, delta
c
c##### laue mods - convert delta to 10 micron
c
      delta = delta * 100.
c
c##### laue mods - read y_scal, x_to_fra, mint
c
      read(1, rec=7) y_scal, x_to_fra, mint
      yscal  =  y_scal(Film)
      xtofra =  x_to_fra(Film)
c
c##### laue mods - convert from mm units to 10 micron
c
      ccx    =  nint(x_c(Film)*100.)
      ccy    =  nint(y_c(Film)*100.*yscal)
      ccom   =  w_c(Film)
c
c##### laue mods - read pack and spot numbers
c
      read(1,rec=10) ibuf
      pack = ibuf(1)
      tospt = ibuf(2)
c
c##### laue mods - select 'nodal' limits
c
       write(iout, 4)
C
        IF(.not.batch) THEN
         if(.not.batch)write(6, 4)
    4    format(/,' APPROX NUMBERS OF REFINEMENT NODALS [300,1000]:',$)
         read(5,*) lim1,lim2
    5    format(2i8)
        ELSE
         Read(5,*) lim1,lim2
        END IF
C
      if(lim1.eq. 0)lim1 = 300
      if(lim2 .eq.0)lim2 = 1000
      write(iout,6)lim1,lim2
6     format(1h+,2I8)
c     initialise nodlims to use all nodals
      nodlim1 = 12
      nodlim2 = 12
      nodmax = 12
      do i = 1,12
         if(ibuf(i+6).le.lim1) nodlim1 = i
         if(ibuf(i+6).le.lim2) nodlim2 = i
         if(ibuf(i+6).le.2000) nodmax = i
      enddo
c
c##### laue mods - set miscellaneous values for laues
c
      packs = 1
      jumpax = 1
      ivee = 0
      films = 6
      cbar   =  0
c
c##### laue mods - base box on delta overlap value
c
c      iras(1) = (delta * fact) + 1
c      if(mod(iras(1),2).eq.0) iras(1) = iras(1)+1
c      iras(2)= iras(1)
c      iras(3) = 5
c      iras(4) = 1
c      iras(5) = iras(4)
c
      iprof = 0
      if (profil) iprof = 1
      call getgexy(1, tospt, film, mint, iprof, nodmax)
      if(xtek) call tkclr
C
C--------------------------------- PRINT TITLE
C##### laue mods
c
        WRITE(IOUT,1002)
        if(.not.batch)write(6,1002)
1002    FORMAT(/10X,10('*****')/)
        WRITE(IOUT,1003) crystal_id(1:6), title
        if(.not.batch)write(6,1003) crystal_id(1:6), title
1003    FORMAT (20X,a6,/,1x,a76)
C
C----------------------------- PRINT PACKID AND FILM
C
        I=IFDAT(FILM)
        WRITE(IOUT,1004) PACKID,I
        if(.not.batch)write(6,1004)PACKID,I
1004    FORMAT(20X,'IMAGE  ',I5,A1)
        WRITE(IOUT,1002)
        if(.not.batch)write(6,1002)
C
C
C-------------------- PRINT SCANNER UNIT
C
c##### laue mods - replace buf(6) by rast
c
        WRITE(IOUT,1005) rast
        if(.not.batch)write(6,1005) rast
1005    FORMAT(' SCANNER UNIT:',f7.2,'  MICRONS')

C
C---------------------------- GET BOX PARAMETERS IN SCANNER UNITS
C-------------- TRANSFORM SOME OF THE BOX PARAMETERS TO ODD NUMBER
C------------------------------ OF SCANNER UNITS
C
        Ichange = 0
11      HXS=NXS/2
        HYS=NYS/2
        NXS=HXS*2+1
        NYS=HYS*2+1
C
C-------------------- PRINT BOX PARAMETERS AND READ NEW ONES
C--------------------------- DRAW SHAPE OF MEASUREMENT BOX
C
        CALL PLTRAS(IRAS)
        WRITE(IOUT,1006)
        WRITE(IOUT,1007) IRAS
        if(Ichange.eq.1) goto 13
        if(.not.batch)write(6,1006)
        if(.not.batch)write(6,1007)IRAS
        IF(.NOT.BATCH)read(5,1008) DMM
        IF(BATCH)read(5,*) DMM
1006    FORMAT('  NXS  NYS   NC  NRX  NRY ')
1007    FORMAT(5I5,' ? (I5 INPUT TO MODIFY)')
1008    FORMAT(5I5)
        if(batch) ichange = 1
        TST=0
        DO 12 I=1,5
        IF(DMM(I).EQ.0) GOTO 12
      if(dmm(i).lt.0) dmm(i) = 0
        TST=1
        IRAS(I)=DMM(I)
12      CONTINUE
        IF(TST.EQ.1) GOTO 11
13      IF(XTEK)CALL TKCLR
C
C----------------------- UPDATE RASTER PARAMETERS ON GENERATE FILE
C##### laue mods
c
        WRITE(1,REC=4) Rastin,Iras,Dummy4
C
C---------------------- TAKE CARE OF PERPENDICULAR DISTANCE TO FILM
C------------------------ FOR VEE-SHAPED CASSETTE = CTOFD*SQRT(3)/2
C
        DTOFD=CTOFD
        IF(VEE) DTOFD=DTOFD*0.866
C
C----------- FACTOR TO CONVERT FROM 1/100TH DEG TO DISTORTION UNITS
C
        FDIST=3.14159/(CTOFD*18000.)
C
C------  NOTE THAT FOR VEE FILMS, TILT IS EQUIVALENT TO VBNEG, TWIST IS
C-----------  EQUIVALENT TO VBPOS AND BULGE IS EQUIVALENT TO VTWIST.
C-----------  ONLY VALUES OF TILT, TWIST AND BULGE ARE STORED BACK
C----------------  IN THE GENERATE FILE FOR B AND C FILMS.
C
        BULGE=BULGEs(film)*FDIST
        IF(VEE) FDIST=1.0/CTOFD
        TWIST=TWISTs(film)*FDIST
        TILT=TILTs(film)*FDIST
        VTILT=0.0
        VVERT=0.0
C
C----------------------------------------------------
c
      genfile = ge_file(1:iendpos) // '.gen'
      if(.not.batch)write(6,788)genfile
      write(iout,788)genfile
788      format(1x,'Opening constants file ',a)
c------------------------------------------------------
C-------------------- READ CONSTANTS FROM FILE 'laueflm.dat'
C##### laue mods
c
7881    OPEN(UNIT=4,FILE=genfile,STATUS='OLD',iostat = imok)
      if(imok.eq.0) goto 7889
      if(.not.batch)write(6,7882) genfile
      write(iout,7882) genfile
7882      format(1x,' ** Unable to find file ** ',a)
      if(batch) then
            if(genfile.eq.'laueflm.dat') stop
            genfile = 'laueflm.dat'
           else
            if(.not.batch)write(6,7883)
            write(iout,7883)
7883      format(/1x,'Name of constants file? ',$)
      read(5,7884) genfile
7884      format(a40)
      end if
      goto 7881
c
c##### laue mods - change laueflm.dat to reduced info on fiducials
c
7889      continue
        do 789 irgen = 1,8
789     read(4,790,end=791) crap
790     format(a1)
      goto 792
791     rewind 4
792      if(mar) then 
                  read(1, rec=7) y_scal, x_to_fra, mint,eps,roff,toff
            if(roff.eq.0.0.and.toff.eq.0.0) then
               read (4,*) crap1,crap2,roff,toff
                     write(1, rec=7) y_scal, x_to_fra, mint,eps,roff,toff
            else
               read(4,790) crap
            endif
      else
            read (4,790) crap
      end if
      read (4,*) yscgen
      read (4,*) crap      
      READ (4,*) NFID,((FSPOS(I,K),K=1,2),I=1,NFID)
        READ (4,*) WFILM,MM,N1OD,BASEOD,G1OD,CURV
        MM=50*MM
        READ(4,*)NREC,IYLEN,DLSCAL,xcenp,ycenp,rastin
        xcenf=xcenp/fact
        ycenf=ycenp/fact
        if(.not.image)then
        NWORD=IYLEN/2
        else
        nword=iylen
        xcen=xcenf+ccx
        ycen=ycenf+ccy
        cosom0=cos(omega0)
        sinom0=sin(omega0)
        endif
c
c----- read new genfile line for line scan ip distortion
c
      if(moldyn) then
      write(8,7922)
7922      format(1x,' logical moldyn set as .TRUE.')
      read(4,*,end=7925) spdx_min,spdx_max,nspdx,(spdx(ni),ni=1,nspdx)
      read(4,*,end=7925) spdy_min,spdy_max,nspdy,(spdy(ni),ni=1,nspdy)
      read(4,*,end=7925) spdxy
      spdx_min = spdx_min*100.
      spdx_max = spdx_max*100.
      spdy_min = spdy_min*100.
      spdy_max = spdy_max*100.
7925    CONTINUE
      endif
c
      if(fspos(1,1).lt.-3000) then
            goto 799
      else
            do 793 ifd = 1,nfid
            do 793 kfd = 1,2
793            fspos(ifd,kfd) = ((fspos(ifd,kfd) - nword)*rast)/10
      end if
799     CLOSE (UNIT=4)
      if(.not.image) then
        WRITE(IOUT,1009) BASEOD,G1OD,CURV
        if(.not.batch)write(6,1009)BASEOD,G1OD,CURV
1009    FORMAT(/10X,'FILM CHARACTERISTICS'/' BASE O.D.=',F4.2,
     1    ' GRANULARITY=',F4.1,' NON-LINEARITY FACTOR=',F5.2/)
1010    FORMAT(18A4)
      endif
        RETURN
        END
C
C
C     SUBROUTINE TO SET UP THE STANDARD HAT-MATRIX.
C
C
      SUBROUTINE STPHAT(ibin,HX,HY)
C
      INCLUDE 'comuab.finc'
      INCLUDE 'comhat.finc'
      INCLUDE 'commaskl.finc'
C
      REAL XTX(3,3),BUF1(3),BUF2(3),
     *     XXINV(5000,3),HATT(3,5000),
     *     HATTSP(3,5000),HATSP(5000,3)
      INTEGER HX,HY,hprint(5000)
c   zero counters
      ratrej = 0
      rgdrej = 0
      ogdrej = 0
      idir = -1
C
C-----------SET UP HATSP MATRIX
C
        NN=HPOBS(ibin)
      DO 100 I=1,NN
      HATP(I,1,ibin) = 1
         DO 110 J=1,3
              HATSP(I,J)=HATP(I,J,ibin)
            HATTSP(J,I)=HATSP(I,J)
110      CONTINUE
100   CONTINUE
C
      DO 200 I=1,3
      DO 200 J=1,3
         XTX(I,J)=0.0
         DO 210 K=1,NN
            PROD=HATTSP(I,K)*HATSP(K,J)
            XTX(I,J)=XTX(I,J)+PROD
210      CONTINUE
200   CONTINUE
C
C-----------------INVERT XTX
C
      CALL MINV(XTX,3,DET,BUF1,BUF2)
C
      DO 300 I=1,NN
         DO 310 J=1,3
            XXINV(I,J)=0.0
            DO 320 K=1,3
               PROD=HATSP(I,K)*XTX(K,J)
               XXINV(I,J)=XXINV(I,J)+PROD
320         CONTINUE
310      CONTINUE
300   CONTINUE
C
C----------CALCULATE HSP(I)'S-----------
C
      DO 400 I=1,NN
         Hsp(I,ibin)=0.0
         DO 410 K=1,3
            PROD=XXINV(I,K)*HATTSP(K,I)
            HSP(I,ibin)=HSP(I,ibin)+PROD
410      CONTINUE
400   CONTINUE
C
C-------------------------- PUT INTO ARRAY HX,HY
C
c      if(.not.correct) return
c      if(.not.streak) return
c      NP = 0
c      DO 510 I=-Hx,Hx,idir
c      DO 509 J=-Hy,Hy
c      NP = NP + 1
c      hprint(np) = 0
c      DO 508 K=1,NN
c      IF(int(HATP(K,2,ibin)).EQ.I.AND.int(HATP(K,3,ibin)).EQ.J)
c     1          THEN
c            HPRINT(NP) = nint(1000.*HSP(K,ibin))
c            goto 509
c      END IF
c508      CONTINUE
c509      CONTINUE
c510      CONTINUE
C
c      if(.not.batch)write(6,606) ibin
c606      format(1x,' Standard values of H(i) for bin ',i3,/)
c      write(8,607) ibin
c607      format(1h1,' Standard values of H(i) for bin ',i3,/)
c      nend = ((2*iabs(hx) + 1) * (2*iabs(hy) + 1))
c      nst = 1
c      istp = -(2*hy + 1)
c      DO 610 I=-hy,hy
c      WRITE(8,609) (HPRINT(J),J=nend,nst,istp)
c      if(.not.batch)write(6,608) (HPRINT(J),J=nend,nst,istp)
c608      FORMAT(2X,40i3)
c609      FORMAT(/1x,40i3)
c      nend = nend - 1
c610      CONTINUE
C      
C
      RETURN
      END
C
C
C           SUBROUTINE TO ADD IN A BG POINT OF VALUE
C         B AT X,Y (RELATIVE TO CENTRE). THE ELEMENTS OF
C       THE AM MATRIX ARE TAKEN CARE OF IN AMBCK, AND THEN
C         BY OFFBCK IF POINT(S) ARE REMOVED.
C
      SUBROUTINE SUMBCK(J, B, P, Q, SIGN)
C
      INCLUDE 'combck.finc'
      INCLUDE 'comprf.finc'
C
      INTEGER P, Q, SIGN
C
      BOP(J) = BOP(J) +  B * FLOAT(P)
      BOQ(J) = BOQ(J) +  B * FLOAT(Q)
      BO(J) = BO(J) +  B
      RETURN
      END
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE TDATE ****        *
C                      *                                           *
C                      *********************************************
C
C
C
        SUBROUTINE TDATE
C
      INCLUDE 'comuab.finc'
C
        CHARACTER*8 ITIME
        COMMON/IOO/IOUT
        DIMENSION JDATE(3)
        CALL IDATE(JDATE(2),JDATE(1),JDATE(3))
        CALL TIME(ITIME)
        WRITE(IOUT,1000) (ITIME),(JDATE(J),J=1,3)
        if(.not.batch)write(6,1000)(ITIME),(JDATE(J),J=1,3)
1000    FORMAT(' OUTPUT FROM INTLAUE AT ',A8,' ON ',I2
     1              ,'/',I2,'/',I2,'.',//)
        RETURN
        END
C
      SUBROUTINE TKCLR
      CALL QGCLR
      RETURN
      END
C
      SUBROUTINE TKPLOT(KX,KY,IFLAG,ICHK)
C
C PARAMETERS
C
C     KX (I)   X POSITION (ACROSS SCREEN LEFT TO RIGHT) 0 TO 1023
C     KY (I)   Y POSITION (UP SCREEN BOTTOM TO TOP) 0 TO 767
C  IFLAG (I)   =0 DRAW, =1 MOVE, =2 POINT PLOT
C   ICHK (O)   =1 OK, =0 ERROR IN CALL
C
C
C SPECIFICATION STATEMENTS
C ------------------------
C
      CHARACTER*1 GS, FS
      CHARACTER*4 POS
C
C CODE
C ----
C
      CALL QGENTR
      GS=CHAR(29)
      FS=CHAR(28)
      ICHK=1
      IF (IFLAG.GT.2) THEN
         ICHK=0
         RETURN
      ENDIF
      IX = KX
      IY = KY
      IF (IX.LT.0) IX=0
      IF (IX.GT.1023) IX=1023
      IF (IY.LT.0) IY=0
      IF (IY.GT.767) IY=767
      X=FLOAT(IX)
      Y=FLOAT(IY)
      IF (IFLAG.EQ.1) THEN
       CALL QGMOVE(X,Y)
       RETURN
      ENDIF
c
      IF (IFLAG.EQ.0) THEN
       CALL QGDRAW(X,Y)
       RETURN
      ENDIF
c
      IF (IFLAG.EQ.2) THEN
      JXH = IX/32
      JYH = IY/32
      JXL = IX-32*JXH
      JYL = IY-32*JYH
      POS(1:1) = CHAR(JYH+32)
      POS(2:2) = CHAR(JYL+96)
      POS(3:3) = CHAR(JXH+32)
      POS(4:4) = CHAR(JXL+64)
      CALL QGSEND (POS)
      ENDIF
      RETURN
      END
C
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE VECPLOT****       *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C
C       SUBROUTINE VECPLOT
C
C  PLOTS IMAGE OF FILM FROM STORED VECTORS
C
C
C=============================================================
C
C
        SUBROUTINE VECPLOT
        ifail=0
        call ccpdpn(-23,'SPOTVEC','OLD','U',0,ifail)
1       READ(23,END=2) IX,IY
        CALL MOVTX(IX,IY)
        READ(23,END=2) IX,IY
        CALL VECTX(IX,IY)
        CALL ALPHA
        GOTO 1
2       CLOSE (UNIT=23)
        RETURN
        END
C
      SUBROUTINE VECTX(IX,IY)
C  REPLACE LAUELIB ROUTINE WITH QGDRAW
      X=FLOAT(IX)
      Y=FLOAT(IY)
      CALL QGDRAW(X,Y)
      RETURN
      END
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE WRGEN   ****      *
C                      *                                           *
C                      *********************************************
C
C
C
c##### laue mods
C=============================================================
C
        SUBROUTINE WRGEN(N,UNIT)
c        INTEGER GENBUF(10)
      integer*2 genbuf(38)
      integer UNIT
C
      INCLUDE 'comlaue.finc'
      INCLUDE 'comuab.finc'
      INCLUDE 'comras.finc'
      INCLUDE 'compro.finc'
C
        integer irsav(1000)
c
c save record numbers before sorting if IBOTH
c
        if(IBOTH) then
         do 100 irs = 1,n
100      irsav(irs) = irec(irs)
        END IF
c
        CALL SORTUP3(N,IREC,INTEN,ISD)
        DO 1 I=1,N
        NREC=IREC(I) + 10
        READ(UNIT,REC=NREC) GENBUF
        IF (INTEN(I).LE.32767) THEN
           GENBUF(IFILL)=INTEN(I)
        ELSE
           GENBUF(IFILL)=32767
        ENDIF
        IF(ISD(I).LE.32767) THEN
           GENBUF(IFILL+6 )=ISD(I)
        ELSE
           GENBUF(IFILL+6)=32767
        ENDIF
        WRITE(UNIT,REC=NREC) GENBUF
c     if this intensity is lt MINT set the 'no measure' flag
c     for subsequent films
      if ( inten(i).lt.mint) then
       if (inten(i).ne.-9999) then
         nodal(nrec-10) = or(int(nodal(nrec-10)),2)
       endif
      endif
c       if(inten(i).lt.1600) goto 1
c       if(inten(i).eq.9999) goto 1
c       write(8,1000) irec(i),inten(i),isd(i)
c1000   format(1x,' In wrgen, Rec,I  and sd  (step 1):',3i7)
  1     CONTINUE
        if(.not.iboth) return
        CALL SORTUP3(N,IRSAV,INTENb,ISDb)
        DO 2 I=1,N
        NREC=IREC(I) + 10
        READ(UNIT,REC=NREC) GENBUF
        GENBUF(IFILL-12)=INTENb(I)
        GENBUF(IFILL-6 )=ISDb(I)
        WRITE(UNIT,REC=NREC) GENBUF
c       if(inten(i).lt.1600) goto 2
c       if(inten(i).eq.9999) goto 2
c       write(8,2000) irec(i),intenb(i),isdb(i)
c2000   format(1x,' In wrgen, Rec,Ib and sdb (step 2):',3i7)
2       continue
        RETURN
        END
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE XYCAL  ****       *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C
C
        SUBROUTINE XYCALC(XC,YC,X,Y)
        REAL XC,YC,X,Y
        DATA A0/2.86E-7/,B0/2.78E-8/
cRCD ...Image Plate Logical
        common/iplate/image,mar,moldyn
        logical image,mar,moldyn
C
C------- A0 AND B0 DEFINE SHAPE OF BULGE FOR VEE FILMS
C------- THESE VALUES GIVE 2% AT X=3700 AND Y=6000
C
      INCLUDE 'comori.finc'
      INCLUDE 'commcs.finc'
c
c--- change comori to add distortion type and type 3 distortion
c--- distortion type is dstor, 3 for line scan ip
C
        XM=X*COSOM0-Y*SINOM0
        YM=X*SINOM0+Y*COSOM0
        IF(VEE) GOTO 1
        R=SQRT(X*X+Y*Y)
cRCD ...Change the calculation for AGWL's MOSFLM IP refinables
        if(r.eq.0.0)then
        cpsi = 1.0
        spsi = 0.0
        else
        cpsi = xm/r
        spsi = ym/r
        endif
c
        if(.not.image)then
        DXTOF=XTOFRA+TILT*X+TWIST*Y+BULGE*R
        XC=DXTOF*XM+XCEN
        YC=DXTOF*YM*YSCAL+YCEN
c
c----- MAR roff, toff parameters (if dstor ne 3)
c
        else if (mar) then
        dxtof=xtofra+tilt*x+twist*y
        xc=dxtof*xm+xcen-toff*spsi+roff*cpsi
        yc=dxtof*ym*yscal+ycen+toff*cpsi+roff*spsi
c
c----- line scan ip, new distortion correction
c
      else if(moldyn) then
              dxtof=xtofra+tilt*x+twist*y
              xc   = dxtof * xm
              yc   = dxtof * ym
c
            xc   = xc + SPDXY*yc
c
                XD = XC
                IF (XD.LT.SPDX_MIN) XD = SPDX_MIN
                IF (XD.GT.SPDX_MAX) XD = SPDX_MAX
                D = 0.0
                DD = 0.0
                XP = (2.0*XD-SPDX_MIN-SPDX_MAX)/(SPDX_MAX-SPDX_MIN)
                XP2 = 2.0*XP
                DO 100 I=NSPDX,2,-1
                   SV = D
                   D = XP2*D - DD + SPDX(I)
                   DD = SV
100             CONTINUE
                XC = XC + 100.0*(XP*D - DD + 0.5*SPDX(1))
                YD = YC
                IF (YD.LT.SPDY_MIN) YD = SPDY_MIN
                IF (YD.GT.SPDY_MAX) YD = SPDY_MAX
                D = 0.0
                DD = 0.0
                YP = (2.0*YD-SPDY_MIN-SPDY_MAX)/(SPDY_MAX-SPDY_MIN)
                YP2 = 2.0*YP
                DO 110 I = NSPDY,2,-1
                   SV = D
                   D = YP2*D - DD + SPDY(I) 
                   DD = SV
110             CONTINUE
                YC = YC + 100.0*(YP*D - DD + 0.5*SPDY(1))

            xc = xc + xcen
            yc = yc * yscal + ycen
      endif
        RETURN
C
C------- CONFUSING CHOICE OF VARIABLE NAMES
C------- BNEG=TWIST*CTOFD(*SIN(VEE))
C------- BPOS=BULGE*CTOFD(*SIN(VEE))
C------- XSHIFT=TILT*CTOFD(*TAN(VEE))
C
 1      CONTINUE
        DOV2=CTOFD/2
        ABSX=ABS(X)
        XDASH=ABSX-4300.
        XD=(ABSX-DOV2)
        XX=XDASH*XDASH*A0
        YY=Y*Y*B0
        FB=EXP(-XX-YY)
        FR=FB*XD
        BY=FB*Y
        SB=CBAR*SINOM0
        CB=CBAR*COSOM0
        DISTOR=VTILT*X + VTWIST*Y + VVERT*SIGN(1.0,X)
C
C------ FOLLOWING 2 LINES ARE VERSION REFINING VEE XSHIFT PARAMETER
C------ XC=XM*XTOFRA+XCEN+ABSX*TILT
C------ YC=YM*XTOFRA*YSCAL+YCEN+TILT*Y*SIGN(1.0,X)
C------ FOLLOWING 2 LINES ARE VERSION REFINING VEE TWIST PARAMETER
C------ XC=XM*XTOFRA+XCEN+TILT*X*Y
C------ YC=YM*XTOFRA*YSCAL+YCEN+TILT*Y*Y
C------ COORDS FOR REFINING TILT, TWIST AND VERT
C
        XC=XM*XTOFRA+XCEN+X*DISTOR
        YC=YM*XTOFRA*YSCAL+YCEN+Y*DISTOR
        IF(X.LT.0.0) GOTO 2
C
C------------------------- POSITIVE X
C
        XC=XC-VBPOS*FR-CB
        YC=YC-VBPOS*BY-SB
        RETURN
C
C------------------------- NEGATIVE X
C
 2      XC=XC+VBNEG*FR+CB
        YC=YC-VBNEG*BY+SB
        RETURN
        END
c**********************************************************
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE XYMD   ****       *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
C
C
        SUBROUTINE XYMD(XC,YC,X,Y)
        REAL XC,YC,X,Y
        DATA A0/2.86E-7/,B0/2.78E-8/
cRCD ...Image Plate Logical
        common/iplate/image,mar,moldyn
        logical image,mar,moldyn
C
C------- A0 AND B0 DEFINE SHAPE OF BULGE FOR VEE FILMS
C------- THESE VALUES GIVE 2% AT X=3700 AND Y=6000
C
      INCLUDE 'comori.finc'
      INCLUDE 'commcs.finc'
c
c--- change comori to add distortion type and type 3 distortion
c--- distortion type is dstor, 3 for line scan ip
C
        XM=X*COSOM0-Y*SINOM0
        YM=X*SINOM0+Y*COSOM0
c
c----- line scan ip, new distortion correction
c
              dxtof=xtofra+tilt*x+twist*y
              xc   = dxtof * xm
              yc   = dxtof * ym
c
            xc   = xc + SPDXY*yc
c
                XD = XC
                IF (XD.LT.SPDX_MIN) XD = SPDX_MIN
                IF (XD.GT.SPDX_MAX) XD = SPDX_MAX
                D = 0.0
                DD = 0.0
                XP = (2.0*XD-SPDX_MIN-SPDX_MAX)/(SPDX_MAX-SPDX_MIN)
                XP2 = 2.0*XP
                DO 100 I=NSPDX,2,-1
                   SV = D
                   D = XP2*D - DD + SPDX(I)
                   DD = SV
100             CONTINUE
                XC = XC + 100.0*(XP*D - DD + 0.5*SPDX(1))
                YD = YC
                IF (YD.LT.SPDY_MIN) YD = SPDY_MIN
                IF (YD.GT.SPDY_MAX) YD = SPDY_MAX
                D = 0.0
                DD = 0.0
                YP = (2.0*YD-SPDY_MIN-SPDY_MAX)/(SPDY_MAX-SPDY_MIN)
                YP2 = 2.0*YP
                DO 110 I = NSPDY,2,-1
                   SV = D
                   D = YP2*D - DD + SPDY(I) 
                   DD = SV
110             CONTINUE
                YC = YC + 100.0*(YP*D - DD + 0.5*SPDY(1))

            xc = xc + xcen
            yc = yc * yscal + ycen
        RETURN
      END
c**********************************************************
C
C
C
C
C                      *********************************************
C                      *                                           *
C                      *        ****  SUBROUTINE XYSHIFT ****      *
C                      *                                           *
C                      *********************************************
C
C
C
C=============================================================
C
        SUBROUTINE XYSHIFT(XSHIFT,YSHIFT,IX,IY,XSH,YSH,NSH,K)
cRCD ...Image Plate logical
        common/iplate/image,mar,moldyn
        logical image,mar,moldyn
c
        REAL XSHIFT,YSHIFT,XSH(1),YSH(1)
        INTEGER IX,IY,NSH(1)
        COMMON/IOO/IOUT
        COMMON/SW/ISWTCH(10),oneref,noref,tworef
C
      INCLUDE 'comuab.finc'
C
        CHARACTER*1 JUNK
        LOGICAL oneref,noref,tworef
        GOTO (1,2),K
1       I = (IX + 9600)/3200 + (((9600 - IY)/3200) - 1) * 4
cRCD ...Change this for 9cm IP
        if(mar)
     1  I = (IX + 14100)/4700 + (((14100 - IY)/4700) - 1) * 4
cTJG ...Change this for cornell IP
      if(moldyn)
     1  I = (IX + 19200)/6400 + (((16500 - IY)/5500) - 1) * 4
        XSH(I)=XSH(I)+XSHIFT
        YSH(I)=YSH(I)+YSHIFT
        NSH(I)=NSH(I)+1
        RETURN
2       DO 3 J=1,16
        IF(NSH(J).EQ.0) GOTO 3
        XSH(J)=XSH(J)/NSH(J)
        YSH(J)=YSH(J)/NSH(J)
3       CONTINUE
        WRITE(IOUT,1000)
        if (.not.batch)WRITE(6,1000)
        WRITE(IOUT,1001) (NSH(I),XSH(I),YSH(I),I=1,16)
        if (.not.batch)WRITE(6,1001)(NSH(I),XSH(I),YSH(I),I=1,16)
1000    FORMAT(//'     ANALYSIS OF C. OF G. SHIFTS FOR STRONG',
     1        '  SINGLETS (10 MICRON UNITS)'/
     1        '    NO.    DX    DY   NO.    DX    DY',
     1        '   NO.    DX   DY    NO.    DX   DY')
1001    FORMAT(4(I6,2F6.1)/)
        RETURN
        END
