include $(top_srcdir)/globals.mak

noinst_LTLIBRARIES = libcodecdescriptor.la

libcodecdescriptor_la_SOURCES = audiocodecfactory.cpp audiocodec.cpp

if BUILD_GSM
GSM_LIB = libcodec_gsm.so
libcodec_gsm_so_SOURCES = gsmcodec.cpp
libcodec_gsm_so_CFLAGS = -fPIC -g -Wall
libcodec_gsm_so_CXXFLAGS = -fPIC -g -Wall
libcodec_gsm_so_LDFLAGS = --shared -lc -lgsm
libcodec_gsm_so_LDADD = libcodecdescriptor.la
INSTALL_GSM_RULE = install-libcodec_gsm_so
endif

if BUILD_SPEEX
SPEEX_NB_LIB = libcodec_speex_nb.so
libcodec_speex_nb_so_SOURCES = speexcodec_nb.cpp
libcodec_speex_nb_so_CFLAGS = -fPIC -g -Wall
libcodec_speex_nb_so_CXXFLAGS = -fPIC -g -Wall $(SPEEXDSP)
libcodec_speex_nb_so_LDFLAGS = --shared -lc -lspeex $(SPEEX_NIMP)
libcodec_speex_nb_so_LDADD = libcodecdescriptor.la
INSTALL_SPEEX_NB_RULE = install-libcodec_speex_nb_so
endif

if BUILD_SPEEX
SPEEX_WB_LIB = libcodec_speex_wb.so
libcodec_speex_wb_so_SOURCES = speexcodec_wb.cpp
libcodec_speex_wb_so_CFLAGS = -fPIC -g -Wall
libcodec_speex_wb_so_CXXFLAGS = -fPIC -g -Wall $(SPEEXDSP)
libcodec_speex_wb_so_LDFLAGS = --shared -lc -lspeex $(SPEEX_NIMP)
libcodec_speex_wb_so_LDADD = libcodecdescriptor.la
INSTALL_SPEEX_WB_RULE = install-libcodec_speex_wb_so
endif

if BUILD_SPEEX
SPEEX_UB_LIB = libcodec_speex_ub.so
libcodec_speex_ub_so_SOURCES = speexcodec_ub.cpp
libcodec_speex_ub_so_CFLAGS = -fPIC -g -Wall
libcodec_speex_ub_so_CXXFLAGS = -fPIC -g -Wall $(SPEEXDSP)
libcodec_speex_ub_so_LDFLAGS = --shared -lc -lspeex $(SPEEX_NIMP)
libcodec_speex_ub_so_LDADD = libcodecdescriptor.la
INSTALL_SPEEX_UB_RULE = install-libcodec_speex_ub_so
endif

if BUILD_CELT_91
CELT_LIB = libcodec_celt.so
libcodec_celt_so_SOURCES = celtcodec.cpp
libcodec_celt_so_CFLAGS = -fPIC -g -Wall -DBUILD_CELT_91
libcodec_celt_so_CXXFLAGS = -fPIC -g -Wall -DBUILD_CELT_91
libcodec_celt_so_LDFLAGS = --shared -lc -lcelt0 $(CELT_NIMP)
libcodec_celt_so_LDADD = libcodecdescriptor.la
INSTALL_CELT_RULE = install-libcodec_celt_so
endif

if BUILD_CELT_71
CELT_LIB = libcodec_celt.so
libcodec_celt_so_SOURCES = celtcodec.cpp
libcodec_celt_so_CFLAGS = -fPIC -g -Wall -DBUILD_CELT_71
libcodec_celt_so_CXXFLAGS = -fPIC -g -Wall -DBUILD_CELT_71
libcodec_celt_so_LDFLAGS = --shared -lc -lcelt0 $(CELT_NIMP)
libcodec_celt_so_LDADD = libcodecdescriptor.la
INSTALL_CELT_RULE = install-libcodec_celt_so
endif

noinst_PROGRAMS = libcodec_ulaw.so libcodec_alaw.so libcodec_g722.so $(GSM_LIB) $(SPEEX_NB_LIB) $(SPEEX_WB_LIB) $(SPEEX_UB_LIB) $(CELT_LIB)

