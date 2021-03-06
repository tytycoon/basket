/***************************************************************************
 *   Copyright (C) 2003 by Sébastien Laoût                                 *
 *   slaout@linux62.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "basketlistview.h"
#include <qregexp.h>
//Added by qt3to4:
#include <QDragLeaveEvent>
#include <QPixmap>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QResizeEvent>
#include <QFocusEvent>
#include <kiconloader.h>
#include <klocale.h>
#include <kstringhandler.h>
#include <qpainter.h>
#include <qbitmap.h>
#include <qpixmapcache.h>
#include <qtooltip.h>
#include <kdebug.h>
#include "global.h"
#include "bnpview.h"
#include "basket.h"
#include "tools.h"
#include "settings.h"
#include "notedrag.h"

/** class BasketListViewItem: */

BasketListViewItem::BasketListViewItem(Q3ListView *parent, Basket *basket)
	: Q3ListViewItem(parent), m_basket(basket)
	, m_isUnderDrag(false)
	, m_isAbbreviated(false)
{
	setDropEnabled(true);
}

BasketListViewItem::BasketListViewItem(Q3ListViewItem *parent, Basket *basket)
	: Q3ListViewItem(parent), m_basket(basket)
	, m_isUnderDrag(false)
	, m_isAbbreviated(false)
{
	setDropEnabled(true);
}

BasketListViewItem::BasketListViewItem(Q3ListView *parent, Q3ListViewItem *after, Basket *basket)
	: Q3ListViewItem(parent, after), m_basket(basket)
	, m_isUnderDrag(false)
	, m_isAbbreviated(false)
{
	setDropEnabled(true);
}

BasketListViewItem::BasketListViewItem(Q3ListViewItem *parent, Q3ListViewItem *after, Basket *basket)
	: Q3ListViewItem(parent, after), m_basket(basket)
	, m_isUnderDrag(false)
	, m_isAbbreviated(false)
{
	setDropEnabled(true);
}

BasketListViewItem::~BasketListViewItem()
{
}

bool BasketListViewItem::acceptDrop(const QMimeSource *) const
{
	kDebug() << "accept";
	return true;
}

void BasketListViewItem::dropped(QDropEvent *event)
{
	kDebug() << "Dropping into basket " << m_basket->name();
	m_basket->contentsDropEvent(event);
	//Global::bnpView->currentBasket()->contentsDropEvent(event); // FIXME
}

int BasketListViewItem::width(const QFontMetrics &/* fontMetrics */, const Q3ListView */*listView*/, int /* column */) const
{
	return listView()->visibleWidth() + 100;
/*
	int BASKET_ICON_SIZE = 16;
	int MARGIN = 1;

	QRect textRect = fontMetrics.boundingRect(0, 0, / *width=* /1, 500000, Qt::AlignLeft | Qt::AlignTop | Qt::TextShowMnemonic, text(column));

	return MARGIN + BASKET_ICON_SIZE + MARGIN + textRect.width() +   BASKET_ICON_SIZE/2   + MARGIN;
*/
}

QString BasketListViewItem::escapedName(const QString &string)
{
    // Underlining the Alt+Letter shortcut (and escape all other '&' characters), if any:
    QString basketName = string;
    basketName.replace('&', "&&"); // First escape all the amperstamp
    QString letter;
    QRegExp letterExp("^Alt\\+(?:Shift\\+)?(.)$");

    QString basketShortcut = m_basket->shortcut().primary().toString();
    if (letterExp.indexIn(basketShortcut) != -1) {
        int index;
        letter = letterExp.cap(1);
        if ((index = basketName.indexOf(letter)) != -1)
            basketName.insert(index, '&');
    }

    return basketName;
}

void BasketListViewItem::setup()
{
	int BASKET_ICON_SIZE = 16;
	int MARGIN = 1;

	setText(/*column=*/0, escapedName(m_basket->basketName()));

	widthChanged();
	QRect textRect = listView()->fontMetrics().boundingRect(0, 0, /*width=*/1, 500000, Qt::AlignLeft | Qt::AlignTop | Qt::TextShowMnemonic, text(/*column=*/0));

	int height = MARGIN + qMax(BASKET_ICON_SIZE, textRect.height()) + MARGIN;
	setHeight(height);

    QPixmap icon = KIconLoader::global()->loadIcon(
        m_basket->icon(), KIconLoader::NoGroup, 16, KIconLoader::DefaultState,
        QStringList(), 0L, /*canReturnNull=*/false
        );

	setPixmap(/*column=*/0, icon);

	repaint();
}

