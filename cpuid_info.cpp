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

struct Register {
    unsigned eax;
    unsigned ebx;
    unsigned ecx;
    unsigned edx;
};

inline Register cpuid(unsigned eax, unsigned ecx)
{
    Register reg;

#ifdef _MSC
    __cpuidex(reinterpret_cast<int *>(&reg), static_cast<int>(eax),
        static_cast<int>(ecx));
#else
    unsigned ebx = 0;
    unsigned edx = 0;
    __asm__ volatile("cpuid\n"
                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                     : "a"(eax), "c"(ecx));
    reg.eax = eax;
    reg.ebx = ebx;
    reg.ecx = ecx;
    reg.edx = edx;
#endif

    return reg;
}

inline unsigned extract_bits(unsigned r, int hi, int lo)
{
    return (r << (31 - hi)) >> (31 - hi + lo);
}

inline unsigned extract_byte(unsigned r, int b)
{
    return (r & (0xFFU << (b * 8))) >> (b * 8);
}

inline bool test_bit(unsigned r, int b) { return r & (0x01U << b); }

class CacheParam
{
    public:
    CacheParam(const Register &reg)
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
        switch (extract_bits(reg.eax, 4, 0)) {
            case 1:
                type_ = "Data";
                break;
            case 2:
                type_ = "Instruction";
                break;
            case 3:
                type_ = "Unified";
                break;
            default:
                type_ = "Null";
                return;
        }

        level_ = extract_bits(reg.eax, 7, 5);
        self_initializing_ = test_bit(reg.eax, 8);
        fully_associative_ = test_bit(reg.eax, 9);
        max_proc_sharing_ = extract_bits(reg.eax, 25, 14) + 1;
        max_proc_physical_ = extract_bits(reg.eax, 31, 26) + 1;

        line_size_ = extract_bits(reg.ebx, 11, 0) + 1;
        partitions_ = extract_bits(reg.ebx, 21, 12) + 1;
        ways_ = extract_bits(reg.ebx, 31, 22) + 1;
        sets_ = reg.ecx + 1;
        size_ = line_size_ * partitions_ * ways_ * sets_;

        wbinvd_ = test_bit(reg.edx, 0);
        inclusiveness_ = test_bit(reg.edx, 1);
        complex_indexing_ = test_bit(reg.edx, 2);
    }

    const std::string &type() const { return type_; }
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
    std::string type_;
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
}; // class CacheParam

inline void print_equal() { std::cout << std::string(100, '=') << std::endl; }

inline void print_dash() { std::cout << std::string(100, '-') << std::endl; }

inline std::string hexnum(unsigned x)
{
    std::stringstream ss;
    ss << std::hex << std::uppercase;
    ss << (x < 0x10 ? "0x0" : "0x") << x;

    return ss.str();
}

inline void print_vendor()
{
    Register reg(cpuid(0x00, 0x00));
    char str[sizeof(unsigned) * 3 + 1] = {'\0'};
    std::memcpy(str + sizeof(unsigned) * 0, &reg.ebx, sizeof(unsigned));
    std::memcpy(str + sizeof(unsigned) * 1, &reg.edx, sizeof(unsigned));
    std::memcpy(str + sizeof(unsigned) * 2, &reg.ecx, sizeof(unsigned));

    std::cout << std::setw(10) << std::left << "Vendor" << str << std::endl;
}

inline void print_brand()
{
    if (cpuid(0x80000000, 0x00).eax < 0x80000004)
        return;

    Register reg2(cpuid(0x80000002, 0));
    Register reg3(cpuid(0x80000003, 0));
    Register reg4(cpuid(0x80000004, 0));
    const std::size_t reg_size = sizeof(unsigned) * 4;
    char str[reg_size * 3] = {'\0'};
    std::memcpy(str + reg_size * 0, &reg2, reg_size);
    std::memcpy(str + reg_size * 1, &reg3, reg_size);
    std::memcpy(str + reg_size * 2, &reg4, reg_size);

    std::cout << std::setw(10) << std::left << "Brand" << str << std::endl;
}

