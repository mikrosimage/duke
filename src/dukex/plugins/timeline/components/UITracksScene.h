#ifndef UITRACKSSCENE_H
#define UITRACKSSCENE_H

#include <QGraphicsScene>

// forward declaration
class UITracksView;

class UITracksScene : public QGraphicsScene {

    Q_OBJECT

public:
    UITracksScene(QObject* parent = 0);
    virtual ~UITracksScene() {
    }

protected:
    virtual void keyPressEvent(QKeyEvent* keyevent);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);
};

#endif // UITRACKSSCENE_H
