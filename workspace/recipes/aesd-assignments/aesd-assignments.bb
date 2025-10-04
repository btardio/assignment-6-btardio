
# Recipe created by recipetool
# This is the basis of a recipe and may need further editing in order to be fully functional.
# (Feel free to remove these comments when editing.)

# WARNING: the following LICENSE and LIC_FILES_CHKSUM values are best guesses - it is
# your responsibility to verify that the values are complete and correct.
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE.txt;md5=fc0af5c24a227c46b9a3ff1927c4dc94"

# No information for SRC_URI yet (only an external source tree was specified)
SRC_URI = "https://github.com/btardio/assignment-6-btardio.git;branch=yocto"
# SRCREV = "f99b82a5d4cb2a22810104f89d4126f52f4dfaba"

S = "${WORKDIR}/git/server"

FILES:${PN} += "${bindir}/aesdsocket"
FILES_:${PN} += "${sysconfdir}/init.d/aesdsocket.sh"

TARGET_CC_ARCH += "${LDFLAGS}"

# NOTE: this is a Makefile-only piece of software, so we cannot generate much of the
# recipe automatically - you will need to examine the Makefile yourself and ensure
# that the appropriate arguments are passed in.

inherit update-rc.d

INITSCRIPT_PACKAGES = "${PN}"
INITSCRIPT_NAME = "aesdsocket.sh"
INITSCRIPT_PARAMS = "start 99 S . stop 10 0 6 ."

SRC_URI += "file://aesdsocket.sh"


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
        install -d ${D}${bindir}
        install -m 0755 ${B}/aesdsocket ${D}${bindir}

        install -d ${D}${sysconfdir}/init.d
        install -m 0755 ${WORKDIR}/aesdsocket.sh ${D}${sysconfdir}/init.d/
}

