#include <dev/uart.hpp>

// Set the base revision to 2, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.
__attribute__((used, section(".requests")))
static volatile LIMINE_BASE_REVISION(2);

__limine_request__ volatile limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};
__limine_request__ volatile limine_bootloader_info_request bootloader_info_request = {
    .id = LIMINE_BOOTLOADER_INFO_REQUEST,
    .revision = 0
};
__limine_request__ volatile limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};
__limine_request__ volatile limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};
__limine_request__ volatile limine_kernel_file_request kernel_file_request = {
    .id = LIMINE_KERNEL_FILE_REQUEST,
    .revision = 0
};
__limine_request__ volatile limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};
__limine_request__ volatile limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0
};

// Finally, define the start and end markers for the Limine requests.
// These can also be moved anywhere, to any .c file, as seen fit.
__attribute__((used, section(".requests_start_marker")))
static volatile LIMINE_REQUESTS_START_MARKER;
__attribute__((used, section(".requests_end_marker")))
static volatile LIMINE_REQUESTS_END_MARKER;

// Global constructors
extern void (*__init_array[])();
extern void (*__init_array_end[])();

static void verify_boot()
{
    // Ensure the bootloader actually understands our base revision
    if (LIMINE_BASE_REVISION_SUPPORTED == false)
        __hnr();

    // Ensure got requests
    if (!hhdm_request.response || !bootloader_info_request.response || !memmap_request.response ||
        !kernel_address_request.response || !kernel_file_request.response ||
        !framebuffer_request.response || !smp_request.response)
        __hnr();
}

static void init_ctors()
{
    for (std::size_t i = 0; &__init_array[i] != __init_array_end; i++)
        __init_array[i]();
}

extern "C" __no_return__ void kmain()
{
    // Verify booted correctly and received all requests
    verify_boot();

    // Enable serial output
    dev::UART::init();
    dev::UART::write("hello world");    
    init_ctors();
    
    // Fetch the first framebuffer.
    limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    // Note: we assume the framebuffer model is RGB with 32-bit pixels.
    for (std::size_t i = 0; i < 500; i++) {
        volatile std::uint32_t *fb_ptr = static_cast<volatile std::uint32_t *>(framebuffer->address);
        fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;
    }
    
    __hnr();
}