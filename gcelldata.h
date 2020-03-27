#ifndef GCELLDATA_H
#define GCELLDATA_H

#include "celldata.h"

class GCellData : public Cell
{
public:
  static const unsigned CLEAR = 1;
  static const unsigned BLACK = 2;
  static const unsigned WHITE = 4;
  static const unsigned LINE5 = 8;
  static const unsigned LAST = 16;

  unsigned flags;

  GCellData(int _col = 0, int _row = 0, unsigned _flags = 0) :
    Cell(_col, _row), flags(_flags)
  {}
};

#endif // GCELLDATA_H