BasketListViewItem* BasketListViewItem::lastChild()
{
	Q3ListViewItem *child = firstChild();
	while (child) {
		if (child->nextSibling())
			child = child->nextSibling();
		else
			return (BasketListViewItem*)child;
	}
	return 0;
}

BasketListViewItem* BasketListViewItem::prevSibling()
{
	BasketListViewItem *item = this;
	while (item) {
		if (item->nextSibling() == this)
			return item;
		item = (BasketListViewItem*)(item->itemAbove());
	}
	return 0;
}

BasketListViewItem* BasketListViewItem::shownItemAbove()
{
	BasketListViewItem *item = (BasketListViewItem*)itemAbove();
	while (item) {
		if (item->isShown())
			return item;
		item = (BasketListViewItem*)(item->itemAbove());
	}
	return 0;
}

BasketListViewItem* BasketListViewItem::shownItemBelow()
{
	BasketListViewItem *item = (BasketListViewItem*)itemBelow();
	while (item) {
		if (item->isShown())
			return item;
		item = (BasketListViewItem*)(item->itemBelow());
	}
	return 0;
}

QStringList BasketListViewItem::childNamesTree(int deep)
{
	QStringList result;
	for (Q3ListViewItem *child = firstChild(); child; child = child->nextSibling()) {
		BasketListViewItem *item = (BasketListViewItem*)child;
		// Compute indentation spaces:
		QString spaces;
		for (int i = 0; i < deep; ++i)
			spaces += "  ";
		// Append the name:
		result.append(spaces + item->basket()->basketName());
		// Append the childs:
		if (child->firstChild()) {
			QStringList childs = item->childNamesTree(deep + 1);
			for (QStringList::iterator it = childs.begin(); it != childs.end(); ++it)
				result.append(*it);
		}
	}
	return result;
}

void BasketListViewItem::moveChildsBaskets()
{
	Q3ListViewItem *insertAfterThis = this;
	Q3ListViewItem *nextOne;
	for (Q3ListViewItem *child = firstChild(); child; child = nextOne) {
		nextOne = child->nextSibling();
		// Re-insert the item with the good parent:
		takeItem(child);
		if (parent())
			parent()->insertItem(child);
		else
			listView()->insertItem(child);
		// And move it at the good place:
		child->moveItem(insertAfterThis);
		insertAfterThis = child;
	}
}

void BasketListViewItem::ensureVisible()
{
	BasketListViewItem *item = this;
	while (item->parent()) {
		item = (BasketListViewItem*)(item->parent());
		item->setOpen(true);
	}
}

bool BasketListViewItem::isShown()
{
	Q3ListViewItem *item = parent();
	while (item) {
		if (!item->isOpen())
			return false;
		item = item->parent();
	}
	return true;
}

bool BasketListViewItem::isCurrentBasket()
{
	return basket() == Global::bnpView->currentBasket();
}

// TODO: Move this function from item.cpp to class Tools:
extern void drawGradient( QPainter *p, const QColor &colorTop, const QColor & colorBottom,
						  int x, int y, int w, int h,
						  bool sunken, bool horz, bool flat  ); /*const*/

