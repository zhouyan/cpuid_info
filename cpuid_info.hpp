#ifndef CPUID_INFO_HPP
#define CPUID_INFO_HPP

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifdef _MSC
#include <intrin.h>
#endif

#define DEFINE_UTILITY_CPUID_FEATURE_INFO(a, c, i, b, feat)                   \
    template <>                                                               \
    class CPUIDFeatureInfo<CPUIDFeature##feat>                                \
    {                                                                         \
        public:                                                               \
        static std::string str() { return std::string(#feat); }               \
        static const unsigned eax = a##U;                                     \
        static const unsigned ecx = c##U;                                     \
        static const unsigned bit = b##U;                                     \
        static const std::size_t index = i;                                   \
    }; // class CPUIDFeatureInfo

#define DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(a, c, i, b, feat)               \
    template <>                                                               \
    class CPUIDFeatureInfo<CPUIDFeatureExt##feat>                             \
    {                                                                         \
        public:                                                               \
        static std::string str() { return std::string(#feat); }               \
        static const unsigned eax = 0x80000000U + a##U;                       \
        static const unsigned ecx = c##U;                                     \
        static const unsigned bit = b##U;                                     \
        static const std::size_t index = i;                                   \
    }; // CPUIDFeatureInfo

template <bool>
class BoolType
{
}; // class BoolType

typedef BoolType<true> TrueType;
typedef BoolType<false> FalseType;

inline void cpuid(unsigned eax, unsigned ecx, unsigned *reg)
{
#ifdef _MSC
    __cpuidex(reinterpret_cast<int *>(reg), static_cast<int>(eax),
        static_cast<int>(ecx));
#else
    unsigned ebx = 0x00;
    unsigned edx = 0x00;
    __asm__ volatile("cpuid\n"
                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                     : "a"(eax), "c"(ecx));
    reg[0] = eax;
    reg[1] = ebx;
    reg[2] = ecx;
    reg[3] = edx;
#endif
}

/// \brief CPU features
/// \ingroup CPUID
enum CPUIDFeature {
    CPUIDFeatureSSE3,         // EAX = 0x01, ECX = 0x00; ECX[00]
    CPUIDFeaturePCLMULQDQ,    // EAX = 0x01, ECX = 0x00; ECX[01]
    CPUIDFeatureDTES64,       // EAX = 0x01, ECX = 0x00; ECX[02]
    CPUIDFeatureMONITOR,      // EAX = 0x01, ECX = 0x00; ECX[03]
    CPUIDFeatureDS_CPL,       // EAX = 0x01, ECX = 0x00; ECX[04]
    CPUIDFeatureVMX,          // EAX = 0x01, ECX = 0x00; ECX[05]
    CPUIDFeatureSMX,          // EAX = 0x01, ECX = 0x00; ECX[06]
    CPUIDFeatureEST,          // EAX = 0x01, ECX = 0x00; ECX[07]
    CPUIDFeatureTM2,          // EAX = 0x01, ECX = 0x00; ECX[08]
    CPUIDFeatureSSSE3,        // EAX = 0x01, ECX = 0x00; ECX[09]
    CPUIDFeatureCNXT_ID,      // EAX = 0x01, ECX = 0x00; ECX[10]
    CPUIDFeatureSDBG,         // EAX = 0x01, ECX = 0x00; ECX[11]
    CPUIDFeatureFMA,          // EAX = 0x01, ECX = 0x00; ECX[12]
    CPUIDFeatureCX16,         // EAX = 0x01, ECX = 0x00; ECX[13]
    CPUIDFeatureXTPR,         // EAX = 0x01, ECX = 0x00; ECX[14]
    CPUIDFeaturePDCM,         // EAX = 0x01, ECX = 0x00; ECX[15]
                              // EAX = 0x01, ECX = 0x00; ECX[16]
    CPUIDFeaturePCID,         // EAX = 0x01, ECX = 0x00; ECX[17]
    CPUIDFeatureDCA,          // EAX = 0x01, ECX = 0x00; ECX[18]
    CPUIDFeatureSSE4_1,       // EAX = 0x01, ECX = 0x00; ECX[19]
    CPUIDFeatureSSE4_2,       // EAX = 0x01, ECX = 0x00; ECX[20]
    CPUIDFeatureX2APIC,       // EAX = 0x01, ECX = 0x00; ECX[21]
    CPUIDFeatureMOVBE,        // EAX = 0x01, ECX = 0x00; ECX[22]
    CPUIDFeaturePOPCNT,       // EAX = 0x01, ECX = 0x00; ECX[23]
    CPUIDFeatureTSC_DEADLINE, // EAX = 0x01, ECX = 0x00; ECX[24]
    CPUIDFeatureAES,          // EAX = 0x01, ECX = 0x00; ECX[25]
    CPUIDFeatureXSAVE,        // EAX = 0x01, ECX = 0x00; ECX[26]
    CPUIDFeatureOSXSAVE,      // EAX = 0x01, ECX = 0x00; ECX[27]
    CPUIDFeatureAVX,          // EAX = 0x01, ECX = 0x00; ECX[28]
    CPUIDFeatureF16C,         // EAX = 0x01, ECX = 0x00; ECX[29]
    CPUIDFeatureRDRAND,       // EAX = 0x01, ECX = 0x00; ECX[30]
    CPUIDFeatureHYPERVISOR,   // EAX = 0x01, ECX = 0x00; ECX[31]

    CPUIDFeatureFPU,    // EAX = 0x01, ECX = 0x00; EDX[00]
    CPUIDFeatureVME,    // EAX = 0x01, ECX = 0x00; EDX[01]
    CPUIDFeatureDE,     // EAX = 0x01, ECX = 0x00; EDX[02]
    CPUIDFeaturePSE,    // EAX = 0x01, ECX = 0x00; EDX[03]
    CPUIDFeatureTSC,    // EAX = 0x01, ECX = 0x00; EDX[04]
    CPUIDFeatureMSR,    // EAX = 0x01, ECX = 0x00; EDX[05]
    CPUIDFeaturePAE,    // EAX = 0x01, ECX = 0x00; EDX[06]
    CPUIDFeatureMCE,    // EAX = 0x01, ECX = 0x00; EDX[07]
    CPUIDFeatureCX8,    // EAX = 0x01, ECX = 0x00; EDX[08]
    CPUIDFeatureAPIC,   // EAX = 0x01, ECX = 0x00; EDX[09]
                        // EAX = 0x01, ECX = 0x00; EDX[10]
    CPUIDFeatureSEP,    // EAX = 0x01, ECX = 0x00; EDX[11]
    CPUIDFeatureMTRR,   // EAX = 0x01, ECX = 0x00; EDX[12]
    CPUIDFeaturePGE,    // EAX = 0x01, ECX = 0x00; EDX[13]
    CPUIDFeatureMCA,    // EAX = 0x01, ECX = 0x00; EDX[14]
    CPUIDFeatureCMOV,   // EAX = 0x01, ECX = 0x00; EDX[15]
    CPUIDFeaturePAT,    // EAX = 0x01, ECX = 0x00; EDX[16]
    CPUIDFeaturePSE_36, // EAX = 0x01, ECX = 0x00; EDX[17]
    CPUIDFeaturePSN,    // EAX = 0x01, ECX = 0x00; EDX[18]
    CPUIDFeatureCLFSH,  // EAX = 0x01, ECX = 0x00; EDX[19]
                        // EAX = 0x01, ECX = 0x00; EDX[20]
    CPUIDFeatureDS,     // EAX = 0x01, ECX = 0x00; EDX[21]
    CPUIDFeatureACPI,   // EAX = 0x01, ECX = 0x00; EDX[22]
    CPUIDFeatureMMX,    // EAX = 0x01, ECX = 0x00; EDX[23]
    CPUIDFeatureFXSR,   // EAX = 0x01, ECX = 0x00; EDX[24]
    CPUIDFeatureSSE,    // EAX = 0x01, ECX = 0x00; EDX[25]
    CPUIDFeatureSSE2,   // EAX = 0x01, ECX = 0x00; EDX[26]
    CPUIDFeatureSS,     // EAX = 0x01, ECX = 0x00; EDX[27]
    CPUIDFeatureHTT,    // EAX = 0x01, ECX = 0x00; EDX[28]
    CPUIDFeatureTM,     // EAX = 0x01, ECX = 0x00; EDX[29]
    CPUIDFeatureIA64,   // EAX = 0x01, ECX = 0x00; EDX[30]
    CPUIDFeaturePBE,    // EAX = 0x01, ECX = 0x00; EDX[31]

    CPUIDFeatureFSGSBASE,        // EAX = 0x07, ECX = 0x00; EBX[00]
    CPUIDFeatureIA32_TSC_ADJUST, // EAX = 0x07, ECX = 0x00; EBX[01]
    CPUIDFeatureSGX,             // EAX = 0x07, ECX = 0x00; EBX[02]
    CPUIDFeatureBMI1,            // EAX = 0x07, ECX = 0x00; EBX[03]
    CPUIDFeatureHLE,             // EAX = 0x07, ECX = 0x00; EBX[04]
    CPUIDFeatureAVX2,            // EAX = 0x07, ECX = 0x00; EBX[05]
                                 // EAX = 0x07, ECX = 0x00; EBX[06]
    CPUIDFeatureSMEP,            // EAX = 0x07, ECX = 0x00; EBX[07]
    CPUIDFeatureBMI2,            // EAX = 0x07, ECX = 0x00; EBX[08]
    CPUIDFeatureERMS,            // EAX = 0x07, ECX = 0x00; EBX[09]
    CPUIDFeatureINVPCID,         // EAX = 0x07, ECX = 0x00; EBX[10]
    CPUIDFeatureRTM,             // EAX = 0x07, ECX = 0x00; EBX[11]
    CPUIDFeaturePQM,             // EAX = 0x07, ECX = 0x00; EBX[12]
    CPUIDFeatureFPU_CS_DS,       // EAX = 0x07, ECX = 0x00; EBX[13]
    CPUIDFeatureMPX,             // EAX = 0x07, ECX = 0x00; EBX[14]
    CPUIDFeaturePQE,             // EAX = 0x07, ECX = 0x00; EBX[15]
    CPUIDFeatureAVX512F,         // EAX = 0x07, ECX = 0x00; EBX[16]
    CPUIDFeatureAVX512DQ,        // EAX = 0x07, ECX = 0x00; EBX[17]
    CPUIDFeatureRDSEED,          // EAX = 0x07, ECX = 0x00; EBX[18]
    CPUIDFeatureADX,             // EAX = 0x07, ECX = 0x00; EBX[19]
    CPUIDFeatureSMAP,            // EAX = 0x07, ECX = 0x00; EBX[20]
    CPUIDFeatureAVX512IFMA52,    // EAX = 0x07, ECX = 0x00; EBX[21]
    CPUIDFeaturePCOMMIT,         // EAX = 0x07, ECX = 0x00; EBX[22]
    CPUIDFeatureCLFLUSHOPT,      // EAX = 0x07, ECX = 0x00; EBX[23]
    CPUIDFeatureCLWB,            // EAX = 0x07, ECX = 0x00; EBX[24]
    CPUIDFeatureINTEL_TRACE,     // EAX = 0x07, ECX = 0x00; EBX[25]
    CPUIDFeatureAVX512PF,        // EAX = 0x07, ECX = 0x00; EBX[26]
    CPUIDFeatureAVX512ER,        // EAX = 0x07, ECX = 0x00; EBX[27]
    CPUIDFeatureAVX512CD,        // EAX = 0x07, ECX = 0x00; EBX[28]
    CPUIDFeatureSHA,             // EAX = 0x07, ECX = 0x00; EBX[29]
    CPUIDFeatureAVX512BW,        // EAX = 0x07, ECX = 0x00; EBX[30]
    CPUIDFeatureAVX512VL,        // EAX = 0x07, ECX = 0x00; EBX[31]

    CPUIDFeaturePREFETCHWT1, // EAX = 0x07, ECX = 0x00; ECX[00]
    CPUIDFeatureAVX512VBMI,  // EAX = 0x07, ECX = 0x00; ECX[01]
    CPUIDFeaturePKU,         // EAX = 0x07, ECX = 0x00; ECX[03]
    CPUIDFeatureOSPKE,       // EAX = 0x07, ECX = 0x00; ECX[04]

    CPUIDFeatureExtLAHF_LM,       // EAX = 0x80000001, ECX = 0x00; ECX[00]
    CPUIDFeatureExtCMP_LEGACY,    // EAX = 0x80000001, ECX = 0x00; ECX[01]
    CPUIDFeatureExtSVM,           // EAX = 0x80000001, ECX = 0x00; ECX[02]
    CPUIDFeatureExtEXTAPIC,       // EAX = 0x80000001, ECX = 0x00; ECX[03]
    CPUIDFeatureExtCR8_LEGACY,    // EAX = 0x80000001, ECX = 0x00; ECX[04]
    CPUIDFeatureExtABM,           // EAX = 0x80000001, ECX = 0x00; ECX[05]
    CPUIDFeatureExtSSE4A,         // EAX = 0x80000001, ECX = 0x00; ECX[06]
    CPUIDFeatureExtMISALIGNSSE,   // EAX = 0x80000001, ECX = 0x00; ECX[07]
    CPUIDFeatureExt3DNOWPREFETCH, // EAX = 0x80000001, ECX = 0x00; ECX[08]
    CPUIDFeatureExtOSVW,          // EAX = 0x80000001, ECX = 0x00; ECX[09]
    CPUIDFeatureExtIBS,           // EAX = 0x80000001, ECX = 0x00; ECX[10]
    CPUIDFeatureExtXOP,           // EAX = 0x80000001, ECX = 0x00; ECX[11]
    CPUIDFeatureExtSKINIT,        // EAX = 0x80000001, ECX = 0x00; ECX[12]
    CPUIDFeatureExtWDT,           // EAX = 0x80000001, ECX = 0x00; ECX[13]
                                  // EAX = 0x80000001, ECX = 0x00; ECX[14]
    CPUIDFeatureExtLWP,           // EAX = 0x80000001, ECX = 0x00; ECX[15]
    CPUIDFeatureExtFMA4,          // EAX = 0x80000001, ECX = 0x00; ECX[16]
    CPUIDFeatureExtTCE,           // EAX = 0x80000001, ECX = 0x00; ECX[17]
                                  // EAX = 0x80000001, ECX = 0x00; ECX[18]
    CPUIDFeatureExtNODEID_MSR,    // EAX = 0x80000001, ECX = 0x00; ECX[19]
                                  // EAX = 0x80000001, ECX = 0x00; ECX[20]
    CPUIDFeatureExtTBM,           // EAX = 0x80000001, ECX = 0x00; ECX[21]
    CPUIDFeatureExtTOPOEXT,       // EAX = 0x80000001, ECX = 0x00; ECX[22]
    CPUIDFeatureExtPERFCTR_CORE,  // EAX = 0x80000001, ECX = 0x00; ECX[23]
    CPUIDFeatureExtPERFCTR_NB,    // EAX = 0x80000001, ECX = 0x00; ECX[24]
                                  // EAX = 0x80000001, ECX = 0x00; ECX[25]
    CPUIDFeatureExtDBX,           // EAX = 0x80000001, ECX = 0x00; ECX[26]
    CPUIDFeatureExtPERFTSC,       // EAX = 0x80000001, ECX = 0x00; ECX[27]
    CPUIDFeatureExtPCX_L2I,       // EAX = 0x80000001, ECX = 0x00; ECX[28]
                                  // EAX = 0x80000001, ECX = 0x00; ECX[29]
                                  // EAX = 0x80000001, ECX = 0x00; ECX[30]
                                  // EAX = 0x80000001, ECX = 0x00; ECX[31]

    CPUIDFeatureExtFPU,      // EAX = 0x80000001, ECX = 0x00; EDX[00]
    CPUIDFeatureExtVME,      // EAX = 0x80000001, ECX = 0x00; EDX[01]
    CPUIDFeatureExtDE,       // EAX = 0x80000001, ECX = 0x00; EDX[02]
    CPUIDFeatureExtPSE,      // EAX = 0x80000001, ECX = 0x00; EDX[03]
    CPUIDFeatureExtTSC,      // EAX = 0x80000001, ECX = 0x00; EDX[04]
    CPUIDFeatureExtMSR,      // EAX = 0x80000001, ECX = 0x00; EDX[05]
    CPUIDFeatureExtPAE,      // EAX = 0x80000001, ECX = 0x00; EDX[06]
    CPUIDFeatureExtMCE,      // EAX = 0x80000001, ECX = 0x00; EDX[07]
    CPUIDFeatureExtCX8,      // EAX = 0x80000001, ECX = 0x00; EDX[08]
    CPUIDFeatureExtAPIC,     // EAX = 0x80000001, ECX = 0x00; EDX[09]
                             // EAX = 0x80000001, ECX = 0x00; EDX[10]
    CPUIDFeatureExtSYSCALL,  // EAX = 0x80000001, ECX = 0x00; EDX[11]
    CPUIDFeatureExtMTRR,     // EAX = 0x80000001, ECX = 0x00; EDX[12]
    CPUIDFeatureExtPGE,      // EAX = 0x80000001, ECX = 0x00; EDX[13]
    CPUIDFeatureExtMCA,      // EAX = 0x80000001, ECX = 0x00; EDX[14]
    CPUIDFeatureExtCMOV,     // EAX = 0x80000001, ECX = 0x00; EDX[15]
    CPUIDFeatureExtPAT,      // EAX = 0x80000001, ECX = 0x00; EDX[16]
    CPUIDFeatureExtPSE36,    // EAX = 0x80000001, ECX = 0x00; EDX[17]
                             // EAX = 0x80000001, ECX = 0x00; EDX[18]
    CPUIDFeatureExtMP,       // EAX = 0x80000001, ECX = 0x00; EDX[19]
    CPUIDFeatureExtNX,       // EAX = 0x80000001, ECX = 0x00; EDX[20]
                             // EAX = 0x80000001, ECX = 0x00; EDX[21]
    CPUIDFeatureExtMMX,      // EAX = 0x80000001, ECX = 0x00; EDX[22]
    CPUIDFeatureExtMMXEXT,   // EAX = 0x80000001, ECX = 0x00; EDX[23]
    CPUIDFeatureExtFXSR,     // EAX = 0x80000001, ECX = 0x00; EDX[24]
    CPUIDFeatureExtFXSR_OPT, // EAX = 0x80000001, ECX = 0x00; EDX[25]
    CPUIDFeatureExtGBPAGES,  // EAX = 0x80000001, ECX = 0x00; EDX[26]
    CPUIDFeatureExtRDTSCP,   // EAX = 0x80000001, ECX = 0x00; EDX[27]
                             // EAX = 0x80000001, ECX = 0x00; EDX[28]
    CPUIDFeatureExtLM,       // EAX = 0x80000001, ECX = 0x00; EDX[29]
    CPUIDFeatureExt3DNOWEXT, // EAX = 0x80000001, ECX = 0x00; EDX[30]
    CPUIDFeatureExt3DNOW     // EAX = 0x80000001, ECX = 0x00; EDX[31]
};                           // enum CPUIDFeature

/// \brief Type of CPU caches
enum CPUIDCacheType {
    CPUIDCacheTypeNull,        // No more cache
    CPUIDCacheTypeData,        // Data cache
    CPUIDCacheTypeInstruction, // Instruction cache
    CPUIDCacheTypeUnified      // Unified cache
};                             // enum CPUIDCacheType

/// \brief CPU feature information
/// \ingroup CPUID
///
/// \details
/// This class is specialized for each value of CPUIDFeature
template <CPUIDFeature>
class CPUIDFeatureInfo
{
    public:
    static std::string str();
    static const unsigned eax = 0x00U;
    static const unsigned ecx = 0x00U;
    static const unsigned index = 0x00U;
    static const unsigned bit = 0x00U;
}; // class CPUIDFeatureInfo

DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 0, SSE3)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 1, PCLMULQDQ)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 2, DTES64)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 3, MONITOR)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 4, DS_CPL)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 5, VMX)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 6, SMX)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 7, EST)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 8, TM2)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 9, SSSE3)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 10, CNXT_ID)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 11, SDBG)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 12, FMA)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 13, CX16)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 14, XTPR)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 15, PDCM)
// DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 16,)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 17, PCID)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 18, DCA)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 19, SSE4_1)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 20, SSE4_2)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 21, X2APIC)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 22, MOVBE)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 23, POPCNT)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 24, TSC_DEADLINE)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 25, AES)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 26, XSAVE)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 27, OSXSAVE)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 28, AVX)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 29, F16C)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 30, RDRAND)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 2, 31, HYPERVISOR)

DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 0, FPU)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 1, VME)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 2, DE)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 3, PSE)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 4, TSC)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 5, MSR)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 6, PAE)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 7, MCE)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 8, CX8)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 9, APIC)
// DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 10,)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 11, SEP)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 12, MTRR)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 13, PGE)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 14, MCA)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 15, CMOV)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 16, PAT)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 17, PSE_36)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 18, PSN)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 19, CLFSH)
// DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 20,)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 21, DS)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 22, ACPI)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 23, MMX)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 24, FXSR)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 25, SSE)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 26, SSE2)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 27, SS)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 28, HTT)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 29, TM)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 30, IA64)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x01, 0x00, 3, 31, PBE)

DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 0, FSGSBASE)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 1, IA32_TSC_ADJUST)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 2, SGX)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 3, BMI1)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 4, HLE)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 5, AVX2)
// DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 6,)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 7, SMEP)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 8, BMI2)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 9, ERMS)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 10, INVPCID)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 11, RTM)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 12, PQM)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 13, FPU_CS_DS)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 14, MPX)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 14, PQE)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 16, AVX512F)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 17, AVX512DQ)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 18, RDSEED)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 19, ADX)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 20, SMAP)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 21, AVX512IFMA52)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 22, PCOMMIT)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 23, CLFLUSHOPT)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 24, CLWB)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 25, INTEL_TRACE)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 26, AVX512PF)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 27, AVX512ER)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 28, AVX512CD)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 29, SHA)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 30, AVX512BW)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 1, 31, AVX512VL)

DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 3, 0, PREFETCHWT1)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 3, 1, AVX512VBMI)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 3, 3, PKU)
DEFINE_UTILITY_CPUID_FEATURE_INFO(0x07, 0x00, 3, 4, OSPKE)

DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 0, LAHF_LM)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 1, CMP_LEGACY)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 2, SVM)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 3, EXTAPIC)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 4, CR8_LEGACY)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 5, ABM)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 6, SSE4A)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 7, MISALIGNSSE)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 8, 3DNOWPREFETCH)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 9, OSVW)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 10, IBS)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 11, XOP)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 12, SKINIT)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 13, WDT)
// DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 14,)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 15, LWP)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 16, FMA4)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 17, TCE)
// DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 18,)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 19, NODEID_MSR)
// DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 20,)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 21, TBM)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 22, TOPOEXT)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 23, PERFCTR_CORE)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 24, PERFCTR_NB)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 26, DBX)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 27, PERFTSC)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 28, PCX_L2I)
// DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 29,)
// DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 30,)
// DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 2, 31,)

DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 0, FPU)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 1, VME)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 2, DE)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 3, PSE)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 4, TSC)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 5, MSR)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 6, PAE)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 7, MCE)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 8, CX8)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 9, APIC)
// DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 10,)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 11, SYSCALL)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 12, MTRR)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 13, PGE)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 14, MCA)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 15, CMOV)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 16, PAT)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 17, PSE36)
// DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 18,)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 19, MP)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 20, NX)
// DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 21,)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 22, MMX)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 23, MMXEXT)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 24, FXSR)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 25, FXSR_OPT)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 26, GBPAGES)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 27, RDTSCP)
// DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 28,)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 29, LM)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 30, 3DNOWEXT)
DEFINE_UTILITY_CPUID_FEATURE_INFO_EXT(0x01, 0x00, 3, 31, 3DNOW)