inline void test_feature(unsigned r, unsigned b, const std::string &feat)
{
    if (test_bit(r, b))
        if (feat != std::string("Reserved"))
            std::cout << feat << std::endl;
}

inline void test_feature(std::vector<std::string> &feats, unsigned r,
    unsigned b, const std::string &feat)
{
    if (test_bit(r, b))
        if (feat != std::string("Reserved"))
            feats.push_back(feat);
}

inline void print_leave(unsigned eax, unsigned ecx, const std::string &info)
{
    print_equal();
    std::cout << info;
    std::cout << " (EAX = " << hexnum(eax);
    std::cout << ", ECX = " << hexnum(ecx);
    std::cout << ")" << std::endl;
    print_dash();
}

inline void print_feature(std::vector<std::string> &feats)
{
    std::sort(feats.begin(), feats.end());
    for (std::size_t i = 0; i != feats.size(); ++i) {
        std::cout << std::setw(16) << std::left << feats[i];
        if (i % 6 == 5 || i + 1 == feats.size())
            std::cout << std::endl;
    }
    print_dash();
}

template <unsigned>
inline void print_eax();

template <>
inline void print_eax<0x01>()
{
    if (cpuid(0x00, 0x00).eax < 0x01)
        return;

    Register reg(cpuid(0x01, 0x00));
    print_leave(0x01, 0x00, "Feature flags");
    std::vector<std::string> feats;

    test_feature(feats, reg.ecx, 0, "SSE3");
    test_feature(feats, reg.ecx, 1, "PCLMULQDQ");
    test_feature(feats, reg.ecx, 2, "DTES64");
    test_feature(feats, reg.ecx, 3, "MONITOR");
    test_feature(feats, reg.ecx, 4, "DS-CPL");
    test_feature(feats, reg.ecx, 5, "VMX");
    test_feature(feats, reg.ecx, 6, "SMX");
    test_feature(feats, reg.ecx, 7, "EIST");
    test_feature(feats, reg.ecx, 8, "TM2");
    test_feature(feats, reg.ecx, 9, "SSSE3");
    test_feature(feats, reg.ecx, 10, "CNXT-ID");
    test_feature(feats, reg.ecx, 11, "SDBG");
    test_feature(feats, reg.ecx, 12, "FMA");
    test_feature(feats, reg.ecx, 13, "CMPXCHG16B");
    test_feature(feats, reg.ecx, 14, "xTPR");
    test_feature(feats, reg.ecx, 15, "PDCM");
    test_feature(feats, reg.ecx, 16, "Reserved");
    test_feature(feats, reg.ecx, 17, "PCID");
    test_feature(feats, reg.ecx, 18, "DCA");
    test_feature(feats, reg.ecx, 19, "SSE4.1");
    test_feature(feats, reg.ecx, 20, "SSE4.2");
    test_feature(feats, reg.ecx, 21, "x2APIC");
    test_feature(feats, reg.ecx, 22, "MOVBE");
    test_feature(feats, reg.ecx, 23, "POPCNT");
    test_feature(feats, reg.ecx, 24, "TSC-Deadline");
    test_feature(feats, reg.ecx, 25, "AESNI");
    test_feature(feats, reg.ecx, 26, "XSAVE");
    test_feature(feats, reg.ecx, 27, "OSXSAVE");
    test_feature(feats, reg.ecx, 28, "AVX");
    test_feature(feats, reg.ecx, 29, "F16C");
    test_feature(feats, reg.ecx, 30, "RDRAND");
    test_feature(feats, reg.ecx, 31, "Hypervisor");

    test_feature(feats, reg.edx, 0, "FPU");
    test_feature(feats, reg.edx, 1, "VME");
    test_feature(feats, reg.edx, 2, "DE");
    test_feature(feats, reg.edx, 3, "PSE");
    test_feature(feats, reg.edx, 4, "TSC");
    test_feature(feats, reg.edx, 5, "MSR");
    test_feature(feats, reg.edx, 6, "PAE");
    test_feature(feats, reg.edx, 7, "MCE");
    test_feature(feats, reg.edx, 8, "CX8");
    test_feature(feats, reg.edx, 9, "APIC");
    test_feature(feats, reg.edx, 10, "Reserved");
    test_feature(feats, reg.edx, 11, "SEP");
    test_feature(feats, reg.edx, 12, "MTRR");
    test_feature(feats, reg.edx, 13, "PGE");
    test_feature(feats, reg.edx, 14, "MCA");
    test_feature(feats, reg.edx, 15, "CMOV");
    test_feature(feats, reg.edx, 16, "PAT");
    test_feature(feats, reg.edx, 17, "PSE-36");
    test_feature(feats, reg.edx, 18, "PSN");
    test_feature(feats, reg.edx, 19, "CLFSH");
    test_feature(feats, reg.edx, 20, "Reserved");
    test_feature(feats, reg.edx, 21, "DS");
    test_feature(feats, reg.edx, 22, "ACPI");
    test_feature(feats, reg.edx, 23, "MMX");
    test_feature(feats, reg.edx, 24, "FXSR");
    test_feature(feats, reg.edx, 25, "SSE");
    test_feature(feats, reg.edx, 26, "SSE2");
    test_feature(feats, reg.edx, 27, "SS");
    test_feature(feats, reg.edx, 28, "HTT");
    test_feature(feats, reg.edx, 29, "TM");
    test_feature(feats, reg.edx, 30, "IA64");
    test_feature(feats, reg.edx, 31, "PBE");

    print_feature(feats);
}

