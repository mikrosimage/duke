#ifndef UITRACKSRULER_H
#define UITRACKSRULER_H

#include "UITracksView.h"
#include <QWidget>
#include <QString>

// forward declaration
class QPaintEvent;
class QStylePainter;

namespace { // empty namespace

const int FRAME_SIZE = 90;
static const int FIX_WIDTH = 30; // widget width in pixel
static const int LINE_END = (FIX_WIDTH - 3);
static const int END_MARK_LENGTH = (FIX_WIDTH - 8);
static const int BIG_MARK_LENGTH = (END_MARK_LENGTH * 3 / 4);
static const int BIG_MARK_X2 = LINE_END;
static const int BIG_MARK_X1 = (BIG_MARK_X2 - BIG_MARK_LENGTH);
static const int MIDDLE_MARK_LENGTH = (END_MARK_LENGTH / 2);
static const int MIDDLE_MARK_X2 = LINE_END;
static const int MIDDLE_MARK_X1 = (MIDDLE_MARK_X2 - MIDDLE_MARK_LENGTH);
static const int LITTLE_MARK_LENGTH = (MIDDLE_MARK_LENGTH / 2);
static const int LITTLE_MARK_X2 = LINE_END;
static const int LITTLE_MARK_X1 = (LITTLE_MARK_X2 - LITTLE_MARK_LENGTH);
static const int LABEL_SIZE = 9;

} // empty namespace

class UITracksRuler : public QWidget {

    Q_OBJECT

    enum RULER_DISPLAY_MODE {
        RDM_TC = 0, RDM_FRAME
    };

public:
    UITracksRuler(UITracksView* tracksView, QWidget* parent = 0);

public:
    void setDuration(int duration);
    void setPixelPerMark(double rate);
    static const int comboScale[];
    int offset() const {
        return m_offset;
    }

public slots:
    void moveRuler(int pos);
    void framerateChanged(double);

protected:
    virtual void paintEvent(QPaintEvent* e);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);

private:
    void drawCacheState(QStylePainter *painter);
    QString getTimeCode(int frames) const;

signals:
    void frameChanged(qint64);

private:
    UITracksView* m_tracksView;
    int m_duration;
    int m_fps;
    int m_offset;
    double m_factor;
    double m_scale;
    double m_textSpacing;
    int m_littleMarkDistance;
    int m_mediumMarkDistance;
    int m_bigMarkDistance;
    RULER_DISPLAY_MODE m_displayMode;
};

#endif // UITRACKSRULER_H
