import os

# toolchains options
ARCH        ='risc-v'
CPU         ='c906'
CROSS_TOOL  ='gcc'

if os.getenv('RTT_ROOT'):
    RTT_ROOT = os.getenv('RTT_ROOT')
else:
    RTT_ROOT = os.path.join(os.getcwd(), '..', '..')

# print("hello")
# print(RTT_ROOT)

if os.getenv('RTT_CC'):
    CROSS_TOOL = os.getenv('RTT_CC')

if  CROSS_TOOL == 'gcc':
    PLATFORM    = 'gcc'
    EXEC_PATH   = r'C:\work\work\d1\riscv64-elf-mingw\bin'
else:
    print('Please make sure your toolchains is GNU GCC!')
    exit(0)

if os.getenv('RTT_EXEC_PATH'):
    EXEC_PATH = os.getenv('RTT_EXEC_PATH')

BUILD = ' '

if PLATFORM == 'gcc':
    # toolchains
    #PREFIX  = 'riscv64-unknown-elf-'
    PREFIX  = os.getenv('RTT_CC_PREFIX') or 'riscv64-unknown-elf-'
    CC      = PREFIX + 'gcc'
    CXX     = PREFIX + 'g++'
    AS      = PREFIX + 'gcc'
    AR      = PREFIX + 'ar'
    LINK    = PREFIX + 'gcc'
    TARGET_EXT = 'elf'
    SIZE    = PREFIX + 'size'
    OBJDUMP = PREFIX + 'objdump'
    OBJCPY  = PREFIX + 'objcopy'

    DEVICE  = ' -march=rv64gcvxtheade  -mabi=lp64d -mtune=c906 '
    CFLAGS  = DEVICE + '  -ffreestanding -fno-common   -ffunction-sections -fdata-sections -fstrict-volatile-bitfields '
    AFLAGS  = ' -c' + DEVICE + ' -x assembler-with-cpp'
    LFLAGS  = DEVICE + ' -nostartfiles -Wl,--gc-sections,-Map=rtthread.map,-cref,-u,_start -T link.lds'
    CPATH   = ''
    LPATH   = ''

    if BUILD == 'debug':
        CFLAGS += ' -O0 -ggdb -fvar-tracking'
        AFLAGS += ' -ggdb'
    else:
        CFLAGS += ' -O2 '

    CXXFLAGS = CFLAGS

DUMP_ACTION = OBJDUMP + ' -D -S $TARGET > rtthread.asm\n'
POST_ACTION = OBJCPY + ' -O binary $TARGET rtthread.bin\n'
