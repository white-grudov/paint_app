#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>

class ScribbleArea;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* pe) override;
    void dropEvent(QDropEvent* pe) override;
    void contextMenuEvent (QContextMenuEvent *event) override;

private slots:
    void open();
    void save();
    void penColor();
    void penWidth();
    void about();

    void selectRect();
    void selectTriang();
    void selectCircle();
    void selectNone();

private:
    void createActions();
    void createMenus();

    bool maybeSave();
    bool saveFile(const QByteArray& fileFormat);

    Ui::MainWindow* ui;
    ScribbleArea* scribbleArea;

    QMenu* saveAsMenu;
    QMenu* fileMenu;
    QMenu* optionMenu;
    QMenu* figureMenu;

    QAction* openAct;

    QList<QAction*> saveAsActs;
    QAction* exitAct;
    QAction* penColorAct;
    QAction* penWidthAct;
    QAction* clearScreenAct;
    QAction* rectAct;
    QAction* triangAct;
    QAction* circleAct;
    QAction* clearFigureAct;
    QAction* aboutAct;
    QAction* indicatorAct;
};

#endif
