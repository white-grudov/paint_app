#include <QtWidgets>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scribblearea.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scribbleArea = new ScribbleArea();
    scribbleArea->setFixedSize(QSize(1150, 760));
    QWidget *wdg = new QWidget(scribbleArea);
    wdg->move(1100, 50);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(scribbleArea);
    layout->addWidget(ui->verticalWidget);

    QWidget *mainWidget = new QWidget();
    mainWidget->setLayout(layout);
    setCentralWidget(mainWidget);

    createActions();
    createMenus();

    connect(ui->rect, SIGNAL(clicked()), this, SLOT(selectRect()));
    connect(ui->draw, SIGNAL(clicked()), this, SLOT(selectNone()));
    connect(ui->circle, SIGNAL(clicked()), this, SLOT(selectCircle()));
    connect(ui->triang, SIGNAL(clicked()), this, SLOT(selectTriang()));

    connect(ui->color, SIGNAL(clicked()), this, SLOT(penColor()));
    connect(ui->size, SIGNAL(clicked()), this, SLOT(penWidth()));

    setWindowTitle(tr("Малювання"));
    resize(1280, 720);
    setAcceptDrops(true);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) event->accept();
    else event->ignore();
}

void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Відкрити файл"), QDir::currentPath());
        if (!fileName.isEmpty())
            scribbleArea->openImage(fileName);
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent* pe)
{
    if (pe->mimeData()->hasFormat("text/uri-list")) {
        pe->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* pe)
{
    QList<QUrl> urlList = pe->mimeData()->urls();
    QString     str;
    foreach(QUrl url, urlList) {
        str += url.toString();
    }
    if (maybeSave()) {
        if (!str.isEmpty())
        {
            scribbleArea->openImage(str.replace("file:///", ""));
        }
    }
}

void MainWindow::save()
{
    QAction *action = qobject_cast<QAction *>(sender());
    QByteArray fileFormat = action->data().toByteArray();
    saveFile(fileFormat);
}

void MainWindow::penColor()
{
    QColor newColor = QColorDialog::getColor(scribbleArea->penColor());
    if (newColor.isValid())
        scribbleArea->setPenColor(newColor);
}

void MainWindow::penWidth()
{
    bool ok;
    int newWidth = QInputDialog::getInt(this, tr("Пензель"),
                                        tr("Оберіть розмір:"),
                                        scribbleArea->penWidth(),
                                        1, 50, 1, &ok);
    if (ok)
        scribbleArea->setPenWidth(newWidth);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("Про програму"),
        tr("<p>Програма для малювання з фунціями зміни кольору і розміру пензля, перетаскування зображення " \
           "малювання фігур, збереження і завантажування.</p>"));
}

void MainWindow::selectRect()
{
    scribbleArea->setFigure(1);
    indicatorAct->setText(tr("Обрано прямокутник"));
    indicatorAct->setVisible(true);
}
void MainWindow::selectTriang()
{
    scribbleArea->setFigure(2);
    indicatorAct->setText(tr("Обрано трикутник"));
    indicatorAct->setVisible(true);
}
void MainWindow::selectCircle()
{
    scribbleArea->setFigure(3);
    indicatorAct->setText(tr("Обрано коло"));
    indicatorAct->setVisible(true);
}
void MainWindow::selectNone()
{
    scribbleArea->setFigure(0);
    indicatorAct->setText(tr(""));
    indicatorAct->setVisible(false);
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Відкрити..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    foreach (QByteArray format, QImageWriter::supportedImageFormats()) {
        QString text = tr("%1...").arg(QString(format).toUpper());
        QAction *action = new QAction(text, this);
        action->setData(format);
        connect(action, SIGNAL(triggered()), this, SLOT(save()));
        saveAsActs.append(action);
    }

    exitAct = new QAction(tr("&Вийти"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    penColorAct = new QAction(tr("&Обрати колір..."), this);
    connect(penColorAct, SIGNAL(triggered()), this, SLOT(penColor()));

    penWidthAct = new QAction(tr("Обрати &розмір..."), this);
    connect(penWidthAct, SIGNAL(triggered()), this, SLOT(penWidth()));

    clearScreenAct = new QAction(tr("&Очистити екран"), this);
    clearScreenAct->setShortcut(tr("Ctrl+L"));
    connect(clearScreenAct, SIGNAL(triggered()), scribbleArea, SLOT(clearImage()));

    rectAct = new QAction(tr("&Прямокутник"), this);
    connect(rectAct, SIGNAL(triggered()), this, SLOT(selectRect()));

    triangAct = new QAction(tr("&Трикутник"), this);
    connect(triangAct, SIGNAL(triggered()), this, SLOT(selectTriang()));

    circleAct = new QAction(tr("&Коло"), this);
    connect(circleAct, SIGNAL(triggered()), this, SLOT(selectCircle()));

    clearFigureAct = new QAction(tr("&Відмінити фігуру"), this);
    connect(clearFigureAct, SIGNAL(triggered()), this, SLOT(selectNone()));

    aboutAct = new QAction(tr("&Про програму"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    indicatorAct = new QAction(tr(""), this);
    indicatorAct->setVisible(false);
}

void MainWindow::createMenus()
{
    saveAsMenu = new QMenu(tr("&Зберегти як"), this);
    foreach (QAction *action, saveAsActs)
        saveAsMenu->addAction(action);

    fileMenu = new QMenu(tr("&Файл"), this);
    fileMenu->addAction(openAct);
    fileMenu->addMenu(saveAsMenu);
    fileMenu->addSeparator();
    fileMenu->addAction(aboutAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    figureMenu = new QMenu(tr("&Малювати фігури"), this);
    figureMenu->addAction(rectAct);
    figureMenu->addAction(triangAct);
    figureMenu->addAction(circleAct);
    figureMenu->addSeparator();
    figureMenu->addAction(clearFigureAct);

    optionMenu = new QMenu(tr("&Опції"), this);
    optionMenu->addAction(penColorAct);
    optionMenu->addAction(penWidthAct);
    optionMenu->addSeparator();
    optionMenu->addMenu(figureMenu);
    optionMenu->addSeparator();
    optionMenu->addAction(clearScreenAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(optionMenu);
    menuBar()->addSeparator();
    menuBar()->addAction(indicatorAct);
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu* contextMenu = new QMenu(this);

    contextMenu->addAction(rectAct);
    contextMenu->addAction(triangAct);
    contextMenu->addAction(circleAct);
    contextMenu->addAction(clearFigureAct);
    contextMenu->addSeparator();
    contextMenu->addAction(clearScreenAct);

    contextMenu->exec(event->globalPos());
}

bool MainWindow::maybeSave()
{
    if (scribbleArea->isModified()) {
       QMessageBox::StandardButton ret;

       ret = QMessageBox::warning(this, tr("Малювання"),
                          tr("Зображення було змінене.\n"
                             "Чи хочете ви зберегти зміни?"),
                          QMessageBox::Save | QMessageBox::Discard
                          | QMessageBox::Cancel);

        if (ret == QMessageBox::Save) {
            return saveFile("png");

        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

bool MainWindow::saveFile(const QByteArray &fileFormat)
{
    QString initialPath = QDir::currentPath() + "/untitled." + fileFormat;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Зберегти як"),
                               initialPath,
                               tr("%1 Files (*.%2);;All Files (*)")
                               .arg(QString::fromLatin1(fileFormat.toUpper()))
                               .arg(QString::fromLatin1(fileFormat)));

    if (fileName.isEmpty()) {
        return false;
    } else {
        return scribbleArea->saveImage(fileName, fileFormat.constData());
    }
}
