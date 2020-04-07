#include "gmodel.h"
#include <assert.h>

using std::endl;

GModel::GModel() : m_ready(true), m_first(USER), m_game(CreateGomoku())
{
  connect(this, &QThread::finished, this, &GModel::handleAiResult);
}

bool GModel::start(bool first)
{
  if (!isReady())
    return false;

  m_game->start();
  clear();

  m_first = first;
  if (first == AI)
  {
    int x, y;
    m_game->hint(x, y);
    doMoveImpl(x, y);
  }

  update();
  return true;
}

void GModel::clear()
{
  for (auto& move: m_points[AI])
    updateMove(move.col, move.row, GCellData::CLEAR);
  m_points[AI].clear();
  for (auto& move: m_points[USER])
    updateMove(move.col, move.row, GCellData::CLEAR);
  m_points[USER].clear();
  m_undo_stack.clear();
}

bool GModel::write(std::ostream& stream)
{
  if (!isReady())
    return false;

  stream << getLevel() << endl;

  stream << m_first << endl;

  GMoveList& points1 = m_points[m_first];
  GMoveList& points2 = m_points[!m_first];

  assert(
    points1.size() == points2.size() ||
    points1.size() == points2.size() + 1);

  stream << (points1.size() + points2.size()) << endl;

  for (auto move1 = points1.begin(), move2 = points2.begin(); move1 != points1.end(); ++move1, ++move2)
  {
    stream << move1->col << ' ' << move1->row << endl;
    if (move2 == points2.end())
      break;
    stream << move2->col << ' ' << move2->row << endl;
  }

  return stream.good();
}

bool GModel::read(std::istream& stream)
{
  if (!isReady())
    return true;

  unsigned level;
  stream >> level;
  if (level > m_game->getMaxAiLevel())
    return false;

  bool first;
  stream >> first;
  if (first != AI && first != USER)
    return false;

  size_t size;
  stream >> size;
  if (size > G_CELL_COUNT)
    return false;

  int x, y;
  GMoveList moves[2];
  for (bool player = first; size > 0; --size, player = !player)
  {
    stream >> x >> y;
    moves[player].emplace_back(x, y);
  }
  if (!stream)
    return false;

  if (restoreGame(moves[first], moves[!first]))
  {
    m_game->setAiLevel(level);
    clear();
    m_first = first;
    m_points[m_first] = std::move(moves[m_first]);
    m_points[!m_first] = std::move(moves[!m_first]);

    std::list<CellDataPtr> moves_to_update;
    getAllMoves(moves_to_update);
    for (auto& move: moves_to_update)
      emit updateMoveSignal(std::move(move));

    update();

    return true;
  }
  else
  {
    restoreGame(m_points[m_first], m_points[!m_first]);
    return false;
  }
}

unsigned GModel::getLevel()
{
  return m_game->getAiLevel();
}

bool GModel::setLevel(unsigned level)
{
  if (!isReady())
    return false;
  m_game->setAiLevel(level);
  return true;
}

bool GModel::doMove(const Cell& move)
{
  if (!isReady())
    return false;

  if (!m_undo_stack.empty() && move == m_undo_stack.back())
    return redo();
  m_undo_stack.clear();

  doMoveImpl(move.col, move.row);

  if (!m_game->isGameOver() && lastMovePlayer() == USER)
    hintImpl();

  update();
  return true;
}

bool GModel::undo()
{
  if (!isReady())
    return false;

  GMoveList line5;
  getLine5Moves(line5);

  Cell cell;
  if (!m_game->undo(cell.col, cell.row))
    return false;
  updateMove(cell, GCellData::CLEAR);

  bool player = lastMovePlayer();
  auto& pmoves = m_points[player];
  assert(!pmoves.empty());

  for (auto& move: line5)
  {
    if (move != pmoves.back())
      updateMove(move, getPlayerFlag(player));
  }

  m_undo_stack.splice(m_undo_stack.end(), pmoves, std::prev(pmoves.end()));

  bool enemy = !player;
  auto& emoves = m_points[enemy];
  if (!emoves.empty())
    updateMove(emoves.back(), getPlayerFlag(enemy)|GCellData::LINE5);

  update();
  return true;
}

bool GModel::redo()
{
  if (!isReady() || m_undo_stack.empty())
    return false;

  assert(!m_game->isGameOver());

  doMoveImpl(m_undo_stack.back().col, m_undo_stack.back().row);
  m_undo_stack.pop_back();
  if (!m_game->isGameOver() && lastMovePlayer() == USER)
  {
    //если в стэке есть ход ии, откатываем его
    if (!m_undo_stack.empty())
    {
      doMoveImpl(m_undo_stack.back().col, m_undo_stack.back().row);
      m_undo_stack.pop_back();
    }
    //иначе запускаем ии
    else
      hintImpl();
  }

  update();
  return true;
}

