#ifndef UIGRAPHICSMOVIEITEM_H
#define UIGRAPHICSMOVIEITEM_H

#include "UIAbstractGraphicsMediaItem.h"
#include "UITracksView.h"

class UIGraphicsMovieItem : public UIAbstractGraphicsMediaItem {

public:
    enum {
        Type = UserType + 1
    };

public:
    UIGraphicsMovieItem(const QString& path, qint64 recin, qint64 recout, qint64 srcin, qint64 srcout);
    virtual ~UIGraphicsMovieItem() {
    }

public:
    virtual int type() const ;
    virtual bool expandable() const;
    virtual bool moveable() const;
    virtual TrackType trackType() const;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

protected:
    void paintRect(QPainter* painter, const QStyleOptionGraphicsItem* option);
    void paintTitle(QPainter* painter, const QStyleOptionGraphicsItem* option);
};

#endif // UIGRAPHICSMOVIEITEM_H
