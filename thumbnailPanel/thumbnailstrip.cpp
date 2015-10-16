#include "thumbnailstrip.h"

ThumbnailStrip::ThumbnailStrip ( QWidget *parent )
     : QGraphicsView ( parent ),
       panelSize ( 122 ),
       current ( -1 )
{
     scrollBar = this->horizontalScrollBar();
     horizontalScrollBar()->setAttribute ( Qt::WA_NoMousePropagation, true );
     verticalScrollBar()->setAttribute ( Qt::WA_NoMousePropagation, true );
     this->setAttribute ( Qt::WA_NoMousePropagation, true );
     this->setFocusPolicy ( Qt::NoFocus );
     widget = new QGraphicsWidget();
     scene = new CustomScene();
     layout = new QGraphicsLinearLayout();
     timeLine = new QTimeLine ( SCROLL_ANIMATION_SPEED, this );
     timeLine->setCurveShape ( QTimeLine::EaseInOutCurve );
     widget->setLayout ( layout );
     layout->setSpacing ( 0 );
     scene->addItem ( widget );

     this->setScene ( scene );

     this->setFrameShape ( QFrame::NoFrame );

     readSettings();
     this->hide();

     loadTimer.setSingleShot ( true );

     connect ( scene, SIGNAL ( sceneClick ( QPointF ) ),
               this, SLOT ( sceneClicked ( QPointF ) ) );

     connect ( &loadTimer, SIGNAL ( timeout() ),
               this, SLOT ( loadVisibleThumbnails() ) );

     connect ( settings, SIGNAL ( settingsChanged() ),
               this, SLOT ( readSettings() ) );
}

void ThumbnailStrip::readSettings()
{
     position = settings->panelPosition();
     panelSize = settings->thumbnailSize() + 22;
     scrollBar->setValue ( 0 );

     for ( int i = 0; i < thumbnailLabels.count(); i++ ) {
          thumbnailLabels.at ( i )->applySettings();
     }

     disconnect ( timeLine, SIGNAL ( frameChanged ( int ) ), scrollBar, SLOT ( setValue ( int ) ) );
     disconnect ( scrollBar, SIGNAL ( valueChanged ( int ) ),
                  this, SLOT ( loadVisibleThumbnailsDelayed() ) );
     disconnect ( scrollBar, SIGNAL ( sliderMoved ( int ) ),
                  this, SLOT ( loadVisibleThumbnailsDelayed() ) );
     if ( position == LEFT || position == RIGHT ) {
          horizontalScrollBar()->setDisabled ( true );
          this->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
          this->setVerticalScrollBarPolicy ( Qt::ScrollBarAsNeeded );
          scrollBar = this->verticalScrollBar();
          layout->setOrientation ( Qt::Vertical );
          layout->setContentsMargins ( 0,2,0,2 );

     } else {
          this->setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
          this->setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded );
          scrollBar = this->horizontalScrollBar();
          layout->setOrientation ( Qt::Horizontal );
          layout->setContentsMargins ( 2,0,2,0 );
     }
     connect ( timeLine, SIGNAL ( frameChanged ( int ) ), scrollBar, SLOT ( setValue ( int ) ), Qt::UniqueConnection );
     connect ( scrollBar, SIGNAL ( valueChanged ( int ) ),
               this, SLOT ( loadVisibleThumbnailsDelayed() ), Qt::UniqueConnection );
     connect ( scrollBar, SIGNAL ( sliderMoved ( int ) ),
               this, SLOT ( loadVisibleThumbnailsDelayed() ), Qt::UniqueConnection );
     layout->invalidate();
     layout->activate();
     scene->setSceneRect ( layout->geometry() );
}

void ThumbnailStrip::fillPanel ( int count )
{
     current = -1;
     loadTimer.stop();
     populate ( count );
     loadVisibleThumbnailsDelayed();
}

void ThumbnailStrip::selectThumbnail ( int pos )
{
     if ( current != -1 ) {
          thumbnailLabels.at ( current )->setHighlighted ( false );
          thumbnailLabels.at ( current )->setOpacityAnimated ( OPACITY_INACTIVE, ANIMATION_SPEED_INSTANT );
     }
     thumbnailLabels.at ( pos )->setHighlighted ( true );
     thumbnailLabels.at ( pos )->setOpacityAnimated ( OPACITY_SELECTED, ANIMATION_SPEED_FAST );
     current = pos;
     if ( !childVisibleEntirely ( pos ) ) {
          centerOn ( thumbnailLabels.at ( pos )->scenePos() );
     }
     loadVisibleThumbnails();
}

void ThumbnailStrip::centerOnSmooth ( const QPointF &pos )
{
     Q_UNUSED ( pos )

     timeLine->setFrameRange ( 0, -1000 );
     timeLine->setUpdateInterval ( 16 );
     timeLine->start();
     //centerOn(pos);
}

void ThumbnailStrip::translateX ( int dx )
{
     translate ( ( qreal ) dx/10, 0 );
}

void ThumbnailStrip::populate ( int count )
{
     // this will fail if list items != layout items
     // shouldnt happen though
     for ( int i = layout->count()-1; i >= 0; --i ) {
          layout->removeAt ( 0 );
          delete thumbnailLabels.takeAt ( 0 );
     }
     thumbnailLabels.clear();

     for ( int i=0; i<count; i++ ) {
          addItem();
     }

     layout->invalidate();
     layout->activate();
     scene->setSceneRect ( layout->geometry() );
}

