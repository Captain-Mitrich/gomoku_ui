#ifndef GVIEW
#define GVIEW

#include "gridview.h"

class GIcon : public QPixmap
{
public:
    void setColor(const QColor& color);
};

class GCellData;

class GView : public GridView
{
  Q_OBJECT

public:

  using Picture = const unsigned;

  static Picture X = 0;
  static Picture O = 1;

  GView(unsigned col_count, unsigned row_count, QWidget* parent = 0);

  int getIconSize() const;

  const QPixmap& getIcon(Picture pic) const;

signals:
  void clickCellSignal(const Cell& cell);
  void resizeSignal();

protected:
  QPixmap xIcon, oIcon;

  void mousePressEvent(QMouseEvent *event) override;

  void resizeEvent(QResizeEvent *event) override;

  void drawCell(QPainter &painter, const QRect& icon_rect, const CellDataPtr& data) override;

  void drawIcons();

  void drawIcon(Picture pic);
};

#endif // GVIEW

