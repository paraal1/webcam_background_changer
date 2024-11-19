#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <opencv2/opencv.hpp>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void processFrame();

private:
    QLabel *videoLabel;                      // Eticheta pentru afișarea video
    QTimer *timer;                           // Timer pentru actualizare
    cv::VideoCapture cap;                    // Obiect pentru capturarea video
    cv::Ptr<cv::BackgroundSubtractor> bgSubtractor; // Detectare prim-plan
};

#endif // MAINWINDOW_H
