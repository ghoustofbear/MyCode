import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Styles 1.4
import "./desktop"
Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")


    Rectangle {
        id: rectangle
        color: "#1f1e58"
        anchors.fill: parent
    }


    Rectangle {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        color: "transparent"
        Component.onCompleted:  {
             contentItem.highlightMoveDuration = 0       //禁用滑动动画
        }

    }

}
