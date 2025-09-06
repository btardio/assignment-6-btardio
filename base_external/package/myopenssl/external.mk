
##############################################################
#
# AESD-ASSIGNMENTS
#
##############################################################

#TODO: Fill up the contents below in order to reference your assignment 3 git contents

# assignment 3 branch: assignment 4
MYOPENSSL_VERSION = '6f3ada8a14233e76d8c809659b81bddaa7be6db8'

# assignment 3 branch: master 
# AESD_ASSIGNMENTS_VERSION = '89fc0b862b9201f41ab45effe0cd755e8caf4174'

# Note: Be sure to reference the *ssh* repository URL here (not https) to work properly
# with ssh keys and the automated build/test system.
# Your site should start with git@github.com:

# Note: Unable to accomplish the git integration.

MYOPENSSL_SITE = 'git@github.com:openssl/openssl.git'
MYOPENSSL_SITE_METHOD = git
MYOPENSSL_GIT_SUBMODULES = YES
#CROSS_COMPILE=aarch64-none-linux-gnu-
#export CROSS_COMPILE
define MYOPENSSL_BUILD_CMDS
	$(CONFIGURE)
	$(MAKE)
endef
#$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/finder-app all
# TODO add your writer, finder and finder-test utilities/scripts to the installation steps below
define MYOPENSSL_INSTALL_TARGET_CMDS
	$(INSTALL)
	
endef

$(eval $(generic-package))
