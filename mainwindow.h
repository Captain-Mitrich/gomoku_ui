#ifndef GMAINWINDOW_H
#define GMAINWINDOW_H

#include <QWidget>
#include <QTranslator>
#include "gmodel.h"
#include "gview.h"
#include "glsdial.h"

namespace Ui {
class GMainWindow;
}

class GView;
class QToolButton;

class GSlider;

class GMainWindow : public QWidget
{
    Q_OBJECT

public:
  explicit GMainWindow(QWidget *parent = 0);

protected:

  void closeEvent(QCloseEvent *event) override;

  void startGame(bool first);

  bool autoSave();
  bool autoLoad();

  void updateAILevel();

  void setNewGameIcon(GView::Picture pic, QToolButton& button);
  void updateNewGameButtons();

  bool translate(const QString& nlang);
  void retranslateUi();
  void enableHelp(bool b);

  void getNDIcon(const QPixmap& pm, QIcon& ndIcon);
  void setNDIcon(const QPixmap& pm, QToolButton& button, QAction* action = 0);

  void updateStatus();
  void onCellSizeChanged();

  void on_langButton_clicked();
  void on_helpButton_toggled();
  void on_xNewGameButton_clicked();
  void on_oNewGameButton_clicked();
  void on_loadButton_clicked();
  void on_saveButton_clicked();
  void on_levelUp();
  void on_levelDown();

protected:
  QGroupBox* levelGroup;
  QGroupBox* langGroup;
  QGroupBox* newGameGroup;
  QGroupBox* loadSaveGroup;
  QGroupBox* undoGroup;

  GSlider* levelSlider;

  QToolButton* xNewGameButton;
  QToolButton* oNewGameButton;
  QToolButton* loadButton;
  QToolButton* saveButton;
  QToolButton* hintButton;
  QToolButton* undoButton;
  QToolButton* redoButton;
  QToolButton* langButton;
  QToolButton* helpButton;

  GModel m_game;

  GView *gView;

  GLoadSaveDialog *lsDialog;

  GIcon langIcon;
  GIcon helpIcon;

  QTranslator translator;
  QString lang;
};

#endif // GMAINWINDOW_H
