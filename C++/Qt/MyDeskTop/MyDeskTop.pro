QT += quick

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        desktop/desktop.cpp \
        main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    desktop/images/alientek.png \
    desktop/images/back.png \
    desktop/images/bell.png \
    desktop/images/bgTime.png \
    desktop/images/bgWeather.png \
    desktop/images/center.png \
    desktop/images/clock-night.png \
    desktop/images/clock.png \
    desktop/images/close.png \
    desktop/images/hour.png \
    desktop/images/minute.png \
    desktop/images/pause.png \
    desktop/images/play.png \
    desktop/images/poweroff.png \
    desktop/images/reboot.png \
    desktop/images/second.png \
    desktop/winstyleicons/3dcube_app.png \
    desktop/winstyleicons/aircondition_app.png \
    desktop/winstyleicons/alarm_app.png \
    desktop/winstyleicons/album_app.png \
    desktop/winstyleicons/calc_app.png \
    desktop/winstyleicons/camera_app.png \
    desktop/winstyleicons/file_app.png \
    desktop/winstyleicons/iotest_app.png \
    desktop/winstyleicons/media_app.png \
    desktop/winstyleicons/music_app_icon.png \
    desktop/winstyleicons/radio_app.png \
    desktop/winstyleicons/sensor_app.png \
    desktop/winstyleicons/settings_app.png \
    desktop/winstyleicons/system_app.png \
    desktop/winstyleicons/tcp_c.png \
    desktop/winstyleicons/tcp_s.png \
    desktop/winstyleicons/udpchat_app.png \
    desktop/winstyleicons/weather_app_cloudy.png \
    desktop/winstyleicons/weather_app_fine.png \
    desktop/winstyleicons/weather_app_rain.png \
    desktop/winstyleicons/weather_app_snow.png \
    desktop/winstyleicons/wirless_app.png

HEADERS +=\
    desktop/desktop.h
