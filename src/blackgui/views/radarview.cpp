/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "radarview.h"

#include <QResizeEvent>
#include <QWheelEvent>

namespace BlackGui
{
    namespace Views
    {
        CRadarView::CRadarView(QWidget *parent)
            : QGraphicsView(parent)
        {
            setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            setBackgroundBrush(Qt::black);
            setRenderHint(QPainter::Antialiasing);
        }

        void CRadarView::resizeEvent(QResizeEvent *event)
        {
            emit radarViewResized();
            QGraphicsView::resizeEvent(event);
        }

        void CRadarView::wheelEvent(QWheelEvent *event)
        {
            QPoint delta = event->angleDelta();
            if (delta.y() > 0)
            {
                emit zoomEvent(true);
            }
            else
            {
                emit zoomEvent(false);
            }
            event->accept();
        }
    }
}
