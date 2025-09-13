# this is a system for squashfs - you cant load binary bc of striping

# Recipe created by recipetool
# This is the basis of a recipe and may need further editing in order to be fully functional.
# (Feel free to remove these comments when editing.)

# WARNING: the following LICENSE and LIC_FILES_CHKSUM values are best guesses - it is
# your responsibility to verify that the values are complete and correct.
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://test/Unity/LICENSE.txt;md5=b7dd0dffc9dda6a87fa96e6ba7f9ce6c"

# No information for SRC_URI yet (only an external source tree was specified)
SRC_URI = "https://github.com/btardio/assignment-6-btardio.git;branch=yocto"
# SRCREV = "f99b82a5d4cb2a22810104f89d4126f52f4dfaba"

S = "${WORKDIR}/git/server"

FILES:${PN} += "${bindir}/aesdsocket"

TARGET_CC_ARCH += "${LDFLAGS}"

# NOTE: this is a Makefile-only piece of software, so we cannot generate much of the
# recipe automatically - you will need to examine the Makefile yourself and ensure
# that the appropriate arguments are passed in.

do_configure () {
	# Specify any needed configure commands here
	:
}

do_compile () {
	# You will almost certainly need to add additional arguments here
	oe_runmake
}

do_install () {
	# This is a guess; additional arguments may be required
	# oe_runmake install

        install -d ${D}${bindir}
        install -m 0755 ${B}/aesdsocket ${D}${bindir}
}

