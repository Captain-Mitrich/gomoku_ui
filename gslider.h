#ifndef GSLIDER_H
#define GSLIDER_H

#include <QAbstractSlider>

class GSlider : public QAbstractSlider
{
public:
  GSlider(QWidget* parent) : QAbstractSlider(parent), m_slider_size(-1)
  {
    setOrientation(Qt::Horizontal);
  }

  int getRange()
  {
    return maximum() - minimum() + 1;
  }

  void setSliderSize(int size)
  {
    m_slider_size = size;
    setMinimumSize(size + 1, size + 1);
  }

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;

  int getSliderSize()
  {
    return (m_slider_size > 0) ? m_slider_size : std::min(width(), height());
  }

protected:
  int m_slider_size;
};

#endif // GSLIDER_H
