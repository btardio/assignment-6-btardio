
##############################################################
#
# AESD-ASSIGNMENTS
#
##############################################################

#TODO: Fill up the contents below in order to reference your assignment 3 git contents

# assignment 3 branch: assignment 4
# AESD_ASSIGNMENTS_VERSION = '0ff47964b3c0e3aa8031215019458bbba041cfb0'
#AESD_ASSIGNMENTS_VERSION = '75b8d35d7932410e55c3823ab5c7ab2845a33a9d'
AESD_ASSIGNMENTS_VERSION = '0a49a6a198cc6c86c71071c70671f6a9bf4c1214'
# assignment 3 branch: master 
# AESD_ASSIGNMENTS_VERSION = '89fc0b862b9201f41ab45effe0cd755e8caf4174'

# Note: Be sure to reference the *ssh* repository URL here (not https) to work properly
# with ssh keys and the automated build/test system.
# Your site should start with git@github.com:

# Note: Unable to accomplish the git integration.

AESD_ASSIGNMENTS_SITE = 'git@github.com:cu-ecen-aeld/assignments-3-and-later-btardio.git'
AESD_ASSIGNMENTS_SITE_METHOD = git
AESD_ASSIGNMENTS_GIT_SUBMODULES = YES
CROSS_COMPILE=aarch64-none-linux-gnu-
export CROSS_COMPILE
define AESD_ASSIGNMENTS_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/finder-app all 
endef
#$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/finder-app all
# TODO add your writer, finder and finder-test utilities/scripts to the installation steps below
define AESD_ASSIGNMENTS_INSTALL_TARGET_CMDS
	$(INSTALL) -d 0755 $(@D)/conf/ $(TARGET_DIR)/etc/finder-app/conf/
	$(INSTALL) -m 0755 $(@D)/conf/* $(TARGET_DIR)/etc/finder-app/conf/
	$(INSTALL) -m 0755 $(@D)/assignment-autotest/test/assignment4/* $(TARGET_DIR)/bin
	$(INSTALL) -m 0755 $(@D)/unit-test.sh $(TARGET_DIR)/bin
	$(INSTALL) -m 0755 $(@D)/full-test.sh $(TARGET_DIR)/bin
	$(INSTALL) -m 0755 $(@D)/finder-app/writer $(TARGET_DIR)/bin
	$(INSTALL) -m 0755 $(@D)/finder-app/finder-test.sh $(TARGET_DIR)/bin
	$(INSTALL) -m 0755 $(@D)/finder-app/finder.sh $(TARGET_DIR)/bin
endef

$(eval $(generic-package))
