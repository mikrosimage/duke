#include "UITracksRuler.h"
#include "UITracksView.h"
#include <QStylePainter>
#include <QPaintEvent>
#include <iostream>

const int UITracksRuler::comboScale[] = { 1, 2, 5, 10, 25, 50, 125, 250, 500, 725, 1500, 3000, 6000, 12000 };

UITracksRuler::UITracksRuler(UITracksView* tracksView, QWidget* parent) :
    QWidget(parent), m_tracksView(tracksView), m_duration(0), m_offset(0), m_displayMode(RDM_FRAME) {
    m_fps = 25.0;
    m_factor = 1;
    m_scale = 3;
    m_littleMarkDistance = FRAME_SIZE;
    m_mediumMarkDistance = FRAME_SIZE * m_fps;
    m_bigMarkDistance = FRAME_SIZE * m_fps * 60;
    setMinimumHeight(30);
    setPixelPerMark(5);
    // Redraw the ruler when the cursor position change
    connect(tracksView->tracksCursor(), SIGNAL(cursorMoved(qint64)), this, SLOT(update()));
}

void UITracksRuler::setPixelPerMark(double rate) {
    int scale = comboScale[(int) rate];
    m_factor = 1.0 / (double) scale * FRAME_SIZE;
    m_scale = 1.0 / (double) scale;
    double fend = m_scale * m_littleMarkDistance;
    switch ((int) rate) {
        case 0:
            m_textSpacing = fend;
            break;
        case 1:
            m_textSpacing = fend * m_fps;
            break;
        case 2:
        case 3:
        case 4:
            m_textSpacing = fend * m_fps * 5;
            break;
        case 5:
        case 6:
            m_textSpacing = fend * m_fps * 10;
            break;
        case 7:
            m_textSpacing = fend * m_fps * 30;
            break;
        case 8:
        case 9:
        case 10:
            m_textSpacing = fend * m_fps * 60;
            break;
        case 11:
        case 12:
            m_textSpacing = fend * m_fps * 300;
            break;
        case 13:
            m_textSpacing = fend * m_fps * 600;
            break;
    }
    update();
}

void UITracksRuler::setDuration(int duration) {
    m_duration = duration;
    update();
}

void UITracksRuler::paintEvent(QPaintEvent* e) {
    QStylePainter painter(this);
    painter.setClipRect(e->rect());

    // Draw the background
    const int projectEnd = (int) (m_duration * m_factor);
    if (projectEnd - m_offset > 1)
        painter.fillRect(0, 0, projectEnd - m_offset, height(), QBrush(QColor(77, 82, 95)));

    double f, step;
    int offsetMax;
    int offsetMin;
    const int maxVal = (e->rect().right() + m_offset) / FRAME_SIZE + 1;
    offsetMax = maxVal * FRAME_SIZE;
    offsetMin = (int) ((e->rect().left() + m_offset) / m_textSpacing);
    offsetMin = (int) (offsetMin * m_textSpacing);

    // Draw text
    QPalette palette;
    painter.setPen(palette.dark().color());
    for (f = offsetMin; f < offsetMax; f += m_textSpacing) {
        QString time = getTimeCode((int) (f / m_factor + 0.5));
        painter.drawText((int) f - m_offset + 2, LABEL_SIZE + 1, time);
    }


    // Draw cache state
    drawCacheState(&painter);


    // Draw the marks
    offsetMin = (e->rect().left() + m_offset) / m_littleMarkDistance;
    offsetMin = offsetMin * m_littleMarkDistance;
    step = m_scale * m_littleMarkDistance;
    if (step > 5)
        for (f = offsetMin - m_offset; f < offsetMax - m_offset; f += step)
            painter.drawLine((int) f, LITTLE_MARK_X1, (int) f, LITTLE_MARK_X2);
    offsetMin = (e->rect().left() + m_offset) / m_mediumMarkDistance;
    offsetMin = offsetMin * m_mediumMarkDistance;
    step = m_scale * m_mediumMarkDistance;
    if (step > 5)
        for (f = offsetMin - m_offset; f < offsetMax - m_offset; f += step)
            painter.drawLine((int) f, MIDDLE_MARK_X1, (int) f, MIDDLE_MARK_X2);
    offsetMin = (e->rect().left() + m_offset) / m_bigMarkDistance;
    offsetMin = offsetMin * m_bigMarkDistance;
    step = m_scale * m_bigMarkDistance;
    if (step > 5)
        for (f = offsetMin - m_offset; f < offsetMax - m_offset; f += step)
            painter.drawLine((int) f, BIG_MARK_X1, (int) f, BIG_MARK_X2);

    // Draw the pointer
    int cursorPos = m_tracksView->cursorPos() * m_factor - offset();
    QPolygon cursor(3);
    cursor.setPoints(3, cursorPos - 9, 11, cursorPos + 9, 11, cursorPos, 30);
    painter.setPen(QColor(200, 200, 200, 150));
    painter.setBrush(QBrush(QColor(82, 97, 122, 150)));
    painter.drawPolygon(cursor);
    if (m_scale < 1) {
        QRectF rect;
        rect = painter.boundingRect(rect, Qt::AlignHCenter, QString::number((qreal)(m_tracksView->cursorPos())));
        painter.drawText(cursorPos - (rect.width() / 2.f), 9, QString::number((qreal)(m_tracksView->cursorPos())));
    }
}