template <>
inline void print_eax<0x02>()
{
    if (cpuid(0x00, 0x00).eax < 0x02)
        return;

    Register reg(cpuid(0x02, 0x00));
    print_leave(0x02, 0x00, "Cache and TLB information");
    std::vector<unsigned> feats;

    if (!test_bit(reg.eax, 31)) {
        feats.push_back(extract_byte(reg.eax, 1));
        feats.push_back(extract_byte(reg.eax, 2));
        feats.push_back(extract_byte(reg.eax, 3));
    }

    if (!test_bit(reg.ebx, 31)) {
        feats.push_back(extract_byte(reg.ebx, 0));
        feats.push_back(extract_byte(reg.ebx, 1));
        feats.push_back(extract_byte(reg.ebx, 2));
        feats.push_back(extract_byte(reg.ebx, 3));
    }

    if (!test_bit(reg.ecx, 31)) {
        feats.push_back(extract_byte(reg.ecx, 0));
        feats.push_back(extract_byte(reg.ecx, 1));
        feats.push_back(extract_byte(reg.ecx, 2));
        feats.push_back(extract_byte(reg.ecx, 3));
    }

    if (!test_bit(reg.edx, 31)) {
        feats.push_back(extract_byte(reg.edx, 0));
        feats.push_back(extract_byte(reg.edx, 1));
        feats.push_back(extract_byte(reg.edx, 2));
        feats.push_back(extract_byte(reg.edx, 3));
    }

    std::sort(feats.begin(), feats.end());
    for (std::size_t i = 0; i != feats.size(); ++i)
        if (feats[i] != 0)
            std::cout << hexnum(feats[i]) << ' ';
    std::cout << std::endl;
    print_dash();
}

