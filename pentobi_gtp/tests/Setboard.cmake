set(filled "(;GM[Blokus]A1[b19,c19][a20]A2[t20]PL[2])")
set(empty "(;GM[Blokus]PL[4])")
set(blocked "(;GM[Blokus]A1[a1,a2,a3,b1][a6,a5,b5,b4,c4][b7,b8][a9,a10,a11][b12,b13,b14,b15][a16,a17,a18,a19,a20]PL[4])")
set(full "(;GM[Blokus]A1[j7][f5,g5][s6,s7,t7][c3,c4,c5][r8,r9,r10,r11][d6,e6,d7,e7][g7,h7,h8,i8][f8,f9,g9,f10][o8,p8,o9,o10][h10,i10,j10,k10,l10][p11,o12,p12,q12,p13][o18,n19,o19,m20,n20][n15,o15,p15,p16,p17][m17,k18,l18,m18,k19][m11,n11,m12,m13,m14][j15,l15,j16,k16,l16][h16,h17,i17,i18,i19][g14,e15,f15,g15,f16][a16,b16,c16,d16,c17][d18,e18,f18,d19,e19][b18,b19,a20,b20,c20]PL[1])")
string(CONCAT terminal "(;GM[Blokus]"
    "A1[r13,r14,s14,s15,t15][r11,s11,t11][o13,p12,p13,q12][i17,j16,j17,j18][a7,a8,b8,c7,c8][m14,n14][a11,b11,c11,c12,c13][m11][j12,k12,l12,l13][d10,d9,e10][g14,h14,i13,i14,j14][f11,g10,g11,g12,h12][e16,f16,g16,h16][d14,d15,e12,e13,e14][b16,b17,c16,c17,d17][a18,a19,a20,b19,c19]"
    "A2[f19,f20][j13][c20,d18,d19,d20][m12,n12,n13,o12][e17,f17,g17,g18,h17][k14,k15,l15][h19,h20,i19,j19,k19][m16,n16,o14,o15,o16][o18,p17,p18,q17][l17,l18,m18,m19,n19][o20,p20,q20][r15,r16,s16,s17,t16][r19,s19,t18,t19,t20]"
    "A3[o1,p1,q1][t7][k10,l10,l11][p14,q13,q14,q15][m8,m9,n10,n9,o10][r12,s12,s13,t13][s8,s9][k4,k5,l5,m5][p11,q10,q11,q9,r10][o6,o7,o8,p6,p8][q5,r4,r5,r6,s5][n3,n4,o3,p3,q3][r2,s1,s2,t1,t2]"
    "A4[g15,h15,i15,i16,j15][l7,m7,n6,n7,n8][f12,f13,f14][d11,d12,e11][k6][i10,i11,j10,j11][c2,c3,d1,d2][a9,b9,c10,c9][j3,k2,k3,l1,l2][h8,h9,i8,j7,j8][h2,h3][g6,h6,i4,i5,i6][e4,f3,f4,f5,g4][d8,e7,e8,e9,f9][b6,c5,c6,d5,d6][a1,a2,a3,a4,b4]"
    "PL[1])")
string(REPEAT "a" 16385 too_long)

set(prefix "setboard")
include(${CMAKE_CURRENT_LIST_DIR}/SetboardUtil.cmake)

