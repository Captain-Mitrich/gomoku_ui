#include "mainwindow.h"
#include "gview.h"
#include "gslider.h"
#include <QtWidgets>
#include <sstream>

extern const unsigned GRID_WIDTH  = 15;
extern const unsigned GRID_HEIGHT = 15;

GMainWindow::GMainWindow(QWidget *parent) :
    QWidget(parent)
{
  QGridLayout* gridLayout = new QGridLayout(this);

  QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Preferred);

  gView = new GView(GRID_WIDTH, GRID_HEIGHT, this);
  sp.setVerticalStretch(6);
  gView->setSizePolicy(sp);
  gridLayout->addWidget(gView, 2, 0, 1, 4);

  sp.setVerticalStretch(1);

  levelGroup = new QGroupBox(this);
  levelGroup->setSizePolicy(sp);
  gridLayout->addWidget(levelGroup, 0, 0, 1, 2);

  langGroup = new QGroupBox(this);
  langGroup->setSizePolicy(sp);
  gridLayout->addWidget(langGroup, 0, 2, 1, 2);

  newGameGroup = new QGroupBox(this);
  newGameGroup->setSizePolicy(sp);
  gridLayout->addWidget(newGameGroup, 1, 0, 1, 2);

  loadSaveGroup = new QGroupBox(this);
  loadSaveGroup->setSizePolicy(sp);
  gridLayout->addWidget(loadSaveGroup, 1, 2, 1, 2);

  undoGroup = new QGroupBox(this);
  undoGroup->setSizePolicy(sp);
  gridLayout->addWidget(undoGroup, 3, 0, 1, 4);

  QHBoxLayout* levelLayout = new QHBoxLayout(levelGroup);

  levelSlider = new GSlider(levelGroup);
  levelSlider->setMinimum(0);
  levelSlider->setMaximum(4);
  levelSlider->setPageStep(1);
  levelSlider->setSingleStep(1);
  levelSlider->setFocusPolicy(Qt::StrongFocus);
  levelLayout->addWidget(levelSlider);
  connect(levelSlider, &QSlider::valueChanged, &m_game, &GModel::setLevel);

  QHBoxLayout *langLayout = new QHBoxLayout(langGroup);

  langButton = new QToolButton(langGroup);
  langButton->setSizePolicy(sp);
  langButton->setFocusPolicy(Qt::NoFocus);
  langLayout->addWidget(langButton);
  connect(langButton, &QAbstractButton::clicked, this, &GMainWindow::on_langButton_clicked);
  QShortcut* langShortCut = new QShortcut({"F2"}, this);
  connect(langShortCut, &QShortcut::activated, this, &GMainWindow::on_langButton_clicked);

  helpButton = new QToolButton(langGroup);
  helpButton->setSizePolicy(sp);
  helpButton->setFocusPolicy(Qt::NoFocus);
  helpButton->setCheckable(true);
  helpButton->setChecked(false);
  langLayout->addWidget(helpButton);
  connect(helpButton, &QToolButton::toggled, this, &GMainWindow::on_helpButton_toggled);
  QShortcut* helpShortCut = new QShortcut({"F1"}, this);
  connect(helpShortCut, &QShortcut::activated, helpButton, /*&GMainWindow::on_F1*/&QToolButton::toggle);

  QHBoxLayout *newGameLayout = new QHBoxLayout(newGameGroup);

  xNewGameButton = new QToolButton(newGameGroup);
  xNewGameButton->setSizePolicy(sp);
  xNewGameButton->setFocusPolicy(Qt::NoFocus);
  newGameLayout->addWidget(xNewGameButton);
  connect(xNewGameButton, &QAbstractButton::clicked, this, &GMainWindow::on_xNewGameButton_clicked);
  QShortcut* xNewShortCut = new QShortcut({"Ctrl+N, Ctrl+X"}, this);
  connect(xNewShortCut, &QShortcut::activated, this, &GMainWindow::on_xNewGameButton_clicked);

  oNewGameButton = new QToolButton(newGameGroup);
  oNewGameButton->setSizePolicy(sp);
  oNewGameButton->setFocusPolicy(Qt::NoFocus);
  newGameLayout->addWidget(oNewGameButton);
  connect(oNewGameButton, &QAbstractButton::clicked, this, &GMainWindow::on_oNewGameButton_clicked);
  QShortcut* oNewShortCut = new QShortcut({"Ctrl+N, Ctrl+O"}, this);
  connect(oNewShortCut, &QShortcut::activated, this, &GMainWindow::on_oNewGameButton_clicked);

  QHBoxLayout* loadSaveLayout = new QHBoxLayout(loadSaveGroup);

  loadButton = new QToolButton(loadSaveGroup);
  loadButton->setSizePolicy(sp);
  loadButton->setFocusPolicy(Qt::NoFocus);
  loadSaveLayout->addWidget(loadButton);
  connect(loadButton, &QAbstractButton::clicked, this, &GMainWindow::on_loadButton_clicked);
  QShortcut* loadShortCut = new QShortcut({"Ctrl+L"}, this);
  connect(loadShortCut, &QShortcut::activated, this, &GMainWindow::on_loadButton_clicked);

  saveButton = new QToolButton(loadSaveGroup);
  saveButton->setSizePolicy(sp);
  saveButton->setFocusPolicy(Qt::NoFocus);
  loadSaveLayout->addWidget(saveButton);
  connect(saveButton, &QAbstractButton::clicked, this, &GMainWindow::on_saveButton_clicked);
  QShortcut* saveShortCut = new QShortcut({"Ctrl+S"}, this);
  connect(saveShortCut, &QShortcut::activated, this, &GMainWindow::on_saveButton_clicked);

  QHBoxLayout* undoLayout = new QHBoxLayout(undoGroup);

  hintButton = new QToolButton(undoGroup);
  hintButton->setSizePolicy(sp);
  hintButton->setFocusPolicy(Qt::NoFocus);
  undoLayout->addWidget(hintButton);
  connect(hintButton, &QAbstractButton::clicked, &m_game, &GModel::hint);

  undoButton = new QToolButton(undoGroup);
  undoButton->setSizePolicy(sp);
  undoButton->setFocusPolicy(Qt::NoFocus);
  undoLayout->addWidget(undoButton);
  connect(undoButton, &QAbstractButton::clicked, &m_game, &GModel::undo);
  QShortcut* undoShortCut = new QShortcut({"Ctrl+Z"}, this);
  connect(undoShortCut, &QShortcut::activated, &m_game, &GModel::undo);

  redoButton = new QToolButton(undoGroup);
  redoButton->setSizePolicy(sp);
  redoButton->setFocusPolicy(Qt::NoFocus);
  undoLayout->addWidget(redoButton);
  connect(redoButton, &QAbstractButton::clicked, &m_game, &GModel::redo);
  QShortcut* redoShortCut = new QShortcut({"Ctrl+Y"}, this);
  connect(redoShortCut, &QShortcut::activated, &m_game, &GModel::redo);


  langIcon.load(":/images/lang24.png");
  langIcon.setColor(palette().color(QPalette::ButtonText));
  helpIcon.load(":/images/help.png");

  connect(gView, &GView::clickCellSignal, &m_game, &GModel::doMove);
  connect(gView, &GridView::updateAllSignal, &m_game, &GModel::getAllMoves);
  connect(&m_game, &GModel::updateMoveSignal, gView, &GView::updateCell);
  connect(&m_game, &GModel::updateStatusSignal, this, &GMainWindow::updateStatus);

  connect(gView, &GView::resizeSignal, this, &GMainWindow::onCellSizeChanged);

  lsDialog = new GLoadSaveDialog(this);

  setWindowOpacity(0.9);

  setFocusPolicy(Qt::NoFocus);

  resize(420, 740);

  autoLoad();
}

