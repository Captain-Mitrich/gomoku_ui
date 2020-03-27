#ifndef GRIDVIEW_H
#define GRIDVIEW_H

#include <celldata.h>
#include <QWidget>

using uint = unsigned int;

class GridView : public QWidget
{
  Q_OBJECT
public:
  GridView(uint col_count, uint row_count, QWidget* parent = 0);

  bool isValidCell(const Cell& cell) const;

signals:
  void updateAllSignal(std::list<CellDataPtr>& cells_to_update);

public slots:
  void updateCell(const CellDataPtr data);

protected:
  int m_col_count, m_row_count;

  CellDataPtr m_cell_data;

  void paintEvent(QPaintEvent *event) override;

  virtual void drawCell(QPainter& painter, const QRect& cell_rect, const CellDataPtr& data) = 0;

  bool getCellCoordinates(int x, int y, Cell& cell) const;

  int getCellSize(QSize view_size) const;

  void getGridRect(QSize view_size, QRect& rect, int& cell_size) const;

  QRect getCellInsideRect(const Cell& cell, const QRect& grid_rect, int cell_size);

  QRect getCellInsideRect(const Cell& cell);
};

#endif // GRIDVIEW_H
