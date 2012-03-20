#include "UITracksScene.h"
#include "UIGraphicsMovieItem.h"
#include "UIGraphicsTrack.h"
#include "UITimeline.h"
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>

UITracksScene::UITracksScene(QObject* parent) :
    QGraphicsScene(parent) {
}

void UITracksScene::keyPressEvent(QKeyEvent* keyEvent) {
    QGraphicsScene::keyPressEvent(keyEvent);
}

void UITracksScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
    QGraphicsScene::contextMenuEvent(event);
    if (event->isAccepted())
        return; // Event handled by an item in the scene.

//    QMenu menu("context");
//    QAction* firstAction = menu.addAction(tr("menu1"));
//    QAction* secondAction = menu.addAction(tr("menu2"));
//    QAction* thirdAction = menu.addAction(tr("menu3"));
//    QAction* selectedAction = menu.exec(event->screenPos());
//    if (!selectedAction)
//        return;
//
//    if (selectedAction == firstAction) {
//    } else if (selectedAction == secondAction) {
//    } else if (selectedAction == thirdAction) {
//    }
}