void UITracksRuler::mouseDoubleClickEvent(QMouseEvent* event) {
    m_displayMode = (m_displayMode == RDM_FRAME) ? RDM_TC : RDM_FRAME;
    update();
}

void UITracksRuler::mousePressEvent(QMouseEvent* event) {
    if (event->buttons() == Qt::LeftButton && event->modifiers() == Qt::NoModifier) {
        emit frameChanged(qMax((qreal) 0, (qreal)qRound((event->x() + offset()) / m_factor)));
    }
}

void UITracksRuler::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() == Qt::LeftButton && event->modifiers() == Qt::NoModifier) {
        emit frameChanged(qMax((qreal) 0, (qreal)qRound((event->x() + offset()) / m_factor)));
    }
}

// slot
void UITracksRuler::moveRuler(int pos) {
    m_offset = pos;
    update();
}

// slot
void UITracksRuler::framerateChanged(double framerate) {
    m_fps = framerate;
    update();
}

// private
void UITracksRuler::drawCacheState(QStylePainter *painter) {
    double step = m_scale * m_littleMarkDistance; // frame length (px)
    double offsetMax = (width() + m_offset) ;
    painter->save();
//    painter.setPen(QColor(77, 82, 95));
    painter->setPen(QColor(77, 255, 95));
    std::cerr << "m_offset " << m_offset << std::endl;
    std::cerr << "offsetMax " << offsetMax << std::endl;
    std::cerr << "step " << step << std::endl;
    for (double f = m_offset ; f < offsetMax ; f += step){
        int frame = (f / m_factor);
        if(frame >= 10 && frame <= 15){
            int here = qRound(frame)*m_factor;
            QLine l(here-m_offset, height()-1, here-m_offset+step, height()-1);
            painter->drawLine(l);
            std::cerr << frame << std::endl;
        }
    }
    painter->restore();
}

// private
QString UITracksRuler::getTimeCode(int frames) const {
    QString text;
    if (m_displayMode == RDM_FRAME) {
        text.append(QString::number(frames).rightJustified(2, '0', false));
    } else {
        int seconds = frames / m_fps;
        frames = frames % m_fps;
        int minutes = seconds / 60;
        seconds = seconds % 60;
        int hours = minutes / 60;
        minutes = minutes % 60;

        text.append(QString::number(hours).rightJustified(2, '0', false));
        text.append(':');
        text.append(QString::number(minutes).rightJustified(2, '0', false));
        text.append(':');
        text.append(QString::number(seconds).rightJustified(2, '0', false));
        text.append(':');
        text.append(QString::number(frames).rightJustified(2, '0', false));
    }
    return text;
}
