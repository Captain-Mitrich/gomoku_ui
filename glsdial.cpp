#include <sstream>
#include "glsdial.h"
#include "gview.h"

extern const unsigned GRID_WIDTH, GRID_HEIGHT;

extern QColor gridColor, grayColor;

GSave::GSave(QWidget* parent) : QGroupBox(parent), gameView(0)
{
  QGridLayout* lo = new QGridLayout(this);
  lo->setMargin(3);

  QString groupBoxStyle =
    "QGroupBox { margin-top: 0.6em; padding: 0.1em; border: 1px solid palette(dark); border-radius: 0.2em } "
    "QGroupBox::title { subcontrol-origin: margin; left: 0.5em } ";
  setStyleSheet(groupBoxStyle);
}

void GSave::mousePressEvent(QMouseEvent *)
{
  emit clicked();
}

void GSave::clear()
{
  if (gameView)
    delete gameView;
  gameView = 0;
}

GLoadSaveDialog::GLoadSaveDialog(QWidget* parent) :
  QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint),
  settings(QSettings::IniFormat, QSettings::UserScope, "Captain Mitrich", "Gomoku"),
  activeSave(-1)
{
  setWindowOpacity(0.95);
  setAutoFillBackground(true);

  swidget = new QWidget;
  slayout = new QVBoxLayout(swidget);
  slayout->setMargin(0);

  QScrollArea* sa = new QScrollArea;
  sa->setWidget(swidget);
  sa->setAlignment(Qt::AlignHCenter);
  sa->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  sa->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  loadSaveButton = new QToolButton;
  loadSaveButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  loadSaveButton->setFocusPolicy(Qt::NoFocus);

  deleteButton = new QToolButton;
  deleteButton->setText(tr("Delete"));
  deleteButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  deleteButton->setFocusPolicy(Qt::NoFocus);

  QVBoxLayout* lo = new QVBoxLayout(this);
  lo->addWidget(loadSaveButton, 1);
  lo->addWidget(sa, 18);
  lo->addWidget(deleteButton, 1);

  QScroller::grabGesture(sa, QScroller::LeftMouseButtonGesture);

  connect(loadSaveButton, &QToolButton::clicked, this, &GLoadSaveDialog::on_loadSaveButton_clicked);
  connect(deleteButton, &QToolButton::clicked, this, &GLoadSaveDialog::on_deleteButton_clicked);

  QShortcut* backShortCut = new QShortcut({Qt::Key_Back}, this);
  connect(backShortCut, &QShortcut::activated, this, &QDialog::reject);
}

void GLoadSaveDialog::resizeEvent(QResizeEvent *event)
{
  swidget->resize(event->size().width() / 2,
                  (event->size().width() / 2 + slayout->spacing()) * saveCount());
  int top, bottom, left, right;
  layout()->getContentsMargins(&left, &top, &right, &bottom);
  left = right = event->size().width() / 4 - 1;
  layout()->setContentsMargins(left, top, right, bottom);
}

int GLoadSaveDialog::load()
{
  loadSaveButton->setText(tr("Load"));

  //инициализация при первом запуске
  if (saveCount() == 0)
      initSaves();

  saveType = false;

  //автосохранение в последнем слоте
  assert(saveCount() > 0);
  save(saveCount() - 1)->clear();
  if (settings.contains("auto"))
    addSave(tr("Autosave"), settings.value("auto").toString());

  //активируем последний слот
  setActive(saveCount() - 1);

  assert(save(saveCount() - 1)->gameView || saveCount() == 1);
  if (!save(saveCount() - 1)->gameView)
    loadSaveButton->setEnabled(false);

  retranslateUI();

  return QDialog::exec();
}

int GLoadSaveDialog::save()
{
  if (gameData.isEmpty())
    return false;

  loadSaveButton->setText(tr("Save"));

  //инициализация при первом запуске
  if (saveCount() == 0)
    initSaves();

  saveType = true;

  assert(saveCount() > 0);
  GSave* sav = save(saveCount() - 1);
  //убираем view автосохранения, оставшееся после запуска диалога загрузки
  sav->clear();
  if (!sav->title().isEmpty())
    sav->setTitle("");

  //активируем последний слот
  setActive(saveCount() - 1);

  retranslateUI();

  return QDialog::exec();
}

void GLoadSaveDialog::on_loadSaveButton_clicked()
{
  if (saveType)
    saveToActiveSlot();
  else
  {
    if (!save(activeSave)->gameView)
    {
      assert(saveCount() == 1);
      return;
    }
    loadFromActiveSlot();
  }
  accept();
}

void GLoadSaveDialog::on_deleteButton_clicked()
{
  assert(activeSave < saveCount() - 1);
  //активируем следующее сохранение
  setActive(activeSave + 1);
  removeSave(activeSave - 1);
  //сдвигаем элементы в инишнике
  settings.beginWriteArray("saves", saveCount() - 1);
  int si;
  for (si = activeSave; si < saveCount() - 1; ++si)
  {
    settings.setArrayIndex(si);
    settings.setValue("date", save(si)->box()->title());
    std::ostringstream ost;
    save(si)->game.write(ost);
    QString gameStr(ost.str().c_str());
    settings.setValue("data", gameStr);
  }
  settings.setArrayIndex(si);
  settings.remove("date");
  settings.remove("data");
  settings.endArray();
}