QPixmap BasketListViewItem::circledTextPixmap(const QString &text, int height, const QFont &font, const QColor &color)
{
	QString key = QString("BLI-%1.%2.%3.%4")
		.arg(text).arg(height).arg(font.toString()).arg(color.rgb());
	if (QPixmap* cached=QPixmapCache::find(key)) {
		return *cached;
	}

	// Compute the sizes of the image components:
	QRect textRect = QFontMetrics(font).boundingRect(0, 0, /*width=*/1, height, Qt::AlignLeft | Qt::AlignTop, text);
	int xMargin = height / 6;
	int width   = xMargin + textRect.width() + xMargin;

	// Create the gradient image:
	QPixmap gradient(3 * width, 3 * height); // We double the size to be able to smooth scale down it (== antialiased curves)
	QPainter gradientPainter(&gradient);
#if 1 // Enable the new look of the gradient:
	QColor topColor       = palette().color(QPalette::Highlight).lighter(130); //120
	QColor topMidColor    = palette().color(QPalette::Highlight).lighter(105); //105
	QColor bottomMidColor = palette().color(QPalette::Highlight).darker(130);  //120
	QColor bottomColor    = palette().color(QPalette::Highlight);
	drawGradient(&gradientPainter, topColor, topMidColor,
				  0, 0, gradient.width(), gradient.height() / 2, /*sunken=*/false, /*horz=*/true, /*flat=*/false);
	drawGradient(&gradientPainter, bottomMidColor, bottomColor,
				  0, gradient.height() / 2, gradient.width(), gradient.height() - gradient.height() / 2, /*sunken=*/false, /*horz=*/true, /*flat=*/false);
	gradientPainter.fillRect(0, 0, gradient.width(), 3, palette().color(QPalette::Highlight));
#else
	drawGradient(&gradientPainter, palette().color(QPalette::Highlight), palette().color(QPalette::Highlight).darker(),
				  0, 0, gradient.width(), gradient.height(), /*sunken=*/false, /*horz=*/true, /*flat=*/false);
#endif
	gradientPainter.end();

	// Draw the curved rectangle:
	QBitmap curvedRectangle(3 * width, 3 * height);
	curvedRectangle.fill(Qt::color0);
	QPainter curvePainter(&curvedRectangle);
	curvePainter.setPen(Qt::color1);
	curvePainter.setBrush(Qt::color1);
	curvePainter.setClipRect(0, 0, 3*(height / 5), 3*(height)); // If the width is small, don't fill the right part of the pixmap
	curvePainter.drawEllipse(0, 3*(-height / 4), 3*(height), 3*(height * 3 / 2)); // Don't forget we double the sizes
	curvePainter.setClipRect(3*(width - height / 5), 0, 3*(height / 5), 3*(height));
	curvePainter.drawEllipse(3*(width - height), 3*(-height / 4), 3*(height), 3*(height * 3 / 2));
	curvePainter.setClipping(false);
	curvePainter.fillRect(3*(height / 6), 0, 3*(width - 2 * height / 6), 3*(height), curvePainter.brush());
	curvePainter.end();

	// Apply the curved rectangle as the mask of the gradient:
	gradient.setMask(curvedRectangle);
	QImage resultImage = gradient.convertToImage();
	resultImage.setAlphaBuffer(true);

	// Scale down the image smoothly to get anti-aliasing:
	QPixmap pmScaled;
	pmScaled.convertFromImage(resultImage.smoothScale(width, height));

	// Draw the text, and return the result:
	QPainter painter(&pmScaled);
	painter.setPen(color);
	painter.setFont(font);
	painter.drawText(0+1, 0, width, height, Qt::AlignHCenter | Qt::AlignVCenter, text);
	painter.end();

	QPixmapCache::insert(key, pmScaled);

	return pmScaled;
}

QPixmap BasketListViewItem::foundCountPixmap(bool isLoading, int countFound, bool childsAreLoading, int countChildsFound, const QFont &font, int height)
{
	if (isLoading)
		return QPixmap();

	QFont boldFont(font);
	boldFont.setBold(true);

	QString text;
	if (childsAreLoading) {
		if (countChildsFound > 0)
			text = i18n("%1+%2+", QString::number(countFound), QString::number(countChildsFound));
		else
			text = i18n("%1+", QString::number(countFound));
	} else {
		if (countChildsFound > 0)
			text = i18n("%1+%2", QString::number(countFound), QString::number(countChildsFound));
		else if (countFound > 0)
			text = QString::number(countFound);
		else
			return QPixmap();
	}

	return circledTextPixmap(text, height, boldFont, palette().color(QPalette::HighlightedText));
}

bool BasketListViewItem::haveChildsLoading()
{
	Q3ListViewItem *child = firstChild();
	while (child) {
		BasketListViewItem *childItem = (BasketListViewItem*)child;
		if (!childItem->basket()->isLoaded() && !childItem->basket()->isLocked())
			return true;
		if (childItem->haveChildsLoading())
			return true;
		child = child->nextSibling();
	}
	return false;
}

bool BasketListViewItem::haveHiddenChildsLoading()
{
	if (isOpen())
		return false;
	return haveChildsLoading();
}

