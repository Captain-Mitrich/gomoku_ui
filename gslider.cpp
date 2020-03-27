#include "gslider.h"
#include <QPainter>
#include <QMouseEvent>

void GSlider::paintEvent(QPaintEvent* /*event*/)
{
  QPainter p(this);

  int range = getRange();

  int segment_width = width() / range;

  int line_width = segment_width * (range - 1) + 1;

  int left = (width() - line_width) / 2 - 1;

  int right = left + line_width - 1;

  int cy = height() / 2;

  p.drawLine(left, cy, right, cy);

  int slider_size = std::min(getSliderSize(), segment_width);

  int grow_step = slider_size / (2 * range);

  for (int x = right, tick_height = slider_size / 2, val = maximum(); ; x -= segment_width, tick_height -= grow_step, --val)
  {
    assert(x >= left);
    if (val != value())
      p.drawLine(x, cy, x, std::max(cy - tick_height, 0));
    if (x == left)
    {
      assert(val == minimum());
      break;
    }
  }

  if (segment_width < 2)
    return;

  p.setRenderHint(QPainter::Antialiasing, true);

  QPen pen(palette().color(QPalette::ButtonText));
  pen.setWidthF(1.3);
  p.setPen(pen);

  QRect sliderRect(left + segment_width * value() - slider_size / 2, cy - slider_size / 2, slider_size, slider_size);
  QPainterPath path;
  path.addEllipse(sliderRect);
  p.fillPath(path, palette().color(QPalette::Button));
  p.drawEllipse(sliderRect);
}

void GSlider::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::RightButton)
  {
    QWidget::mousePressEvent(event);
    return;
  }

  assert(event->x() < width());
  int val = (getRange()) * event->x() / width();
  assert(val <= maximum());
  if (val != value())
    setValue(val);
}