bool GModel::hint()
{
  if (!isReady())
    return false;
  if (hintImpl())
    update();
  return true;
}

bool GModel::isEmpty()
{
  return m_points[m_first].empty();
}

bool GModel::isGameOver()
{
  return isReady() && m_game->isGameOver();
}

bool GModel::isReady()
{
  return m_ready;
}

bool GModel::isSaveReady()
{
  return isReady() && !m_points[m_first].empty();
}

bool GModel::isHintReady()
{
  return isReady() && !isGameOver();
}

bool GModel::isUndoReady()
{
  return isReady() && !m_points[m_first].empty();
}

bool GModel::isRedoReady()
{
  return isReady() && !m_undo_stack.empty();
}

bool GModel::doMoveImpl(int x, int y)
{
  if (!m_game->doMove(x, y))
    return false;

  bool player = !lastMovePlayer();

  GMoveList& player_moves = m_points[player];
  player_moves.emplace_back(x, y);

  //убираем признак последнего хода у хода противника
  GMoveList& enemy_moves = m_points[!player];
  if (!enemy_moves.empty())
    updateMove(enemy_moves.back(), (player == m_first) ? GCellData::WHITE : GCellData::BLACK);

  unsigned player_flag = (player == m_first) ? GCellData::BLACK : GCellData::WHITE;
  unsigned last_move_flags = player_flag | GCellData::LAST;

  GMoveList line5;
  getLine5Moves(line5);
  for (auto& move: line5)
  {
    if (move.col == x && move.row == y)
      last_move_flags |= GCellData::LINE5;
    else
      updateMove(move, player_flag | GCellData::LINE5);
  }

  updateMove(player_moves.back(), last_move_flags);

  return true;
}

bool GModel::hintImpl()
{
  if (m_game->isGameOver())
    return false;
  setReady(false);
  QThread::start();
  return true;
}

void GModel::run()
{
  m_game->hint(m_ai_x, m_ai_y);
}

void GModel::handleAiResult()
{
  assert(!isReady());
  doMoveImpl(m_ai_x, m_ai_y);
  setReady(true);
  update();
}

void GModel::update()
{
  emit updateStatusSignal();
}

void GModel::updateMove(int x, int y, unsigned flags)
{
  emit updateMoveSignal(std::make_shared<GCellData>(x, y, flags));
}

void GModel::updateMove(const Cell& cell, unsigned flags)
{
  updateMove(cell.col, cell.row, flags);
}

bool GModel::lastMovePlayer() const
{
  assert(
    m_points[m_first].size() == m_points[!m_first].size() ||
    m_points[m_first].size() == m_points[!m_first].size() + 1);
  return (m_points[m_first].size() == m_points[!m_first].size()) ? !m_first : m_first;
}

void GModel::getAllMoves(std::list<CellDataPtr>& moves)
{
  GMoveList line5;
  getLine5Moves(line5);

  auto gam = [&](const GMoveList& player_moves, unsigned player_flag, bool last)
  {
    for (const auto& move: player_moves)
    {
      unsigned flags = player_flag;
      if (std::find(line5.begin(), line5.end(), move) != line5.end())
        flags |= GCellData::LINE5;
      if (last && move == player_moves.back())
        flags |= GCellData::LAST;
      moves.push_back(std::make_shared<GCellData>(move.col, move.row, flags));
    }
  };

  gam(m_points[m_first], GCellData::BLACK, lastMovePlayer() == m_first);
  gam(m_points[!m_first], GCellData::WHITE, lastMovePlayer() == !m_first);
}

void GModel::getLine5Moves(GMoveList &moves)
{
  moves.clear();
  const GLine* line5 = m_game->getLine5();
  if (!line5)
    return;
  int x, y;
  for (getStartPoint(*line5, x, y); ; getNextPoint(*line5, x, y))
  {
    moves.emplace_back(x, y);
    if (moves.size() == 5)
      break;
  }
}

bool GModel::restoreGame(const GMoveList& moves1, const GMoveList& moves2)
{
  assert(moves1.size() == moves2.size() || moves1.size() == moves2.size() + 1);

  m_game->start();

  for (auto move1 = moves1.begin(), move2 = moves2.begin(); move1 != moves1.end(); ++move1, ++move2)
  {
    if (!m_game->doMove(move1->col, move1->row))
      return false;
    if (move2 == moves2.end())
    {
      assert(std::next(move1) == moves1.end());
      break;
    }
    if (!m_game->doMove(move2->col, move2->row))
      return false;
  }

  return true;
}