bool BasketListViewItem::haveChildsLocked()
{
	Q3ListViewItem *child = firstChild();
	while (child) {
		BasketListViewItem *childItem = (BasketListViewItem*)child;
		if (/*!*/childItem->basket()->isLocked())
			return true;
		if (childItem->haveChildsLocked())
			return true;
		child = child->nextSibling();
	}
	return false;
}

bool BasketListViewItem::haveHiddenChildsLocked()
{
	if (isOpen())
		return false;
	return haveChildsLocked();
}

int BasketListViewItem::countChildsFound()
{
	int count = 0;
	Q3ListViewItem *child = firstChild();
	while (child) {
		BasketListViewItem *childItem = (BasketListViewItem*)child;
		count += childItem->basket()->countFounds();
		count += childItem->countChildsFound();
		child = child->nextSibling();
	}
	return count;
}

int BasketListViewItem::countHiddenChildsFound()
{
	if (isOpen())
		return 0;
	return countChildsFound();
}

void BasketListViewItem::paintCell(QPainter *painter, const QColorGroup &/*colorGroup*/, int /*column*/, int width, int /*align*/)
{
	// Workaround a Qt bug:
	// When the splitter is moved to hide the tree view and then the application is restarted,
	// Qt try to draw items with a negative size!
	if (width <= 0) {
		kDebug() << "width <= 0";
		return;
	}

	int BASKET_ICON_SIZE = 16;
	int MARGIN = 1;


	// If we are filtering all baskets, and are effectively filtering on something:
	bool showLoadingIcon = false;
	bool showEncryptedIcon = false;
	QPixmap countPixmap;
	bool showCountPixmap = Global::bnpView->isFilteringAllBaskets() &&
		Global::bnpView->currentBasket()->decoration()->filterBar()->filterData().isFiltering;
	if (showCountPixmap) {
		showLoadingIcon = (!m_basket->isLoaded() && !m_basket->isLocked()) || haveHiddenChildsLoading();
		showEncryptedIcon = m_basket->isLocked() || haveHiddenChildsLocked();
		countPixmap = foundCountPixmap(!m_basket->isLoaded(), m_basket->countFounds(), haveHiddenChildsLoading() || haveHiddenChildsLocked(),
										countHiddenChildsFound(), listView()->font(), height() - 2 * MARGIN);
	}
	int effectiveWidth = width - (countPixmap.isNull() ? 0 : countPixmap.width() + MARGIN)
			- (showLoadingIcon || showEncryptedIcon ? BASKET_ICON_SIZE + MARGIN : 0)/*
			- (showEncryptedIcon ? BASKET_ICON_SIZE + MARGIN : 0)*/;


	bool drawRoundRect = m_basket->backgroundColorSetting().isValid() || m_basket->textColorSetting().isValid();
	QColor textColor = (drawRoundRect ?
                        m_basket->textColor() :
                        (isCurrentBasket() ?
                         palette().color(QPalette::HighlightedText) :
                         palette().color(QPalette::Text)));

	BasketListViewItem *shownAbove = shownItemAbove();
	BasketListViewItem *shownBelow = shownItemBelow();

	// Don't forget to update the key computation if parameters
	// affecting the rendering logic change
	QString key = QString("BLVI::pC-%1.%2.%3.%4.%5.%6.%7.%8.%9.%10.%11.%12.%13.%14.%15")
		.arg(effectiveWidth)
		.arg(drawRoundRect)
		.arg(textColor.rgb())
		.arg(m_basket->backgroundColor().rgb())
		.arg(isCurrentBasket())
		.arg(shownBelow && shownBelow->isCurrentBasket())
		.arg(shownAbove && shownAbove->isCurrentBasket())
		.arg(showLoadingIcon)
		.arg(showEncryptedIcon)
		.arg(showCountPixmap)
		.arg(m_basket->countFounds())
		.arg(countHiddenChildsFound())
		.arg(m_isUnderDrag)
		.arg(m_basket->basketName())
		.arg(m_basket->icon());
	if (QPixmap* cached = QPixmapCache::find(key)) {
		// Qt's documentation recommends copying the pointer
		// into a QPixmap immediately
		QPixmap cachedBuffer = *cached;
		painter->drawPixmap(0, 0, cachedBuffer);
		return;
	}

	// Bufferize the drawing of items (otherwize, resizing the splitter make the tree act like a Christmas Tree ;-D ):
	QPixmap theBuffer(width, height());
	QPainter thePainter(&theBuffer);
	QWidget *viewport  = listView()->viewport();

	// Fill with the basket background color:
	QColor background = palette().color(QPalette::Highlight);

	if (!isCurrentBasket())
		background = viewport->palette().color(viewport->backgroundRole());

	thePainter.fillRect(0, 0, width, height(), background);

	int textWidth = effectiveWidth - MARGIN - BASKET_ICON_SIZE - MARGIN - MARGIN;

	// Draw the rounded rectangle:
	if (drawRoundRect) {
		QRect textRect = listView()->fontMetrics().boundingRect(0, 0, /*width=*/1, 500000, Qt::AlignLeft | Qt::AlignTop | Qt::TextShowMnemonic, text(/*column=*/0));
		int xRound = MARGIN;
		int yRound = MARGIN;
		int hRound = height() - 2 * MARGIN;
		int wRound = qMin(BASKET_ICON_SIZE + MARGIN + textRect.width() + hRound/2,  effectiveWidth - MARGIN - MARGIN);
		if (wRound > 0) { // Do not crash if there is no space anymore to draw the rounded rectangle:
			QPixmap buffer(wRound * 2, hRound * 2);
			buffer.fill(background);
			QPainter pBuffer(&buffer);
			QColor colorRound = m_basket->backgroundColor();
			pBuffer.setPen(colorRound);
			pBuffer.setBrush(colorRound);
			if (wRound > hRound) { // If the rectangle is smaller in width than in height, don't overlap ellipses...
				pBuffer.drawEllipse(0,                                 0, hRound * 2, hRound * 2);
				pBuffer.drawEllipse(wRound * 2 - hRound * 2, 0, hRound * 2, hRound * 2);
				pBuffer.fillRect(hRound*2/2, 0, wRound * 2 - hRound * 2, hRound * 2, colorRound);
			} else
				pBuffer.drawEllipse(0, 0, wRound * 2, hRound * 2);
				pBuffer.end();
				QImage imageToScale = buffer.convertToImage();
				QPixmap pmScaled;
				pmScaled.convertFromImage(imageToScale.smoothScale(wRound, hRound));
				thePainter.drawPixmap(xRound, yRound, pmScaled);
				textWidth -= hRound/2;
		}
	}

	QColor bgColor  = viewport->palette().color(viewport->backgroundRole());
	QColor selColor = palette().color(QPalette::Highlight);
	QColor midColor = Tools::mixColor(bgColor, selColor);
	// Draw the left selection roundings:
	if (isCurrentBasket()) {
		thePainter.setPen(bgColor);
		thePainter.drawPoint(0, 0);
		thePainter.drawPoint(1, 0);
		thePainter.drawPoint(0, 1);
		thePainter.drawPoint(0, height() - 1);
		thePainter.drawPoint(1, height() - 1);
		thePainter.drawPoint(0, height() - 2);
		thePainter.setPen(midColor);
		thePainter.drawPoint(2, 0);
		thePainter.drawPoint(0, 2);
		thePainter.drawPoint(2, height() - 1);
		thePainter.drawPoint(0, height() - 3);
	}
	// Draw the bottom-right selection roundings:
	//BasketListViewItem *shownBelow = shownItemBelow();
	if (shownBelow && shownBelow->isCurrentBasket()) {
		thePainter.setPen(selColor);
		thePainter.drawPoint(width - 1, height() - 1);
		thePainter.drawPoint(width - 2, height() - 1);
		thePainter.drawPoint(width - 1, height() - 2);
		thePainter.setPen(midColor);
		thePainter.drawPoint(width - 3, height() - 1);
		thePainter.drawPoint(width - 1, height() - 3);
	}
	// Draw the top-right selection roundings:
	//	BasketListViewItem *shownAbove = shownItemAbove();
	if (shownAbove && shownAbove->isCurrentBasket()) {
		thePainter.setPen(selColor);
		thePainter.drawPoint(width - 1, 0);
		thePainter.drawPoint(width - 2, 0);
		thePainter.drawPoint(width - 1, 1);
		thePainter.setPen(midColor);
		thePainter.drawPoint(width - 3, 0);
		thePainter.drawPoint(width - 1, 2);
	}

	// Draw the icon and text:
	int yPixmap = (height() - BASKET_ICON_SIZE) / 2;
	thePainter.drawPixmap(MARGIN, yPixmap, *pixmap(/*column=*/0));
	thePainter.setPen(textColor);
	if (textWidth > 0) { // IF there is space left to draw the text:
		int xText = MARGIN + BASKET_ICON_SIZE + MARGIN;
        QFontMetrics fm = painter->fontMetrics();
		QString theText = m_basket->basketName();
		if (fm.width(theText) > textWidth) {
			theText = fm.elidedText(theText, Qt::ElideRight, textWidth);
			m_isAbbreviated = true;
		}
		else {
			m_isAbbreviated = false;
		}
		theText = escapedName(theText);
		thePainter.drawText(xText, 0, textWidth, height(), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, theText);
	}

	// If we are filtering all baskets, and are effectively filtering on something:
	if (!countPixmap.isNull())
	{
		thePainter.drawPixmap(effectiveWidth, 1, countPixmap);
		effectiveWidth += countPixmap.width() + MARGIN;
	}
	if (showLoadingIcon) {
		QPixmap icon = KIconLoader::global()->loadIcon(
            "edit-find", KIconLoader::NoGroup, 16, KIconLoader::DefaultState,
            QStringList(), 0L, /*canReturnNull=*/false
            );
		thePainter.drawPixmap(effectiveWidth, 0, icon);
		effectiveWidth += BASKET_ICON_SIZE + MARGIN;
	}
	if (showEncryptedIcon && !showLoadingIcon) {
		QPixmap icon = KIconLoader::global()->loadIcon(
            "encrypted", KIconLoader::NoGroup, 16, KIconLoader::DefaultState,
            QStringList(), 0L, /*canReturnNull=*/false);
		thePainter.drawPixmap(effectiveWidth, 0, icon);
	}

	if (m_isUnderDrag) {
		QStyleOption opt;
		opt.initFrom(listView());
		opt.rect = QRect(0, 0, width, height());
		listView()->style()->drawPrimitive(QStyle::PE_FrameFocusRect, &opt,
										   &thePainter);
	}
	thePainter.end();

	QPixmapCache::insert(key, theBuffer);
	// Apply the buffer:
	painter->drawPixmap(0, 0, theBuffer);
}