class CPUID
{
    public:
    struct reg_type {
        unsigned v[4];
    };

    class cache_param_type
    {
        public:
        cache_param_type(const reg_type &reg)
            : level_(0)
            , max_proc_sharing_(0)
            , max_proc_physical_(0)
            , line_size_(0)
            , partitions_(0)
            , ways_(0)
            , sets_(0)
            , self_initializing_(false)
            , fully_associative_(false)
            , wbinvd_(false)
            , inclusiveness_(false)
            , complex_indexing_(false)
        {
            unsigned t = extract_bits<4, 0>(reg.v[0]);
            switch (t) {
                case 1:
                    type_ = CPUIDCacheTypeData;
                    break;
                case 2:
                    type_ = CPUIDCacheTypeInstruction;
                    break;
                case 3:
                    type_ = CPUIDCacheTypeUnified;
                    break;
                default:
                    type_ = CPUIDCacheTypeNull;
                    return;
            }

            level_ = extract_bits<7, 5>(reg.v[0]);
            self_initializing_ = test_bit<8>(reg.v[0]);
            fully_associative_ = test_bit<9>(reg.v[0]);
            max_proc_sharing_ = extract_bits<25, 14>(reg.v[0]) + 1;
            max_proc_physical_ = extract_bits<31, 26>(reg.v[0]) + 1;

            line_size_ = extract_bits<11, 0>(reg.v[1]) + 1;
            partitions_ = extract_bits<21, 12>(reg.v[1]) + 1;
            ways_ = extract_bits<31, 22>(reg.v[1]) + 1;
            sets_ = extract_bits<31, 0>(reg.v[2]) + 1;
            size_ = line_size_ * partitions_ * ways_ * sets_;

            wbinvd_ = test_bit<0>(reg.v[3]);
            inclusiveness_ = test_bit<1>(reg.v[3]);
            complex_indexing_ = test_bit<2>(reg.v[3]);
        }

