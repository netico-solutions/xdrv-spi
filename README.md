x-mcspi
============

Xenomai Real-Time driver for Multichannel Serial Port Interface (McSPI) found in TI AM335x series MPUs.

Dependencies:
- Linux kernel with Xenomai v2.6.2.1 (https://github.com/nradulovic/linux-am335x)


Before using this driver you must disable in-kernel omap drivers.

# Building

    make clean
    make modules
    
