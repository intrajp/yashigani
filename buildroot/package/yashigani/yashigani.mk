################################################################################
#
# yashigani 
#
################################################################################

# Use a tag or a full commit ID
YASHIGANI_VERSION = v0.0.18
YASHIGANI_SITE = $(call github,intrajp,yashigani,$(YASHIGANI_VERSION))
YASHIGANI_LICENSE = GPL-2.0+
YASHIGANI_LICENSE_FILES = COPYING 

define YASHIGANI_BUILD_CMDS
	$(MAKE) -C $(@D)
endef

define YASHIGANI_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/src/yashigani $(TARGET_DIR)/usr/sbin/yashigani
endef

$(eval $(autotools-package))