        CPUIDCacheType type() const { return type_; }
        unsigned level() const { return level_; }
        unsigned max_proc_sharing() const { return max_proc_sharing_; }
        unsigned max_proc_physical() const { return max_proc_physical_; }
        unsigned line_size() const { return line_size_; }
        unsigned partitions() const { return partitions_; }
        unsigned ways() const { return ways_; }
        unsigned sets() const { return sets_; }
        unsigned size() const { return size_; }
        bool self_initializing() const { return self_initializing_; }
        bool fully_associative() const { return fully_associative_; }
        bool wbinvd() const { return wbinvd_; }
        bool inclusiveness() const { return inclusiveness_; }
        bool complex_indexing() const { return complex_indexing_; }

        private:
        CPUIDCacheType type_;
        unsigned level_;
        unsigned max_proc_sharing_;
        unsigned max_proc_physical_;
        unsigned line_size_;
        unsigned partitions_;
        unsigned ways_;
        unsigned sets_;
        unsigned size_;
        bool self_initializing_;
        bool fully_associative_;
        bool wbinvd_;
        bool inclusiveness_;
        bool complex_indexing_;
    }; // class cache_param_type

    template <typename CharT, typename Traits>
    static std::basic_ostream<CharT, Traits> &info(
        std::basic_ostream<CharT, Traits> &os)
    {
        if (!os.good())
            return os;

        print_equal(os);
        const int width = 24;
        os << std::setw(width) << std::left << "Vendor ID" << vendor() << '\n';
        if (max_eax_ext() >= ext0_ + 4U) {
            os << std::setw(width) << std::left << "Processor brand" << brand()
               << '\n';
        }
        if (max_eax() >= 0x16) {
            os << std::setw(width) << std::left << "Base frequency (MHz)"
               << base_freq() << '\n';
            os << std::setw(width) << std::left << "Maximum frequency (MHz)"
               << max_freq() << '\n';
            os << std::setw(width) << std::left << "Bus frequency (MHz)"
               << bus_freq() << '\n';
        }
        if (max_eax() >= 0x04) {
            print_equal(os);
            os << "Deterministic cache parameters\n";
            print_dash(os);
            print_cache(os);
        }
        print_equal(os);
        print_feature(os);
        print_equal(os);

        return os;
    }

    template <unsigned EAX, unsigned ECX>
    static const reg_type &info()
    {
        static reg_type reg(
            info_dispatch<EAX, ECX>(BoolType<(EAX == 0x00 || EAX == ext0_)>(),
                BoolType<(EAX < ext0_)>()));

        return reg;
    }

    static unsigned max_eax() { return info<0x00, 0x00>().v[0]; }
    static unsigned max_eax_ext() { return info<ext0_, 0x00>().v[0]; }

    static std::string vendor()
    {
        reg_type reg(info<0x00, 0x00>());
        const unsigned *uptr = reg.v;
        char str[sizeof(unsigned) * 3 + 1] = {'\0'};
        std::memcpy(str + sizeof(unsigned) * 0, uptr + 1, sizeof(unsigned));
        std::memcpy(str + sizeof(unsigned) * 1, uptr + 3, sizeof(unsigned));
        std::memcpy(str + sizeof(unsigned) * 2, uptr + 2, sizeof(unsigned));

        return std::string(str);
    }

    static std::string brand()
    {
        reg_type reg2(info<ext0_ + 2U, 0>());
        reg_type reg3(info<ext0_ + 3U, 0>());
        reg_type reg4(info<ext0_ + 4U, 0>());
        const std::size_t reg_size = sizeof(unsigned) * 4;
        char str[reg_size * 3] = {'\0'};
        std::memcpy(str + reg_size * 0, reg2.v, reg_size);
        std::memcpy(str + reg_size * 1, reg3.v, reg_size);
        std::memcpy(str + reg_size * 2, reg4.v, reg_size);

        return std::string(str);
    }

    static unsigned cache_param_num()
    {
        reg_type reg;
        unsigned ecx = 0x00;
        while (true) {
            cpuid(0x04, ecx, reg.v);
            if (extract_bits<4, 0>(reg.v[0]) == 0)
                break;
            ++ecx;
        }

        return ecx;
    }

    static cache_param_type cache_param(unsigned cache_index)
    {
        reg_type reg;
        cpuid(0x04, cache_index, reg.v);

        return cache_param_type(reg);
    }

    static bool intel_turbo_boost()
    {
        return test_bit<1>(info<0x06, 0x00>().v[0]);
    }

    static unsigned base_freq()
    {
        return extract_bits<15, 0>(info<0x16, 0x00>().v[0]);
    }

    static unsigned max_freq()
    {
        return extract_bits<15, 0>(info<0x16, 0x00>().v[1]);
    }

    static unsigned bus_freq()
    {
        return extract_bits<15, 0>(info<0x16, 0x00>().v[2]);
    }

    template <CPUIDFeature Feat>
    static bool has_feature()
    {
        return test_bit<CPUIDFeatureInfo<Feat>::bit>(
            info<CPUIDFeatureInfo<Feat>::eax, CPUIDFeatureInfo<Feat>::ecx>()
                .v[CPUIDFeatureInfo<Feat>::index]);
    }

    private:
    static const unsigned ext0_ = 0x80000000U;

    template <unsigned, unsigned>
    static reg_type info_dispatch(TrueType, TrueType)
    {
        reg_type reg;
        cpuid(0x00, 0x00, reg.v);

        return reg;
    }

    template <unsigned, unsigned>
    static reg_type info_dispatch(TrueType, FalseType)
    {
        reg_type reg;
        cpuid(ext0_, 0x00, reg.v);

        return reg;
    }

    template <unsigned EAX, unsigned ECX, bool Basic>
    static reg_type info_dispatch(FalseType, BoolType<Basic>)
    {
        reg_type reg(info_dispatch<EAX, ECX>(TrueType(), BoolType<Basic>()));

        if (EAX > reg.v[0])
            std::fill(reg.v, reg.v + 4, 0);
        else
            cpuid(EAX, ECX, reg.v);

        return reg;
    }

    template <unsigned Hi, unsigned Lo>
    static unsigned extract_bits(unsigned val)
    {
        return (val << (31U - Hi)) >> (31U - Hi + Lo);
    }

    template <unsigned Bit>
    static bool test_bit(unsigned val)
    {
        return (val & (1U << Bit)) != 0;
    }

    template <typename CharT, typename Traits>
    static void print_equal(std::basic_ostream<CharT, Traits> &os)
    {
        os << std::string(80, '=') << '\n';
    }

    template <typename CharT, typename Traits>
    static void print_dash(std::basic_ostream<CharT, Traits> &os)
    {
        os << std::string(80, '-') << '\n';
    }