void GLoadSaveDialog::on_slot_clicked()
{
  int si;
  for (si = 0; si < saveCount(); ++si)
  {
    if ((QObject*)save(si)->box() == sender())
      setActive(si);
  }
}

void GLoadSaveDialog::initSaves()
{
  swidget->resize(swidget->width(), 0);

  int size = settings.beginReadArray("saves");
  int si;
  for (si = 0; si < size; ++si)
  {
    assert(si == saveCount());
    addSlot();
    settings.setArrayIndex(si);
    addSave(settings.value("date").toString(), settings.value("data").toString());
  }
  settings.endArray();
  //добавляем пустой слот
  addSlot();
}

void GLoadSaveDialog::saveToActiveSlot()
{
  //создаем новое сохранение в пустом слоте
  addSave(QLocale::system().toString(QDateTime::currentDateTime(), QLocale::ShortFormat), gameData);
  //удаляем перезаписанное сохранение
  if (activeSave < saveCount() - 1)
    removeSave(activeSave);
  //сдвигаем элементы в инишнике
  settings.beginWriteArray("saves");
  int si;
  for (si = activeSave; si < saveCount(); ++si)
  {
    settings.setArrayIndex(si);
    settings.setValue("date", save(si)->box()->title());
    std::ostringstream ost;
    save(si)->game.write(ost);
    QString gameStr(ost.str().c_str());
    settings.setValue("data", gameStr);
  }
  settings.endArray();
  //добавляем пустой слот
  addSlot();
}

bool GLoadSaveDialog::loadFromActiveSlot()
{
  std::ostringstream ost;
  save(activeSave)->game.write(ost);
  gameData = ost.str().c_str();
  return true;
}

int GLoadSaveDialog::saveCount()
{
  return saves.size();
}

GSave* GLoadSaveDialog::save(int si)
{
  return saves[si];
}

int GLoadSaveDialog::addSlot()
{
  swidget->resize(swidget->width(), swidget->height() + swidget->width() + slayout->spacing());
  int si = saveCount();
  GSave* sav = new GSave;
  saves.push_back(sav);
  QGroupBox* gb = sav->box();
  slayout->insertWidget(0, gb);
  connect(gb, &QGroupBox::clicked, this, &GLoadSaveDialog::on_slot_clicked);
  return si;
}

void GLoadSaveDialog::highlightActiveSave(bool b)
{
  if (activeSave < 0)
    return;
  QString groupBoxStyle = b ?
    "QGroupBox { margin-top: 0.6em; padding: 0.1em; border: 1px solid palette(text); border-radius: 0.2em } "
    "QGroupBox::title { subcontrol-origin: margin; left: 0.5em } " :
    "QGroupBox { margin-top: 0.6em; padding: 0.1em; border: 1px solid palette(dark); border-radius: 0.2em } "
    "QGroupBox::title { subcontrol-origin: margin; left: 0.5em } ";
  save(activeSave)->box()->setStyleSheet(groupBoxStyle);
}

void GLoadSaveDialog::setActive(int si)
{
  assert(si >= 0 && si < saveCount());

  if (si == activeSave)
    return;

  if (saveType && (activeSave == saveCount() - 1 || si == saveCount() - 1))
    loadSaveButton->setText((si == saveCount() - 1) ? tr("Save") : tr("Overwrite"));

  highlightActiveSave(false);
  activeSave = si;
  highlightActiveSave(true);

  deleteButton->setEnabled(activeSave < saveCount() - 1);
}

void GLoadSaveDialog::addSave(QString dateStr, QString dataStr)
{
  GSave* sav = save(saveCount() - 1);
  assert(!sav->gameView);
  sav->gameView = new GView(GRID_WIDTH, GRID_HEIGHT);
  connect(sav->gameView, &GridView::updateAllSignal, &sav->game, &GModel::getAllMoves);
  sav->box()->layout()->addWidget(sav->gameView);
  std::istringstream ist(qPrintable(dataStr));
  sav->game.read(ist);
  sav->gameView->setEnabled(false);
  sav->box()->setTitle(dateStr);
}

void GLoadSaveDialog::removeSave(int si)
{
  assert(si >= 0 && si < (saveCount() - 1));
  GSave* sav = save(si);
  slayout->removeWidget(sav->box());
  swidget->resize(swidget->width(), swidget->height() - swidget->width() - slayout->spacing());
  saves.remove(si);
  delete sav;
  if (activeSave > si)
    --activeSave;
}

void GLoadSaveDialog::retranslateUI()
{
    loadSaveButton->setText(saveType ? tr("Save") : tr("Load"));
    deleteButton->setText(tr("Delete"));
}
