
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${QIMAGEBLITZ_INCLUDES} ${GPGME_INCLUDES})

########### next target ###############

set(basketcommon_LIB_SRCS
    archive.cpp
    bnpview.cpp
    settings.cpp
    basket.cpp
    basketproperties.cpp
    linklabel.cpp
    variouswidgets.cpp
    xmlwork.cpp
    keyboard.cpp
    global.cpp
    exporterdialog.cpp
    htmlexporter.cpp
    notefactory.cpp
    softwareimporters.cpp
    focusedwidgets.cpp
    basketfactory.cpp
    tag.cpp
    kcolorcombo2.cpp
    newbasketdialog.cpp
    tagsedit.cpp
    formatimporter.cpp
    note.cpp
    notecontent.cpp
    notedrag.cpp
    noteedit.cpp
    filter.cpp
    tools.cpp
    backgroundmanager.cpp
    regiongrabber.cpp
    kgpgme.cpp
    likeback.cpp
    crashhandler.cpp
    password.cpp
    colorpicker.cpp
    basketlistview.cpp
    debugwindow.cpp
    systemtray.cpp
    aboutdata.cpp
    basketstatusbar.cpp
    backup.cpp)

kde4_add_ui_files(basketcommon_LIB_SRCS passwordlayout.ui)

QT4_ADD_DBUS_ADAPTOR(basketcommon_LIB_SRCS org.basket.BNPView.xml bnpview.h BNPView)

QT4_ADD_RESOURCES(basketcommon_LIB_SRCS ../basket.qrc)

kde4_add_library(basketcommon SHARED ${basketcommon_LIB_SRCS})

target_link_libraries(basketcommon
  ${KDE4_KDE3SUPPORT_LIBS} # Remove me
  ${KDE4_KIO_LIBS}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KPARTS_LIBS}
  ${KDE4_KFILE_LIBS}
  ${KDE4_PHONON_LIBRARY}
  ${QIMAGEBLITZ_LIBRARIES}
  ${GPGME_VANILLA_LIBRARIES})

if(Q_WS_X11)
    target_link_libraries(basketcommon ${X11_X11_LIB})
endif(Q_WS_X11)

set_target_properties(basketcommon PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})

install(TARGETS basketcommon DESTINATION ${LIB_INSTALL_DIR})

########### next target ###############

set(basket_SRCS main.cpp mainwindow.cpp application.cpp)

kde4_add_executable(basket ${basket_SRCS})
target_link_libraries(basket
  ${KDE4_KDE3SUPPORT_LIBS} # Remove me as well
  ${KDE4_KUTILS_LIBS}
  ${KDE4_KFILE_LIBS}
  ${KDE4_PHONON_LIBRARY}
  basketcommon)

install(TARGETS basket DESTINATION ${BIN_INSTALL_DIR})

if(BUILD_KPARTS)
########### next target ###############

set(kcm_basket_PART_SRCS kcm_basket.cpp)

kde4_add_plugin(kcm_basket ${kcm_basket_PART_SRCS})

target_link_libraries(kcm_basket ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} basketcommon)

install(TARGETS kcm_basket DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(basketpart_PART_SRCS basket_part.cpp)

kde4_add_plugin(basketpart WITH_PREFIX ${basketpart_PART_SRCS})

target_link_libraries(basketpart ${KDE4_KDECORE_LIBS} basketcommon kparts kdeui DCOP)

install(TARGETS basketpart DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install(FILES  basket_part.rc DESTINATION ${DATA_INSTALL_DIR}/basket)
install(FILES  basket_part.desktop DESTINATION ${SERVICES_INSTALL_DIR})

endif(BUILD_KPARTS)

install(FILES  basket.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES  basket_config_general.desktop  	basket_config_baskets.desktop  	basket_config_new_notes.desktop  	basket_config_notes_appearance.desktop  	basket_config_apps.desktop  	basket_config_features.desktop  	basket_config_notes.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES  basketui.rc DESTINATION ${DATA_INSTALL_DIR}/basket)

kde4_install_icons(${ICON_INSTALL_DIR})


