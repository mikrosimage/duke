#include "UIImageInfo.h"
#include "UIVideoPage.h"
#include <dukexcore/dkxNodeManager.h>
#include <dukexcore/nodes/InfoNode.h>
#include <QShowEvent>

UIImageInfo::UIImageInfo(NodeManager* _manager) :
    m_manager(_manager) {
    ui.setupUi(this);
}

void UIImageInfo::update(::google::protobuf::serialize::SharedHolder sharedholder) {
    using namespace ::duke::protocol;
    using namespace ::google::protobuf::serialize;

    if (isType<Playlist> (*sharedholder) || isType<Transport> (*sharedholder)) {
        InfoNode::ptr i = m_manager->nodeByName<InfoNode> ("fr.mikrosimage.dukex.info");
        if (i.get() == NULL)
            return;
        i->callCurrentImageInfo();
    } else if (isType<Info> (*sharedholder)) {
        const Info & info = unpackTo<Info> (*sharedholder);
        for (int i = 0; i < info.image_size(); ++i) {
            UIVideoPage * videopage = NULL;
            if (i >= ui.imageInfoToolBox->count()) {
                videopage = new UIVideoPage(this);
                ui.imageInfoToolBox->insertItem(i, videopage, QString("Video #%1").arg(i));
            } else {
                videopage = qobject_cast<UIVideoPage*> (ui.imageInfoToolBox->widget(i));
            }
            assert(videopage != NULL);
            if (videopage == NULL)
                return;
            const ::duke::protocol::Info_ImageInfo & imginfo = info.image(i);
            videopage->setFilename(imginfo.filename());
            videopage->setPath(imginfo.filename());
            videopage->setWidth(imginfo.width());
            videopage->setHeight(imginfo.height());
            videopage->setDepth(imginfo.depth());
            videopage->setFormat(imginfo.format());
            videopage->setImageSize(imginfo.imagesize());
            videopage->setFileSize(imginfo.filesize());
        }
    }
}