    template <typename CharT, typename Traits>
    static void print_cache(std::basic_ostream<CharT, Traits> &os)
    {
        std::vector<cache_param_type> caches;
        unsigned max_ecx = cache_param_num();
        for (unsigned ecx = 0x00; ecx != max_ecx; ++ecx)
            caches.push_back(cache_param(ecx));

        std::stringstream ss;

        const int fix = 12;
        const int width = 36;
        os << std::setw(width) << std::left << "Cache level";
        for (std::size_t i = 0; i != caches.size(); ++i)
            os << std::setw(fix) << caches[i].level();
        os << '\n';

        os << std::setw(width) << std::left << "Cache type";
        for (std::size_t i = 0; i != caches.size(); ++i) {
            switch (caches[i].type()) {
                case CPUIDCacheTypeNull:
                    break;
                case CPUIDCacheTypeData:
                    os << std::setw(fix) << "Data";
                    break;
                case CPUIDCacheTypeInstruction:
                    os << std::setw(fix) << "Instruction";
                    break;
                case CPUIDCacheTypeUnified:
                    os << std::setw(fix) << "Unified";
                    break;
            }
        }
        os << '\n';

        os << std::setw(width) << std::left << "Cache size (byte)";
        for (std::size_t i = 0; i != caches.size(); ++i) {
            unsigned b = caches[i].size();
            ss.str(std::string());
            if (b < 1024) {
                ss << b;
            } else if ((b /= 1024) < 1024) {
                ss << b << "K";
            } else if ((b /= 1024) < 1024) {
                ss << b << "M";
            } else {
                ss << b / 1024 << "G";
            }
            os << std::setw(fix) << ss.str();
        }
        os << '\n';

        os << std::setw(width) << std::left << "Maximum Proc sharing";
        for (std::size_t i = 0; i != caches.size(); ++i)
            os << std::setw(fix) << caches[i].max_proc_sharing();
        os << '\n';

        os << std::setw(width) << std::left << "Maximum Proc physical";
        for (std::size_t i = 0; i != caches.size(); ++i)
            os << std::setw(fix) << caches[i].max_proc_physical();
        os << '\n';

        os << std::setw(width) << std::left << "Coherency line size (byte)";
        for (std::size_t i = 0; i != caches.size(); ++i)
            os << std::setw(fix) << caches[i].line_size();
        os << '\n';

        os << std::setw(width) << std::left << "Physical line partitions";
        for (std::size_t i = 0; i != caches.size(); ++i)
            os << std::setw(fix) << caches[i].partitions();
        os << '\n';

        os << std::setw(width) << std::left << "Ways of associative";
        for (std::size_t i = 0; i != caches.size(); ++i)
            os << std::setw(fix) << caches[i].ways();
        os << '\n';

        os << std::setw(width) << std::left << "Number of sets";
        for (std::size_t i = 0; i != caches.size(); ++i)
            os << std::setw(fix) << caches[i].sets();
        os << '\n';

        os << std::setw(width) << std::left << "Self initializing";
        for (std::size_t i = 0; i != caches.size(); ++i) {
            os << std::setw(fix)
               << (caches[i].self_initializing() ? "Yes" : "No");
        }
        os << '\n';

        os << std::setw(width) << std::left << "Fully associative";
        for (std::size_t i = 0; i != caches.size(); ++i) {
            os << std::setw(fix)
               << (caches[i].fully_associative() ? "Yes" : "No");
        }
        os << '\n';

        os << std::setw(width) << std::left << "Write-back invalidate";
        for (std::size_t i = 0; i != caches.size(); ++i) {
            os << std::setw(fix) << (caches[i].wbinvd() ? "Yes" : "No");
        }
        os << '\n';

        os << std::setw(width) << std::left << "Cache inclusiveness";
        for (std::size_t i = 0; i != caches.size(); ++i) {
            os << std::setw(fix) << (caches[i].inclusiveness() ? "Yes" : "No");
        }
        os << '\n';

        os << std::setw(width) << std::left << "Complex cache indexing";
        for (std::size_t i = 0; i != caches.size(); ++i) {
            os << std::setw(fix)
               << (caches[i].complex_indexing() ? "Yes" : "No");
        }
        os << '\n';
    }

    template <typename CharT, typename Traits>
    static void print_feature(std::basic_ostream<CharT, Traits> &os)
    {
        if (max_eax() >= 0x01) {
            std::vector<std::string> feats;
            feature_str1(feats);
            if (feats.size() != 0) {
                os << "Processor features (EAX = 0x01, ECX = 0x00)\n";
                print_dash(os);
                print_feature(os, feats);
            }
        }
        if (max_eax() >= 0x07) {
            std::vector<std::string> feats;
            feature_str7(feats);
            if (feats.size() != 0) {
                print_equal(os);
                os << "Extended features (EAX = 0x07, ECX = 0x00)\n";
                print_dash(os);
                print_feature(os, feats);
            }
        }
        if (max_eax_ext() >= ext0_ + 1U) {
            std::vector<std::string> feats;
            feature_str_ext1(feats);
            if (feats.size() != 0) {
                print_equal(os);
                os << "Extended features (EAX = 0x80000001, ECX = 0x00)\n";
                print_dash(os);
                print_feature(os, feats);
            }
        }
    }

    template <typename CharT, typename Traits>
    static void print_feature(
        std::basic_ostream<CharT, Traits> &os, std::vector<std::string> &feats)
    {
        std::sort(feats.begin(), feats.end());
        for (std::size_t i = 0; i != feats.size(); ++i) {
            print_feat(os, feats[i], 16);
            if (i % 5 == 4 || i == feats.size() - 1)
                os << '\n';
        }
    }

    template <typename CharT, typename Traits>
    static void print_feat(std::basic_ostream<CharT, Traits> &os,
        std::string &str, std::size_t fix)
    {
        os << str;
        if (str.size() < fix)
            os << std::string(fix - str.size(), ' ');
    }