// in theory faster than scene's version
QRectF ThumbnailStrip::itemsBoundingRect()
{
     QRectF boundingRect ( 0,0,0,0 );
     if ( !thumbnailLabels.isEmpty() ) {
          boundingRect.setTopLeft (
               thumbnailLabels.at ( 0 )->sceneBoundingRect().topLeft() );
          boundingRect.setBottomRight (
               thumbnailLabels.at ( thumbnailLabels.count()-1 )->sceneBoundingRect().bottomRight() );
     }
     return boundingRect;
}

void ThumbnailStrip::loadVisibleThumbnailsDelayed()
{
     loadTimer.stop();
     loadTimer.start ( LOAD_DELAY );
}

void ThumbnailStrip::loadVisibleThumbnails()
{
     loadTimer.stop();
     updateVisibleRegion();

     for ( int i=0; i<thumbnailLabels.count(); i++ ) {
          requestThumbnailLoad ( i );
     }
}

void ThumbnailStrip::requestThumbnailLoad ( int pos )
{
     if ( thumbnailLabels.at ( pos )->state == EMPTY  && childVisible ( pos ) ) {
          thumbnailLabels.at ( pos )->state = LOADING;
          emit thumbnailRequested ( pos );
     }
}

void ThumbnailStrip::addItem()
{
     ThumbnailLabel *thumbLabel = new ThumbnailLabel();
     thumbLabel->setOpacityAnimated ( 0.0, ANIMATION_SPEED_INSTANT );
     thumbnailLabels.append ( thumbLabel );
     layout->addItem ( thumbLabel );
     requestThumbnailLoad ( thumbnailLabels.length()-1 );
}

void ThumbnailStrip::setThumbnail ( int pos, Thumbnail* thumb )
{
     thumbnailLabels.at ( pos )->setThumbnail ( thumb );
     thumbnailLabels.at ( pos )->state = LOADED;
     if ( pos != current ) {
          thumbnailLabels.at ( pos )->setOpacityAnimated ( OPACITY_INACTIVE, ANIMATION_SPEED_NORMAL );
     }
}

void ThumbnailStrip::updateVisibleRegion()
{
     QRect viewport_rect ( 0, 0, width(), height() );
     preloadArea = visibleRegion = mapToScene ( viewport_rect ).boundingRect();
     if ( layout->orientation() == Qt::Vertical ) {
          preloadArea.adjust ( 0,-OFFSCREEN_PRELOAD_AREA,0,OFFSCREEN_PRELOAD_AREA );
     } else {
          preloadArea.adjust ( -OFFSCREEN_PRELOAD_AREA,0,OFFSCREEN_PRELOAD_AREA,0 );
     }
}

bool ThumbnailStrip::childVisible ( int pos )
{
     if ( thumbnailLabels.count() > pos ) {
          return thumbnailLabels.at ( pos )->
                 sceneBoundingRect().intersects ( preloadArea );
     } else {
          return false;
     }
}

bool ThumbnailStrip::childVisibleEntirely ( int pos )
{
     if ( thumbnailLabels.count() > pos ) {
          return thumbnailLabels.at ( pos )->
                 sceneBoundingRect().intersects ( visibleRegion );
     } else {
          return false;
     }
}

void ThumbnailStrip::sceneClicked ( QPointF pos )
{
     ThumbnailLabel *item = ( ThumbnailLabel* ) scene->itemAt ( pos, QGraphicsView::transform() );
     int itemPos = thumbnailLabels.indexOf ( item );
     if ( itemPos!=-1 ) {
          emit thumbnailClicked ( itemPos );
     }
}

void ThumbnailStrip::wheelEvent ( QWheelEvent *event )
{
     event->accept();
     if ( timeLine->state() == QTimeLine::Running ) {
          timeLine->stop();
          timeLine->setFrameRange ( scrollBar->value(),
                                    timeLine->endFrame() -
                                    event->angleDelta().ry() * SCROLL_SPEED_MULTIPLIER );
          //scrollBar->setValue(timeLine->endFrame());
     } else {
          timeLine->setFrameRange ( scrollBar->value(),
                                    scrollBar->value() -
                                    event->angleDelta().ry() * SCROLL_SPEED_MULTIPLIER );
     }
     timeLine->setUpdateInterval ( SCROLL_UPDATE_RATE );
     timeLine->start();
}

void ThumbnailStrip::parentResized ( QSize parentSz )
{
     if ( position == BOTTOM ) {
          this->setGeometry ( QRect ( QPoint ( 0, parentSz.height() - panelSize + 1 ),
                                      QPoint ( parentSz.width(), parentSz.height() ) ) );
     } else if ( position == LEFT ) {
          this->setGeometry ( QRect ( QPoint ( 0, 0 ),
                                      QPoint ( panelSize, parentSz.height() ) ) );
     } else if ( position == RIGHT ) {
          this->setGeometry ( QRect ( QPoint ( parentSz.width() - panelSize, 0 ),
                                      QPoint ( parentSz.width(), parentSz.height() ) ) );
     } else if ( position == TOP ) {
          this->setGeometry ( QRect ( QPoint ( 0, 0 ),
                                      QPoint ( parentSz.width(), panelSize ) ) );
     }
     loadVisibleThumbnailsDelayed();
}

void ThumbnailStrip::leaveEvent ( QEvent *event )
{
     Q_UNUSED ( event )
     hide();
}

ThumbnailStrip::~ThumbnailStrip()
{
}