void BasketListViewItem::setUnderDrag(bool underDrag)
{
	m_isUnderDrag = underDrag;
}

bool BasketListViewItem::isAbbreviated()
{
	return m_isAbbreviated;
}

/** class BasketTreeListView: */

BasketTreeListView::BasketTreeListView(QWidget *parent)
	: K3ListView(parent), m_autoOpenItem(0)
	, m_itemUnderDrag(0)
{
	connect( &m_autoOpenTimer, SIGNAL(timeout()), this, SLOT(autoOpen()) );
}

bool BasketTreeListView::event(QEvent *e)
{
	if (e->type() == QEvent::ToolTip) {
		QHelpEvent *he = static_cast<QHelpEvent *>(e);
		Q3ListViewItem *item = itemAt(contentsToViewport(he->pos()));
		BasketListViewItem* bitem = dynamic_cast<BasketListViewItem*>(item);
		if (bitem && bitem->isAbbreviated()) {
			QRect rect = itemRect(bitem);
			QToolTip::showText(rect.topLeft(), bitem->basket()->basketName(),
							   viewport(), rect);
		}
		return true;
	}
	return K3ListView::event(e);
}

void BasketTreeListView::viewportResizeEvent(QResizeEvent *event)
{
	K3ListView::viewportResizeEvent(event);
	triggerUpdate();
}

