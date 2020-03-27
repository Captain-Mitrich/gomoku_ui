#include "gview.h"
#include "gmodel.h"
#include <QtWidgets>
#include <QPainter>

void GIcon::setColor(const QColor &color)
{
  QPainter p(this);
  p.setCompositionMode(QPainter::CompositionMode_SourceIn);
  p.fillRect(rect(), color);
}

GView::GView(unsigned col_count, unsigned row_count, QWidget* parent) :
  GridView(col_count, row_count, parent)
{
  int min_cell_size = 9;
  setMinimumSize(min_cell_size * col_count + 1, min_cell_size * row_count + 1);
}

int GView::getIconSize() const
{
  return getCellSize(size()) - 1;
}

const QPixmap& GView::getIcon(Picture pic) const
{
  return (pic == X) ? xIcon : oIcon;
}

void GView::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::RightButton)
  {
    QWidget::mousePressEvent(event);
    return;
  }

  Cell cell;
  if (!getCellCoordinates(event->x(), event->y(), cell))
    return;

  emit clickCellSignal(cell);
}

void GView::resizeEvent(QResizeEvent *event)
{
  if (getCellSize(event->oldSize()) == getCellSize(event->size()))
    return;

  drawIcons();
  emit resizeSignal();
}

void GView::drawCell(QPainter& painter, const QRect& icon_rect, const CellDataPtr& data)
{
  GCellData& gdata = (GCellData&)(*data);

  unsigned shape_flag = gdata.flags & (gdata.BLACK|gdata.WHITE);
  assert(shape_flag != (gdata.BLACK|gdata.WHITE));

  if (shape_flag)
  {
    GIcon icon;
    (QPixmap&)icon = (shape_flag & gdata.BLACK) ? xIcon : oIcon;
    assert(icon.size() == icon_rect.size());

    unsigned color_flags = gdata.flags & (gdata.LAST|gdata.LINE5);
    if (color_flags)
      icon.setColor(palette().color(QPalette::Highlight));
    painter.drawPixmap(icon_rect, icon);
  }
}

void GView::drawIcons()
{
  drawIcon(X);
  drawIcon(O);
}

void GView::drawIcon(Picture pic)
{
  QPixmap& icon = (pic == X) ? xIcon : oIcon;

  int icon_size = getIconSize();
  assert(icon_size >= 0);

  (QPixmap&)icon = QPixmap(icon_size, icon_size);
  if (icon.width() < 2)
    return;

  icon.fill(Qt::transparent);

  int space = icon.width() / 6;

  QPainter p(&icon);
  p.setRenderHint(QPainter::Antialiasing, true);
  QPen pen;
  pen.setColor(palette().color(QPalette::Text));
  pen.setWidthF(1.3);
  p.setPen(pen);
  if (pic == X)
  {
    p.drawLine(space, space, icon_size - space, icon_size - space);
    p.drawLine(icon_size - space, space, space, icon_size - space);
  }
  else
    p.drawEllipse(space, space, icon_size - space * 2, icon_size - space * 2);
}
