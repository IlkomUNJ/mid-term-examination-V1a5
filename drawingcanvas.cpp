#include "drawingcanvas.h"
#include <QColor> // Untuk warna magenta

DrawingCanvas::DrawingCanvas(QWidget *parent) : QWidget(parent) {
    // Set a minimum size for the canvas
    setMinimumSize(this->WINDOW_WIDTH, this->WINDOW_HEIGHT);
    // Set a solid background color
    setStyleSheet("background-color: white; border: 1px solid gray;");

    // 1. Horizontal Segment Body (Pola 77, 118)
    bool p1[3][3] = {{0, 0, 0}, {1, 1, 1}, {0, 0, 0}};
    m_segment_filters.push_back(CustomMatrix(p1));

    // 2. Thick Segment Body (Pola 78, 118)
    bool p2[3][3] = {{1, 1, 1}, {1, 1, 1}, {0, 0, 0}};
    m_segment_filters.push_back(CustomMatrix(p2));

    // 3. Slanted/Thick Segment (Pola 78, 117)
    bool p3[3][3] = {{0, 1, 1}, {1, 1, 1}, {0, 0, 0}};
    m_segment_filters.push_back(CustomMatrix(p3));

    // 4. Slanted/Thick Segment Mirror (Pola 78, 122)
    bool p4[3][3] = {{1, 1, 0}, {1, 1, 1}, {0, 0, 0}};
    m_segment_filters.push_back(CustomMatrix(p4));

    // 5. Vertical Continuation (Pola 81, 114)
    bool p5[3][3] = {{0, 1, 1}, {0, 1, 1}, {0, 0, 0}};
    m_segment_filters.push_back(CustomMatrix(p5));

    // 6. Vertical Continuation Mirror (Pola 81, 125)
    bool p6[3][3] = {{1, 1, 0}, {1, 1, 0}, {0, 0, 0}};
    m_segment_filters.push_back(CustomMatrix(p6));

    // 7. Segment End (Pola 77, 117) - penting untuk kelanjutan
    bool p7[3][3] = {{0, 0, 0}, {0, 1, 1}, {0, 0, 0}};
    m_segment_filters.push_back(CustomMatrix(p7));

    // 8. Segment End Mirror (Pola 77, 122) - penting untuk kelanjutan
    bool p8[3][3] = {{0, 0, 0}, {1, 1, 0}, {0, 0, 0}};
    m_segment_filters.push_back(CustomMatrix(p8));
}

void DrawingCanvas::clearPoints(){
    m_points.clear();
    m_segment_candidates.clear(); // Bersihkan juga kandidat
    // Trigger a repaint to clear the canvas
    update();
}

void DrawingCanvas::paintLines(){
    isPaintLinesClicked = true;
    update();
}

void DrawingCanvas::segmentDetection(){
    m_segment_candidates.clear(); // Hapus kandidat lama

    QPixmap pixmap = this->grab(); //
    QImage image = pixmap.toImage();

    cout << "image width " << image.width() << endl;
    cout << "image height " << image.height() << endl;

    // Iterasi melalui setiap piksel (i, j) yang merupakan pusat dari jendela 3x3
    for(int i = 1; i < image.width()-1;i++){
        for(int j = 1; j < image.height()-1;j++){
            bool local_window[3][3] = {false};

            // Ambil jendela 3x3
            for(int m=-1;m<=1;m++){
                for(int n=-1;n<=1;n++){
                    QRgb rgbValue = image.pixel(i+m, j+n);
                    // True jika piksel bukan putih
                    local_window[m+1][n+1] = (rgbValue != 0xffffffff);
                }
            }

            CustomMatrix mat(local_window);

            // Periksa apakah matriks lokal cocok dengan salah satu filter
            if (!mat.isEmpty()) {

                for (const auto& filter : m_segment_filters) {
                    bool match = true;
                    for (int r = 0; r < 3; ++r) {
                        for (int c = 0; c < 3; ++c) {
                            // Jika ada ketidakcocokan, pola ini gagal
                            if (mat.mat[r][c] != filter.mat[r][c]) {
                                match = false;
                                break;
                            }
                        }
                        if (!match) break;
                    }

                    if (match) {
                        // Jika cocok, tambahkan titik pusat (i, j) sebagai kandidat segmen
                        m_segment_candidates.append(QPoint(i, j));
                        break; // Pindah ke piksel (i, j) berikutnya
                    }
                }
            }
        }
    }
    cout << "Total segment candidates found: " << m_segment_candidates.size() << endl;
    update(); // Minta repaint untuk menampilkan penanda ungu
    return;
}

void DrawingCanvas::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 1. Gambar Titik Biru
    QPen pen(Qt::blue, 5);
    painter.setPen(pen);
    painter.setBrush(QBrush(Qt::blue));

    for (const QPoint& point : std::as_const(m_points)) {
        painter.drawEllipse(point, 3, 3);
    }

    // 2. Gambar Garis Merah
    if(isPaintLinesClicked){
        cout << "paint lines block is called" << endl;
        pen.setColor(Qt::red);
        pen.setWidth(4); // 4-pixel wide line
        pen.setStyle(Qt::SolidLine);
        painter.setPen(pen);

        for(int i=0;i<m_points.size()-1;i+=2){
            painter.drawLine(m_points[i], m_points[i+1]);
        }
        isPaintLinesClicked = false;

        // Kembalikan pen ke biru (atau biarkan penanda ungu yang mengambil alih)
        pen.setColor(Qt::blue);
        painter.setPen(pen);
    }

    // 3. NEW: Penandaan Kandidat Segmen (Sasaran 3)
    if (!m_segment_candidates.isEmpty()) {
        QPen purplePen(Qt::magenta, 1); // Penanda ungu (magenta)
        painter.setPen(purplePen);
        painter.setBrush(QBrush(Qt::magenta));

        const int rectSize = 4; // Ukuran persegi penanda

        for (const QPoint& point : std::as_const(m_segment_candidates)) {
            // Gambar persegi kecil di sekitar titik pusat
            painter.drawRect(point.x() - rectSize/2, point.y() - rectSize/2, rectSize, rectSize);
        }
    }
}

void DrawingCanvas::mousePressEvent(QMouseEvent *event) {
    // Add the mouse click position to our vector of points
    m_points.append(event->pos());
    // Trigger a repaint
    update();
}
