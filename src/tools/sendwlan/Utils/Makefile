include $(TOPDIR)/rules.mk

PKG_NAME:=sendwlan
PKG_VERSION:=0
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(PKG_NAME)-$(PKG_VERSION).$(PKG_RELEASE)
PKG_SOURCE:=sendwlan.tar.gz
PKG_SOURCE_URL:=file:///home/iftikar/trunk/sendwlan/
include $(INCLUDE_DIR)/package.mk

define Package/sendwlan
 SECTION:=utils
 CATEGORY:=Utilities
 TITLE:=SendWLAN
endef

define Package/sendwlan/description
Packet injecting command for WLAN1
endef

define Package/sendwlan/Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) sendwlan
endef

define Package/sendwlan/install
	$(INSTALL_DIR) $(1)/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sendwlan $(1)/bin/sendwlan
endef
$(eval $(call BuildPackage,sendwlan))