void BasketTreeListView::contentsDragEnterEvent(QDragEnterEvent *event)
{
	if (event->provides("application/x-qlistviewitem")) {
		Q3ListViewItemIterator it(this); // TODO: Don't show expanders if it's not a basket drag...
		while (it.current()) {
			Q3ListViewItem *item = it.current();
			if (!item->firstChild()) {
				item->setExpandable(true);
				item->setOpen(true);
			}
			++it;
		}
		update();
	}

	K3ListView::contentsDragEnterEvent(event);
}

void BasketTreeListView::removeExpands()
{
	Q3ListViewItemIterator it(this);
	while (it.current()) {
		Q3ListViewItem *item = it.current();
		if (!item->firstChild())
			item->setExpandable(false);
		++it;
	}
	viewport()->update();
}

void BasketTreeListView::contentsDragLeaveEvent(QDragLeaveEvent *event)
{
	kDebug() << "BasketTreeListView::contentsDragLeaveEvent";
	m_autoOpenItem = 0;
	m_autoOpenTimer.stop();
	setItemUnderDrag(0);
	removeExpands();
	K3ListView::contentsDragLeaveEvent(event);
}

void BasketTreeListView::contentsDropEvent(QDropEvent *event)
{
	kDebug() << "BasketTreeListView::contentsDropEvent()";
	if (event->provides("application/x-qlistviewitem"))
	{
		K3ListView::contentsDropEvent(event);
	}
	else {
		kDebug() << "Forwarding dropped data to the basket";
		Q3ListViewItem *item = itemAt(contentsToViewport(event->pos()));
		BasketListViewItem* bitem = dynamic_cast<BasketListViewItem*>(item);
		if (bitem) {
			bitem->basket()->blindDrop(event);
		}
		else {
			kDebug() << "Forwarding failed: no bitem found";
		}
	}

	m_autoOpenItem = 0;
	m_autoOpenTimer.stop();
	setItemUnderDrag(0);
	removeExpands();

	Global::bnpView->save(); // TODO: Don't save if it was not a basket drop...
}

