#include "gridview.h"
#include <QPainter>
#include <QPaintEvent>

GridView::GridView(uint col_count, uint row_count, QWidget* parent) :
  QWidget(parent),
  m_col_count((int)col_count),
  m_row_count((int)row_count)
{
  assert(col_count > 0 && row_count > 0);
}

bool GridView::isValidCell(const Cell& cell) const
{
  return cell.col >= 0 && cell.col <= m_col_count && cell.row >= 0 && cell.row <= m_row_count;
}

bool GridView::getCellCoordinates(int x, int y, Cell& cell) const
{
  int cell_size;
  QRect grect;
  getGridRect(size(), grect, cell_size);

  if (cell_size <= 0)
    return false;

  cell.col = (x - grect.left()) / cell_size;
  cell.row = (y - grect.top()) / cell_size;

  return isValidCell(cell);
}

void GridView::updateCell(CellDataPtr data)
{
  m_cell_data = std::move(data);
  repaint(getCellInsideRect(*m_cell_data));
}

void GridView::paintEvent(QPaintEvent* event)
{
  int cell_size;
  QRect grid_rect;
  getGridRect(size(), grid_rect, cell_size);

  QPainter painter(this);

  QRect inside_rect;
  if (m_cell_data && event->rect() == (inside_rect = getCellInsideRect(*m_cell_data, grid_rect, cell_size)))
  {
    drawCell(painter, inside_rect, m_cell_data);
    m_cell_data.reset();
  }
  else
  {
    if (width() <= m_col_count || height() <= m_row_count)
      return;

    painter.setPen(palette().color(QPalette::WindowText));

    //draw grid
    for (int x = grid_rect.left(); ; x += cell_size)
    {
      painter.drawLine(x, grid_rect.top(), x, grid_rect.bottom());
      assert(x <= grid_rect.right());
      if (x == grid_rect.right())
        break;
    }
    for (int y = grid_rect.top(); ; y += cell_size)
    {
      painter.drawLine(grid_rect.left(), y, grid_rect.right(), y);
      assert(y <= grid_rect.bottom());
      if (y == grid_rect.bottom())
        break;
    }
    std::list<CellDataPtr> cells_to_update;
    updateAllSignal(cells_to_update);

    //draw cells
    QRect cell_rect(0, 0, cell_size - 1, cell_size - 1);
    for (auto& cell: cells_to_update)
    {
      assert(isValidCell(*cell));
      cell_rect.moveTo(grid_rect.left() + cell->col * cell_size + 1, grid_rect.top() + cell->row * cell_size + 1);
      drawCell(painter, cell_rect, std::move(cell));
    }
  }
}

int GridView::getCellSize(QSize view_size) const
{
  return std::min((view_size.width() - 1) / m_col_count, (view_size.height() - 1) / m_row_count);
}

void GridView::getGridRect(QSize view_size, QRect &rect, int &cell_size) const
{
  cell_size = getCellSize(view_size);

  rect.setWidth(m_col_count * cell_size + 1);
  rect.setHeight(m_row_count * cell_size + 1);
  rect.moveTo((view_size.width() - rect.width()) / 2, (view_size.height() - rect.height()) / 2);
}

QRect GridView::getCellInsideRect(const Cell &cell, const QRect &grid_rect, int cell_size)
{
  return QRect(grid_rect.left() + cell.col * cell_size + 1, grid_rect.top() + cell.row * cell_size + 1, cell_size - 1, cell_size - 1);
}

QRect GridView::getCellInsideRect(const Cell &cell)
{
  int cell_size;
  QRect grid_rect;
  getGridRect(size(), grid_rect, cell_size);
  return getCellInsideRect(cell, grid_rect, cell_size);
}