run(classic "1 known_command set_board
2 set_board ${filled}
3 showboard
4 all_legal 2
5 reg_genmove 2
6 showboard
7 set_board (;GM[Blokus]A1[a20]A1[b19]PL[2])
8 set_board (;GM[Blokus]A1[a20]A2[a20]PL[2])
9 set_board (;GM[Blokus]A1[a20])
10 set_board (;GM[Blokus]PL[2])junk
27 set_board (;GM[Blokus]PL[0])
28 set_board (;GM[Blokus]PL[2];2[t20])
29 set_board ${too_long}
30 set_board (;GM[Blokus]A1[a20][b19]PL[2])
31 set_board (;GM[BlokusDuo]PL[2])
11 showboard
26 g
12 set_board ${empty}
13 all_legal 4
14 reg_genmove 4
15 set_board ${filled}
16 all_legal 2
17 reg_genmove 2
18 set_board ${blocked}
19 all_legal 4
20 all_legal 1
21 set_board ${full}
22 all_legal 1
23 all_legal 2
24 reg_genmove 2
32 set_board ${terminal}
33 all_legal 1
34 all_legal 2
35 all_legal 3
36 all_legal 4
25 quit
")
response(1 "=" known)
response(2 "=" initial)
response(3 "=" before)
response(6 "=" after_search)
foreach(id RANGE 7 10)
    response(${id} "?" rejection)
endforeach()
foreach(id RANGE 27 31)
    response(${id} "?" rejection)
endforeach()
response(11 "=" after_failures)
if(NOT known STREQUAL "true"
    OR initial OR NOT before STREQUAL after_search
    OR NOT before STREQUAL after_failures)
    message(FATAL_ERROR "Discovery, non-mutation, or atomic rejection failed")
endif()
check_move(4 5)
check_move(4 26)
check_move(13 14)
check_move(16 17)
check_move(23 24)
response(19 "=" blocked_legal)
response(20 "=" other_legal)
response(22 "=" full_legal)
if(blocked_legal OR NOT other_legal OR full_legal)
    message(FATAL_ERROR "Blocked-corner or exhausted inventory was not preserved")
endif()
response(32 "=" terminal_setup)
foreach(id RANGE 33 36)
    response(${id} "=" terminal_legal)
    if(terminal_legal)
        message(FATAL_ERROR "Terminal position has legal moves")
    endif()
endforeach()
response(4 "=" inline_legal)

set(sgf "${CMAKE_CURRENT_BINARY_DIR}/setboard-reference.blksgf")
file(WRITE "${sgf}" "${filled}")
run(classic "1 loadsgf \"${sgf}\"
2 all_legal 2
3 reg_genmove 2
")
response(2 "=" file_legal)
file(WRITE "${sgf}" "${empty}")
set(first "${CMAKE_CURRENT_BINARY_DIR}/setboard-filled.blksgf")
file(WRITE "${first}" "${filled}")
run(classic "1 loadsgf \"${first}\"
2 reg_genmove 2
3 loadsgf \"${sgf}\"
4 all_legal 4
5 reg_genmove 4
")
file(REMOVE "${sgf}" "${first}")
check_move(4 5)
if(NOT file_legal STREQUAL inline_legal)
    message(FATAL_ERROR "Inline and file setup differ")
endif()

# Turn from PL, undo back to the setup root, static rules, wrong variant
set(saved "${CMAKE_CURRENT_BINARY_DIR}/setboard-saved.blksgf")
run(classic "1 set_board ${filled}
2 p s19,s18
3 undo
4 p s19,s18
5 undo
6 undo
7 savesgf \"${saved}\"
8 set_board (;GM[Blokus]A1[a20][a19,b19]PL[1])
9 set_board (;GM[Blokus]A1[a20][c17,c16]PL[1])
10 set_board (;GM[Blokus]A1[b19]PL[1])
11 set_board (;GM[Blokus]A3[a20]PL[1])
12 set_board (;GM[Blokus]A1[A20]PL[1])
13 set_board (;GM[Blokus]A1[a20,a20]PL[1])
14 set_board (;GM[Blokus]A5[a20]PL[1])
15 set_game Blokus Duo
16 set_board (;GM[Blokus]PL[1])
")
foreach(id 1 2 3 4 5 7 15)
    response(${id} "=" ignored)
endforeach()
foreach(id 6 8 9 10 11 12 13 14 16)
    response(${id} "?" rejection)
endforeach()
file(READ "${saved}" saved_sgf)
file(REMOVE "${saved}")
if(NOT saved_sgf MATCHES "A1\\[" OR NOT saved_sgf MATCHES "A2\\[t20\\]"
    OR NOT saved_sgf MATCHES "PL\\[2\\]")
    message(FATAL_ERROR "savesgf does not contain the setup: ${saved_sgf}")
endif()
