# Tests of set_board in all game variants.
# Each position was taken from a game that pentobi-gtp played against itself.
# The setup must give the same legal moves as playing the moves of the game.

set(prefix "setboard-variants")
include(${CMAKE_CURRENT_LIST_DIR}/SetboardUtil.cmake)

# Set up a position, check that a rejected setup does not change it and that
# playing the moves of the game gives the same legal moves.
function(check_variant game color setup moves reject)
    run(${game} "1 set_board ${setup}
2 all_legal ${color}
3 reg_genmove ${color}
4 set_board ${reject}
5 all_legal ${color}
6 clear_board
${moves}7 all_legal ${color}
")
    response(1 "=" ignored)
    string(REGEX MATCH "\n\\?[1-9][0-9][0-9]+ [^\n]*" failed_move "\n\n${output}")
    if(failed_move)
        message(FATAL_ERROR "${game}: move of the game failed: ${failed_move}")
    endif()
    check_move(2 3)
    response(4 "?" ignored)
    response(2 "=" setup_legal)
    response(5 "=" after_reject)
    response(7 "=" moves_legal)
    if(NOT setup_legal STREQUAL after_reject)
        message(FATAL_ERROR "${game}: rejected setup changed the position")
    endif()
    if(NOT setup_legal STREQUAL moves_legal)
        message(FATAL_ERROR "${game}: setup and moves give different legal moves")
    endif()
endfunction()

# Check that each command of a variant succeeds or fails as expected.
function(check_responses game commands accepted rejected)
    run(${game} "${commands}")
    foreach(id ${accepted})
        response(${id} "=" ignored)
    endforeach()
    foreach(id ${rejected})
        response(${id} "?" ignored)
    endforeach()
endfunction()

# Blokus
string(CONCAT classic_setup
    "(;GM[Blokus]A1[c18,a19,b19,c19,a20][e15,f15,d16,e16,d17]"
    "[i12,i13,g14,h14,i14][j11,k11,k12,k13,k14][j15,i16,j16,k16,j17]"
    "A2[r18,r19,s19,t19,t20][o15,o16,p16,p17,q17][l12,m12,n12,n13,n14]"
    "[k15,l15,m15,l16,m16][l10,m10,n10,o10,o11]A3[t1,r2,s2,t2,r3][q4,p5,q5,o6,p6]"
    "[l7,m7,n7,l8,l9][j7,j8,j9,j10,k10][o8,p8,p9,q9,p10]A4[a1,a2,b2,c2,c3]"
    "[d4,e4,e5,f5,f6][g7,g8,g9,h9,i9][h5,i5,h6,i6,j6][k3,j4,k4,l4,k5]PL[1])")
string(CONCAT classic_moves
    "100 play 1 c18,a19,b19,c19,a20\n"
    "101 play 2 r18,r19,s19,t19,t20\n"
    "102 play 3 t1,r2,s2,t2,r3\n"
    "103 play 4 a1,a2,b2,c2,c3\n"
    "104 play 1 e15,f15,d16,e16,d17\n"
    "105 play 2 o15,o16,p16,p17,q17\n"
    "106 play 3 q4,p5,q5,o6,p6\n"
    "107 play 4 d4,e4,e5,f5,f6\n"
    "108 play 1 i12,i13,g14,h14,i14\n"
    "109 play 2 l12,m12,n12,n13,n14\n"
    "110 play 3 l7,m7,n7,l8,l9\n"
    "111 play 4 g7,g8,g9,h9,i9\n"
    "112 play 1 j11,k11,k12,k13,k14\n"
    "113 play 2 k15,l15,m15,l16,m16\n"
    "114 play 3 j7,j8,j9,j10,k10\n"
    "115 play 4 h5,i5,h6,i6,j6\n"
    "116 play 1 j15,i16,j16,k16,j17\n"
    "117 play 2 l10,m10,n10,o10,o11\n"
    "118 play 3 o8,p8,p9,q9,p10\n"
    "119 play 4 k3,j4,k4,l4,k5\n")
check_variant(classic 1 "${classic_setup}" "${classic_moves}"
    "(;GM[Blokus]A1[e15,f15,d16,e16,d17]PL[1])")

# Blokus Two-Player
string(CONCAT classic_2_setup
    "(;GM[Blokus Two-Player]A1[b18,c18,b19,a20,b20][f15,e16,f16,d17,e17]"
    "[g12,h12,i12,g13,g14][j8,j9,k9,j10,j11][h5,g6,h6,i6,i7][f3,e4,f4,g4,f5]"
    "A2[r18,s18,s19,s20,t20][o15,o16,p16,p17,q17][l12,l13,l14,m14,n14]"
    "[k10,l10,m10,n10,k11][o7,p7,o8,o9,p9][n3,n4,m5,n5,n6]A3[t1,r2,s2,t2,r3]"
    "[p4,q4,o5,p5,o6][n7,n8,l9,m9,n9][o10,o11,p11,q11,p12][q6,q7,q8,r8,q9]"
    "[o13,q13,o14,p14,q14]A4[a1,b1,b2,b3,c3][d4,d5,e5,e6,f6][g7,g8,g9,h9,i9]"
    "[f10,f11,g11,h11,i11][j12,k12,j13,k13,k14]PL[4])")
string(CONCAT classic_2_moves
    "100 play 1 b18,c18,b19,a20,b20\n"
    "101 play 2 r18,s18,s19,s20,t20\n"
    "102 play 3 t1,r2,s2,t2,r3\n"
    "103 play 4 a1,b1,b2,b3,c3\n"
    "104 play 1 f15,e16,f16,d17,e17\n"
    "105 play 2 o15,o16,p16,p17,q17\n"
    "106 play 3 p4,q4,o5,p5,o6\n"
    "107 play 4 d4,d5,e5,e6,f6\n"
    "108 play 1 g12,h12,i12,g13,g14\n"
    "109 play 2 l12,l13,l14,m14,n14\n"
    "110 play 3 n7,n8,l9,m9,n9\n"
    "111 play 4 g7,g8,g9,h9,i9\n"
    "112 play 1 j8,j9,k9,j10,j11\n"
    "113 play 2 k10,l10,m10,n10,k11\n"
    "114 play 3 o10,o11,p11,q11,p12\n"
    "115 play 4 f10,f11,g11,h11,i11\n"
    "116 play 1 h5,g6,h6,i6,i7\n"
    "117 play 2 o7,p7,o8,o9,p9\n"
    "118 play 3 q6,q7,q8,r8,q9\n"
    "119 play 4 j12,k12,j13,k13,k14\n"
    "120 play 1 f3,e4,f4,g4,f5\n"
    "121 play 2 n3,n4,m5,n5,n6\n"
    "122 play 3 o13,q13,o14,p14,q14\n")
check_variant(classic_2 4 "${classic_2_setup}" "${classic_2_moves}"
    "(;GM[Blokus Two-Player]A1[f15,e16,f16,d17,e17]PL[1])")

# Blokus Three-Player
string(CONCAT classic_3_setup
    "(;GM[Blokus Three-Player]A1[b18,c18,b19,a20,b20][f15,e16,f16,d17,e17]"
    "[i12,i13,g14,h14,i14][j11,k11,k12,k13,k14][g9,h9,h10,g11,h11]"
    "[j15,h16,i16,j16,j17]A2[r18,r19,s19,t19,t20][o15,o16,p16,p17,q17]"
    "[l12,m12,n12,n13,n14][k15,l15,m15,l16,m16][l10,m10,n10,o10,o11]"
    "A3[s1,t1,s2,r3,s3][p4,q4,o5,p5,o6][l7,m7,n7,l8,l9][j5,j6,k6,j7,j8]"
    "[o8,p8,p9,q9,p10]A4[a1,a2,b2,c2,c3][d4,d5,e5,e6,f6][h6,g7,h7,i7,h8]"
    "[i9,j9,i10,j10,k10][e8,f8,f9,f10,f11]PL[2])")
string(CONCAT classic_3_moves
    "100 play 1 b18,c18,b19,a20,b20\n"
    "101 play 2 r18,r19,s19,t19,t20\n"
    "102 play 3 s1,t1,s2,r3,s3\n"
    "103 play 4 a1,a2,b2,c2,c3\n"
    "104 play 1 f15,e16,f16,d17,e17\n"
    "105 play 2 o15,o16,p16,p17,q17\n"
    "106 play 3 p4,q4,o5,p5,o6\n"
    "107 play 4 d4,d5,e5,e6,f6\n"
    "108 play 1 i12,i13,g14,h14,i14\n"
    "109 play 2 l12,m12,n12,n13,n14\n"
    "110 play 3 l7,m7,n7,l8,l9\n"
    "111 play 4 h6,g7,h7,i7,h8\n"
    "112 play 1 j11,k11,k12,k13,k14\n"
    "113 play 2 k15,l15,m15,l16,m16\n"
    "114 play 3 j5,j6,k6,j7,j8\n"
    "115 play 4 i9,j9,i10,j10,k10\n"
    "116 play 1 g9,h9,h10,g11,h11\n"
    "117 play 2 l10,m10,n10,o10,o11\n"
    "118 play 3 o8,p8,p9,q9,p10\n"
    "119 play 4 e8,f8,f9,f10,f11\n"
    "120 play 1 j15,h16,i16,j16,j17\n")
check_variant(classic_3 2 "${classic_3_setup}" "${classic_3_moves}"
    "(;GM[Blokus Three-Player]A1[f15,e16,f16,d17,e17]PL[1])")

# Blokus Duo
string(CONCAT duo_setup
    "(;GM[Blokus Duo]AB[d9,c10,d10,e10,d11][h7,g8,h8,f9,g9][d6,d7,e7,f7,e8]"
    "[k8,l8,i9,j9,k9][c3,b4,c4,b5,c5]AW[j5,i6,j6,k6,j7][g5,h5,f6,g6,g7]"
    "[f3,e4,f4,d5,e5][c6,c7,b8,c8,c9][a3,a4,a5,a6,a7]PL[B])")
string(CONCAT duo_moves
    "100 play b d9,c10,d10,e10,d11\n"
    "101 play w j5,i6,j6,k6,j7\n"
    "102 play b h7,g8,h8,f9,g9\n"
    "103 play w g5,h5,f6,g6,g7\n"
    "104 play b d6,d7,e7,f7,e8\n"
    "105 play w f3,e4,f4,d5,e5\n"
    "106 play b k8,l8,i9,j9,k9\n"
    "107 play w c6,c7,b8,c8,c9\n"
    "108 play b c3,b4,c4,b5,c5\n"
    "109 play w a3,a4,a5,a6,a7\n")
check_variant(duo b "${duo_setup}" "${duo_moves}"
    "(;GM[Blokus Duo]AB[h7,g8,h8,f9,g9]PL[B])")

# Blokus Junior
string(CONCAT junior_setup
    "(;GM[Blokus Junior]AB[d10,e10,c11,d11,e11][f8,g8,h8,i8,f9]"
    "[g10,h10,i10,g11,h11][i12,j12,k12,l12,i13][d14,e14,f14,g14,h14]"
    "[n12,m13,n13,n14]AW[i5,j5,i6,j6,i7][k7,j8,k8,j9,k9][l10,i11,j11,k11,l11]"
    "[e12,f12,g12,h12,h13][i14,j14,k14,l14,m14][b12,c12,c13,d13]PL[B])")
string(CONCAT junior_moves
    "100 play b d10,e10,c11,d11,e11\n"
    "101 play w i5,j5,i6,j6,i7\n"
    "102 play b f8,g8,h8,i8,f9\n"
    "103 play w k7,j8,k8,j9,k9\n"
    "104 play b g10,h10,i10,g11,h11\n"
    "105 play w l10,i11,j11,k11,l11\n"
    "106 play b i12,j12,k12,l12,i13\n"
    "107 play w e12,f12,g12,h12,h13\n"
    "108 play b d14,e14,f14,g14,h14\n"
    "109 play w i14,j14,k14,l14,m14\n"
    "110 play b n12,m13,n13,n14\n"
    "111 play w b12,c12,c13,d13\n")
check_variant(junior b "${junior_setup}" "${junior_moves}"
    "(;GM[Blokus Junior]AB[f8,g8,h8,i8,f9]PL[B])")

# Blokus Trigon
string(CONCAT trigon_setup
    "(;GM[Blokus Trigon]A1[r12,q13,r13,q14,r14,r15][o11,p11,n12,o12,p12,n13]"
    "[l9,m9,n9,l10,m10,n10][s9,r10,s10,t10,r11,s11][n6,n7,o7,p7,q7,o8]"
    "[q16,r16,r17,s17,t17,s18][l12,i13,j13,k13,l13,i14]A2[r4,r5,s5,r6,s6,r7]"
    "[u6,v6,w6,x6,y6,z6][p8,q8,r8,s8,t8,p9][m7,j8,k8,l8,m8,n8]"
    "[k9,i10,j10,k10,i11,j11][f8,e9,f9,g9,h9,e10]A3[k11,l11,m11,n11,j12,k12]"
    "[o9,q9,r9,o10,p10,q10][v8,w8,t9,u9,v9,u10][t11,u11,t12,u12,v12,w12]"
    "[f10,f11,g11,g12,h12,h13][z10,aa10,y11,z11,y12,z12]A4[y7,z7,x8,y8,w9,x9]"
    "[t6,t7,u7,v7,w7,u8][v10,x10,y10,v11,w11,x11][x12,t13,u13,v13,w13,x13]"
    "[s14,t14,s15,t15,s16,t16][aa8,ab8,z9,aa9,ab9,ac9]PL[2])")
string(CONCAT trigon_moves
    "100 play 1 r12,q13,r13,q14,r14,r15\n"
    "101 play 2 r4,r5,s5,r6,s6,r7\n"
    "102 play 3 k11,l11,m11,n11,j12,k12\n"
    "103 play 4 y7,z7,x8,y8,w9,x9\n"
    "104 play 1 o11,p11,n12,o12,p12,n13\n"
    "105 play 2 u6,v6,w6,x6,y6,z6\n"
    "106 play 3 o9,q9,r9,o10,p10,q10\n"
    "107 play 4 t6,t7,u7,v7,w7,u8\n"
    "108 play 1 l9,m9,n9,l10,m10,n10\n"
    "109 play 2 p8,q8,r8,s8,t8,p9\n"
    "110 play 3 v8,w8,t9,u9,v9,u10\n"
    "111 play 4 v10,x10,y10,v11,w11,x11\n"
    "112 play 1 s9,r10,s10,t10,r11,s11\n"
    "113 play 2 m7,j8,k8,l8,m8,n8\n"
    "114 play 3 t11,u11,t12,u12,v12,w12\n"
    "115 play 4 x12,t13,u13,v13,w13,x13\n"
    "116 play 1 n6,n7,o7,p7,q7,o8\n"
    "117 play 2 k9,i10,j10,k10,i11,j11\n"
    "118 play 3 f10,f11,g11,g12,h12,h13\n"
    "119 play 4 s14,t14,s15,t15,s16,t16\n"
    "120 play 1 q16,r16,r17,s17,t17,s18\n"
    "121 play 2 f8,e9,f9,g9,h9,e10\n"
    "122 play 3 z10,aa10,y11,z11,y12,z12\n"
    "123 play 4 aa8,ab8,z9,aa9,ab9,ac9\n"
    "124 play 1 l12,i13,j13,k13,l13,i14\n")
check_variant(trigon 2 "${trigon_setup}" "${trigon_moves}"
    "(;GM[Blokus Trigon]A1[o11,p11,n12,o12,p12,n13]PL[1])")

# Blokus Trigon Two-Player
string(CONCAT trigon_2_setup
    "(;GM[Blokus Trigon Two-Player]A1[r12,q13,r13,q14,r14,r15]"
    "[u9,t10,u10,s11,t11,u11][v12,w12,x12,x13,y13,z13][m11,m12,n12,o12,p12,n13]"
    "[t14,u14,v14,w14,x14,v15][q9,p10,q10,r10,p11,q11][n16,o16,p16,q16,o17,p17]"
    "A2[r4,q5,r5,q6,r6,r7][q8,r8,s8,r9,s9,t9][n9,o9,p9,n10,o10,n11]"
    "[j11,k11,l11,j12,k12,l12][q3,r3,s3,t3,u3,v3][m3,n3,o3,l4,m4,l5]"
    "[i8,j8,i9,j9,k9,k10]A3[j7,k7,k8,l8,l9,m9][p6,o7,p7,q7,n8,o8]"
    "[s7,t7,u7,v7,w7,w8][t4,u4,s5,t5,u5,u6][n4,o4,p4,q4,n5,o5][j5,k5,k6,l6,m6,n6]"
    "A4[v11,w11,x11,y11,y12,z12][t8,u8,v8,v9,w9,w10][s12,t12,u12,s13,t13,s14]"
    "[s15,t15,u15,r16,s16,t16][aa13,y14,z14,aa14,y15,z15][x7,y7,z7,x8,y8,z8]PL[3]"
    ")")
string(CONCAT trigon_2_moves
    "100 play 1 r12,q13,r13,q14,r14,r15\n"
    "101 play 2 r4,q5,r5,q6,r6,r7\n"
    "102 play 3 j7,k7,k8,l8,l9,m9\n"
    "103 play 4 v11,w11,x11,y11,y12,z12\n"
    "104 play 1 u9,t10,u10,s11,t11,u11\n"
    "105 play 2 q8,r8,s8,r9,s9,t9\n"
    "106 play 3 p6,o7,p7,q7,n8,o8\n"
    "107 play 4 t8,u8,v8,v9,w9,w10\n"
    "108 play 1 v12,w12,x12,x13,y13,z13\n"
    "109 play 2 n9,o9,p9,n10,o10,n11\n"
    "110 play 3 s7,t7,u7,v7,w7,w8\n"
    "111 play 4 s12,t12,u12,s13,t13,s14\n"
    "112 play 1 m11,m12,n12,o12,p12,n13\n"
    "113 play 2 j11,k11,l11,j12,k12,l12\n"
    "114 play 3 t4,u4,s5,t5,u5,u6\n"
    "115 play 4 s15,t15,u15,r16,s16,t16\n"
    "116 play 1 t14,u14,v14,w14,x14,v15\n"
    "117 play 2 q3,r3,s3,t3,u3,v3\n"
    "118 play 3 n4,o4,p4,q4,n5,o5\n"
    "119 play 4 aa13,y14,z14,aa14,y15,z15\n"
    "120 play 1 q9,p10,q10,r10,p11,q11\n"
    "121 play 2 m3,n3,o3,l4,m4,l5\n"
    "122 play 3 j5,k5,k6,l6,m6,n6\n"
    "123 play 4 x7,y7,z7,x8,y8,z8\n"
    "124 play 1 n16,o16,p16,q16,o17,p17\n"
    "125 play 2 i8,j8,i9,j9,k9,k10\n")
check_variant(trigon_2 3 "${trigon_2_setup}" "${trigon_2_moves}"
    "(;GM[Blokus Trigon Two-Player]A1[u9,t10,u10,s11,t11,u11]PL[1])")

# Blokus Trigon Three-Player
string(CONCAT trigon_3_setup
    "(;GM[Blokus Trigon Three-Player]A1[r11,q12,r12,p13,q13,p14]"
    "[p9,o10,p10,n11,o11,p11][s8,u8,r9,s9,t9,u9][w7,x7,w8,x8,w9,x9]"
    "[w4,x4,y4,v5,w5,v6][v2,w2,x2,y2,v3,w3][s4,t4,u4,s5,t5,t6]"
    "A2[p3,p4,q4,q5,r5,r6][p7,q7,o8,p8,q8,r8][u6,s7,t7,u7,v7,v8]"
    "[x5,w6,x6,y6,y7,z7][y3,z3,z4,aa4,z5,aa5][m5,n5,o5,l6,m6,n6]"
    "[k4,l4,h5,i5,j5,k5]A3[h11,i11,j11,k11,l11,m11][l9,m9,n9,o9,l10,n10]"
    "[m12,n12,o12,p12,n13,o13][o14,n15,o15,n16,o16,p16][o6,p6,q6,n7,o7,n8]"
    "[j6,k6,j7,k7,l7,l8]PL[3])")
string(CONCAT trigon_3_moves
    "100 play 1 r11,q12,r12,p13,q13,p14\n"
    "101 play 2 p3,p4,q4,q5,r5,r6\n"
    "102 play 3 h11,i11,j11,k11,l11,m11\n"
    "103 play 1 p9,o10,p10,n11,o11,p11\n"
    "104 play 2 p7,q7,o8,p8,q8,r8\n"
    "105 play 3 l9,m9,n9,o9,l10,n10\n"
    "106 play 1 s8,u8,r9,s9,t9,u9\n"
    "107 play 2 u6,s7,t7,u7,v7,v8\n"
    "108 play 3 m12,n12,o12,p12,n13,o13\n"
    "109 play 1 w7,x7,w8,x8,w9,x9\n"
    "110 play 2 x5,w6,x6,y6,y7,z7\n"
    "111 play 3 o14,n15,o15,n16,o16,p16\n"
    "112 play 1 w4,x4,y4,v5,w5,v6\n"
    "113 play 2 y3,z3,z4,aa4,z5,aa5\n"
    "114 play 3 o6,p6,q6,n7,o7,n8\n"
    "115 play 1 v2,w2,x2,y2,v3,w3\n"
    "116 play 2 m5,n5,o5,l6,m6,n6\n"
    "117 play 3 j6,k6,j7,k7,l7,l8\n"
    "118 play 1 s4,t4,u4,s5,t5,t6\n"
    "119 play 2 k4,l4,h5,i5,j5,k5\n")
check_variant(trigon_3 3 "${trigon_3_setup}" "${trigon_3_moves}"
    "(;GM[Blokus Trigon Three-Player]A1[p9,o10,p10,n11,o11,p11]PL[1])")

# Nexos
string(CONCAT nexos_setup
    "(;GM[Nexos]A1[h17,g18,g20,f21][m14,m16,j17,l17][h13,i14,i16,i18]"
    "[e14,f15,h15,g16][k18,l19,m20,m22][e8,e10,f11,e12][o14,n15,p15,o16]"
    "A2[q18,r19,t19,u20][n13,p13,q14,q16][n17,p17,r17,m18][r9,q10,q12,r13]"
    "[t11,v11,u12,t13][t15,s16,t17,v17][q20,p21,o22,n23]A3[r5,t5,q6,q8]"
    "[p9,o10,n11,m12][h9,j9,l9,n9][s6,s8,t9,u10][s10,s12,s14,r15][v9,w10,w12,v13]"
    "A4[e6,f7,h7,i8][i10,i12,j13,l13][h11,g12,d13,f13][k14,l15,k16]"
    "[c14,c16,c18,c20][d7,c8,b9,d9]PL[3])")
string(CONCAT nexos_moves
    "100 play 1 h17,g18,g20,f21\n"
    "101 play 2 q18,r19,t19,u20\n"
    "102 play 3 r5,t5,q6,q8\n"
    "103 play 4 e6,f7,h7,i8\n"
    "104 play 1 m14,m16,j17,l17\n"
    "105 play 2 n13,p13,q14,q16\n"
    "106 play 3 p9,o10,n11,m12\n"
    "107 play 4 i10,i12,j13,l13\n"
    "108 play 1 h13,i14,i16,i18\n"
    "109 play 2 n17,p17,r17,m18\n"
    "110 play 3 h9,j9,l9,n9\n"
    "111 play 4 h11,g12,d13,f13\n"
    "112 play 1 e14,f15,h15,g16\n"
    "113 play 2 r9,q10,q12,r13\n"
    "114 play 3 s6,s8,t9,u10\n"
    "115 play 4 k14,l15,k16\n"
    "116 play 1 k18,l19,m20,m22\n"
    "117 play 2 t11,v11,u12,t13\n"
    "118 play 3 s10,s12,s14,r15\n"
    "119 play 4 c14,c16,c18,c20\n"
    "120 play 1 e8,e10,f11,e12\n"
    "121 play 2 t15,s16,t17,v17\n"
    "122 play 3 v9,w10,w12,v13\n"
    "123 play 4 d7,c8,b9,d9\n"
    "124 play 1 o14,n15,p15,o16\n"
    "125 play 2 q20,p21,o22,n23\n")
check_variant(nexos 3 "${nexos_setup}" "${nexos_moves}"
    "(;GM[Nexos]A1[m14,m16,j17,l17]PL[1])")

# Nexos Two-Player
string(CONCAT nexos_2_setup
    "(;GM[Nexos Two-Player]A1[i18,i20,f21,h21][l13,k14,k16,j17][h13,i14,i16,h17]"
    "[m20,j21,l21,m22][l19,n19,p19,q20][l23,n23,p23,r23][f15,h15,e16,g16]"
    "A2[q18,r19,t19,u20][n13,p13,q14,q16][r11,t11,v11,q12][m14,l15,m16,m18]"
    "[s20,p21,r21,q22][y8,x9,w10,w12][m24,q24,n25,p25]A3[t5,s6,r7,q8]"
    "[q10,n11,p11,m12][k8,m8,l9,m10][l5,n5,p5,r5][j1,k2,k4,j5][y6,t7,v7,x7]"
    "A4[e6,f7,h7,i8][j9,k10,k12,j13][i2,h3,g4,f5][k6,j7,l7,n7][b1,d1,f1,h1]"
    "[h11,g12,i12,g14]PL[3])")
string(CONCAT nexos_2_moves
    "100 play 1 i18,i20,f21,h21\n"
    "101 play 2 q18,r19,t19,u20\n"
    "102 play 3 t5,s6,r7,q8\n"
    "103 play 4 e6,f7,h7,i8\n"
    "104 play 1 l13,k14,k16,j17\n"
    "105 play 2 n13,p13,q14,q16\n"
    "106 play 3 q10,n11,p11,m12\n"
    "107 play 4 j9,k10,k12,j13\n"
    "108 play 1 h13,i14,i16,h17\n"
    "109 play 2 r11,t11,v11,q12\n"
    "110 play 3 k8,m8,l9,m10\n"
    "111 play 4 i2,h3,g4,f5\n"
    "112 play 1 m20,j21,l21,m22\n"
    "113 play 2 m14,l15,m16,m18\n"
    "114 play 3 l5,n5,p5,r5\n"
    "115 play 4 k6,j7,l7,n7\n"
    "116 play 1 l19,n19,p19,q20\n"
    "117 play 2 s20,p21,r21,q22\n"
    "118 play 3 j1,k2,k4,j5\n"
    "119 play 4 b1,d1,f1,h1\n"
    "120 play 1 l23,n23,p23,r23\n"
    "121 play 2 y8,x9,w10,w12\n"
    "122 play 3 y6,t7,v7,x7\n"
    "123 play 4 h11,g12,i12,g14\n"
    "124 play 1 f15,h15,e16,g16\n"
    "125 play 2 m24,q24,n25,p25\n")
check_variant(nexos_2 3 "${nexos_2_setup}" "${nexos_2_moves}"
    "(;GM[Nexos Two-Player]A1[l13,k14,k16,j17]PL[1])")

# Callisto
string(CONCAT callisto_setup
    "(;GM[Callisto]A1[k17][f7][m15,k16,l16,m16,m17][d8,f8,d9,e9,f9]"
    "[o15,n16,o16,p16,o17][g4,f5,g5,e6,f6][m12,l13,m13,m14]A2[f15][g7]"
    "[i5,g6,h6,i6,i7][j6,k6,k7,l7,l8][m7,n7,n8,n9][p7,o8,p8,q8,p9]A3[e7][l17]"
    "[n17,l18,m18,n18,n19][c7,d7,b8,c8,b9][j16,j17,j18,k18][j13,k13,j14,j15,k15]"
    "A4[e8][h7][j7,h8,i8,j8,j9][k8,m8,k9,l9,m9][o9,m10,n10,o10][r8,q9,r9,p10,q10]"
    "PL[2])")
string(CONCAT callisto_moves
    "100 play 1 k17\n"
    "101 play 2 f15\n"
    "102 play 3 e7\n"
    "103 play 4 e8\n"
    "104 play 1 f7\n"
    "105 play 2 g7\n"
    "106 play 3 l17\n"
    "107 play 4 h7\n"
    "108 play 1 m15,k16,l16,m16,m17\n"
    "109 play 2 i5,g6,h6,i6,i7\n"
    "110 play 3 n17,l18,m18,n18,n19\n"
    "111 play 4 j7,h8,i8,j8,j9\n"
    "112 play 1 d8,f8,d9,e9,f9\n"
    "113 play 2 j6,k6,k7,l7,l8\n"
    "114 play 3 c7,d7,b8,c8,b9\n"
    "115 play 4 k8,m8,k9,l9,m9\n"
    "116 play 1 o15,n16,o16,p16,o17\n"
    "117 play 2 m7,n7,n8,n9\n"
    "118 play 3 j16,j17,j18,k18\n"
    "119 play 4 o9,m10,n10,o10\n"
    "120 play 1 g4,f5,g5,e6,f6\n"
    "121 play 2 p7,o8,p8,q8,p9\n"
    "122 play 3 j13,k13,j14,j15,k15\n"
    "123 play 4 r8,q9,r9,p10,q10\n"
    "124 play 1 m12,l13,m13,m14\n")
check_variant(callisto 2 "${callisto_setup}" "${callisto_moves}"
    "(;GM[Callisto]A1[m15,k16,l16,m16,m17]PL[1])")

# Callisto Two-Player
string(CONCAT callisto_2_setup
    "(;GM[Callisto Two-Player]AB[f7][j11][h9,i9,h10,h11,i11][f11,f12,g12,h12,f13]"
    "[e5,f5,e6,e7][h4,g5,h5,i5,h6]AW[j6][g11][j7,h8,i8,j8,j9][f6,g6,g7,f8,g8]"
    "[d7,c8,d8,e8,d9][h3,i3,i4,j4,j5]PL[B])")
string(CONCAT callisto_2_moves
    "100 play b f7\n"
    "101 play w j6\n"
    "102 play b j11\n"
    "103 play w g11\n"
    "104 play b h9,i9,h10,h11,i11\n"
    "105 play w j7,h8,i8,j8,j9\n"
    "106 play b f11,f12,g12,h12,f13\n"
    "107 play w f6,g6,g7,f8,g8\n"
    "108 play b e5,f5,e6,e7\n"
    "109 play w d7,c8,d8,e8,d9\n"
    "110 play b h4,g5,h5,i5,h6\n"
    "111 play w h3,i3,i4,j4,j5\n")
check_variant(callisto_2 b "${callisto_2_setup}" "${callisto_2_moves}"
    "(;GM[Callisto Two-Player]AB[h9,i9,h10,h11,i11]PL[B])")

# Callisto Two-Player Four-Color
string(CONCAT callisto_2_4_setup
    "(;GM[Callisto Two-Player Four-Color]A1[h16][h6][j14,h15,i15,j15,j16]"
    "[f13,g13,h13,f14,h14][k15,l15,l16,m16,m17][l17,k18,l18,m18,l19]A2[h5][i16]"
    "[g5,g6,f7,g7,h7][k16,i17,j17,k17][f15,f16,g16,g17,h17][i7,k7,i8,j8,k8]"
    "A3[m14][i5][o12,m13,n13,o13,o14][q12,p13,q13,r13,q14][i3,j3,j4,j5]"
    "[r9,s9,q10,r10,q11]A4[n14][i4][p14,n15,o15,p15,p16][s13,r14,s14,q15,r15]"
    "[s10,r11,s11,t11,s12][n16,n17,n18,o18]PL[1])")
string(CONCAT callisto_2_4_moves
    "100 play 1 h16\n"
    "101 play 2 h5\n"
    "102 play 3 m14\n"
    "103 play 4 n14\n"
    "104 play 1 h6\n"
    "105 play 2 i16\n"
    "106 play 3 i5\n"
    "107 play 4 i4\n"
    "108 play 1 j14,h15,i15,j15,j16\n"
    "109 play 2 g5,g6,f7,g7,h7\n"
    "110 play 3 o12,m13,n13,o13,o14\n"
    "111 play 4 p14,n15,o15,p15,p16\n"
    "112 play 1 f13,g13,h13,f14,h14\n"
    "113 play 2 k16,i17,j17,k17\n"
    "114 play 3 q12,p13,q13,r13,q14\n"
    "115 play 4 s13,r14,s14,q15,r15\n"
    "116 play 1 k15,l15,l16,m16,m17\n"
    "117 play 2 f15,f16,g16,g17,h17\n"
    "118 play 3 i3,j3,j4,j5\n"
    "119 play 4 s10,r11,s11,t11,s12\n"
    "120 play 1 l17,k18,l18,m18,l19\n"
    "121 play 2 i7,k7,i8,j8,k8\n"
    "122 play 3 r9,s9,q10,r10,q11\n"
    "123 play 4 n16,n17,n18,o18\n")
check_variant(callisto_2_4 1 "${callisto_2_4_setup}" "${callisto_2_4_moves}"
    "(;GM[Callisto Two-Player Four-Color]A1[j14,h15,i15,j15,j16]PL[1])")

# Callisto Three-Player
string(CONCAT callisto_3_setup
    "(;GM[Callisto Three-Player]A1[i14][i16][j14,k14,k15,j16,k16]"
    "[f15,g15,g16,h16,h17][j11,h12,i12,j12,j13][f10,g10,g11,g12][j8,j9,k9,j10]"
    "A2[j5][j15][k5,l5,l6,m6,m7][h5,i5,h6,h7,i7][m8,m9,l10,m10,n10]"
    "[h8,g9,h9,i9,h10]A3[i15][p10][g13,h13,i13,h14,h15][e11,f11,e12,e13,f13]"
    "[o10,n11,o11,p11,o12][f8,g8,e9,f9,e10]PL[2])")
string(CONCAT callisto_3_moves
    "100 play 1 i14\n"
    "101 play 2 j5\n"
    "102 play 3 i15\n"
    "103 play 1 i16\n"
    "104 play 2 j15\n"
    "105 play 3 p10\n"
    "106 play 1 j14,k14,k15,j16,k16\n"
    "107 play 2 k5,l5,l6,m6,m7\n"
    "108 play 3 g13,h13,i13,h14,h15\n"
    "109 play 1 f15,g15,g16,h16,h17\n"
    "110 play 2 h5,i5,h6,h7,i7\n"
    "111 play 3 e11,f11,e12,e13,f13\n"
    "112 play 1 j11,h12,i12,j12,j13\n"
    "113 play 2 m8,m9,l10,m10,n10\n"
    "114 play 3 o10,n11,o11,p11,o12\n"
    "115 play 1 f10,g10,g11,g12\n"
    "116 play 2 h8,g9,h9,i9,h10\n"
    "117 play 3 f8,g8,e9,f9,e10\n"
    "118 play 1 j8,j9,k9,j10\n")
check_variant(callisto_3 2 "${callisto_3_setup}" "${callisto_3_moves}"
    "(;GM[Callisto Three-Player]A1[j14,k14,k15,j16,k16]PL[1])")

# GembloQ, a setup longer than 4096 bytes
string(CONCAT gembloq_setup
    "(;GM[GembloQ]"
    "A1[j23,k23,h24,i24,j24,k24,f25,g25,h25,i25,d26,e26,f26,g26,b27,c27,d27,e27,b28,c28]"
    "[r21,s21,v21,w21,p22,q22,r22,s22,t22,u22,v22,w22,n23,o23,p23,q23,t23,u23,n24,o24]"
    "[ab16,ac16,z17,aa17,ab17,ac17,ad17,ae17,z18,aa18,ab18,ac18,ad18,ae18,z19,aa19,ab19,ac19,y20,z20,aa20,y21]"
    "[r13,s13,v13,w13,r14,s14,t14,u14,v14,w14,x14,y14,t15,u15,x15,y15,z15,aa15]"
    "[z23,aa23,t24,u24,x24,y24,z24,aa24,ab24,ac24,t25,u25,v25,w25,x25,y25,ab25,ac25,v26,w26]"
    "[af24,ag24,aj24,ak24,an24,ao24,af25,ag25,ah25,ai25,aj25,ak25,al25,am25,an25,ao25,ah26,ai26,al26,am26]"
    "[d18,e18,d19,e19,f19,g19,f20,g20,h20,i20,l20,m20,h21,i21,j21,k21,l21,m21,j22,k22]"
    "[ar24,as24,ar25,as25,at25,au25,ar26,as26,at26,au26,av26,aw26,ap27,aq27,ar27,as27,av27,aw27,ap28,aq28]"
    "[l18,m18,j19,k19,l19,m19][l16,m16,l17,m17][j13,k13,j14,k14,l14,m14,l15,m15]"
    "[d8,e8,d9,e9,f9,g9,d10,e10,f10,g10,h10,i10,d11,e11,h11,i11,j11,k11,j12,k12]"
    "[d4,e4,d5,e5,f5,g5,d6,e6,f6,g6,h6,i6,d7,e7,h7,i7]"
    "[l6,m6,l7,m7,n7,o7,n8,o8,p8,q8,n9,o9,p9,q9,n10,o10]"
    "[f1,g1,f2,g2,h2,i2,h3,i3,j3,k3,j4,k4,l4,m4,l5,m5]"
    "[n25,o25,l26,m26,n26,o26,p26,q26,l27,m27,n27,o27,p27,q27,n28,o28]"
    "[d22,e22,b23,c23,d23,e23,b24,c24,d24,d25]"
    "A2[at23,au23,at24,au24,av24,aw24,av25,aw25,ax25,ay25,ax26,ay26,az26,ba26,az27,ba27,bb27,bc27,bb28,bc28]"
    "[ah21,ai21,ah22,ai22,aj22,ak22,an22,ao22,aj23,ak23,al23,am23,an23,ao23,ap23,aq23,al24,am24,ap24,aq24]"
    "[ad15,ae15,ad16,ae16,af16,ag16,af17,ag17,ah17,ai17,ah18,ai18,aj18,ak18,ah19,ai19,aj19,ak19,ah20,ai20]"
    "[af18,ad19,ae19,af19,ab20,ac20,ad20,ae20,z21,aa21,ab21,ac21,ad21,ae21,z22,aa22,ab22,ac22,ad22,ae22,ab23,ac23]"
    "[t16,u16,x16,y16,z16,aa16,r17,s17,t17,u17,v17,w17,x17,y17,r18,s18,v18,w18]"
    "[an14,ao14,al15,am15,an15,ao15,al16,am16,an16,ao16,an17,ao17,ap17,aq17,an18,ao18,ap18,aq18,an19,ao19]"
    "[an10,ao10,ar10,as10,an11,ao11,ap11,aq11,ar11,as11,an12,ao12,ap12,aq12,ar12,as12,an13,ao13,ar13,as13]"
    "[at17,au17,at18,au18,av18,aw18,az18,ba18,av19,aw19,ax19,ay19,az19,ba19,av20,aw20,ax20,ay20,av21,aw21]"
    "[bb9,bc9,az10,ba10,bb10,bc10,ax11,ay11,az11,ba11,av12,aw12,ax12,ay12,az12,ba12,av13,aw13,az13,ba13]"
    "[bb3,bc3,az4,ba4,bb4,bc4,az5,ba5,bb5,bc5,az6,ba6,bb6,bc6,az7,ba7,bb7,bc7,bb8,bc8]"
    "[av14,aw14,az14,ba14,av15,aw15,ax15,ay15,az15,ba15,ax16,ay16,az16,ba16,az17,ba17]"
    "[ap25,aq25,aj26,ak26,an26,ao26,ap26,aq26,aj27,ak27,al27,am27,an27,ao27,al28,am28]"
    "[ad25,ae25,ab26,ac26,ad26,ae26,af26,ag26,ab27,ac27,ad27,ae27,af27,ag27,ad28,ae28]"
    "[ar6,as6,ap7,aq7,ar7,as7,an8,ao8,ap8,aq8,an9,ao9]"
    "[ax1,ay1,av2,aw2,ax2,ay2,at3,au3,av3,aw3,ar4,as4,at4,au4,ar5,as5]"
    "[r26,s26,t26,u26,x26,y26,t27,u27,v27,w27,x27,y27,v28,w28]"
    "A3[bb1,bc1,az2,ba2,bb2,bc2,ax3,ay3,az3,ba3,av4,aw4,ax4,ay4,at5,au5,av5,aw5,at6,au6]"
    "[ap5,aq5,aj6,ak6,an6,ao6,ap6,aq6,ah7,ai7,aj7,ak7,al7,am7,an7,ao7,ah8,ai8,al8,am8]"
    "[ah9,af10,ag10,ah10,ad11,ae11,af11,ag11,ab12,ac12,ad12,ae12,af12,ag12,ab13,ac13,ad13,ae13,af13,ag13,ad14,ae14]"
    "[ab6,ac6,z7,aa7,ab7,ac7,ad7,ae7,x8,y8,z8,aa8,ad8,ae8,v9,w9,x9,y9,v10,w10]"
    "[al9,am9,aj10,ak10,al10,am10,aj11,ak11,al11,am11,aj12,ak12,al12,am12,aj13,ak13,al13,am13,al14,am14]"
    "[ap13,aq13,ap14,aq14,ar14,as14,ar15,as15,at15,au15,ar16,as16,at16,au16,av16,aw16,ar17,as17,av17,aw17]"
    "[at7,au7,ar8,as8,at8,au8,ap9,aq9,ar9,as9,at9,au9,ap10,aq10,at10,au10,av10,aw10,av11,aw11]"
    "[ar18,as18,ar19,as19,at19,au19,ar20,as20,at20,au20,ap21,aq21,ar21,as21,ap22,aq22,ar22,as22,ar23,as23]"
    "[af2,ag2,aj2,ak2,af3,ag3,ah3,ai3,aj3,ak3,af4,ag4,ah4,ai4,aj4,ak4,af5,ag5,aj5,ak5]"
    "[x2,y2,ab2,ac2,v3,w3,x3,y3,z3,aa3,ab3,ac3,t4,u4,v4,w4,z4,aa4]"
    "[ax21,ay21,av22,aw22,ax22,ay22,av23,aw23,ax23,ay23,ax24,ay24,az24,ba24,az25,ba25]"
    "[al19,am19,aj20,ak20,al20,am20,aj21,ak21,al21,am21,al22,am22]"
    "[ap1,aq1,an2,ao2,ap2,aq2,ar2,as2,an3,ao3,ap3,aq3,ar3,as3,ap4,aq4]"
    "[ax7,ay7,ax8,ay8,az8,ba8,ay9,az9,ba9,ay10]"
    "[p4,q4,p5,q5,r5,s5,r6,s6,t6,u6,t7,u7][bb19,bc19,az20,ba20,bb20,bc20]"
    "A4[b1,c1,b2,c2,d2,e2,d3,e3,f3,g3,f4,g4,h4,i4,h5,i5,j5,k5,j6,k6]"
    "[n5,o5,n6,o6,p6,q6,p7,q7,r7,s7,v7,w7,r8,s8,t8,u8,v8,w8,t9,u9]"
    "[t10,u10,t11,u11,v11,w11,z11,aa11,v12,w12,x12,y12,z12,aa12,x13,y13,z13,aa13,z14,aa14]"
    "[r9,p10,q10,r10,n11,o11,p11,q11,l12,m12,n12,o12,p12,q12,l13,m13,n13,o13,p13,q13,n14,o14]"
    "[n15,o15,n16,o16,p16,q16,n17,o17,p17,q17,n18,o18,p18,q18,p19,q19,r19,s19,r20,s20]"
    "[ab8,ac8,af8,ag8,z9,aa9,ab9,ac9,ad9,ae9,af9,ag9,x10,y10,z10,aa10,ad10,ae10]"
    "[d16,e16,b17,c17,d17,e17,f17,g17,j17,k17,b18,c18,f18,g18,h18,i18,j18,k18,h19,i19]"
    "[x4,y4,ab4,ac4,v5,w5,x5,y5,z5,aa5,ab5,ac5,ad5,ae5,v6,w6,z6,aa6,ad6,ae6]"
    "[h12,i12,f13,g13,h13,i13,d14,e14,f14,g14,h14,i14,d15,e15,h15,i15,j15,k15,j16,k16]"
    "[n1,o1,r1,s1,n2,o2,p2,q2,r2,s2,n3,o3,p3,q3,r3,s3,n4,o4,r4,s4]"
    "[b19,c19,b20,c20,d20,e20,d21,e21,f21,g21,f22,g22,h22,i22,h23,i23]"
    "[f7,g7,j7,k7,f8,g8,h8,i8,j8,k8,h9,i9,j9,k9,j10,k10][a6,a7,b7,c7,b8,c8]"
    "[x18,y18,v19,w19,x19,y19,v20,w20,x20,x21][n21,o21,l22,m22,n22,o22,l23,m23]"
    "[l24,m24,j25,k25,l25,m25,h26,i26,j26,k26,h27,i27,j27,k27,j28,k28]PL[2])")
string(CONCAT gembloq_moves
    "100 play 1 j23,k23,h24,i24,j24,k24,f25,g25,h25,i25,d26,e26,f26,g26,b27,c27,d27,e27,b28,c28\n"
    "101 play 2 at23,au23,at24,au24,av24,aw24,av25,aw25,ax25,ay25,ax26,ay26,az26,ba26,az27,ba27,bb27,bc27,bb28,bc28\n"
    "102 play 3 bb1,bc1,az2,ba2,bb2,bc2,ax3,ay3,az3,ba3,av4,aw4,ax4,ay4,at5,au5,av5,aw5,at6,au6\n"
    "103 play 4 b1,c1,b2,c2,d2,e2,d3,e3,f3,g3,f4,g4,h4,i4,h5,i5,j5,k5,j6,k6\n"
    "104 play 1 r21,s21,v21,w21,p22,q22,r22,s22,t22,u22,v22,w22,n23,o23,p23,q23,t23,u23,n24,o24\n"
    "105 play 2 ah21,ai21,ah22,ai22,aj22,ak22,an22,ao22,aj23,ak23,al23,am23,an23,ao23,ap23,aq23,al24,am24,ap24,aq24\n"
    "106 play 3 ap5,aq5,aj6,ak6,an6,ao6,ap6,aq6,ah7,ai7,aj7,ak7,al7,am7,an7,ao7,ah8,ai8,al8,am8\n"
    "107 play 4 n5,o5,n6,o6,p6,q6,p7,q7,r7,s7,v7,w7,r8,s8,t8,u8,v8,w8,t9,u9\n"
    "108 play 1 ab16,ac16,z17,aa17,ab17,ac17,ad17,ae17,z18,aa18,ab18,ac18,ad18,ae18,z19,aa19,ab19,ac19,y20,z20,aa20,y21\n"
    "109 play 2 ad15,ae15,ad16,ae16,af16,ag16,af17,ag17,ah17,ai17,ah18,ai18,aj18,ak18,ah19,ai19,aj19,ak19,ah20,ai20\n"
    "110 play 3 ah9,af10,ag10,ah10,ad11,ae11,af11,ag11,ab12,ac12,ad12,ae12,af12,ag12,ab13,ac13,ad13,ae13,af13,ag13,ad14,ae14\n"
    "111 play 4 t10,u10,t11,u11,v11,w11,z11,aa11,v12,w12,x12,y12,z12,aa12,x13,y13,z13,aa13,z14,aa14\n"
    "112 play 1 r13,s13,v13,w13,r14,s14,t14,u14,v14,w14,x14,y14,t15,u15,x15,y15,z15,aa15\n"
    "113 play 2 af18,ad19,ae19,af19,ab20,ac20,ad20,ae20,z21,aa21,ab21,ac21,ad21,ae21,z22,aa22,ab22,ac22,ad22,ae22,ab23,ac23\n"
    "114 play 3 ab6,ac6,z7,aa7,ab7,ac7,ad7,ae7,x8,y8,z8,aa8,ad8,ae8,v9,w9,x9,y9,v10,w10\n"
    "115 play 4 r9,p10,q10,r10,n11,o11,p11,q11,l12,m12,n12,o12,p12,q12,l13,m13,n13,o13,p13,q13,n14,o14\n"
    "116 play 1 z23,aa23,t24,u24,x24,y24,z24,aa24,ab24,ac24,t25,u25,v25,w25,x25,y25,ab25,ac25,v26,w26\n"
    "117 play 2 t16,u16,x16,y16,z16,aa16,r17,s17,t17,u17,v17,w17,x17,y17,r18,s18,v18,w18\n"
    "118 play 3 al9,am9,aj10,ak10,al10,am10,aj11,ak11,al11,am11,aj12,ak12,al12,am12,aj13,ak13,al13,am13,al14,am14\n"
    "119 play 4 n15,o15,n16,o16,p16,q16,n17,o17,p17,q17,n18,o18,p18,q18,p19,q19,r19,s19,r20,s20\n"
    "120 play 1 af24,ag24,aj24,ak24,an24,ao24,af25,ag25,ah25,ai25,aj25,ak25,al25,am25,an25,ao25,ah26,ai26,al26,am26\n"
    "121 play 2 an14,ao14,al15,am15,an15,ao15,al16,am16,an16,ao16,an17,ao17,ap17,aq17,an18,ao18,ap18,aq18,an19,ao19\n"
    "122 play 3 ap13,aq13,ap14,aq14,ar14,as14,ar15,as15,at15,au15,ar16,as16,at16,au16,av16,aw16,ar17,as17,av17,aw17\n"
    "123 play 4 ab8,ac8,af8,ag8,z9,aa9,ab9,ac9,ad9,ae9,af9,ag9,x10,y10,z10,aa10,ad10,ae10\n"
    "124 play 1 d18,e18,d19,e19,f19,g19,f20,g20,h20,i20,l20,m20,h21,i21,j21,k21,l21,m21,j22,k22\n"
    "125 play 2 an10,ao10,ar10,as10,an11,ao11,ap11,aq11,ar11,as11,an12,ao12,ap12,aq12,ar12,as12,an13,ao13,ar13,as13\n"
    "126 play 3 at7,au7,ar8,as8,at8,au8,ap9,aq9,ar9,as9,at9,au9,ap10,aq10,at10,au10,av10,aw10,av11,aw11\n"
    "127 play 4 d16,e16,b17,c17,d17,e17,f17,g17,j17,k17,b18,c18,f18,g18,h18,i18,j18,k18,h19,i19\n"
    "128 play 1 ar24,as24,ar25,as25,at25,au25,ar26,as26,at26,au26,av26,aw26,ap27,aq27,ar27,as27,av27,aw27,ap28,aq28\n"
    "129 play 2 at17,au17,at18,au18,av18,aw18,az18,ba18,av19,aw19,ax19,ay19,az19,ba19,av20,aw20,ax20,ay20,av21,aw21\n"
    "130 play 3 ar18,as18,ar19,as19,at19,au19,ar20,as20,at20,au20,ap21,aq21,ar21,as21,ap22,aq22,ar22,as22,ar23,as23\n"
    "131 play 4 x4,y4,ab4,ac4,v5,w5,x5,y5,z5,aa5,ab5,ac5,ad5,ae5,v6,w6,z6,aa6,ad6,ae6\n"
    "132 play 1 l18,m18,j19,k19,l19,m19\n"
    "133 play 2 bb9,bc9,az10,ba10,bb10,bc10,ax11,ay11,az11,ba11,av12,aw12,ax12,ay12,az12,ba12,av13,aw13,az13,ba13\n"
    "134 play 3 af2,ag2,aj2,ak2,af3,ag3,ah3,ai3,aj3,ak3,af4,ag4,ah4,ai4,aj4,ak4,af5,ag5,aj5,ak5\n"
    "135 play 4 h12,i12,f13,g13,h13,i13,d14,e14,f14,g14,h14,i14,d15,e15,h15,i15,j15,k15,j16,k16\n"
    "136 play 1 l16,m16,l17,m17\n"
    "137 play 2 bb3,bc3,az4,ba4,bb4,bc4,az5,ba5,bb5,bc5,az6,ba6,bb6,bc6,az7,ba7,bb7,bc7,bb8,bc8\n"
    "138 play 3 x2,y2,ab2,ac2,v3,w3,x3,y3,z3,aa3,ab3,ac3,t4,u4,v4,w4,z4,aa4\n"
    "139 play 4 n1,o1,r1,s1,n2,o2,p2,q2,r2,s2,n3,o3,p3,q3,r3,s3,n4,o4,r4,s4\n"
    "140 play 1 j13,k13,j14,k14,l14,m14,l15,m15\n"
    "141 play 2 av14,aw14,az14,ba14,av15,aw15,ax15,ay15,az15,ba15,ax16,ay16,az16,ba16,az17,ba17\n"
    "142 play 3 ax21,ay21,av22,aw22,ax22,ay22,av23,aw23,ax23,ay23,ax24,ay24,az24,ba24,az25,ba25\n"
    "143 play 4 b19,c19,b20,c20,d20,e20,d21,e21,f21,g21,f22,g22,h22,i22,h23,i23\n"
    "144 play 1 d8,e8,d9,e9,f9,g9,d10,e10,f10,g10,h10,i10,d11,e11,h11,i11,j11,k11,j12,k12\n"
    "145 play 2 ap25,aq25,aj26,ak26,an26,ao26,ap26,aq26,aj27,ak27,al27,am27,an27,ao27,al28,am28\n"
    "146 play 3 al19,am19,aj20,ak20,al20,am20,aj21,ak21,al21,am21,al22,am22\n"
    "147 play 4 f7,g7,j7,k7,f8,g8,h8,i8,j8,k8,h9,i9,j9,k9,j10,k10\n"
    "148 play 1 d4,e4,d5,e5,f5,g5,d6,e6,f6,g6,h6,i6,d7,e7,h7,i7\n"
    "149 play 2 ad25,ae25,ab26,ac26,ad26,ae26,af26,ag26,ab27,ac27,ad27,ae27,af27,ag27,ad28,ae28\n"
    "150 play 3 ap1,aq1,an2,ao2,ap2,aq2,ar2,as2,an3,ao3,ap3,aq3,ar3,as3,ap4,aq4\n"
    "151 play 4 a6,a7,b7,c7,b8,c8\n"
    "152 play 1 l6,m6,l7,m7,n7,o7,n8,o8,p8,q8,n9,o9,p9,q9,n10,o10\n"
    "153 play 2 ar6,as6,ap7,aq7,ar7,as7,an8,ao8,ap8,aq8,an9,ao9\n"
    "154 play 3 ax7,ay7,ax8,ay8,az8,ba8,ay9,az9,ba9,ay10\n"
    "155 play 4 x18,y18,v19,w19,x19,y19,v20,w20,x20,x21\n"
    "156 play 1 f1,g1,f2,g2,h2,i2,h3,i3,j3,k3,j4,k4,l4,m4,l5,m5\n"
    "157 play 2 ax1,ay1,av2,aw2,ax2,ay2,at3,au3,av3,aw3,ar4,as4,at4,au4,ar5,as5\n"
    "158 play 3 p4,q4,p5,q5,r5,s5,r6,s6,t6,u6,t7,u7\n"
    "159 play 4 n21,o21,l22,m22,n22,o22,l23,m23\n"
    "160 play 1 n25,o25,l26,m26,n26,o26,p26,q26,l27,m27,n27,o27,p27,q27,n28,o28\n"
    "161 play 2 r26,s26,t26,u26,x26,y26,t27,u27,v27,w27,x27,y27,v28,w28\n"
    "162 play 3 bb19,bc19,az20,ba20,bb20,bc20\n"
    "163 play 4 l24,m24,j25,k25,l25,m25,h26,i26,j26,k26,h27,i27,j27,k27,j28,k28\n"
    "164 play 1 d22,e22,b23,c23,d23,e23,b24,c24,d24,d25\n")
check_variant(gembloq 2 "${gembloq_setup}" "${gembloq_moves}"
    "(;GM[GembloQ]A1[r21,s21,v21,w21,p22,q22,r22,s22,t22,u22,v22,w22,n23,o23,p23,q23,t23,u23,n24,o24]PL[1])")

# GembloQ Two-Player
string(CONCAT gembloq_2_setup
    "(;GM[GembloQ Two-Player]"
    "AB[v17,w17,t18,u18,v18,w18,r19,s19,t19,u19,p20,q20,r20,s20,n21,o21,p21,q21,n22,o22]"
    "[v11,w11,v12,w12,x12,y12,v13,w13,x13,y13,t14,u14,v14,w14,t15,u15,v15,w15,v16,w16]"
    "[ab6,ac6,z7,aa7,ab7,ac7,ad7,ae7,z8,aa8,ab8,ac8,ad8,ae8,z9,aa9,ab9,ac9,y10,z10,aa10,y11]"
    "[p10,q10,n11,o11,p11,q11,n12,o12,p12,q12,n13,o13,p13,q13,n14,o14,p14,q14,p15,q15]"
    "[h10,i10,f11,g11,h11,i11,d12,e12,f12,g12,h12,i12,d13,e13,h13,i13,j13,k13,j14,k14]"
    "AW[ad1,ae1,ab2,ac2,ad2,ae2,z3,aa3,ab3,ac3,x4,y4,z4,aa4,v5,w5,x5,y5,v6,w6]"
    "[v7,w7,t8,u8,v8,w8,t9,u9,v9,w9,t10,u10,v10,w10,r11,s11,t11,u11,r12,s12]"
    "[ab5,ac5,ad5,ae5,ah5,ai5,ad6,ae6,af6,ag6,ah6,ai6,aj6,ak6,af7,ag7,ah7,ai7,aj7,ak7,ah8,ai8]"
    "[p8,q8,n9,o9,p9,q9,l10,m10,n10,o10,j11,k11,l11,m11,j12,k12,l12,m12,l13,m13]"
    "[l14,m14,j15,k15,l15,m15,n15,o15,r15,s15,j16,k16,n16,o16,p16,q16,r16,s16,p17,q17]"
    "PL[B])")
string(CONCAT gembloq_2_moves
    "100 play b v17,w17,t18,u18,v18,w18,r19,s19,t19,u19,p20,q20,r20,s20,n21,o21,p21,q21,n22,o22\n"
    "101 play w ad1,ae1,ab2,ac2,ad2,ae2,z3,aa3,ab3,ac3,x4,y4,z4,aa4,v5,w5,x5,y5,v6,w6\n"
    "102 play b v11,w11,v12,w12,x12,y12,v13,w13,x13,y13,t14,u14,v14,w14,t15,u15,v15,w15,v16,w16\n"
    "103 play w v7,w7,t8,u8,v8,w8,t9,u9,v9,w9,t10,u10,v10,w10,r11,s11,t11,u11,r12,s12\n"
    "104 play b ab6,ac6,z7,aa7,ab7,ac7,ad7,ae7,z8,aa8,ab8,ac8,ad8,ae8,z9,aa9,ab9,ac9,y10,z10,aa10,y11\n"
    "105 play w ab5,ac5,ad5,ae5,ah5,ai5,ad6,ae6,af6,ag6,ah6,ai6,aj6,ak6,af7,ag7,ah7,ai7,aj7,ak7,ah8,ai8\n"
    "106 play b p10,q10,n11,o11,p11,q11,n12,o12,p12,q12,n13,o13,p13,q13,n14,o14,p14,q14,p15,q15\n"
    "107 play w p8,q8,n9,o9,p9,q9,l10,m10,n10,o10,j11,k11,l11,m11,j12,k12,l12,m12,l13,m13\n"
    "108 play b h10,i10,f11,g11,h11,i11,d12,e12,f12,g12,h12,i12,d13,e13,h13,i13,j13,k13,j14,k14\n"
    "109 play w l14,m14,j15,k15,l15,m15,n15,o15,r15,s15,j16,k16,n16,o16,p16,q16,r16,s16,p17,q17\n")
check_variant(gembloq_2 b "${gembloq_2_setup}" "${gembloq_2_moves}"
    "(;GM[GembloQ Two-Player]AB[v11,w11,v12,w12,x12,y12,v13,w13,x13,y13,t14,u14,v14,w14,t15,u15,v15,w15,v16,w16]PL[B])")

# GembloQ Two-Player Four-Color
string(CONCAT gembloq_2_4_setup
    "(;GM[GembloQ Two-Player Four-Color]"
    "A1[j23,k23,h24,i24,j24,k24,f25,g25,h25,i25,d26,e26,f26,g26,b27,c27,d27,e27,b28,c28]"
    "[v21,w21,t22,u22,v22,w22,n23,o23,r23,s23,t23,u23,n24,o24,p24,q24,r24,s24,p25,q25]"
    "[ab16,ac16,z17,aa17,ab17,ac17,ad17,ae17,z18,aa18,ab18,ac18,ad18,ae18,z19,aa19,ab19,ac19,y20,z20,aa20,y21]"
    "[r13,s13,v13,w13,r14,s14,t14,u14,v14,w14,x14,y14,t15,u15,x15,y15,z15,aa15]"
    "[p16,q16,n17,o17,p17,q17,r17,s17,v17,w17,n18,o18,r18,s18,t18,u18,v18,w18,t19,u19]"
    "[j19,k19,n19,o19,j20,k20,l20,m20,n20,o20,j21,k21,l21,m21,n21,o21,j22,k22,n22,o22]"
    "A2[at23,au23,at24,au24,av24,aw24,av25,aw25,ax25,ay25,ax26,ay26,az26,ba26,az27,ba27,bb27,bc27,bb28,bc28]"
    "[ah21,ai21,al21,am21,ah22,ai22,aj22,ak22,al22,am22,an22,ao22,aj23,ak23,an23,ao23,ap23,aq23,ap24,aq24]"
    "[ad15,ae15,ad16,ae16,af16,ag16,af17,ag17,ah17,ai17,ah18,ai18,aj18,ak18,ah19,ai19,aj19,ak19,ah20,ai20]"
    "[aj10,ak10,ah11,ai11,aj11,ak11,al11,am11,ah12,ai12,aj12,ak12,al12,am12,ah13,ai13,aj13,ak13,ag14,ah14,ai14,ag15]"
    "[at15,au15,ar16,as16,at16,au16,ap17,aq17,ar17,as17,an18,ao18,ap18,aq18,ar18,as18,an19,ao19,ar19,as19]"
    "[ad21,ae21,ab22,ac22,ad22,ae22,z23,aa23,ab23,ac23,x24,y24,z24,aa24,x25,y25]"
    "A3[bb1,bc1,az2,ba2,bb2,bc2,ax3,ay3,az3,ba3,av4,aw4,ax4,ay4,at5,au5,av5,aw5,at6,au6]"
    "[al5,am5,ap5,aq5,aj6,ak6,al6,am6,an6,ao6,ap6,aq6,ah7,ai7,aj7,ak7,an7,ao7,ah8,ai8]"
    "[ah9,af10,ag10,ah10,ad11,ae11,af11,ag11,ab12,ac12,ad12,ae12,af12,ag12,ab13,ac13,ad13,ae13,af13,ag13,ad14,ae14]"
    "[an8,ao8,al9,am9,an9,ao9,al10,am10,an10,ao10,an11,ao11,ap11,aq11,an12,ao12,ap12,aq12,an13,ao13]"
    "[at11,au11,at12,au12,av12,aw12,at13,au13,av13,aw13,ar14,as14,at14,au14,ap15,aq15,ar15,as15,ap16,aq16]"
    "A4[b1,c1,b2,c2,d2,e2,d3,e3,f3,g3,f4,g4,h4,i4,h5,i5,j5,k5,j6,k6]"
    "[n5,o5,n6,o6,p6,q6,t6,u6,p7,q7,r7,s7,t7,u7,v7,w7,r8,s8,v8,w8]"
    "[v9,w9,t10,u10,v10,w10,t11,u11,v11,w11,v12,w12,x12,y12,x13,y13,z13,aa13,z14,aa14]"
    "[r11,p12,q12,r12,n13,o13,p13,q13,l14,m14,n14,o14,p14,q14,l15,m15,n15,o15,p15,q15,n16,o16]"
    "[h14,i14,f15,g15,h15,i15,d16,e16,f16,g16,h16,i16,d17,e17,h17,i17,j17,k17,j18,k18]"
    "PL[3])")
string(CONCAT gembloq_2_4_moves
    "100 play 1 j23,k23,h24,i24,j24,k24,f25,g25,h25,i25,d26,e26,f26,g26,b27,c27,d27,e27,b28,c28\n"
    "101 play 2 at23,au23,at24,au24,av24,aw24,av25,aw25,ax25,ay25,ax26,ay26,az26,ba26,az27,ba27,bb27,bc27,bb28,bc28\n"
    "102 play 3 bb1,bc1,az2,ba2,bb2,bc2,ax3,ay3,az3,ba3,av4,aw4,ax4,ay4,at5,au5,av5,aw5,at6,au6\n"
    "103 play 4 b1,c1,b2,c2,d2,e2,d3,e3,f3,g3,f4,g4,h4,i4,h5,i5,j5,k5,j6,k6\n"
    "104 play 1 v21,w21,t22,u22,v22,w22,n23,o23,r23,s23,t23,u23,n24,o24,p24,q24,r24,s24,p25,q25\n"
    "105 play 2 ah21,ai21,al21,am21,ah22,ai22,aj22,ak22,al22,am22,an22,ao22,aj23,ak23,an23,ao23,ap23,aq23,ap24,aq24\n"
    "106 play 3 al5,am5,ap5,aq5,aj6,ak6,al6,am6,an6,ao6,ap6,aq6,ah7,ai7,aj7,ak7,an7,ao7,ah8,ai8\n"
    "107 play 4 n5,o5,n6,o6,p6,q6,t6,u6,p7,q7,r7,s7,t7,u7,v7,w7,r8,s8,v8,w8\n"
    "108 play 1 ab16,ac16,z17,aa17,ab17,ac17,ad17,ae17,z18,aa18,ab18,ac18,ad18,ae18,z19,aa19,ab19,ac19,y20,z20,aa20,y21\n"
    "109 play 2 ad15,ae15,ad16,ae16,af16,ag16,af17,ag17,ah17,ai17,ah18,ai18,aj18,ak18,ah19,ai19,aj19,ak19,ah20,ai20\n"
    "110 play 3 ah9,af10,ag10,ah10,ad11,ae11,af11,ag11,ab12,ac12,ad12,ae12,af12,ag12,ab13,ac13,ad13,ae13,af13,ag13,ad14,ae14\n"
    "111 play 4 v9,w9,t10,u10,v10,w10,t11,u11,v11,w11,v12,w12,x12,y12,x13,y13,z13,aa13,z14,aa14\n"
    "112 play 1 r13,s13,v13,w13,r14,s14,t14,u14,v14,w14,x14,y14,t15,u15,x15,y15,z15,aa15\n"
    "113 play 2 aj10,ak10,ah11,ai11,aj11,ak11,al11,am11,ah12,ai12,aj12,ak12,al12,am12,ah13,ai13,aj13,ak13,ag14,ah14,ai14,ag15\n"
    "114 play 3 an8,ao8,al9,am9,an9,ao9,al10,am10,an10,ao10,an11,ao11,ap11,aq11,an12,ao12,ap12,aq12,an13,ao13\n"
    "115 play 4 r11,p12,q12,r12,n13,o13,p13,q13,l14,m14,n14,o14,p14,q14,l15,m15,n15,o15,p15,q15,n16,o16\n"
    "116 play 1 p16,q16,n17,o17,p17,q17,r17,s17,v17,w17,n18,o18,r18,s18,t18,u18,v18,w18,t19,u19\n"
    "117 play 2 at15,au15,ar16,as16,at16,au16,ap17,aq17,ar17,as17,an18,ao18,ap18,aq18,ar18,as18,an19,ao19,ar19,as19\n"
    "118 play 3 at11,au11,at12,au12,av12,aw12,at13,au13,av13,aw13,ar14,as14,at14,au14,ap15,aq15,ar15,as15,ap16,aq16\n"
    "119 play 4 h14,i14,f15,g15,h15,i15,d16,e16,f16,g16,h16,i16,d17,e17,h17,i17,j17,k17,j18,k18\n"
    "120 play 1 j19,k19,n19,o19,j20,k20,l20,m20,n20,o20,j21,k21,l21,m21,n21,o21,j22,k22,n22,o22\n"
    "121 play 2 ad21,ae21,ab22,ac22,ad22,ae22,z23,aa23,ab23,ac23,x24,y24,z24,aa24,x25,y25\n")
check_variant(gembloq_2_4 3 "${gembloq_2_4_setup}" "${gembloq_2_4_moves}"
    "(;GM[GembloQ Two-Player Four-Color]A1[v21,w21,t22,u22,v22,w22,n23,o23,r23,s23,t23,u23,n24,o24,p24,q24,r24,s24,p25,q25]PL[1])")

# GembloQ Three-Player
string(CONCAT gembloq_3_setup
    "(;GM[GembloQ Three-Player]"
    "A1[z1,aa1,z2,aa2,ab2,ac2,ab3,ac3,ad3,ae3,ad4,ae4,af4,ag4,ad5,ae5,af5,ag5,ad6,ae6]"
    "[ad7,ae7,ab8,ac8,ad8,ae8,af8,ag8,ab9,ac9,ad9,ae9,af9,ag9,ab10,ac10,ad10,ae10,aa11,ab11,ac11,aa12]"
    "[v13,w13,z13,aa13,ad13,ae13,v14,w14,x14,y14,z14,aa14,ab14,ac14,ad14,ae14,x15,y15,ab15,ac15]"
    "[p10,q10,t10,u10,p11,q11,r11,s11,t11,u11,v11,w11,r12,s12,v12,w12,x12,y12]"
    "[r13,s13,l14,m14,p14,q14,r14,s14,l15,m15,n15,o15,p15,q15,l16,m16,n16,o16,l17,m17]"
    "[aj8,ak8,aj9,ak9,al9,am9,ap9,aq9,al10,am10,an10,ao10,ap10,aq10,ar10,as10,an11,ao11,ar11,as11]"
    "A2[j17,k17,h18,i18,j18,k18,b19,c19,f19,g19,h19,i19,b20,c20,d20,e20,f20,g20,d21,e21]"
    "[t14,u14,r15,s15,t15,u15,v15,w15,p16,q16,r16,s16,t16,u16,v16,w16,n17,o17,p17,q17,t17,u17]"
    "[z15,aa15,z16,aa16,ab16,ac16,ab17,ac17,ad17,ae17,ad18,ae18,af18,ag18,af19,ag19,ah19,ai19,ah20,ai20]"
    "[an16,ao16,an17,ao17,ap17,aq17,an18,ao18,ap18,aq18,ar18,as18,al19,am19,an19,ao19,ar19,as19,al20,am20]"
    "[ah21,ai21,ah22,ai22,aj22,ak22,aj23,ak23,al23,am23,aj24,ak24,al24,am24,an24,ao24,aj25,ak25,an25,ao25]"
    "[f11,g11,f12,g12,h12,i12,h13,i13,j13,k13,h14,i14,j14,k14,h15,i15,j15,k15,j16,k16]"
    "A3[ap15,aq15,ap16,aq16,ar16,as16,ar17,as17,at17,au17,at18,au18,av18,aw18,av19,aw19,ax19,ay19,ax20,ay20]"
    "[ad15,ae15,ah15,ai15,al15,am15,ad16,ae16,af16,ag16,ah16,ai16,aj16,ak16,al16,am16,af17,ag17,aj17,ak17]"
    "[af10,ag10,ad11,ae11,af11,ag11,ab12,ac12,ad12,ae12,af12,ag12,ab13,ac13,af13,ag13,ah13,ai13,ah14,ai14]"
    "[an20,ao20,ar20,as20,at20,au20,al21,am21,an21,ao21,ap21,aq21,ar21,as21,al22,am22,an22,ao22,ap22,aq22,an23,ao23]"
    "[al11,am11,ap11,aq11,al12,am12,an12,ao12,ap12,aq12,al13,am13,an13,ao13,ap13,aq13,al14,am14,ap14,aq14]"
    "PL[3])")
string(CONCAT gembloq_3_moves
    "100 play 1 z1,aa1,z2,aa2,ab2,ac2,ab3,ac3,ad3,ae3,ad4,ae4,af4,ag4,ad5,ae5,af5,ag5,ad6,ae6\n"
    "101 play 2 j17,k17,h18,i18,j18,k18,b19,c19,f19,g19,h19,i19,b20,c20,d20,e20,f20,g20,d21,e21\n"
    "102 play 3 ap15,aq15,ap16,aq16,ar16,as16,ar17,as17,at17,au17,at18,au18,av18,aw18,av19,aw19,ax19,ay19,ax20,ay20\n"
    "103 play 1 ad7,ae7,ab8,ac8,ad8,ae8,af8,ag8,ab9,ac9,ad9,ae9,af9,ag9,ab10,ac10,ad10,ae10,aa11,ab11,ac11,aa12\n"
    "104 play 2 t14,u14,r15,s15,t15,u15,v15,w15,p16,q16,r16,s16,t16,u16,v16,w16,n17,o17,p17,q17,t17,u17\n"
    "105 play 3 ad15,ae15,ah15,ai15,al15,am15,ad16,ae16,af16,ag16,ah16,ai16,aj16,ak16,al16,am16,af17,ag17,aj17,ak17\n"
    "106 play 1 v13,w13,z13,aa13,ad13,ae13,v14,w14,x14,y14,z14,aa14,ab14,ac14,ad14,ae14,x15,y15,ab15,ac15\n"
    "107 play 2 z15,aa15,z16,aa16,ab16,ac16,ab17,ac17,ad17,ae17,ad18,ae18,af18,ag18,af19,ag19,ah19,ai19,ah20,ai20\n"
    "108 play 3 af10,ag10,ad11,ae11,af11,ag11,ab12,ac12,ad12,ae12,af12,ag12,ab13,ac13,af13,ag13,ah13,ai13,ah14,ai14\n"
    "109 play 1 p10,q10,t10,u10,p11,q11,r11,s11,t11,u11,v11,w11,r12,s12,v12,w12,x12,y12\n"
    "110 play 2 an16,ao16,an17,ao17,ap17,aq17,an18,ao18,ap18,aq18,ar18,as18,al19,am19,an19,ao19,ar19,as19,al20,am20\n"
    "111 play 3 an20,ao20,ar20,as20,at20,au20,al21,am21,an21,ao21,ap21,aq21,ar21,as21,al22,am22,an22,ao22,ap22,aq22,an23,ao23\n"
    "112 play 1 r13,s13,l14,m14,p14,q14,r14,s14,l15,m15,n15,o15,p15,q15,l16,m16,n16,o16,l17,m17\n"
    "113 play 2 ah21,ai21,ah22,ai22,aj22,ak22,aj23,ak23,al23,am23,aj24,ak24,al24,am24,an24,ao24,aj25,ak25,an25,ao25\n"
    "114 play 3 al11,am11,ap11,aq11,al12,am12,an12,ao12,ap12,aq12,al13,am13,an13,ao13,ap13,aq13,al14,am14,ap14,aq14\n"
    "115 play 1 aj8,ak8,aj9,ak9,al9,am9,ap9,aq9,al10,am10,an10,ao10,ap10,aq10,ar10,as10,an11,ao11,ar11,as11\n"
    "116 play 2 f11,g11,f12,g12,h12,i12,h13,i13,j13,k13,h14,i14,j14,k14,h15,i15,j15,k15,j16,k16\n")
check_variant(gembloq_3 3 "${gembloq_3_setup}" "${gembloq_3_moves}"
    "(;GM[GembloQ Three-Player]A1[ad7,ae7,ab8,ac8,ad8,ae8,af8,ag8,ab9,ac9,ad9,ae9,af9,ag9,ab10,ac10,ad10,ae10,aa11,ab11,ac11,aa12]PL[1])")

# Variant-specific properties and rules
check_responses(duo "1 set_board (;GM[Blokus Duo]A1[d9,c10,d10,e10,d11]PL[B])
2 set_board (;GM[Blokus Duo]AB[d9,c10,d10,e10,d11]PL[1])
3 set_board (;GM[Blokus]AB[d9,c10,d10,e10,d11]PL[W])
4 set_board (;GM[Blokus Duo]AB[d9,c10,d10,e10,d11]PL[W])
" "4" "1;2;3")
check_responses(classic_2 "1 set_board (;GM[Blokus Two-Player]AB[a20]PL[1])
2 set_board (;GM[Blokus Two-Player]A3[t1]PL[4])
" "2" "1")
check_responses(trigon_3 "1 set_board (;GM[Blokus Trigon Three-Player]PL[4])
2 set_board (;GM[Blokus Trigon Three-Player]A4[n9,o9,p9]PL[1])
3 set_board (;GM[Blokus Trigon Three-Player]PL[3])
" "3" "1;2")
# Junior has two instances of each piece, junior_setup contains two L5
check_responses(junior "1 set_board ${junior_setup}
2 set_board (;GM[Blokus Junior]AB[d10,e10,c11,d11,e11][f8,g8,h8,i8,f9][i12,j12,k12,l12,i13][a1,b1,c1,d1,a2]PL[W])
" "1" "2")
# Callisto: a 1-piece cannot be in the center and two 1-pieces must be played
# before other pieces
check_responses(callisto "1 set_board (;GM[Callisto]A1[k17][m15,k16,l16,m16,m17]PL[1])
2 set_board (;GM[Callisto]A1[k17][f7][m15,k16,l16,m16,m17]PL[1])
3 set_board (;GM[Callisto]A1[j10]PL[1])
4 set_board (;GM[Callisto]A1[k17][f7]PL[2])
" "2;4" "1;3")
