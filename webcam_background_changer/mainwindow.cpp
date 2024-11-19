#include "mainwindow.h"
#include <QVBoxLayout>
#include <QImage>
#include <QPixmap>
#include <stdexcept>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), bgSubtractor(cv::createBackgroundSubtractorMOG2()) {
    // Configurăm fereastra principală
    setWindowTitle("Webcam Filter");
    resize(800, 600);

    // Creăm un QLabel pentru afișarea fluxului video
    videoLabel = new QLabel(this);
    videoLabel->setAlignment(Qt::AlignCenter);

    // Setăm layout-ul ferestrei
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(videoLabel);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Inițializăm camera
    if (!cap.open(0)) {
        throw std::runtime_error("Unable to access webcam");
    }

    // Configurăm timer-ul pentru actualizarea cadrelor
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::processFrame);
    timer->start(30); // 30ms între cadre (~30 FPS)
}

MainWindow::~MainWindow() {
    cap.release();
}

void MainWindow::processFrame() {
    cv::Mat frame, foregroundMask, blurredFrame, result;

    // Capture a frame from the webcam
    cap >> frame;
    if (frame.empty()) return;

    // Detect foreground using BackgroundSubtractorMOG2
    bgSubtractor->apply(frame, foregroundMask);

    // Refine the foreground mask: Morphological operations to reduce noise
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
    cv::morphologyEx(foregroundMask, foregroundMask, cv::MORPH_CLOSE, kernel);

    // Ensure the foreground mask is binary (0 or 255)
    cv::threshold(foregroundMask, foregroundMask, 128, 255, cv::THRESH_BINARY);

    // Apply Gaussian blur to the entire frame to create a blurred version
    cv::GaussianBlur(frame, blurredFrame, cv::Size(21, 21), 0);

    // Combine foreground and blurred background
    result = cv::Mat::zeros(frame.size(), frame.type()); // Initialize the result frame
    frame.copyTo(result, foregroundMask);               // Copy foreground where mask is 255
    blurredFrame.copyTo(result, 255 - foregroundMask);  // Copy blurred background where mask is 0

    // Convert the result to RGB format for Qt
    cv::cvtColor(result, result, cv::COLOR_BGR2RGB);
    QImage image(result.data, result.cols, result.rows, result.step, QImage::Format_RGB888);

    // Display the processed frame in the QLabel
    videoLabel->setPixmap(QPixmap::fromImage(image));
    videoLabel->resize(image.size());
}

