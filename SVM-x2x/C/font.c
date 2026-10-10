#include "defines.h"
#include "font.h"

__flash TWord CHARACTER_SET14[CHARACTER_SET14_LEN]=
  {
    SEG_EMPTY,                                                    // #0
    SEG_A|SEG_D|SEG_J|SEG_L|SEG_M|SEG_P,                          // #1
    SEG_A|SEG_D|SEG_J|SEG_K|SEG_L|SEG_M|SEG_P,                    // #2
    SEG_A|SEG_D|SEG_J|SEG_L|SEG_M|SEG_N|SEG_P,                    // #3
    SEG_B|SEG_C|SEG_E|SEG_F|SEG_J|SEG_L|SEG_M|SEG_P,              // #4
    SEG_D|SEG_G1|SEG_G2|SEG_K|SEG_N,                              // #5
    SEG_A|SEG_D|SEG_J|SEG_L|SEG_N,                                // #6
    SEG_G1|SEG_G2|SEG_K|SEG_N,                                    // #7
    SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G1|SEG_G2|SEG_J|
      SEG_K|SEG_L|SEG_M|SEG_N|SEG_P,                              // #8
    SEG_G1|SEG_G2|SEG_J|SEG_K|SEG_L|SEG_M|SEG_N|SEG_P,            // #9
    SEG_A|SEG_J|SEG_L,                                            // #10
    SEG_C|SEG_D|SEG_E|SEG_G1|SEG_G2|SEG_L,                        // #11
    SEG_A|SEG_B|SEG_D|SEG_F|SEG_G1|SEG_G2|SEG_N,                  // #12
    SEG_D|SEG_K|SEG_M|SEG_P,                                      // #13
    SEG_A|SEG_C|SEG_E|SEG_G1|SEG_G2|SEG_K|SEG_M|SEG_P,            // #14
    SEG_G1|SEG_G2|SEG_J|SEG_K|SEG_L|SEG_M|SEG_N|SEG_P,            // #15
    SEG_E|SEG_F|SEG_J|SEG_P,                                      // #16
    SEG_B|SEG_C|SEG_K|SEG_N,                                      // #17
    SEG_K|SEG_N,                                                  // #18
    SEG_B|SEG_C|SEG_E|SEG_F,                                      // #19
    SEG_A|SEG_B|SEG_C|SEG_F|SEG_K|SEG_N,                          // #20
    SEG_A|SEG_C|SEG_D|SEG_F|SEG_G1|SEG_G2|SEG_K|SEG_N,            // #21
    SEG_D,                                                        // #22
    SEG_A|SEG_D|SEG_K|SEG_N,                                      // #23
    SEG_A|SEG_K|SEG_N,                                            // #24
    SEG_D|SEG_K|SEG_N,                                            // #25
    SEG_B|SEG_C|SEG_G1|SEG_G2,                                    // #26
    SEG_E|SEG_F|SEG_G1|SEG_G2,                                    // #27
    SEG_D|SEG_E,                                                  // #28
    SEG_B|SEG_C|SEG_E|SEG_F|SEG_G1|SEG_G2,                        // #29
    SEG_D|SEG_M|SEG_P,                                            // #30
    SEG_A|SEG_J|SEG_L,                                            // #31
    SEG_EMPTY,                                                    // space
    SEG_K|SEG_N,                                                  // !
    SEG_B|SEG_K,                                                  // "
    SEG_EMPTY,                                                    // #
    SEG_A|SEG_C|SEG_D|SEG_F|SEG_G1|SEG_G2|SEG_K|SEG_N,            // $
    SEG_C|SEG_F|SEG_G1|SEG_G2|SEG_K|SEG_M|SEG_L|SEG_P,            // %
    SEG_A|SEG_D|SEG_K|SEG_L|SEG_N|SEG_P,                          // &
    SEG_K,                                                        // '
    SEG_A|SEG_D|SEG_E|SEG_F,                                      // (
    SEG_A|SEG_B|SEG_C|SEG_D,                                      // )
    SEG_G1|SEG_G2|SEG_J|SEG_L|SEG_M|SEG_P,                        // *
    SEG_G1|SEG_G2|SEG_K|SEG_N,                                    // +
    SEG_N,                                                        // ,
    SEG_G1|SEG_G2,                                                // -
    SEG_DP,                                                       // .
    SEG_L|SEG_P,                                                  // /
    SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F,                          // 0
    SEG_B|SEG_C,                                                  // 1
    SEG_A|SEG_B|SEG_D|SEG_E|SEG_G1|SEG_G2,                        // 2
    SEG_A|SEG_B|SEG_C|SEG_D|SEG_G1|SEG_G2,                        // 3
    SEG_B|SEG_C|SEG_F|SEG_G1|SEG_G2,                              // 4
    SEG_A|SEG_C|SEG_D|SEG_F|SEG_G1|SEG_G2,                        // 5
    SEG_A|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G1|SEG_G2,                  // 6
    SEG_A|SEG_B|SEG_C,                                            // 7
    SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G1|SEG_G2,            // 8
    SEG_A|SEG_B|SEG_C|SEG_D|SEG_F|SEG_G1|SEG_G2,                  // 9
    SEG_K|SEG_N,                                                  // :
    SEG_K|SEG_P,                                                  // ;
    SEG_L|SEG_M,                                                  // <
    SEG_D|SEG_G1|SEG_G2,                                          // =
    SEG_K|SEG_P,                                                  // >
    SEG_A|SEG_B|SEG_D|SEG_G2|SEG_N,                               // ?
    SEG_A|SEG_B|SEG_D|SEG_E|SEG_F|SEG_G2|SEG_J,                   // @
    SEG_A|SEG_B|SEG_C|SEG_E|SEG_F|SEG_G1|SEG_G2,                  // A
    SEG_A|SEG_B|SEG_C|SEG_D|SEG_G2|SEG_K|SEG_N,                   // B
    SEG_A|SEG_D|SEG_E|SEG_F,                                      // C
    SEG_A|SEG_B|SEG_C|SEG_D|SEG_K|SEG_N,                          // D
    SEG_A|SEG_D|SEG_E|SEG_F|SEG_G1,                               // E
    SEG_A|SEG_E|SEG_F|SEG_G1,                                     // F
    SEG_A|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G2,                         // G
    SEG_B|SEG_C|SEG_E|SEG_F|SEG_G1|SEG_G2,                        // H
    SEG_A|SEG_D|SEG_K|SEG_N,                                      // I
    SEG_B|SEG_C|SEG_D|SEG_E,                                      // J
    SEG_E|SEG_F|SEG_G1|SEG_L|SEG_M,                               // K
    SEG_D|SEG_E|SEG_F,                                            // L
    SEG_B|SEG_C|SEG_E|SEG_F|SEG_J|SEG_L,                          // M
    SEG_B|SEG_C|SEG_E|SEG_F|SEG_J|SEG_M,                          // N
    SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F,                          // O
    SEG_A|SEG_B|SEG_E|SEG_F|SEG_G1|SEG_G2,                        // P
    SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_M,                    // Q
    SEG_A|SEG_B|SEG_E|SEG_F|SEG_G1|SEG_G2|SEG_M,                  // R
    SEG_A|SEG_C|SEG_D|SEG_F|SEG_G1|SEG_G2,                        // S
    SEG_A|SEG_K|SEG_N,                                            // T
    SEG_B|SEG_C|SEG_D|SEG_E|SEG_F,                                // U
    SEG_E|SEG_F|SEG_L|SEG_P,                                      // V
    SEG_B|SEG_C|SEG_E|SEG_F|SEG_M|SEG_P,                          // W
    SEG_J|SEG_L|SEG_M|SEG_P,                                      // X
    SEG_J|SEG_L|SEG_N,                                            // Y
    SEG_A|SEG_D|SEG_L|SEG_P,                                      // Z
    SEG_A|SEG_D|SEG_E|SEG_F,                                      // [
    SEG_K|SEG_M,                                                  // "\"
    SEG_B|SEG_C|SEG_A|SEG_D,                                      // ]
    SEG_M|SEG_P,                                                  // ^
    SEG_D,                                                        // _
    SEG_P,                                                        // `
    SEG_D|SEG_E|SEG_G1|SEG_N,                                     // a
    SEG_C|SEG_D|SEG_E|SEG_F|SEG_G1|SEG_G2,                        // b
    SEG_D|SEG_E|SEG_G1|SEG_G2,                                    // c
    SEG_B|SEG_C|SEG_D|SEG_E|SEG_G1|SEG_G2,                        // d
    SEG_D|SEG_E|SEG_G1|SEG_P,                                     // e
    SEG_D|SEG_G1|SEG_G2|SEG_L|SEG_N,                              // f
    SEG_A|SEG_B|SEG_C|SEG_D|SEG_G2|SEG_J,                         // g
    SEG_C|SEG_E|SEG_F|SEG_G1|SEG_G2,                              // h
    SEG_D|SEG_G1|SEG_N,                                           // i
    SEG_C|SEG_D|SEG_G2,                                           // j
    SEG_E|SEG_F|SEG_G1|SEG_L|SEG_M,                               // k
    SEG_K|SEG_N,                                                  // l
    SEG_C|SEG_E|SEG_G1|SEG_G2|SEG_N,                              // m
    SEG_C|SEG_E|SEG_G1|SEG_G2,                                    // n
    SEG_C|SEG_D|SEG_E|SEG_G1|SEG_G2,                              // o
    SEG_A|SEG_E|SEG_F|SEG_G1|SEG_L,                               // p
    SEG_A|SEG_B|SEG_C|SEG_F|SEG_G1|SEG_G2,                        // q
    SEG_E|SEG_G1|SEG_G2,                                          // r
    SEG_A|SEG_C|SEG_D|SEG_G2|SEG_J,                               // s
    SEG_D|SEG_E|SEG_F|SEG_G1,                                     // t
    SEG_C|SEG_D|SEG_E,                                            // u
    SEG_E|SEG_P,                                                  // v
    SEG_C|SEG_E|SEG_M|SEG_P,                                      // w
    SEG_J|SEG_L|SEG_M|SEG_P,                                      // x
    SEG_B|SEG_C|SEG_D|SEG_F|SEG_G1|SEG_G2,                        // y
    SEG_A|SEG_D|SEG_L|SEG_P,                                      // z
    SEG_A|SEG_D|SEG_E|SEG_F,                                      // {
    SEG_K|SEG_N,                                                  // |
    SEG_A|SEG_B|SEG_C|SEG_D,                                      // }
    SEG_G1|SEG_G2,                                                // ~
    SEG_EMPTY,                                                    // DEL
    SEG_A,
    SEG_B,
    SEG_C,
    SEG_D,
    SEG_E,
    SEG_F,
    SEG_G1,
    SEG_J,
    SEG_K,
    SEG_L,
    SEG_G2,
    SEG_M,
    SEG_N,
    SEG_P,
    SEG_DP
  };
