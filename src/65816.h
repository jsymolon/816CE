

#ifndef CPU_65816_H
#define CPU_65816_H

#include <stdint.h>

// Interrupt Vectors
#define CPU_VEC_NATIVE_COP 0xffe4
#define CPU_VEC_NATIVE_BRK 0xffe6
#define CPU_VEC_NATIVE_ABORT 0xffe8
#define CPU_VEC_NATIVE_NMI 0xffea
#define CPU_VEC_NATIVE_IRQ 0xffee
#define CPU_VEC_EMU_COP 0xfff4
#define CPU_VEC_EMU_ABORT 0xfff8
#define CPU_VEC_EMU_NMI 0xfffa
#define CPU_VEC_RESET 0xfffc
#define CPU_VEC_EMU_IRQ 0xfffe

// CPU "Class"
typedef struct CPU_t CPU_t;

struct CPU_t 
{
    int32_t C;
    int32_t DBR;
    int32_t X;
    int32_t Y;
    int32_t D;
    int32_t SP;
    int32_t PBR;
    int32_t PC;
    struct {
        // Order matters:
        unsigned char C : 1;   
        unsigned char Z : 1;
        unsigned char I : 1;
        unsigned char D : 1;
        unsigned char X : 1; // B in emulation
        unsigned char M : 1;
        unsigned char V : 1;
        unsigned char N : 1;
        // Order no longer matters:
        unsigned char E : 1;
        unsigned char RST : 1; // 1 if the CPU was reset, 0 if reset vector has been jumped to
        unsigned char IRQ : 1; // 1 if IRQ input is asserted, 0 else
        unsigned char NMI : 1; // 1 if NMI input is asserted, 0 else
        // unsigned char ABT : 1; // 1 if ABORT input is asserted, 0 else
    } P;
    uint32_t cycles; // Total phi-1 cycles the CPU has run
};

// Possible error codes from CPU public (non-static) functions
 typedef enum CPU_Error_Code_t{
     CPU_ERR_OK = 0,
     CPU_ERR_UNKNOWN_OPCODE,
     CPU_ERR_NULL_CPU, // Only used if `CPU_DEBUG_CHECK_NULL` is defined
 } CPU_Error_Code_t;

 // Get the value of a CPU's SR
 // NOTE: __CPU__ must be a pointer to a CPU struct
#define CPU_GET_SR(__CPU__) (*(uint8_t*)&(__CPU__->P))

// Set the value of the CPU's SR
// NOTE: __CPU__ must be a pointer to a CPU struct
#define CPU_SET_SR(__CPU__, __BYTE__) ( (*(uint8_t*)&__CPU__->P) = __BYTE__)

// Get a CPU's 24 bit PC address
// NOTE: __CPU__ must be a pointer to a CPU struct
#define CPU_GET_EFFECTIVE_PC24(__CPU__) ( ((__CPU__->PBR & 0xff) << 16) | (__CPU__->PC & 0xffff) )

// Update a CPU's PC value by an offset
// Enforces bank wrapping
// NOTE: __CPU__ must be a pointer to a CPU struct
#define CPU_UPDATE_PC16(__CPU__, __OFFSET__) ( __CPU__->PC = (__CPU__->PC + __OFFSET__) & 0xffff )

// Get a byte from memory
// NOTE: __MEM__ must be a pointer to an int16_t array
#define ADDR_GET_MEM_BYTE(__MEM__, __ADDR__) (__MEM__[__ADDR__] & 0xff)

// Get the byte stored in memory at a CPU's PC+1
// NOTE: __CPU__ must be a pointer to a CPU struct
#define ADDR_GET_MEM_IMMD_BYTE(__CPU__, __MEM__) (__MEM__[CPU_GET_EFFECTIVE_PC(__CPU__) + 1] & 0xff)

// Get the word stored in memory at a CPU's PC+1 and PC+2
// NOTE: __CPU__ must be a pointer to a CPU struct
#define ADDR_GET_MEM_IMMD_WORD(__CPU__, __MEM__) ( (__MEM__[CPU_GET_EFFECTIVE_PC(__CPU__) + 1] & 0xff) | ((__MEM__[CPU_GET_EFFECTIVE_PC(__CPU__) + 2] & 0xff) << 8) )

// Add a value to an address, wrapping around the page if necessary
#define ADDR_ADD_VAL_PAGE_WRAP(__ADDR__, __OFFSET__) ( (__ADDR__ & 0xffff00) | ( (__ADDR__ + __OFFSET__) & 0xff) )

// Add a value to an address, wrapping around the bank if necessary
#define ADDR_ADD_VAL_BANK_WRAP(__ADDR__, __OFFSET__) ( (__ADDR__ & 0xff0000) | ( (__ADDR__ + __OFFSET__) & 0xffff) )


CPU_Error_Code_t resetCPU(CPU_t *);
CPU_Error_Code_t stepCPU(CPU_t*, int16_t*);
static void _stackCPU_pushByte(CPU_t *, int16_t *mem, int32_t);
static void _stackCPU_pushWord(CPU_t *, int16_t *mem, int32_t);
static int32_t _stackCPU_popByte(CPU_t *, int16_t *mem);
static int32_t _stackCPU_popWord(CPU_t *, int16_t *mem);

static int32_t _addrCPU_getAbsoluteIndexedIndirectX(CPU_t *cpu, int16_t *mem);
static int32_t _addrCPU_getAbsoluteIndirect(CPU_t *cpu, int16_t *mem);
static int32_t _addrCPU_getAbsoluteIndirectLong(CPU_t *cpu, int16_t *mem);

#endif
