//
// HW_STK.c
//

/// \file HW_STK.c
/// Definition der Registerbank für den System-Tick (Core-Komponente).

#include <HW_STK.h>

RegisterBankSTK volatile STK __attribute__((at(0xE000E000)));

// ACHTUNG: Adresse ist 0xE000E000 statt 0xE000E010, da in der Struktur RegisterBankSTK 16 Bytes reservierter Bereich am Anfang 
// (notwendiger Trick bei Compiler-Option -OTime (Optimize for execution time))






























































