void GMainWindow::startGame(bool first)
{
  if (!m_game.isEmpty())
    autoSave();
  bool replaceMarker = m_game.firstMovePlayer() != first;
  m_game.start(first);
  if (replaceMarker)
    updateNewGameButtons();
}

void GMainWindow::on_xNewGameButton_clicked()
{
  startGame(GModel::USER);
}

void GMainWindow::on_oNewGameButton_clicked()
{
  startGame(GModel::AI);
}

void GMainWindow::on_loadButton_clicked()
{
  lsDialog->setMinimumSize(size());
  if (!lsDialog->load())
    return;
  if (!m_game.isEmpty())
    autoSave();
  bool first = m_game.firstMovePlayer();
  std::istringstream ist(qPrintable(lsDialog->gameData));
  m_game.read(ist);
  bool replaceMarker = m_game.firstMovePlayer() != first;
  if (replaceMarker)
    updateNewGameButtons();
}

void GMainWindow::on_saveButton_clicked()
{
  lsDialog->setMinimumSize(size());
  std::ostringstream ost;
  m_game.write(ost);
  lsDialog->gameData = ost.str().c_str();
  if (lsDialog->save())
    saveButton->setEnabled(false);
}

void GMainWindow::on_levelUp()
{
  levelSlider->setValue(levelSlider->value() + 1);
}