void BasketTreeListView::contentsDragMoveEvent(QDragMoveEvent *event)
{
	kDebug() << "BasketTreeListView::contentsDragMoveEvent";
	if (event->provides("application/x-qlistviewitem"))
		K3ListView::contentsDragMoveEvent(event);
	else {
		Q3ListViewItem *item = itemAt(contentsToViewport(event->pos()));
		BasketListViewItem* bitem = dynamic_cast<BasketListViewItem*>(item);
		if (m_autoOpenItem != item) {
			m_autoOpenItem = item;
			m_autoOpenTimer.start(1700, /*singleShot=*/true);
		}
		if (item) {
			event->acceptAction(true);
			event->accept(true);
		}
		setItemUnderDrag(bitem);

		K3ListView::contentsDragMoveEvent(event); // FIXME: ADDED
	}
}

void BasketTreeListView::setItemUnderDrag(BasketListViewItem* item)
{
	if (m_itemUnderDrag != item) {
		if (m_itemUnderDrag) {
			// Remove drag status from the old item
			m_itemUnderDrag->setUnderDrag(false);
			repaintItem(m_itemUnderDrag);
		}

		m_itemUnderDrag = item;

		if (m_itemUnderDrag) {
			// add drag status to the new item
			m_itemUnderDrag->setUnderDrag(true);
			repaintItem(m_itemUnderDrag);
		}
	}
}

void BasketTreeListView::autoOpen()
{
	BasketListViewItem *item = (BasketListViewItem*)m_autoOpenItem;
	if (item)
		Global::bnpView->setCurrentBasket(item->basket());
}

void BasketTreeListView::resizeEvent(QResizeEvent *event)
{
	K3ListView::resizeEvent(event);
}

void BasketTreeListView::paintEmptyArea(QPainter *painter, const QRect &rect)
{
	Q3ListView::paintEmptyArea(painter, rect);

	BasketListViewItem *last = Global::bnpView->lastListViewItem();
	if (last && !last->isShown())
		last = last->shownItemAbove();
	if (last && last->isCurrentBasket()) {
		int y = last->itemPos() + last->height();
		QColor bgColor  = viewport()->palette().color(viewport()->backgroundRole());
		QColor selColor = palette().color(QPalette::Highlight);
		QColor midColor = Tools::mixColor(bgColor, selColor);
		painter->setPen(selColor);
		painter->drawPoint(visibleWidth() - 1, y);
		painter->drawPoint(visibleWidth() - 2, y);
		painter->drawPoint(visibleWidth() - 1, y + 1);
		painter->setPen(midColor);
		painter->drawPoint(visibleWidth() - 3, y);
		painter->drawPoint(visibleWidth() - 1, y + 2);
	}
}

/** We should NEVER get focus (because of QWidget::NoFocus focusPolicy())
 * but K3ListView can programatically give us the focus.
 * So we give it to the basket.
 */
void BasketTreeListView::focusInEvent(QFocusEvent*)
{
	//K3ListView::focusInEvent(event);
	Basket *basket = Global::bnpView->currentBasket();
	if (basket)
		basket->setFocus();
}

