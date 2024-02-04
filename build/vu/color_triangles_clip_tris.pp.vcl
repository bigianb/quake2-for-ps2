
.name VU1Prog_Color_Triangles

.init_vi VI02, VI03, VI04, VI05, VI06, VI07, VI08, VI09, VI10, VI11, VI12, VI13, VI14
.init_vf_all
.syntax new
.vu

--enter
--endenter

    fcset 0
    ilw.w iNumVerts, 4(vi00)
    iaddiu iVert,    vi00, 0 
    iaddiu iVertPtr, vi00, 0 
    lq fScales, 4(vi00)

    lq fMVPMatrix[0], 0+0(vi00)
    lq fMVPMatrix[1], 0+1(vi00)
    lq fMVPMatrix[2], 0+2(vi00)
    lq fMVPMatrix[3], 0+3(vi00)

    lTrianglesLoop:

    lq fVert0, 7+0(iVertPtr)
    mul  acc,    fMVPMatrix[0], fVert0[x]
    madd acc,    fMVPMatrix[1], fVert0[y]
    madd acc,    fMVPMatrix[2], fVert0[z]
    madd fVert0, fMVPMatrix[3], fVert0[w]
    clipw.xyz fVert0, fVert0
    div q, vf00[w], fVert0[w]
    mul.xyz fVert0, fVert0, q
    mula.xyz  acc,    fScales, vf00[w]
    madd.xyz  fVert0, fVert0, fScales
    ftoi4.xyz fVert0, fVert0


    lq fVert1, 7+2(iVertPtr)
    mul  acc,    fMVPMatrix[0], fVert1[x]
    madd acc,    fMVPMatrix[1], fVert1[y]
    madd acc,    fMVPMatrix[2], fVert1[z]
    madd fVert1, fMVPMatrix[3], fVert1[w]
    clipw.xyz fVert1, fVert1
    div q, vf00[w], fVert1[w]
    mul.xyz fVert1, fVert1, q
    mula.xyz  acc,    fScales, vf00[w]
    madd.xyz  fVert1, fVert1, fScales
    ftoi4.xyz fVert1, fVert1


    lq fVert2, 7+4(iVertPtr)
    mul  acc,    fMVPMatrix[0], fVert2[x]
    madd acc,    fMVPMatrix[1], fVert2[y]
    madd acc,    fMVPMatrix[2], fVert2[z]
    madd fVert2, fMVPMatrix[3], fVert2[w]
    clipw.xyz fVert2, fVert2
    div q, vf00[w], fVert2[w]
    mul.xyz fVert2, fVert2, q
    mula.xyz  acc,    fScales, vf00[w]
    madd.xyz  fVert2, fVert2, fScales
    ftoi4.xyz fVert2, fVert2

        fcand  vi01, 0x3FFFF
        iaddiu iADC, vi01, 0x7FFF
        sq.xyz fVert0, 7+0(iVertPtr)
        isw.w  iADC,   7+0(iVertPtr)
        sq.xyz fVert1, 7+2(iVertPtr)
        isw.w  iADC,   7+2(iVertPtr)
        sq.xyz fVert2, 7+4(iVertPtr)
        isw.w  iADC,   7+4(iVertPtr)
        iaddiu iVertPtr, iVertPtr, 6
        iaddiu iVert, iVert, 3
        ibne   iVert, iNumVerts, lTrianglesLoop
    iaddiu iGIFTag, vi00, 5 
    xgkick iGIFTag                

--exit
--endexit