noinst_HEADERS = audiocodec.h audiocodecfactory.h speexcodec.h

libcodec_ulaw_so_SOURCES = ulaw.cpp
libcodec_ulaw_so_CFLAGS = -fPIC -g -Wall
libcodec_ulaw_so_CXXFLAGS = -fPIC -g -Wall
libcodec_ulaw_so_LDADD = libcodecdescriptor.la
libcodec_ulaw_so_LDFLAGS = --shared -lc

libcodec_alaw_so_SOURCES = alaw.cpp
libcodec_alaw_so_CFLAGS = -fPIC -g -Wall
libcodec_alaw_so_CXXFLAGS = -fPIC -g -Wall
libcodec_alaw_so_LDADD = libcodecdescriptor.la
libcodec_alaw_so_LDFLAGS = --shared -lc

libcodec_g722_so_SOURCES = g722.cpp
libcodec_g722_so_CFLAGS = -fPIC -g -Wall
libcodec_g722_so_CXXFLAGS = -fPIC -g -Wall
libcodec_g722_so_LDADD = libcodecdescriptor.la
libcodec_g722_so_LDFLAGS = --shared -lc

install-exec-local:  install-libcodec_ulaw_so install-libcodec_alaw_so install-libcodec_g722_so $(INSTALL_GSM_RULE) $(INSTALL_SPEEX_NB_RULE) $(INSTALL_SPEEX_WB_RULE) $(INSTALL_SPEEX_UB_RULE) $(INSTALL_CELT_RULE) $(INSTALL_ILBC_RULE) 
uninstall-local:  uninstall-libcodec_ulaw_so uninstall-libcodec_alaw_so uninstall-libcodec_g722_so uninstall-libcodec_gsm_so uninstall-libcodec_speex_nb_so uninstall-libcodec_speex_wb_so uninstall-libcodec_speex_ub_so uninstall-libcodec_celt_so


install-libcodec_ulaw_so: libcodec_ulaw.so
	mkdir -p $(sflcodecdir)
	$(INSTALL_PROGRAM)  libcodec_ulaw.so $(sflcodecdir)
install-libcodec_alaw_so: libcodec_alaw.so
	$(INSTALL_PROGRAM)  libcodec_alaw.so $(sflcodecdir)
install-libcodec_g722_so: libcodec_g722.so
	$(INSTALL_PROGRAM)  libcodec_g722.so $(sflcodecdir)
install-libcodec_gsm_so: libcodec_gsm.so
	$(INSTALL_PROGRAM)  libcodec_gsm.so $(sflcodecdir)
install-libcodec_speex_nb_so: libcodec_speex_nb.so
	$(INSTALL_PROGRAM)  libcodec_speex_nb.so $(sflcodecdir)
install-libcodec_speex_wb_so: libcodec_speex_wb.so
	$(INSTALL_PROGRAM)  libcodec_speex_wb.so $(sflcodecdir)
install-libcodec_speex_ub_so: libcodec_speex_ub.so
	$(INSTALL_PROGRAM)  libcodec_speex_ub.so $(sflcodecdir)
install-libcodec_celt_so: libcodec_celt.so
	$(INSTALL_PROGRAM)  libcodec_celt.so $(sflcodecdir)


uninstall-libcodec_ulaw_so:
	rm -f $(sflcodecdir)/libcodec_ulaw.so
uninstall-libcodec_alaw_so:
	rm -f $(sflcodecdir)/libcodec_alaw.so
uninstall-libcodec_g722_so:
	rm -f $(sflcodecdir)/libcodec_g722.so
uninstall-libcodec_gsm_so:
	rm -f $(sflcodecdir)/libcodec_gsm.so
uninstall-libcodec_speex_nb_so:
	rm -f $(sflcodecdir)/libcodec_speex_nb.so
uninstall-libcodec_speex_wb_so:
	rm -f $(sflcodecdir)/libcodec_speex_wb.so
uninstall-libcodec_speex_ub_so:
	rm -f $(sflcodecdir)/libcodec_speex_ub.so
uninstall-libcodec_celt_so:
	rm -f $(sflcodecdir)/libcodec_celt.so
	rm -rf $(sflcodecdir)

