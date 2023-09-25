#ifndef PREVIEWWINDOW_H
#define PREVIEWWINDOW_H

#pragma once

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QGraphicsScene>
#include <QLabel>
#include <QPushButton>
#include "supervisor.h"

#include "drawingarea.h"

#define REFERENCE_WIDTH 290
#define REFERENCE_HEIGHT 460

#define REFERENCE_POSX 80
#define REFERENCE_POSY 50

class MainWindow;

class PreviewWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit PreviewWindow(Supervisor* sv, QWidget *parent = 0);

    void initialize(MainWindow* window);

    DrawingArea* getDrawingArea(void);

    bool DrawImage(void);
    bool ResetMessageBox(void);
    QRect UpdateMarqueeSizeLabel(void);

    QPushButton *m_CloseButton;

    QImage getPreviewImage(void);

    void DeleteMarquee();

private:
    MainWindow* m_main_window;

    QFrame *m_DrawFrame;
    QFrame *m_CloseFrame;
    QFrame *m_SizeFrame;

    QLabel *m_WidthIconImageLabel;
    QLabel *m_WidthSizeLabel;
    QLabel *m_HeightIconImageLabel;
    QLabel *m_HeightSizeLabel;

    QPushButton *m_DeleteButton;
    QPushButton *m_FocusPositionButton;

    DrawingArea* m_graphicsView; //プレビュー画像を描画用にサイズ調整した描画領域
    QGraphicsScene* m_Scene_;
    Supervisor *m_sv;

    QImage m_preview_image; //実際のプレビュー画像

    void PreviewImageAdjustment(void);
    QPoint PreviewImagePositionAdjustment(double preview_img_width, double preview_img_height);

public slots:
    bool on_Close_Button_pressed(bool Confirm=true);
private slots:
    void on_Delete_Button_pressed();
    void on_FocusPosition_Button_pressed();

    void MarqueeCreated(bool exist);
};

#endif // PREVIEWWINDOW_H