template <>
inline void print_eax<0x04>()
{
    if (cpuid(0x00, 0x00).eax < 0x04)
        return;

    print_leave(0x04, 0x00, "Deterministic Cache Parameters");
    unsigned ecx = 0x00;
    std::vector<CacheParam> caches;
    while (true) {
        Register reg(cpuid(0x04, ecx));
        if (extract_bits(reg.eax, 4, 0) == 0)
            break;
        caches.push_back(CacheParam(reg));
        ++ecx;
    }

    std::stringstream ss;

    const int fix = 12;
    const int width = 40;
    std::cout << std::setw(width) << std::left << "Cache level";
    for (std::size_t i = 0; i != caches.size(); ++i)
        std::cout << std::setw(fix) << caches[i].level();
    std::cout << std::endl;

    std::cout << std::setw(width) << std::left << "Cache type";
    for (std::size_t i = 0; i != caches.size(); ++i)
        std::cout << std::setw(fix) << caches[i].type();
    std::cout << std::endl;

    std::cout << std::setw(width) << std::left << "Cache size (byte)";
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
        std::cout << std::setw(fix) << ss.str();
    }
    std::cout << std::endl;

    std::cout << std::setw(width) << std::left << "Maximum Proc sharing";
    for (std::size_t i = 0; i != caches.size(); ++i)
        std::cout << std::setw(fix) << caches[i].max_proc_sharing();
    std::cout << std::endl;

    std::cout << std::setw(width) << std::left << "Maximum Proc physical";
    for (std::size_t i = 0; i != caches.size(); ++i)
        std::cout << std::setw(fix) << caches[i].max_proc_physical();
    std::cout << std::endl;

    std::cout << std::setw(width) << std::left << "Coherency line size (byte)";
    for (std::size_t i = 0; i != caches.size(); ++i)
        std::cout << std::setw(fix) << caches[i].line_size();
    std::cout << std::endl;

    std::cout << std::setw(width) << std::left << "Physical line partitions";
    for (std::size_t i = 0; i != caches.size(); ++i)
        std::cout << std::setw(fix) << caches[i].partitions();
    std::cout << std::endl;

    std::cout << std::setw(width) << std::left << "Ways of associative";
    for (std::size_t i = 0; i != caches.size(); ++i)
        std::cout << std::setw(fix) << caches[i].ways();
    std::cout << std::endl;

    std::cout << std::setw(width) << std::left << "Number of sets";
    for (std::size_t i = 0; i != caches.size(); ++i)
        std::cout << std::setw(fix) << caches[i].sets();
    std::cout << std::endl;

    std::cout << std::setw(width) << std::left << "Self initializing";
    for (std::size_t i = 0; i != caches.size(); ++i) {
        std::cout << std::setw(fix)
                  << (caches[i].self_initializing() ? "Yes" : "No");
    }
    std::cout << std::endl;

    std::cout << std::setw(width) << std::left << "Fully associative";
    for (std::size_t i = 0; i != caches.size(); ++i) {
        std::cout << std::setw(fix)
                  << (caches[i].fully_associative() ? "Yes" : "No");
    }
    std::cout << std::endl;

    std::cout << std::setw(width) << std::left << "Write-back invalidate";
    for (std::size_t i = 0; i != caches.size(); ++i) {
        std::cout << std::setw(fix) << (caches[i].wbinvd() ? "Yes" : "No");
    }
    std::cout << std::endl;

    std::cout << std::setw(width) << std::left << "Cache inclusiveness";
    for (std::size_t i = 0; i != caches.size(); ++i) {
        std::cout << std::setw(fix)
                  << (caches[i].inclusiveness() ? "Yes" : "No");
    }
    std::cout << std::endl;

    std::cout << std::setw(width) << std::left << "Complex cache indexing";
    for (std::size_t i = 0; i != caches.size(); ++i) {
        std::cout << std::setw(fix)
                  << (caches[i].complex_indexing() ? "Yes" : "No");
    }
    std::cout << std::endl;

    print_dash();
}

