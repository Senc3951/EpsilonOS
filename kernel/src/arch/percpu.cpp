#include <arch/percpu.hpp>
#include <arch/msr.hpp>

namespace kernel
{
    static PerCPU cpus[MAX_CPU];
    
    PerCPU *current_cpu()
    {
        MSR msr(MSR_IA32_GS);
        return reinterpret_cast<PerCPU *>(msr.read());
    }
}