void GMainWindow::on_levelDown()
{
  levelSlider->setValue(levelSlider->value() - 1);
}

void GMainWindow::closeEvent(QCloseEvent *)
{
    autoSave();
}

void GMainWindow::getNDIcon(const QPixmap& pm, QIcon& ndIcon)
{
  GIcon gi;
  (QPixmap&)gi = pm;
  ndIcon.addPixmap(gi, QIcon::Normal);
  gi.setColor(palette().color(QPalette::Shadow));
  ndIcon.addPixmap(gi, QIcon::Disabled);
}

void GMainWindow::setNDIcon(const QPixmap& pm, QToolButton& button, QAction* action)
{
    QIcon ndIcon;
    getNDIcon(pm, ndIcon);
    button.setIconSize(pm.size());
    if (action)
        action->setIcon(ndIcon);
    else
        button.setIcon(ndIcon);
}

void GMainWindow::setNewGameIcon(GView::Picture pic, QToolButton& button)
{
  QColor buttonTextColor = palette().color(QPalette::ButtonText);
  GIcon icon;
  (QPixmap&)icon = gView->getIcon(pic);
  icon.setColor(buttonTextColor);
  //Маркер на кнопке игрока
  if ((m_game.firstMovePlayer() == m_game.USER) == (pic == GView::X))
  {
    QPainter p(&icon);
    p.setRenderHint(QPainter::Antialiasing, true);
    QPen pen;
    pen.setColor(buttonTextColor);
    pen.setWidthF(1.3);
    p.setPen(pen);
    QRectF rect(0.5, 0.5, (double)icon.width() - 1.0, (double)icon.height() - 1.0);
    p.drawRoundedRect(rect, icon.width()/6, icon.height()/6);
  }
  setNDIcon(icon, button);
}

void GMainWindow::updateNewGameButtons()
{
  setNewGameIcon(gView->X, *xNewGameButton);
  setNewGameIcon(gView->O, *oNewGameButton);
}

/*void GMainWindow::updateStatusMessage(GModel::Status status)
{
    ui->nextMoveGroup->setTitle(
              (status == GModel::NEXT_NI) ? tr("Choose cell and make your move") :
              (status == GModel::NEXT_AI) ? tr("Choose cell and replace AI move") :
              (status == GModel::COMPUTING) ? tr("Wait for AI move") :
              (gView->m_game.lastMovePlayer() == G_NI) ? tr("Victory") : tr("Defeat")
            );
}*/

void GMainWindow::updateStatus()
{
  if (m_game.isReady() != loadButton->isEnabled())
  {
    xNewGameButton->setEnabled(m_game.isReady());
    oNewGameButton->setEnabled(m_game.isReady());
    loadButton->setEnabled(m_game.isReady());

    QCursor c = cursor();
    c.setShape(m_game.isReady() ? Qt::ArrowCursor : Qt::BusyCursor);
    setCursor(c);
  }
  saveButton->setEnabled(m_game.isSaveReady());
  hintButton->setEnabled(m_game.isHintReady());
  undoButton->setEnabled(m_game.isUndoReady());
  redoButton->setEnabled(m_game.isRedoReady());
  levelSlider->setEnabled(m_game.isReady());
  updateAILevel();
}

