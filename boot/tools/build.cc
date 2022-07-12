#include <iostream>
#include <fstream>
#include <cstring>
#include <filesystem>

using namespace std;
using namespace std::filesystem;

#define DEFAULT_MAJOR_ROOT 0
#define DEFAULT_MINOR_ROOT 0
#define DEFAULT_ROOT_DEV (DEFAULT_MAJOR_ROOT << 8 | DEFAULT_MINOR_ROOT)

/* Minimal number of setup sectors */
#define SETUP_SECT_MIN 5
#define SETUP_SECT_MAX 64

/* This must be large enough to hold the entire setup */
union [[gnu::packed]] {
    char data[SETUP_SECT_MAX*512];
    struct {
        uint8_t __ignore[0x1f1];
        uint8_t setup_sects;
        uint16_t root_flags;
        uint32_t syssize;
        uint16_t ram_size;
        uint16_t vid_mode;
        uint16_t root_dev;
        uint16_t boot_flag;
    };
} buf;

static void usage(void)
{
    std::cerr << "Usage: build setup system image\n";
}

int main(int argc, char** argv)
{
    if (argc != 4) {
        usage();
        return 1;
    }

    ifstream setup(argv[1]);
    if (!setup.is_open()) {
        std::cerr << "Unable to open " << argv[1] << ": " << errno;
        return 1;
    }

    ifstream kernel(argv[2]);
    if (!kernel.is_open()) {
        std::cerr << "Unable to open " << argv[2] << ": " << errno;
        return 1;
    }

    ofstream image(argv[3]);
    if (!image.is_open()) {
        std::cerr << "Unable to write " << argv[3] << ": " << errno;
        return 1;
    }

    /* Copy the setup code */
    if (setup.read(buf.data, sizeof buf).bad()) {
        std::cerr << "read error on setup\n";
        return 1;
    }

    streamsize c;
    if ((c = setup.gcount()) < 1024) {
        std::cerr << "The setup must be at least 1024 bytes\n";
        return 1;    
    }

    if (buf.boot_flag != 0xAA55) {
        std::cerr << "Boot block hasn't got boot flag (0xAA55)\n";
        return 1;      
    }
    
    /* Pad unused space with zeros */
    auto setup_sectors = (c + 511) / 512;
    if (setup_sectors < SETUP_SECT_MIN)
        setup_sectors = SETUP_SECT_MIN;
    auto i = setup_sectors * 512;
    memset(buf.data + c, 0, i - c);

    /* Set the default root device */
    buf.root_dev = DEFAULT_ROOT_DEV;

    /* stat the kernel file */
    path system(argv[2]);
    auto sz = file_size(system);

    /* Number of 16-byte paragraphs */
    auto sys_size = (sz + 15) / 16;

    /* Patch the setup code with the appropriate size parameters */
    buf.setup_sects = setup_sectors - 1;
    buf.syssize = sys_size;

    if (image.write(buf.data, i).bad()) {
        std::cerr << "Writing setup failed\n";
        return 1;   
    }
    
    return 0;
}
