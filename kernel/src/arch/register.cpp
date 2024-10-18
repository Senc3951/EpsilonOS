#include <arch/register.hpp>

namespace kernel
{
    u64 Register::read(const RegisterName& name)
    {
        u64 value;
        u32 low, high;

        switch (name)
        {
            case CR0:
                asm volatile("mov %%cr0, %0" : "=r" (value));
                break;
            case CR2:
                asm volatile("mov %%cr2, %0" : "=r" (value));
                break;
            case CR3:
                asm volatile("mov %%cr3, %0" : "=r" (value));
                break;
            case CR4:
                asm volatile("mov %%cr4, %0" : "=r" (value));
                break;
            case XCR0:
                asm volatile("xgetbv"
                            : "=a"(low), "=d"(high)
                            : "c"(0));
                value = ((u64)high << 32) | low;
                
                break;
            default:
                panic("invalid read register %u", name);
        }

        return value;
    }

    void Register::write(const RegisterName &name, const u64 value)
    {
        u32 low = static_cast<u32>(value);
        u32 high = static_cast<u32>(value >> 32);
        
        switch (name)
        {
            case CR0:
                asm volatile("mov %0, %%cr0" : : "r"(value));
                break;
            case CR2:
                asm volatile("mov %0, %%cr2" : : "r"(value));
                break;
            case CR3:
                asm volatile("mov %0, %%cr3" : : "r"(value));
                break;
            case CR4:
                asm volatile("mov %0, %%cr4" : : "r"(value));
                break;
            case XCR0:
                asm volatile("xsetbv" ::"a"(low), "d"(high), "c"(0));
                break;
            default:
                panic("invalid write register %u", name);
        }
    }
}