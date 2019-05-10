
#######################################################################################
# Package
plasma_install_package(package org.kde.plasma.lliurexquota)


#######################################################################################
# Notes Library
add_definitions(-DTRANSLATION_DOMAIN="plasma_applet_org.kde.plasma.lliurexquota")

set(diskquota_SRCS
    plugin/plugin.cpp
    plugin/LliurexDiskQuota.cpp
    plugin/LliurexQuotaListModel.cpp
    plugin/LliurexQuotaItem.cpp
)

add_library(lliurexquotaplugin SHARED ${diskquota_SRCS})

target_link_libraries(lliurexquotaplugin
                      Qt5::Quick
                      KF5::CoreAddons
                      KF5::I18n)

install(FILES plugin/qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/private/lliurexquota)
install(TARGETS lliurexquotaplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/private/lliurexquota)

install(FILES "icons/lliurexquota.svg"
        DESTINATION ${DATAROOTDIR}/usr/share/plasma/desktoptheme/default/icons/)
install(FILES "icons/org.kde.plasma.lliurexquota.svg"
        DESTINATION ${DATAROOTDIR}/usr/share/icons/breeze/applets/256/ )