template <>
inline void print_eax<0x06>()
{
    if (cpuid(0x00, 0x00).eax < 0x06)
        return;

    Register reg(cpuid(0x06, 0x00));
    print_leave(0x06, 0x00, "Thermal and Power Management");

    test_feature(reg.eax, 0, "Digital temperature sensor");
    test_feature(reg.eax, 1, "Intel Turbo Boost Technology");
    test_feature(reg.eax, 2, "ARAT. APIC-Timer-always-running feature");
    test_feature(reg.eax, 3, "Reserved");
    test_feature(reg.eax, 4, "PLN. Power limit notification controls");
    test_feature(reg.eax, 5, "ECMD. Clock modulation duty cycle extension");
    test_feature(reg.eax, 6, "PTM. Package thermal management");
    test_feature(reg.eax, 7, "HWP base registers");
    test_feature(reg.eax, 8, "HWP_Notification ");
    test_feature(reg.eax, 9, "HWP_Activity_Window");
    test_feature(reg.eax, 10, "HWP_Energy_Performance_Preference");
    test_feature(reg.eax, 11, "HWP_Package_Level_Request");
    test_feature(reg.eax, 12, "Reserved");
    test_feature(reg.eax, 13, "HDC base registers");

    std::cout << "Number of Interrupt Thresholds in Digitial Thermal Sensor: "
              << (reg.ebx & 7) << std::endl;

    test_feature(reg.ecx, 0, "Hardware Coordination Feedback Capability");
    test_feature(reg.ecx, 1, "Reserved");
    test_feature(reg.ecx, 2, "Reserved");
    test_feature(reg.ecx, 3, "Performance-energy bias preference");

    print_dash();
}

template <>
inline void print_eax<0x07>()
{
    if (cpuid(0x00, 0x00).eax < 0x07)
        return;

    Register reg(cpuid(0x07, 0x00));
    print_leave(0x07, 0x00, "Extended feature flags");
    std::vector<std::string> feats;

    test_feature(feats, reg.ebx, 0, "FSGSBASE");
    test_feature(feats, reg.ebx, 1, "IA32_TSC_ADJUST");
    test_feature(feats, reg.ebx, 2, "SGX");
    test_feature(feats, reg.ebx, 3, "BMI1");
    test_feature(feats, reg.ebx, 4, "HLE");
    test_feature(feats, reg.ebx, 5, "AVX2");
    test_feature(feats, reg.ebx, 6, "Reserved");
    test_feature(feats, reg.ebx, 7, "SMEP");
    test_feature(feats, reg.ebx, 8, "BMI2");
    test_feature(feats, reg.ebx, 9, "ERMS");
    test_feature(feats, reg.ebx, 10, "INVPCID");
    test_feature(feats, reg.ebx, 11, "RTM");
    test_feature(feats, reg.ebx, 12, "PQM");
    test_feature(feats, reg.ebx, 13, "FPU_CS_DS");
    test_feature(feats, reg.ebx, 14, "MPX");
    test_feature(feats, reg.ebx, 15, "PQE");
    test_feature(feats, reg.ebx, 16, "AVX512F");
    test_feature(feats, reg.ebx, 17, "AVX512DQ");
    test_feature(feats, reg.ebx, 18, "RDSEED");
    test_feature(feats, reg.ebx, 19, "ADX");
    test_feature(feats, reg.ebx, 20, "SMAP");
    test_feature(feats, reg.ebx, 21, "AVX512IFMA52");
    test_feature(feats, reg.ebx, 22, "PCOMMIT");
    test_feature(feats, reg.ebx, 23, "CLFLUSHOPT");
    test_feature(feats, reg.ebx, 24, "CLWB");
    test_feature(feats, reg.ebx, 25, "INTEL_TRACE");
    test_feature(feats, reg.ebx, 26, "AVX512PF");
    test_feature(feats, reg.ebx, 27, "AVX512ER");
    test_feature(feats, reg.ebx, 28, "AVX512CD");
    test_feature(feats, reg.ebx, 29, "SHA");
    test_feature(feats, reg.ebx, 30, "AVX512BW");
    test_feature(feats, reg.ebx, 31, "AVX512VL");

    test_feature(feats, reg.ecx, 0, "PREFETCHHWT1");
    test_feature(feats, reg.ecx, 1, "AVX512VBMI");
    test_feature(feats, reg.ecx, 2, "Reserved");
    test_feature(feats, reg.ecx, 3, "PKU");
    test_feature(feats, reg.ecx, 4, "OSPKE");
    test_feature(feats, reg.ecx, 5, "Reserved");
    test_feature(feats, reg.ecx, 6, "Reserved");
    test_feature(feats, reg.ecx, 7, "Reserved");
    test_feature(feats, reg.ecx, 8, "Reserved");
    test_feature(feats, reg.ecx, 9, "Reserved");
    test_feature(feats, reg.ecx, 10, "Reserved");
    test_feature(feats, reg.ecx, 11, "Reserved");
    test_feature(feats, reg.ecx, 12, "Reserved");
    test_feature(feats, reg.ecx, 13, "Reserved");
    test_feature(feats, reg.ecx, 14, "Reserved");
    test_feature(feats, reg.ecx, 15, "Reserved");
    test_feature(feats, reg.ecx, 16, "Reserved");
    test_feature(feats, reg.ecx, 17, "Reserved");
    test_feature(feats, reg.ecx, 18, "Reserved");
    test_feature(feats, reg.ecx, 19, "Reserved");
    test_feature(feats, reg.ecx, 20, "Reserved");
    test_feature(feats, reg.ecx, 21, "Reserved");
    test_feature(feats, reg.ecx, 22, "Reserved");
    test_feature(feats, reg.ecx, 23, "Reserved");
    test_feature(feats, reg.ecx, 24, "Reserved");
    test_feature(feats, reg.ecx, 25, "Reserved");
    test_feature(feats, reg.ecx, 26, "Reserved");
    test_feature(feats, reg.ecx, 27, "Reserved");
    test_feature(feats, reg.ecx, 28, "Reserved");
    test_feature(feats, reg.ecx, 29, "Reserved");
    test_feature(feats, reg.ecx, 30, "Reserved");
    test_feature(feats, reg.ecx, 31, "Reserved");

    print_feature(feats);
}