    // EAX = 0x01, ECX = 0x00
    static void feature_str1(std::vector<std::string> &feats)
    {
        feature_str<CPUIDFeatureSSE3>(feats);         // ECX[00]
        feature_str<CPUIDFeaturePCLMULQDQ>(feats);    // ECX[01]
        feature_str<CPUIDFeatureDTES64>(feats);       // ECX[02]
        feature_str<CPUIDFeatureMONITOR>(feats);      // ECX[03]
        feature_str<CPUIDFeatureDS_CPL>(feats);       // ECX[04]
        feature_str<CPUIDFeatureVMX>(feats);          // ECX[05]
        feature_str<CPUIDFeatureSMX>(feats);          // ECX[06]
        feature_str<CPUIDFeatureEST>(feats);          // ECX[07]
        feature_str<CPUIDFeatureTM2>(feats);          // ECX[08]
        feature_str<CPUIDFeatureSSSE3>(feats);        // ECX[09]
        feature_str<CPUIDFeatureCNXT_ID>(feats);      // ECX[10]
        feature_str<CPUIDFeatureSDBG>(feats);         // ECX[11]
        feature_str<CPUIDFeatureFMA>(feats);          // ECX[12]
        feature_str<CPUIDFeatureCX16>(feats);         // ECX[13]
        feature_str<CPUIDFeatureXTPR>(feats);         // ECX[14]
        feature_str<CPUIDFeaturePDCM>(feats);         // ECX[15]
                                                      // ECX[16]
        feature_str<CPUIDFeaturePCID>(feats);         // ECX[17]
        feature_str<CPUIDFeatureDCA>(feats);          // ECX[18]
        feature_str<CPUIDFeatureSSE4_1>(feats);       // ECX[19]
        feature_str<CPUIDFeatureSSE4_2>(feats);       // ECX[20]
        feature_str<CPUIDFeatureX2APIC>(feats);       // ECX[21]
        feature_str<CPUIDFeatureMOVBE>(feats);        // ECX[22]
        feature_str<CPUIDFeaturePOPCNT>(feats);       // ECX[23]
        feature_str<CPUIDFeatureTSC_DEADLINE>(feats); // ECX[24]
        feature_str<CPUIDFeatureAES>(feats);          // ECX[25]
        feature_str<CPUIDFeatureXSAVE>(feats);        // ECX[26]
        feature_str<CPUIDFeatureOSXSAVE>(feats);      // ECX[27]
        feature_str<CPUIDFeatureAVX>(feats);          // ECX[28]
        feature_str<CPUIDFeatureF16C>(feats);         // ECX[29]
        feature_str<CPUIDFeatureRDRAND>(feats);       // ECX[30]
        feature_str<CPUIDFeatureHYPERVISOR>(feats);   // ECX[31]

        feature_str<CPUIDFeatureFPU>(feats);    // EDX[00]
        feature_str<CPUIDFeatureVME>(feats);    // EDX[01]
        feature_str<CPUIDFeatureDE>(feats);     // EDX[02]
        feature_str<CPUIDFeaturePSE>(feats);    // EDX[03]
        feature_str<CPUIDFeatureTSC>(feats);    // EDX[04]
        feature_str<CPUIDFeatureMSR>(feats);    // EDX[05]
        feature_str<CPUIDFeaturePAE>(feats);    // EDX[06]
        feature_str<CPUIDFeatureMCE>(feats);    // EDX[07]
        feature_str<CPUIDFeatureCX8>(feats);    // EDX[08]
        feature_str<CPUIDFeatureAPIC>(feats);   // EDX[09]
                                                // EDX[10]
        feature_str<CPUIDFeatureSEP>(feats);    // EDX[11]
        feature_str<CPUIDFeatureMTRR>(feats);   // EDX[12]
        feature_str<CPUIDFeaturePGE>(feats);    // EDX[13]
        feature_str<CPUIDFeatureMCA>(feats);    // EDX[14]
        feature_str<CPUIDFeatureCMOV>(feats);   // EDX[15]
        feature_str<CPUIDFeaturePAT>(feats);    // EDX[16]
        feature_str<CPUIDFeaturePSE_36>(feats); // EDX[17]
        feature_str<CPUIDFeaturePSN>(feats);    // EDX[18]
        feature_str<CPUIDFeatureCLFSH>(feats);  // EDX[19]
                                                // EDX[20]
        feature_str<CPUIDFeatureDS>(feats);     // EDX[21]
        feature_str<CPUIDFeatureACPI>(feats);   // EDX[22]
        feature_str<CPUIDFeatureMMX>(feats);    // EDX[23]
        feature_str<CPUIDFeatureFXSR>(feats);   // EDX[24]
        feature_str<CPUIDFeatureSSE>(feats);    // EDX[25]
        feature_str<CPUIDFeatureSSE2>(feats);   // EDX[26]
        feature_str<CPUIDFeatureSS>(feats);     // EDX[27]
        feature_str<CPUIDFeatureHTT>(feats);    // EDX[28]
        feature_str<CPUIDFeatureTM>(feats);     // EDX[29]
        feature_str<CPUIDFeatureIA64>(feats);   // EDX[30]
        feature_str<CPUIDFeaturePBE>(feats);    // EDX[31]
    }

    // EAX = 0x07, ECX = 0x00
    static void feature_str7(std::vector<std::string> &feats)
    {
        feature_str<CPUIDFeatureFSGSBASE>(feats);        // EBX[00]
        feature_str<CPUIDFeatureIA32_TSC_ADJUST>(feats); // EBX[01]
        feature_str<CPUIDFeatureSGX>(feats);             // EBX[02]
        feature_str<CPUIDFeatureBMI1>(feats);            // EBX[03]
        feature_str<CPUIDFeatureHLE>(feats);             // EBX[04]
        feature_str<CPUIDFeatureAVX2>(feats);            // EBX[05]
                                                         // EBX[06]
        feature_str<CPUIDFeatureSMEP>(feats);            // EBX[07]
        feature_str<CPUIDFeatureBMI2>(feats);            // EBX[08]
        feature_str<CPUIDFeatureERMS>(feats);            // EBX[09]
        feature_str<CPUIDFeatureINVPCID>(feats);         // EBX[10]
        feature_str<CPUIDFeatureRTM>(feats);             // EBX[11]
        feature_str<CPUIDFeaturePQM>(feats);             // EBX[12]
        feature_str<CPUIDFeatureFPU_CS_DS>(feats);       // EBX[13]
        feature_str<CPUIDFeatureMPX>(feats);             // EBX[14]
        feature_str<CPUIDFeaturePQE>(feats);             // EBX[15]
        feature_str<CPUIDFeatureAVX512F>(feats);         // EBX[16]
        feature_str<CPUIDFeatureAVX512DQ>(feats);        // EBX[17]
        feature_str<CPUIDFeatureRDSEED>(feats);          // EBX[18]
        feature_str<CPUIDFeatureADX>(feats);             // EBX[19]
        feature_str<CPUIDFeatureSMAP>(feats);            // EBX[20]
        feature_str<CPUIDFeatureAVX512IFMA52>(feats);    // EBX[21]
        feature_str<CPUIDFeaturePCOMMIT>(feats);         // EBX[22]
        feature_str<CPUIDFeatureCLFLUSHOPT>(feats);      // EBX[23]
        feature_str<CPUIDFeatureCLWB>(feats);            // EBX[24]
        feature_str<CPUIDFeatureINTEL_TRACE>(feats);     // EBX[25]
        feature_str<CPUIDFeatureAVX512PF>(feats);        // EBX[26]
        feature_str<CPUIDFeatureAVX512ER>(feats);        // EBX[27]
        feature_str<CPUIDFeatureAVX512CD>(feats);        // EBX[28]
        feature_str<CPUIDFeatureSHA>(feats);             // EBX[29]
        feature_str<CPUIDFeatureAVX512BW>(feats);        // EBX[30]
        feature_str<CPUIDFeatureAVX512VL>(feats);        // EBX[31]

        feature_str<CPUIDFeaturePREFETCHWT1>(feats); // ECX[00]
        feature_str<CPUIDFeatureAVX512VBMI>(feats);  // ECX[01]
        feature_str<CPUIDFeaturePKU>(feats);         // ECX[03]
        feature_str<CPUIDFeatureOSPKE>(feats);       // ECX[04]
    }

