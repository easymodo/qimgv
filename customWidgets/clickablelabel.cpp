#include "clickablelabel.h"

ClickableLabel::ClickableLabel ( QWidget *parent )
{
     this->setParent ( parent );
}

void ClickableLabel::mousePressEvent ( QMouseEvent *event )
{
     Q_UNUSED ( event )
     emit clicked();
}
