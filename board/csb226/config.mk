#
# Linux-Kernel is expected to be at c000'8000, entry c000'8000
#
# we load ourself to c170'0000, the upper 1 MB of second bank
#
# download areas is c800'0000
#

# This is the address where ppcboot lives in flash: 
#TEXT_BASE = 0

# FIXME: armboot does only work correctly when being compiled 
# for the addresses _after_ relocation to RAM!! Otherwhise the 
# .bss segment is assumed in flash...
TEXT_BASE = 0xa1fe0000

