/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "flowlayout.h"

#include <qmath.h>

#include <QWidget>

FlowLayout::FlowLayout()
{
    m_spacing[0] = 0;
    m_spacing[1] = 0;
    m_rows = 0;
    m_columns = 0;
    QSizePolicy sp = sizePolicy();
    sp.setHeightForWidth(true);
    setSizePolicy(sp);
}

int FlowLayout::itemAbove(int index) {
    if(index >= m_items.count() || index < 0)
        return -1;

    int indexAbove = index - m_columns;
    if(indexAbove >= 0)
        return indexAbove;
    else
        return index;
}

int FlowLayout::itemBelow(int index) {
    if(index >= m_items.count() || index < 0)
        return -1;

    if(sameRow(index, m_items.count() - 1))
        return index;

    int indexBelow = index + m_columns;
    if(indexBelow < m_items.count())
        return indexBelow;
    else
        return m_items.count() - 1;
}

bool FlowLayout::sameRow(int one, int two) {
    return ((one / m_columns) == (two / m_columns));
}

int FlowLayout::columnOf(int index) {
    if(index >= m_items.count() || index < 0)
        return -1;

    int col = index % m_columns;
    return col;
}

int FlowLayout::rows() {
    return m_rows;
}

int FlowLayout::columns() {
    return m_columns;
}

void FlowLayout::insertItem(int index, QGraphicsLayoutItem *item) {
    item->setParentLayoutItem(this);
    if(uint(index) > uint(m_items.count()))
        index = m_items.count();
    m_items.insert(index, item);
    invalidate();
}

int FlowLayout::count() const
{
    return m_items.count();
}

QGraphicsLayoutItem *FlowLayout::itemAt(int index) const
{
    return m_items.value(index);
}

void FlowLayout::removeAt(int index)
{
    m_items.removeAt(index);
    invalidate();
}

void FlowLayout::clear()
{
    m_items.clear();
    invalidate();
}

qreal FlowLayout::spacing(Qt::Orientation o) const
{
    return m_spacing[int(o) - 1];
}

void FlowLayout::setSpacing(Qt::Orientations o, qreal spacing)
{
    if (o & Qt::Horizontal)
        m_spacing[0] = spacing;
    if (o & Qt::Vertical)
        m_spacing[1] = spacing;
}

void FlowLayout::setGeometry(const QRectF &geom)
{
    QGraphicsLayout::setGeometry(geom);
    GridInfo gInfo = doLayout(geom, true);
    m_columns = gInfo.columns;
    m_rows = gInfo.rows;
}

// this assumes every item is of the same size
GridInfo FlowLayout::doLayout(const QRectF &geom, bool applyNewGeometry) const {
    QElapsedTimer t;
    t.start();

    qreal leftMargin, topMargin, rightMargin, bottomMargin;
    getContentsMargins(&leftMargin, &topMargin, &rightMargin, &bottomMargin);

    const qreal maxRowWidth = geom.width() - leftMargin - rightMargin;
    int centerOffset = 0;

    qreal x = 0;
    qreal y = 0;

    QSizeF itemSize;

    int columns = m_items.count();
    int rows = 0;
    if(columns)
        rows = 1;

    if(m_items.count()) {
        // calculate offset for centering
        const qreal itemWidth = m_items.at(0)->effectiveSizeHint(Qt::PreferredSize).width();
        int maxCols = static_cast<int>(maxRowWidth / itemWidth);
        if(m_items.count() >= maxCols)
            centerOffset = static_cast<int>(fmod(maxRowWidth, itemWidth) / 2);
        QGraphicsLayoutItem *item = m_items.at(0);
        itemSize = item->effectiveSizeHint(Qt::PreferredSize);
    }

    for (int i = 0; i < m_items.count(); ++i) {
        qreal next_x;
        next_x = x + itemSize.width();
        if (next_x > maxRowWidth) {
            if(x == 0) {
                itemSize.setWidth(maxRowWidth);
            } else {
                x = 0;
                next_x = itemSize.width();
                if(rows == 1)
                    columns = i;
                rows++;
            }
            y += itemSize.height() + spacing(Qt::Vertical);
        }
        if(applyNewGeometry)
            m_items.at(i)->setGeometry(QRectF(QPointF(leftMargin + x + centerOffset, topMargin + y), itemSize));
        x = next_x + spacing(Qt::Horizontal);
    }
    //qDebug() << "elapsed: " << t.elapsed();
    return GridInfo(columns, rows, topMargin + y + itemSize.height() + bottomMargin);
}

QSizeF FlowLayout::minSize(const QSizeF &constraint) const
{
    QSizeF size(0, 0);
    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    if (constraint.width() >= 0) {   // height for width
        const qreal height = doLayout(QRectF(QPointF(0,0), constraint), false).height;
        size = QSizeF(constraint.width(), height);
    } else if (constraint.height() >= 0) {  // width for height?
        // not supported
    } else {
        QGraphicsLayoutItem *item;
        foreach (item, m_items)
            size = size.expandedTo(item->effectiveSizeHint(Qt::MinimumSize));
        size += QSize(left + right, top + bottom);
    }
    return size;
}

QSizeF FlowLayout::prefSize() const
{
    qreal left, right;
    getContentsMargins(&left, 0, &right, 0);

    QGraphicsLayoutItem *item;
    qreal maxh = 0;
    qreal totalWidth = 0;
    foreach (item, m_items) {
        if (totalWidth > 0)
            totalWidth += spacing(Qt::Horizontal);
        QSizeF pref = item->effectiveSizeHint(Qt::PreferredSize);
        totalWidth += pref.width();
        maxh = qMax(maxh, pref.height());
    }
    maxh += spacing(Qt::Vertical);

    const qreal goldenAspectRatio = 1.61803399;
    qreal w = qSqrt(totalWidth * maxh * goldenAspectRatio) + left + right;

    return minSize(QSizeF(w, -1));
}

QSizeF FlowLayout::maxSize() const
{
    QGraphicsLayoutItem *item;
    qreal totalWidth = 0;
    qreal totalHeight = 0;
    foreach (item, m_items) {
        if (totalWidth > 0)
            totalWidth += spacing(Qt::Horizontal);
        if (totalHeight > 0)
            totalHeight += spacing(Qt::Vertical);
        QSizeF pref = item->effectiveSizeHint(Qt::PreferredSize);
        totalWidth += pref.width();
        totalHeight += pref.height();
    }

    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    return QSizeF(left + totalWidth + right, top + totalHeight + bottom);
}

QSizeF FlowLayout::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    QSizeF sh = constraint;
    switch (which) {
    case Qt::PreferredSize:
        sh = prefSize();
        break;
    case Qt::MinimumSize:
        sh = minSize(constraint);
        break;
    case Qt::MaximumSize:
        sh = maxSize();
        break;
    default:
        break;
    }
    return sh;
}
