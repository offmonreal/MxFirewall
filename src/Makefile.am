bin_PROGRAMS = mxfirewall
mxfirewall_SOURCES = main.cpp MainWindow.cpp NftablesParser.cpp TableModelColumns.cpp Utilites.cpp
mxfirewall_CPPFLAGS = $(GTKMM_CFLAGS) $(MY_CPPFLAGS)
mxfirewall_LDADD = $(GTKMM_LIBS)

# Path for install
bindir = $(prefix)/mx/firewall
app_uidir = $(bindir)
app_ui_DATA = ../ui/interface.ui

# Copy Icon, Docs...
icondir = $(prefix)/share/icons/hicolor/96x96/apps
desktopdir = $(prefix)/share/applications
docdir = $(bindir)
icon_DATA = ../mx-firewall.png
desktop_DATA = ../MxFirewall.desktop
doc_DATA = ../README.md
