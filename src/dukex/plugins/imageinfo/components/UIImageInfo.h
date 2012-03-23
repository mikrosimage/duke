#ifndef UIIMAGEINFO_H
#define UIIMAGEINFO_H

#include "player.pb.h"
#include "ui_imageinfo.h"
#include <dukexgui/UIWidget.h>

// forward declaration
class NodeManager;

class UIImageInfo : public UIWidget {

    Q_OBJECT
    Q_DISABLE_COPY(UIImageInfo)

public:
    UIImageInfo(NodeManager*);
    virtual ~UIImageInfo(){}

public:
    virtual void update(::google::protobuf::serialize::SharedHolder holder);

private:
    Ui::ImageInfo ui;
    NodeManager* m_manager;
};

#endif // UIIMAGEINFO_H