template <>
inline void print_eax<0x0B>()
{
    if (cpuid(0x00, 0x00).eax < 0x0B)
        return;

    print_leave(0x0B, 0x00, "Extended Topology Enumeration");
    Register reg;
    unsigned ecx = 0x00;
    while (true) {
        Register reg(cpuid(0x0B, ecx));
        unsigned np = extract_bits(reg.ebx, 15, 0);
        unsigned type = extract_bits(reg.ecx, 16, 8);
        if (type == 0)
            break;
        std::cout << "Number of logical processors at level ";
        switch (type) {
            case 1:
                std::cout << "SMT:  ";
                break;
            case 2:
                std::cout << "Core: ";
                break;
            default:
                break;
        }
        std::cout << np << std::endl;
        ++ecx;
    }
}

template <>
inline void print_eax<0x16>()
{
    if (cpuid(0x00, 0x00).eax < 0x16)
        return;

    Register reg(cpuid(0x16, 0x00));
    print_leave(0x16, 0x00, "Processor Frequency Information");

    std::cout << std::setw(20) << std::left << "Processor Base Frequence:";
    std::cout << std::setw(10) << std::right << (reg.eax & 0xFFFF) << " MHz";
    std::cout << std::endl;
    std::cout << std::setw(20) << std::left << "Maximum Frequence:";
    std::cout << std::setw(10) << std::right << (reg.ebx & 0xFFFF) << " MHz";
    std::cout << std::endl;
    std::cout << std::setw(20) << std::left << "Bus (Reference) frequence:";
    std::cout << std::setw(10) << std::right << (reg.ecx & 0xFFFF) << " MHz";
    std::cout << std::endl;

    print_dash();
}

