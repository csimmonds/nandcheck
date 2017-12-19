SUMMARY = "Print NAND block usage and bad blocks in an MTD partition"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=801f80980d171dd6425610833a22dbe6"
SRC_URI = "git://github.com/csimmonds/nandcheck.git"
SRCREV = "${AUTOREV}"
PV = "1.0+git${SRCPV}"

S = "${WORKDIR}/git"

do_compile() {
	oe_runmake
}

do_install() {
	oe_runmake install DESTDIR=${D}
}

