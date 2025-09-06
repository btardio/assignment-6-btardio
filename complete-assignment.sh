#!/bin/bash

set -e

ARCH=arm64

make

bash manual-linux.sh

cp /tmp/aeld/linux-stable/arch/${ARCH}/boot/Image buildroot/output/images