void GMainWindow::onCellSizeChanged()
{
  updateNewGameButtons();
  int is = gView->getIconSize();
  QSize iconSize(is, is);
  QPixmap pm = langIcon.scaled(iconSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  setNDIcon(pm, *langButton);

  helpButton->setIconSize(iconSize);
  helpButton->setIcon(helpIcon.scaled(iconSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

  levelSlider->setSliderSize(is - is / 3);
}

bool GMainWindow::autoSave()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Captain Mitrich", "Gomoku");

    settings.setValue("help", helpButton->isChecked());
    settings.setValue("lang", lang);

    std::ostringstream ost;
    if (!m_game.write(ost))
        return false;
    QString gameData(ost.str().c_str());
    settings.setValue("auto", gameData);

    return true;
}

bool GMainWindow::autoLoad()
{
  QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Captain Mitrich", "Gomoku");

  translate(settings.contains("lang") ? settings.value("lang").toString() : QLocale::system().name());

  bool help = settings.contains("help") ? settings.value("help").toBool() : true;
  enableHelp(help);
  helpButton->setChecked(help);

  if (!settings.contains("auto"))
      return false;
  QString gameData = settings.value("auto").toString();
  std::istringstream ist(qPrintable(gameData));
  if (!m_game.read(ist))
      return false;
  updateNewGameButtons();
  return true;
}

void GMainWindow::updateAILevel()
{
  int level = m_game.getLevel();
  if (level > levelSlider->maximum())
    level = levelSlider->maximum();
  levelSlider->setValue(level);
}

bool GMainWindow::translate(const QString &nlang)
{
  if (nlang.isEmpty())
    qApp->removeTranslator(&translator);
  else if (!translator.load("gomoku_" + nlang, ":/translations") || !qApp->installTranslator(&translator))
    return false;
  lang = nlang;
  retranslateUi();
  return true;
}

void GMainWindow::on_langButton_clicked()
{
  translate(lang.isEmpty() ? "ru" : "");
}

void GMainWindow::retranslateUi()
{
  setWindowTitle(tr("Gomoku"));
  loadButton->setText(tr("Load"));
  saveButton->setText(tr("Save"));
  hintButton->setText(tr("Hint"));
  undoButton->setText(tr("Undo"));
  redoButton->setText(tr("Redo"));
  levelSlider->setToolTip(tr("AI level") + "\n<- / ->");
  langButton->setToolTip("F2");
  helpButton->setToolTip("F1");
  xNewGameButton->setToolTip(tr("Start new game as") + " X\nCtrl+N, Ctrl+X");
  oNewGameButton->setToolTip(tr("Start new game as") + " O\nCtrl+N, Ctrl+O");
  loadButton->setToolTip("Ctrl+L");
  saveButton->setToolTip("Ctrl+S");
  undoButton->setToolTip("Ctrl+Z");
  redoButton->setToolTip("Ctrl+Y");

  bool help = helpButton->isChecked();
  levelGroup->setTitle(help ? tr("AI level") : "");
  langGroup->setTitle(help ? (tr("Language") + "/" + tr("Help")) : "");
  newGameGroup->setTitle(help ? tr("New game") : "");
}

void GMainWindow::enableHelp(bool b)
{
  helpIcon.setColor(palette().color(b ? QPalette::Highlight : QPalette::ButtonText));
  helpButton->setIcon(helpIcon);
  QString groupBoxStyle = b ?
    "QGroupBox { margin-top: 0.6em; padding: 0.1em; border: 1px solid palette(dark); border-radius: 0.2em } "
    "QGroupBox::title { subcontrol-origin: margin; left: 0.5em } " :
    "QGroupBox { border: 1px solid palette(dark); border-radius: 0.2em } ";
  qApp->setStyleSheet(groupBoxStyle);
  retranslateUi();
}

void GMainWindow::on_helpButton_toggled()
{
  enableHelp(helpButton->isChecked());
}
