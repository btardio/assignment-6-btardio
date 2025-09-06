#!/bin/bash

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.15.163
BUSYBOX_VERSION=1_33_1
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-
MYPWD=$(pwd)


if [ ! -d "arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu" ]; then
	wget -q https://developer.arm.com/-/media/Files/downloads/gnu/13.3.rel1/binrel/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu.tar.xz
	tar -xf arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu.tar.xz
	rm arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu.tar.xz
fi

PATH=$PATH:$(pwd)/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu/bin/
export PATH

# echo "export PATH=$PATH:/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu/bin/" >> /root/.bashrc


if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
	mkdir -p ${OUTDIR}
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

#    CXX=/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-gcc
#    export CXX
    
#    make clean

#    make defconfig
    
#    make
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- mrproper

    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- defconfig

    make -j8 ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- all


    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- modules

    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- dtbs

    #make
fi

echo -e "Done\a"
