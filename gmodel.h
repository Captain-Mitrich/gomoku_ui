#ifndef GMODEL
#define GMODEL

#include "igomoku.h"
#include "gcelldata.h"
#include <QThread>
#include <QAction>

using namespace nsg;

using GMoveList = std::list<Cell>;

class GModel : public QThread
{
    Q_OBJECT

public:
  static const bool USER = 0;
  static const bool AI   = 1;

  GModel();

  bool start(bool first);
  bool write(std::ostream& stream);
  bool read(std::istream& stream);
  unsigned getLevel();
  bool setLevel(unsigned level);
  bool doMove(const Cell& move);
  bool undo();
  bool redo();
  bool hint();

  bool isEmpty();
  bool isGameOver();

  bool isReady();
  bool isSaveReady();
  bool isHintReady();
  bool isUndoReady();
  bool isRedoReady();

  bool firstMovePlayer() const
  {
    return m_first;
  }

  bool lastMovePlayer() const;

public slots:
  void getAllMoves(std::list<CellDataPtr>& moves);

signals:
  void updateStatusSignal();
  void updateMoveSignal(CellDataPtr cell_data);

protected:
  bool m_ready;

  bool m_first; //who moves first - USER or AI

  GomokuPtr m_game;

  GMoveList m_points[2];

  int m_ai_x, m_ai_y;

  GMoveList m_undo_stack;

  void setReady(bool ready)
  {
    m_ready = ready;
  }

  void clear();

  bool doMoveImpl(int x, int y);

  bool hintImpl();

  void run() override;

  void update();

  void updateMove(int x, int y, unsigned flags);
  void updateMove(const Cell& cell, unsigned flags);

  //выполняется в основном потоке по окончании работы потока ии
  void handleAiResult();

  void getLine5Moves(GMoveList& moves);

  bool restoreGame(const GMoveList& moves1, const GMoveList& moves2);

  unsigned getPlayerFlag(bool player)
  {
    return (player == m_first) ? GCellData::BLACK : GCellData::WHITE;
  }
};

#endif // GMODEL