template <>
inline void print_eax<0x80000001>()
{
    if (cpuid(0x80000000, 0x00).eax < 0x80000001)
        return;

    Register reg(cpuid(0x80000001, 0x00));
    print_leave(
        0x80000001, 0x00, "Extended Processor Signature and Feature Bits");
    std::vector<std::string> feats;

    test_feature(feats, reg.ecx, 0, "LAHF_LM");
    test_feature(feats, reg.ecx, 1, "CMP_LEGACY");
    test_feature(feats, reg.ecx, 2, "SVM");
    test_feature(feats, reg.ecx, 3, "EXTAPIC");
    test_feature(feats, reg.ecx, 4, "CR8_LEGACY");
    test_feature(feats, reg.ecx, 5, "ABM");
    test_feature(feats, reg.ecx, 6, "SSE4A");
    test_feature(feats, reg.ecx, 7, "MISALIGNSSE");
    test_feature(feats, reg.ecx, 8, "3DNOWPREFETCH");
    test_feature(feats, reg.ecx, 9, "OSVW");
    test_feature(feats, reg.ecx, 10, "IBS");
    test_feature(feats, reg.ecx, 11, "XOP");
    test_feature(feats, reg.ecx, 12, "SKINIT");
    test_feature(feats, reg.ecx, 13, "WDT");
    test_feature(feats, reg.ecx, 14, "Reserved");
    test_feature(feats, reg.ecx, 15, "LWP");
    test_feature(feats, reg.ecx, 16, "FMA4");
    test_feature(feats, reg.ecx, 17, "TCE");
    test_feature(feats, reg.ecx, 18, "Reserved");
    test_feature(feats, reg.ecx, 19, "NODEID_MSR");
    test_feature(feats, reg.ecx, 20, "Reserved");
    test_feature(feats, reg.ecx, 21, "TBM");
    test_feature(feats, reg.ecx, 22, "TOPOEXT");
    test_feature(feats, reg.ecx, 23, "PERFCTR_CORE");
    test_feature(feats, reg.ecx, 24, "PERFCTR_NB");
    test_feature(feats, reg.ecx, 25, "Reserved");
    test_feature(feats, reg.ecx, 26, "DBX");
    test_feature(feats, reg.ecx, 27, "PERFTSC");
    test_feature(feats, reg.ecx, 28, "PCX_L2I");
    test_feature(feats, reg.ecx, 29, "Reserved");
    test_feature(feats, reg.ecx, 30, "Reserved");
    test_feature(feats, reg.ecx, 31, "Reserved");

    test_feature(feats, reg.edx, 0, "FPU");
    test_feature(feats, reg.edx, 1, "VME");
    test_feature(feats, reg.edx, 2, "DE");
    test_feature(feats, reg.edx, 3, "PSE");
    test_feature(feats, reg.edx, 4, "TSC");
    test_feature(feats, reg.edx, 5, "MSR");
    test_feature(feats, reg.edx, 6, "PAE");
    test_feature(feats, reg.edx, 7, "MCE");
    test_feature(feats, reg.edx, 8, "CX8");
    test_feature(feats, reg.edx, 9, "APIC");
    test_feature(feats, reg.edx, 10, "Reserved");
    test_feature(feats, reg.edx, 11, "SYSCALL");
    test_feature(feats, reg.edx, 12, "MTRR");
    test_feature(feats, reg.edx, 13, "PGE");
    test_feature(feats, reg.edx, 14, "MCA");
    test_feature(feats, reg.edx, 15, "CMOV");
    test_feature(feats, reg.edx, 16, "PAT");
    test_feature(feats, reg.edx, 17, "PSE36");
    test_feature(feats, reg.edx, 18, "Reserved");
    test_feature(feats, reg.edx, 19, "MP");
    test_feature(feats, reg.edx, 20, "NX");
    test_feature(feats, reg.edx, 21, "Reserved");
    test_feature(feats, reg.edx, 22, "MMX");
    test_feature(feats, reg.edx, 23, "MMXEXT");
    test_feature(feats, reg.edx, 24, "FXSR");
    test_feature(feats, reg.edx, 25, "FXSR_OPT");
    test_feature(feats, reg.edx, 26, "GBPAGES");
    test_feature(feats, reg.edx, 27, "RDTSCP");
    test_feature(feats, reg.edx, 28, "Reserved");
    test_feature(feats, reg.edx, 29, "LM");
    test_feature(feats, reg.edx, 30, "3DNOWEXT");
    test_feature(feats, reg.edx, 31, "3DNOW");

    print_feature(feats);
}

int main()
{
    print_equal();
    print_vendor();
    print_brand();
    print_dash();

    print_eax<0x01>();
    print_eax<0x02>();
    print_eax<0x04>();
    print_eax<0x06>();
    print_eax<0x07>();
    print_eax<0x0B>();
    print_eax<0x16>();
    print_eax<0x80000001>();

    return 0;
}
