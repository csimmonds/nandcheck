SUMMARY = "NAND check"
LICENSE = "CLOSED"
SRC_URI = "git://github.com/csimmonds/nandcheck.git"
SRCREV = "aeab1466cd5a7e057e646f7ea6aea740e1605273"

S = "${WORKDIR}/git"

do_compile() {
	oe_runmake
}

do_install() {
	oe_runmake install DESTDIR=${D}
}

