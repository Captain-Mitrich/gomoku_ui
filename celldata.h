#ifndef CELLDATA_H
#define CELLDATA_H

#include <memory>

class Cell
{
public:
  int col;
  int row;

  Cell(int _col = 0, int _row = 0) : col(_col), row(_row)
  {}

  bool operator==(const Cell& cell) const
  {
    return col == cell.col && row == cell.row;
  }

  bool operator!=(const Cell& cell) const
  {
    return !operator==(cell);
  }
};

using CellDataPtr = std::shared_ptr<Cell>;

#endif // CELLDATA_H
