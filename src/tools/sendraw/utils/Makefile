include $(TOPDIR)/rules.mk

PKG_NAME:=sendraw
PKG_VERSION:=0
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(PKG_NAME)-$(PKG_VERSION).$(PKG_RELEASE)
PKG_SOURCE:=sendraw.tar.gz
PKG_SOURCE_URL:=file:///home/iftikar/trunk/sendraw/
include $(INCLUDE_DIR)/package.mk

define Package/sendraw
 SECTION:=utils
 CATEGORY:=Utilities
 TITLE:=HW
endef

define Package/sendraw/description
Packet injecting command
endef

define Package/sendraw/Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) sendraw
endef

define Package/sendraw/install
	$(INSTALL_DIR) $(1)/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sendraw $(1)/bin/sendraw
endef
$(eval $(call BuildPackage,sendraw))

