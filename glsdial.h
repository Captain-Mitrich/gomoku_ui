#ifndef GLSDIAL_H
#define GLSDIAL_H

#include "gmodel.h"
#include <QtWidgets>

class GView;

class GSave : public QGroupBox
{
    //Q_OBJECT

public:
    GView* gameView;
    GModel game;

    GSave(QWidget* parent = 0);

    QGroupBox* box()
    {
        return this;
    }

    void clear();

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
};

typedef QVector<GSave*> GSaveList;

class QToolButton;
class QGridLayout;

class GLoadSaveDialog : public QDialog
{
    Q_OBJECT

public:
    QString gameData;

    GLoadSaveDialog(QWidget* parent);

    int load();
    int save();
    void retranslateUI();

public slots:
    void on_loadSaveButton_clicked();
    void on_deleteButton_clicked();
    void on_slot_clicked();

protected:
    QWidget* swidget;
    QVBoxLayout* slayout;
    QToolButton* loadSaveButton;
    QToolButton* deleteButton;

    QSettings settings;
    bool saveType;
    GSaveList saves;
    int activeSave;

    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

    int saveCount();
    GSave* save(int si);
    void initSaves();
    void saveToActiveSlot();
    bool loadFromActiveSlot();
    void setActive(int saveIndex);
    int addSlot();
    void addSave(QString dateStr, QString gameData);
    void removeSave(int si);
    void highlightActiveSave(bool b);
};

#endif // GLSDIAL_H
