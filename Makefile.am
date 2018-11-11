# Makefile.am - use automake to generate Makefile.in

SUBDIRS = src

YASHIGANID_CONF_PATH = @YASHIGANID_CONF_PATH@
YASHIGANID_DATA_PATH = @YASHIGANID_DATA_PATH@

install-data-local: install-yashiganid_conf install-yashiganid_data
uninstall-local: uninstall-yashiganid_conf uninstall-yashiganid_data

# install a default configuration file if it is not already there
install-yashiganid_conf:
	@if [ -f $(DESTDIR)$(YASHIGANID_CONF_PATH) ]; then \
            echo "$(DESTDIR)$(YASHIGANID_CONF_PATH) already exists, install will not overwrite"; \
	else \
            $(mkinstalldirs) `dirname $(DESTDIR)$(YASHIGANID_CONF_PATH)`; \
            $(INSTALL) -m 600 $(srcdir)/yashiganid.conf $(DESTDIR)$(YASHIGANID_CONF_PATH); \
	fi

uninstall-yashiganid_conf:
	-rm -f $(DESTDIR)$(YASHIGANID_CONF_PATH)

# install white-list files if it is not already there
install-yashiganid_data:
	@if [ -f $(DESTDIR)$(YASHIGANID_DATA_PATH) ]; then \
            echo "Data file will be overritten."; \
            $(INSTALL) -m 644 $(srcdir)/list_bin $(DESTDIR)$(YASHIGANID_DATA_PATH); \
            $(INSTALL) -m 644 $(srcdir)/list_lib $(DESTDIR)$(YASHIGANID_DATA_PATH); \
	else \
            $(MKDIR_P) $(DESTDIR)$(YASHIGANID_DATA_PATH); \
            $(INSTALL) -m 644 $(srcdir)/list_bin $(DESTDIR)$(YASHIGANID_DATA_PATH); \
            $(INSTALL) -m 644 $(srcdir)/list_lib $(DESTDIR)$(YASHIGANID_DATA_PATH); \
	fi

uninstall-yashiganid_data:
	-rm -rf $(DESTDIR)$(YASHIGANID_DATA_PATH)

uninstall-yashiganid_pid:
	-rm -rf $(DESTDIR)$(YASHIGANID_PID_PATH)