    // EAX = 0x80000001, ECX = 0x00
    static void feature_str_ext1(std::vector<std::string> &feats)
    {
        feature_str<CPUIDFeatureExtLAHF_LM>(feats);       // ECX[00]
        feature_str<CPUIDFeatureExtCMP_LEGACY>(feats);    // ECX[01]
        feature_str<CPUIDFeatureExtSVM>(feats);           // ECX[02]
        feature_str<CPUIDFeatureExtEXTAPIC>(feats);       // ECX[03]
        feature_str<CPUIDFeatureExtCR8_LEGACY>(feats);    // ECX[04]
        feature_str<CPUIDFeatureExtABM>(feats);           // ECX[05]
        feature_str<CPUIDFeatureExtSSE4A>(feats);         // ECX[06]
        feature_str<CPUIDFeatureExtMISALIGNSSE>(feats);   // ECX[07]
        feature_str<CPUIDFeatureExt3DNOWPREFETCH>(feats); // ECX[08]
        feature_str<CPUIDFeatureExtOSVW>(feats);          // ECX[09]
        feature_str<CPUIDFeatureExtIBS>(feats);           // ECX[10]
        feature_str<CPUIDFeatureExtXOP>(feats);           // ECX[11]
        feature_str<CPUIDFeatureExtSKINIT>(feats);        // ECX[12]
        feature_str<CPUIDFeatureExtWDT>(feats);           // ECX[13]
                                                          // ECX[14]
        feature_str<CPUIDFeatureExtLWP>(feats);           // ECX[15]
        feature_str<CPUIDFeatureExtFMA4>(feats);          // ECX[16]
        feature_str<CPUIDFeatureExtTCE>(feats);           // ECX[17]
                                                          // ECX[18]
        feature_str<CPUIDFeatureExtNODEID_MSR>(feats);    // ECX[19]
                                                          // ECX[20]
        feature_str<CPUIDFeatureExtTBM>(feats);           // ECX[21]
        feature_str<CPUIDFeatureExtTOPOEXT>(feats);       // ECX[22]
        feature_str<CPUIDFeatureExtPERFCTR_CORE>(feats);  // ECX[22]
        feature_str<CPUIDFeatureExtPERFCTR_NB>(feats);    // ECX[24]
                                                          // ECX[25]
        feature_str<CPUIDFeatureExtDBX>(feats);           // ECX[26]
        feature_str<CPUIDFeatureExtPERFTSC>(feats);       // ECX[27]
        feature_str<CPUIDFeatureExtPCX_L2I>(feats);       // ECX[28]
                                                          // ECX[29]
                                                          // ECX[30]
                                                          // ECX[31]

        feature_str<CPUIDFeatureExtFPU>(feats);      // EDX[00]
        feature_str<CPUIDFeatureExtVME>(feats);      // EDX[01]
        feature_str<CPUIDFeatureExtDE>(feats);       // EDX[02]
        feature_str<CPUIDFeatureExtPSE>(feats);      // EDX[03]
        feature_str<CPUIDFeatureExtTSC>(feats);      // EDX[04]
        feature_str<CPUIDFeatureExtMSR>(feats);      // EDX[05]
        feature_str<CPUIDFeatureExtPAE>(feats);      // EDX[06]
        feature_str<CPUIDFeatureExtMCE>(feats);      // EDX[07]
        feature_str<CPUIDFeatureExtCX8>(feats);      // EDX[08]
        feature_str<CPUIDFeatureExtAPIC>(feats);     // EDX[09]
                                                     // EDX[10]
        feature_str<CPUIDFeatureExtSYSCALL>(feats);  // EDX[11]
        feature_str<CPUIDFeatureExtMTRR>(feats);     // EDX[12]
        feature_str<CPUIDFeatureExtPGE>(feats);      // EDX[13]
        feature_str<CPUIDFeatureExtMCA>(feats);      // EDX[14]
        feature_str<CPUIDFeatureExtCMOV>(feats);     // EDX[15]
        feature_str<CPUIDFeatureExtPAT>(feats);      // EDX[16]
        feature_str<CPUIDFeatureExtPSE36>(feats);    // EDX[17]
                                                     // EDX[18]
        feature_str<CPUIDFeatureExtMP>(feats);       // EDX[19]
        feature_str<CPUIDFeatureExtNX>(feats);       // EDX[20]
                                                     // EDX[21]
        feature_str<CPUIDFeatureExtMMX>(feats);      // EDX[22]
        feature_str<CPUIDFeatureExtMMXEXT>(feats);   // EDX[23]
        feature_str<CPUIDFeatureExtFXSR>(feats);     // EDX[24]
        feature_str<CPUIDFeatureExtFXSR_OPT>(feats); // EDX[25]
        feature_str<CPUIDFeatureExtGBPAGES>(feats);  // EDX[26]
        feature_str<CPUIDFeatureExtRDTSCP>(feats);   // EDX[27]
                                                     // EDX[28]
        feature_str<CPUIDFeatureExtLM>(feats);       // EDX[29]
        feature_str<CPUIDFeatureExt3DNOWEXT>(feats); // EDX[30]
        feature_str<CPUIDFeatureExt3DNOW>(feats);    // EDX[31]
    }

    template <CPUIDFeature Feat>
    static void feature_str(std::vector<std::string> &feats)
    {
        if (has_feature<Feat>())
            feats.push_back(CPUIDFeatureInfo<Feat>::str());
    }
}; // class CPUID

#endif // CPUID_INFO_HPP
