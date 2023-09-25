#include "drawingarea.h"

#define MARQUEE_WIDTH 2
#define MARQUEE_WIDTH_PLUS 3
#define MINIMUM_MARQUEE_SIZE 10

DrawingArea::DrawingArea(QWidget *parent) :
    QGraphicsView(parent),
    m_rubberBand_(nullptr),
    m_MousePressed(false),
    m_MouseEdgePressed(false),
    m_MousePos(0, 0),
    m_marqueeEdge(TopEdge),
    m_MarqueeEnebled(true)
{
}

DrawingArea::~DrawingArea(){
    if (m_mqrqueeCreatedCallBack)
    {
        m_mqrqueeCreatedCallBack(false, QRect(0,0,0,0));
    }

    this->SetMarqueeCreatedCallBack(nullptr);
    this->SetMarqueeCreatedForDelButtonCallBack(nullptr);

    if(m_rubberBand_){
        delete m_rubberBand_;
        m_rubberBand_ = nullptr;
    }
}

void DrawingArea::setMarqueeEnebled(bool Enabled)
{
    m_MarqueeEnebled = Enabled;
}

void DrawingArea::mousePressEvent(QMouseEvent *event)
{
    if(!m_MarqueeEnebled){
        return;
    }

    //常にマウスの位置を取得するために(デフォルトではクリック時の座標しか取得できないため)
    this->viewport()->setMouseTracking(true);

    //qDebug() << "////////////////////////////////////////////////////////////////////";
    //qDebug() << "PressMouse" << event->pos();

    m_MousePressed = true;

    // QRubberBand をまだ作成していない場合は、作成する。
    if (!m_rubberBand_) {
        m_rubberBand_ = new MyRubberBand(MyRubberBand::Rectangle, this);
    }

    //マーキーが既にあるかどうか？
    if(m_rubberBand_->m_existMarquee == true){
        //qDebug() << "s event->pos()" << event->pos();

        if(CheckMouseOnEdge(event->pos())){
            m_MouseEdgePressed = true;
            m_rubberBand_->m_onMouse = true;
        }else {
            //クリック時の座標がXY共にマーキー内にあるとき
            if(this->CheckMouseOnMarquee(event->pos())){

                m_MousePos = CalculateDiff(event->pos(), m_rubberBand_->m_startPos_, false);
                //qDebug() << "On Mouse";
                QApplication::setOverrideCursor(Qt::ClosedHandCursor);
                m_Before_Pos = event->pos();

                //qDebug() << "CurrentPos in marquee" << event->pos();
                m_rubberBand_->m_onMouse = true;
                //マーキーの内側をクリックしたとき
            }else {
                m_rubberBand_->m_existMarquee = false;
                m_rubberBand_->m_onMouse = false;
                //マーキーがあってもその外側でクリックしたとき
            }
        }
    }
    //マーキーアニメーションストップ
    m_rubberBand_->m_timer->stop();

    //新規マーキー作成の準備
    if(m_rubberBand_->m_existMarquee == false){
        QApplication::setOverrideCursor(Qt::ArrowCursor);
        m_rubberBand_->repaint();

        // マウスをクリックしたときの座標を取得し、始点とする。
        m_rubberBand_->m_startPos_ = event->pos();

        // QRubberBand ウィジェットのジオメトリを
        // 始点がクリックした座標で、大きさが縦横ともに 0 の長方形に変更する。
        m_rubberBand_->setGeometry(QRect(m_rubberBand_->m_startPos_, QSize()));

        // QRubberBand ウィジェットを表示する。
        m_rubberBand_->show();
    }
}

void DrawingArea::mouseMoveEvent(QMouseEvent *event)
{
    if(!m_MarqueeEnebled || !m_rubberBand_){
        return;
    }

    if(m_MouseEdgePressed){
        //マーキーの拡大縮小

        m_rubberBand_->m_existMarquee = false;

        QPoint curPos = MarqueeRangeLimit(event->pos());
        if(m_marqueeEdge == TopEdge){
            curPos.setX(m_rubberBand_->m_endPos_.x());
            //マーキーサイズ制限
            if(this->CalculateDiff(curPos, m_rubberBand_->m_endPos_, true).y() < MINIMUM_MARQUEE_SIZE){
                m_rubberBand_->m_startPos_.setY(m_rubberBand_->m_endPos_.y() - MINIMUM_MARQUEE_SIZE);

                if(m_rubberBand_->m_endPos_.y() != this->height()-this->frameWidth()){
                    m_marqueeEdge = DownEdge;
                }
            }else {
                if(curPos.y() <= 0){
                    m_rubberBand_->m_startPos_.setY(0);
                }else {
                    m_rubberBand_->m_startPos_.setY(curPos.y());
                }
            }
        }else if(m_marqueeEdge == DownEdge){
            curPos.setX(m_rubberBand_->m_endPos_.x());
            //マーキーサイズ制限
            if(this->CalculateDiff(curPos, m_rubberBand_->m_startPos_, true).y() < MINIMUM_MARQUEE_SIZE){
                m_rubberBand_->m_endPos_.setY(m_rubberBand_->m_startPos_.y() + MINIMUM_MARQUEE_SIZE);

                if(m_rubberBand_->m_startPos_.y() != 0){
                    m_marqueeEdge = TopEdge;
                }
            }else {
                if(curPos.y() >= this->height()-this->frameWidth()){
                    m_rubberBand_->m_endPos_.setY(this->height()-this->frameWidth());
                }else {
                    m_rubberBand_->m_endPos_.setY(curPos.y());
                }
            }
        }else if(m_marqueeEdge == LeftEdge){
            curPos.setY(m_rubberBand_->m_endPos_.y());
            //マーキーサイズ制限
            if(this->CalculateDiff(curPos, m_rubberBand_->m_endPos_, true).x() < MINIMUM_MARQUEE_SIZE){
                m_rubberBand_->m_startPos_.setX(m_rubberBand_->m_endPos_.x() - MINIMUM_MARQUEE_SIZE);

                if(m_rubberBand_->m_endPos_.x() != this->width()-this->frameWidth()){
                    m_marqueeEdge = RightEdge;
                }
            }else {
                if(curPos.x() <= 0){
                    m_rubberBand_->m_startPos_.setX(0);
                }else {
                    m_rubberBand_->m_startPos_.setX(curPos.x());
                }
            }
        }else if(m_marqueeEdge == RightEdge){
            curPos.setY(m_rubberBand_->m_endPos_.y());
            //マーキーサイズ制限
            if(this->CalculateDiff(curPos, m_rubberBand_->m_startPos_, true).x() < MINIMUM_MARQUEE_SIZE){
                m_rubberBand_->m_endPos_.setX(m_rubberBand_->m_startPos_.x() + MINIMUM_MARQUEE_SIZE);

                if(m_rubberBand_->m_startPos_.x() != 0){
                    m_marqueeEdge = LeftEdge;
                }
            }else {
                if(curPos.x() >= this->width()-this->frameWidth()){
                    m_rubberBand_->m_endPos_.setX(this->width()-this->frameWidth());
                }else {
                    m_rubberBand_->m_endPos_.setX(curPos.x());
                }
            }
        }else if(m_marqueeEdge == TopLeftPos){
            //マーキーサイズ制限
            if(this->CalculateDiff(curPos, m_rubberBand_->m_endPos_, true).x() < MINIMUM_MARQUEE_SIZE){
                m_rubberBand_->m_startPos_.setX(m_rubberBand_->m_endPos_.x() - MINIMUM_MARQUEE_SIZE);

                if(m_rubberBand_->m_endPos_.x() != this->width()-this->frameWidth()){
                    m_marqueeEdge = TopRightPos;
                }
            }else {
                if(curPos.x() <= 0){
                    m_rubberBand_->m_startPos_.setX(0);
                }else {
                    m_rubberBand_->m_startPos_.setX(curPos.x());
                }
            }
            if(this->CalculateDiff(curPos, m_rubberBand_->m_endPos_, true).y() < MINIMUM_MARQUEE_SIZE){
                m_rubberBand_->m_startPos_.setY(m_rubberBand_->m_endPos_.y() - MINIMUM_MARQUEE_SIZE);

                if(m_rubberBand_->m_endPos_.y() != this->height()-this->frameWidth()){
                    m_marqueeEdge = DownLeftPos;
                }
            }else {
                if(curPos.y() <= 0){
                    m_rubberBand_->m_startPos_.setY(0);
                }else {
                    m_rubberBand_->m_startPos_.setY(curPos.y());
                }
            }
        }else if(m_marqueeEdge == TopRightPos){
            //マーキーサイズ制限
            if(this->CalculateDiff(curPos, m_rubberBand_->m_startPos_, true).x() < MINIMUM_MARQUEE_SIZE){
                m_rubberBand_->m_endPos_.setX(m_rubberBand_->m_startPos_.x() + MINIMUM_MARQUEE_SIZE);

                if(m_rubberBand_->m_startPos_.x() != 0){
                    m_marqueeEdge = TopLeftPos;
                }
            }else {
                if(curPos.x() >= this->width()-this->frameWidth()){
                    m_rubberBand_->m_endPos_.setX(this->width()-this->frameWidth());
                }else {
                    m_rubberBand_->m_endPos_.setX(curPos.x());
                }
            }
            if(this->CalculateDiff(curPos, m_rubberBand_->m_endPos_, true).y() < MINIMUM_MARQUEE_SIZE){
                m_rubberBand_->m_startPos_.setY(m_rubberBand_->m_endPos_.y() - MINIMUM_MARQUEE_SIZE);

                if(m_rubberBand_->m_endPos_.y() != this->height()-this->frameWidth()){
                    m_marqueeEdge = DownRightPos;
                }
            }else {
                if(curPos.y() <= 0){
                    m_rubberBand_->m_startPos_.setY(0);
                }else {
                    m_rubberBand_->m_startPos_.setY(curPos.y());
                }
            }
        }else if(m_marqueeEdge == DownLeftPos){
            //マーキーサイズ制限
            if(this->CalculateDiff(curPos, m_rubberBand_->m_endPos_, true).x() < MINIMUM_MARQUEE_SIZE){
                m_rubberBand_->m_startPos_.setX(m_rubberBand_->m_endPos_.x() - MINIMUM_MARQUEE_SIZE);

                if(m_rubberBand_->m_endPos_.x() != this->width()-this->frameWidth()){
                    m_marqueeEdge = DownRightPos;
                }
            }else {
                if(curPos.x() <= 0){
                    m_rubberBand_->m_startPos_.setX(0);
                }else {
                    m_rubberBand_->m_startPos_.setX(curPos.x());
                }
            }
            if(this->CalculateDiff(curPos, m_rubberBand_->m_startPos_, true).y() < MINIMUM_MARQUEE_SIZE){
                m_rubberBand_->m_endPos_.setY(m_rubberBand_->m_startPos_.y() + MINIMUM_MARQUEE_SIZE);

                if(m_rubberBand_->m_startPos_.y() != 0){
                    m_marqueeEdge = TopLeftPos;
                }
            }else {
                if(curPos.y() >= this->height()-this->frameWidth()){
                    m_rubberBand_->m_endPos_.setY(this->height()-this->frameWidth());
                }else {
                    m_rubberBand_->m_endPos_.setY(curPos.y());
                }
            }
        }else if(m_marqueeEdge == DownRightPos){
            //マーキーサイズ制限
            if(this->CalculateDiff(curPos, m_rubberBand_->m_startPos_, true).x() < MINIMUM_MARQUEE_SIZE){
                m_rubberBand_->m_endPos_.setX(m_rubberBand_->m_startPos_.x() + MINIMUM_MARQUEE_SIZE);

                if(m_rubberBand_->m_startPos_.x() != 0){
                    m_marqueeEdge = DownLeftPos;
                }
            }else {
                if(curPos.x() >= this->width()-this->frameWidth()){
                    m_rubberBand_->m_endPos_.setX(this->width()-this->frameWidth());
                }else {
                    m_rubberBand_->m_endPos_.setX(curPos.x());
                }
            }
            if(this->CalculateDiff(curPos, m_rubberBand_->m_startPos_, true).y() < MINIMUM_MARQUEE_SIZE){
                m_rubberBand_->m_endPos_.setY(m_rubberBand_->m_startPos_.y() + MINIMUM_MARQUEE_SIZE);

                if(m_rubberBand_->m_startPos_.y() != 0){
                    m_marqueeEdge = TopRightPos;
                }
            }else {
                if(curPos.y() >= this->height()-this->frameWidth()){
                    m_rubberBand_->m_endPos_.setY(this->height()-this->frameWidth());
                }else {
                    m_rubberBand_->m_endPos_.setY(curPos.y());
                }
            }
        }

        // QRubberBand ウィジェットのジオメトリを
        // 始点がクリックした座標で、終点が現在の現在のマウスの座標に変更する。
        m_rubberBand_->setGeometry(QRect(m_rubberBand_->m_startPos_, m_rubberBand_->m_endPos_).normalized());
    }else if(m_MousePressed){

        //マーキー移動
        if(m_rubberBand_->m_onMouse == true){

            //qDebug() << "///////////////////////////////////////////////";
            //qDebug() << "m_rubberBand_->m_startPos_ :" << m_rubberBand_->m_startPos_;
            //qDebug() << "event->pos() :" << event->pos();
            //qDebug() << "m_Before_Pos :" << m_Before_Pos;
            m_rubberBand_->m_existMarquee = false;

            m_Diff_Pos = CalculateDiff(event->pos(), m_Before_Pos, false);
            if(m_rubberBand_->m_startPos_.x() == 0){
                if(event->pos().x() <= m_MousePos.x()){
                    m_Diff_Pos.setX(0);
                }
            }
            if(m_rubberBand_->m_startPos_.y() == 0){
                if(event->pos().y() <= m_MousePos.y()){
                    m_Diff_Pos.setY(0);
                }
            }
            //qDebug() << "m_diff :" << m_Diff_Pos.x() << m_Diff_Pos.y();
            m_Before_Pos = event->pos();

            //start position
            int smove_x = m_rubberBand_->m_startPos_.x() + m_Diff_Pos.x();
            int smove_y = m_rubberBand_->m_startPos_.y() + m_Diff_Pos.y();
            //qDebug() << "smove :" << smove_x << smove_y;

            //Startマーキー移動制限
            if(smove_x < 0){
                smove_x = 0;
                m_Diff_Pos.setX(0-m_rubberBand_->m_startPos_.x());
            }
            if(smove_y < 0){
                smove_y = 0;
                m_Diff_Pos.setY(0-m_rubberBand_->m_startPos_.y());
            }

            //qDebug() << "m_endPos_ :" << m_rubberBand_->m_endPos_;
            if(m_rubberBand_->m_endPos_.x() == this->width()-this->frameWidth()){
                if(event->pos().x() >= smove_x + m_MousePos.x()){
                    m_Diff_Pos.setX(0);
                }
            }
            if(m_rubberBand_->m_endPos_.y() == this->height()-this->frameWidth()){
                if(event->pos().y() >= smove_y + m_MousePos.y()){
                    m_Diff_Pos.setY(0);
                }
            }

            //end position
            //qDebug() << "marquee :" << m_rubberBand_->width() << m_rubberBand_->height();
            int emove_x = m_rubberBand_->m_endPos_.x() + m_Diff_Pos.x();
            int emove_y = m_rubberBand_->m_endPos_.y() + m_Diff_Pos.y();
            //qDebug() << "emove :" << emove_x << emove_y;

            //qDebug() << "area :" << this->width() << this->height();
            if(emove_x >= this->width()-this->frameWidth()){
                smove_x = (this->width()-this->frameWidth()) - (m_rubberBand_->m_endPos_.x() - m_rubberBand_->m_startPos_.x());
                emove_x = this->width()-this->frameWidth();
            }
            if(emove_y >= this->height()-this->frameWidth()){
                smove_y = (this->height()-this->frameWidth()) - (m_rubberBand_->m_endPos_.y() - m_rubberBand_->m_startPos_.y());
                emove_y = this->height()-this->frameWidth();
            }
            m_rubberBand_->m_startPos_.setX(smove_x);
            m_rubberBand_->m_startPos_.setY(smove_y);
            m_rubberBand_->m_endPos_.setX(emove_x);
            m_rubberBand_->m_endPos_.setY(emove_y);

            //m_rubberBand_->move(m_rubberBand_->m_startPos_);
            m_rubberBand_->setGeometry(QRect(m_rubberBand_->m_startPos_, m_rubberBand_->m_endPos_).normalized());

        //マーキー新規作成
        }else {
            //qDebug() << "Create Marquee";
            QPoint endPos = event->pos();

            //マーキーサイズ制限
            m_rubberBand_->m_endPos_ = MarqueeRangeLimit(endPos);

            // QRubberBand ウィジェットのジオメトリを
            // 始点がクリックした座標で、終点が現在の現在のマウスの座標に変更する。
            m_rubberBand_->setGeometry(QRect(m_rubberBand_->m_startPos_, m_rubberBand_->m_endPos_).normalized());
        }
    }else {
        //マウスクリックなし時のアイコン変更処理
        if(!CheckMouseOnEdge(event->pos())){
            if(this->CheckMouseOnMarquee(event->pos())){
                QApplication::setOverrideCursor(Qt::OpenHandCursor);
            }else {
                QApplication::setOverrideCursor(Qt::ArrowCursor);
            }
        }
    }
}

void DrawingArea::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)

    if(!m_MarqueeEnebled){
        return;
    }

    m_MousePos.setX(0);
    m_MousePos.setY(0);

    //移動なしのクリック時にend座標が0になったときにカレント座標を代入する
    if(m_rubberBand_->m_startPos_ == event->pos() &&
            (m_marqueeEdge != TopLeftPos
             || (m_marqueeEdge == TopLeftPos && m_rubberBand_->m_onMouse == false))){
        m_rubberBand_->m_endPos_ = event->pos();
    }
    //qDebug() << "event->pos()" << event->pos();

    //マーキー最小サイズ制限
    if(this->CalculateDiff(m_rubberBand_->m_startPos_, m_rubberBand_->m_endPos_, true).x() < MINIMUM_MARQUEE_SIZE ||
            this->CalculateDiff(m_rubberBand_->m_startPos_, m_rubberBand_->m_endPos_, true).y() < MINIMUM_MARQUEE_SIZE){
        m_rubberBand_->m_existMarquee = false;
        m_rubberBand_->m_onMouse = false;
        //最小サイズ以下の場合描画しない
        m_rubberBand_->hide();
        m_rubberBand_->m_startPos_.setX(0);
        m_rubberBand_->m_startPos_.setY(0);
        m_rubberBand_->m_endPos_.setX(0);
        m_rubberBand_->m_endPos_.setY(0);
        QApplication::setOverrideCursor(Qt::ArrowCursor);

        //マーキーがないときサイズ表示を画像サイズに戻す
        this->UpdateMarqueeSizeLabel(this->width(), this->height());
    }else {

        if(m_rubberBand_->m_onMouse == true){
            QApplication::setOverrideCursor(Qt::OpenHandCursor);
        }else {
            this->CheckMarqueeArea(event->pos());
        }
        //マーキーの開始点と終了点をマウスリリース時に確認し、常に左上が開始、右下が終了点となるように座標を修正する
        this->OptimizeArea();

        m_rubberBand_->m_existMarquee = true;
        //m_rubberBand_->m_onMouse = false;
        //マーキーアニメーションスタート
        m_rubberBand_->m_timer->start();

        m_rubberBand_->repaint();

        // 選択した範囲を出力する。
        //qDebug() << "ReleaseMouse" << m_rubberBand_->m_startPos_ << event->pos() << m_rubberBand_->m_endPos_;
        //qDebug() << "MarqueeSize" << m_rubberBand_->geometry();

        if(m_rubberBand_->width() == 0 && m_rubberBand_->height() == 0){
            qDebug() << "end event->pos()" << event->pos();
        }
        this->UpdateMarqueeSizeLabel(m_rubberBand_->width(), m_rubberBand_->height());
    }

    this->m_MousePressed = false;
    this->m_MouseEdgePressed = false;
}

//マーキーの座標を最適化する
void DrawingArea::OptimizeArea()
{
    int start_x = m_rubberBand_->m_startPos_.x();
    int start_y = m_rubberBand_->m_startPos_.y();
    int end_x = m_rubberBand_->m_endPos_.x();
    int end_y = m_rubberBand_->m_endPos_.y();

    m_rubberBand_->m_startPos_.setX(end_x > start_x ? start_x : end_x);
    m_rubberBand_->m_startPos_.setY(end_y > start_y ? start_y : end_y);
    m_rubberBand_->m_endPos_.setX(end_x > start_x ? end_x : start_x);
    m_rubberBand_->m_endPos_.setY(end_y > start_y ? end_y : start_y);
}

//マーキーサイズの範囲制限
QPoint DrawingArea::MarqueeRangeLimit(QPoint current_pos)
{
    QPoint ResultAreaSize;

    //マーキーサイズ制限
    if(current_pos.x() >= this->width()-this->frameWidth()){
        ResultAreaSize.setX(this->width()-this->frameWidth());
    }else if(current_pos.x() <= 0){
        ResultAreaSize.setX(0);
    }else {
        ResultAreaSize.setX(current_pos.x());
    }

    if(current_pos.y() >= this->height()-this->frameWidth()){
        ResultAreaSize.setY(this->height()-this->frameWidth());
    }else if(current_pos.y() <= 0){
        ResultAreaSize.setY(0);
    }else {
        ResultAreaSize.setY(current_pos.y());
    }
    //qDebug() << "ResultAreaSize :" << ResultAreaSize;
    return ResultAreaSize;
}

//座標の差分を計算する
QPoint DrawingArea::CalculateDiff(QPoint A_pos, QPoint B_pos, bool mode)
{
    QPoint MarqueeAreaSize;

    if(mode){
        //width
        if(A_pos.x() > B_pos.x()){
            MarqueeAreaSize.setX(A_pos.x() - B_pos.x());
        }else if(A_pos.x() < B_pos.x()){
            MarqueeAreaSize.setX(B_pos.x() - A_pos.x());
        }else {
            MarqueeAreaSize.setX(0);
        }
        //height
        if(A_pos.y() > B_pos.y()){
            MarqueeAreaSize.setY(A_pos.y() - B_pos.y());
        }else if(A_pos.y() < B_pos.y()){
            MarqueeAreaSize.setY(B_pos.y() - A_pos.y());
        }else {
            MarqueeAreaSize.setY(0);
        }
    }else {
        MarqueeAreaSize.setX(A_pos.x() - B_pos.x());
        MarqueeAreaSize.setY(A_pos.y() - B_pos.y());
    }

    return MarqueeAreaSize;
}

//マーキー作成エリアの判別
void DrawingArea::CheckMarqueeArea(QPoint current_pos)
{
    if(current_pos.x() >= m_rubberBand_->m_startPos_.x() && current_pos.y() >= m_rubberBand_->m_startPos_.y()){
        m_marqueeArea = DownRight;
        //qDebug() << "DownRight area";
    }else if(current_pos.x() > m_rubberBand_->m_startPos_.x() && current_pos.y() < m_rubberBand_->m_startPos_.y()){
        m_marqueeArea = TopRight;
        //qDebug() << "TopRight area";
    }else if(current_pos.x() < m_rubberBand_->m_startPos_.x() && current_pos.y() > m_rubberBand_->m_startPos_.y()){
        m_marqueeArea = DownLeft;
        //qDebug() << "DownLeft area";
    }else if(current_pos.x() < m_rubberBand_->m_startPos_.x() && current_pos.y() < m_rubberBand_->m_startPos_.y()){
        m_marqueeArea = TopLeft;
        //qDebug() << "TopLeft area";
    }
}

//マウスのカレント座標がマーキー内にあるかチェック
bool DrawingArea::CheckMouseOnMarquee(QPoint current_pos)
{
    bool In_x = false;
    bool In_y = false;
    //マウスのX座標がマーキー内にあるか？
    if(current_pos.x() >= m_rubberBand_->m_startPos_.x() && current_pos.x() <= m_rubberBand_->m_endPos_.x()){
        In_x = true;
    }else if(current_pos.x() <= m_rubberBand_->m_startPos_.x() && current_pos.x() >= m_rubberBand_->m_endPos_.x()){
        In_x = true;
    }
    //マウスのY座標がマーキー内にあるか？
    if(current_pos.y() >= m_rubberBand_->m_startPos_.y() && current_pos.y() <= m_rubberBand_->m_endPos_.y()){
        In_y = true;
    }else if(current_pos.y() <= m_rubberBand_->m_startPos_.y() && current_pos.y() >= m_rubberBand_->m_endPos_.y()){
        In_y = true;
    }

    //マウスの座標がXY共にマーキー内にあるとき
    if(In_x && In_y){
        return true;
    }else {
        return false;
    }
}

//マウスのカレント座標がマーキーのエッジ上にあるかチェック
bool DrawingArea::CheckMouseOnEdge(QPoint current_pos)
{
    if(m_rubberBand_->m_existMarquee){
        //描画領域の右辺と下辺の3ピクセル分マウスイベントを取得しない不具合の対応
        if(m_rubberBand_->m_endPos_.x() == this->width()-this->frameWidth() || m_rubberBand_->m_endPos_.y() == this->height()-this->frameWidth()){
            //右辺と重なった場合
            if(m_rubberBand_->m_endPos_.x() - MARQUEE_WIDTH_PLUS -3 <= current_pos.x() && current_pos.x() <= m_rubberBand_->m_endPos_.x() + MARQUEE_WIDTH_PLUS -3){
                if(m_rubberBand_->m_startPos_.y() + MARQUEE_WIDTH_PLUS < current_pos.y() && current_pos.y() < m_rubberBand_->m_endPos_.y() - MARQUEE_WIDTH_PLUS){
                    QApplication::setOverrideCursor(Qt::SizeHorCursor);
                    m_marqueeEdge = RightEdge;
                    return true;
                }
            }
            //右上の点と重なった場合
            if((m_rubberBand_->m_endPos_.x() - MARQUEE_WIDTH_PLUS -3 <= current_pos.x() && current_pos.x() <= m_rubberBand_->m_endPos_.x() + MARQUEE_WIDTH_PLUS -3)
                     && (m_rubberBand_->m_startPos_.y() - MARQUEE_WIDTH_PLUS -3 <= current_pos.y() && current_pos.y() <= m_rubberBand_->m_startPos_.y() + MARQUEE_WIDTH_PLUS -3)){
                QApplication::setOverrideCursor(Qt::SizeBDiagCursor);
                m_marqueeEdge = TopRightPos;
                return true;
            //右下の点と重なった場合
            }
            if((m_rubberBand_->m_endPos_.x() - MARQUEE_WIDTH_PLUS -3 <= current_pos.x() && current_pos.x() <= m_rubberBand_->m_endPos_.x() + MARQUEE_WIDTH_PLUS -3)
                     && (m_rubberBand_->m_endPos_.y() - MARQUEE_WIDTH_PLUS -3 <= current_pos.y() && current_pos.y() <= m_rubberBand_->m_endPos_.y() + MARQUEE_WIDTH_PLUS -3)){
                QApplication::setOverrideCursor(Qt::SizeFDiagCursor);
                m_marqueeEdge = DownRightPos;
                return true;
            }
            //下辺と重なった場合
            if(m_rubberBand_->m_endPos_.y() - MARQUEE_WIDTH_PLUS -3 <= current_pos.y() && current_pos.y() <= m_rubberBand_->m_endPos_.y() + MARQUEE_WIDTH_PLUS -3){
                if(m_rubberBand_->m_startPos_.x() + MARQUEE_WIDTH_PLUS < current_pos.x() && current_pos.x() < m_rubberBand_->m_endPos_.x() - MARQUEE_WIDTH_PLUS){
                    QApplication::setOverrideCursor(Qt::SizeVerCursor);
                    m_marqueeEdge = DownEdge;
                    return true;
                }
            }
            //左下の点と重なった場合
            if((m_rubberBand_->m_startPos_.x() - MARQUEE_WIDTH_PLUS -3 <= current_pos.x() && current_pos.x() <= m_rubberBand_->m_startPos_.x() + MARQUEE_WIDTH_PLUS -3)
                     && (m_rubberBand_->m_endPos_.y() - MARQUEE_WIDTH_PLUS -3 <= current_pos.y() && current_pos.y() <= m_rubberBand_->m_endPos_.y() + MARQUEE_WIDTH_PLUS -3)){
                QApplication::setOverrideCursor(Qt::SizeBDiagCursor);
                m_marqueeEdge = DownLeftPos;
                return true;
            }
        }

        //左辺と重なった場合
        if(m_rubberBand_->m_startPos_.x() - MARQUEE_WIDTH <= current_pos.x() && current_pos.x() <= m_rubberBand_->m_startPos_.x() + MARQUEE_WIDTH){
            if(m_rubberBand_->m_startPos_.y() + MARQUEE_WIDTH < current_pos.y() && current_pos.y() < m_rubberBand_->m_endPos_.y() - MARQUEE_WIDTH){
                QApplication::setOverrideCursor(Qt::SizeHorCursor);
                m_marqueeEdge = LeftEdge;
                return true;
            }
        //右辺と重なった場合
        }else if(m_rubberBand_->m_endPos_.x() - MARQUEE_WIDTH <= current_pos.x() && current_pos.x() <= m_rubberBand_->m_endPos_.x() + MARQUEE_WIDTH){
            if(m_rubberBand_->m_startPos_.y() + MARQUEE_WIDTH < current_pos.y() && current_pos.y() < m_rubberBand_->m_endPos_.y() - MARQUEE_WIDTH){
                QApplication::setOverrideCursor(Qt::SizeHorCursor);
                m_marqueeEdge = RightEdge;
                return true;
            }
        //上辺と重なった場合
        }else if(m_rubberBand_->m_startPos_.y() - MARQUEE_WIDTH <= current_pos.y() && current_pos.y() <= m_rubberBand_->m_startPos_.y() + MARQUEE_WIDTH){
            if(m_rubberBand_->m_startPos_.x() + MARQUEE_WIDTH < current_pos.x() && current_pos.x() < m_rubberBand_->m_endPos_.x() - MARQUEE_WIDTH){
                QApplication::setOverrideCursor(Qt::SizeVerCursor);
                m_marqueeEdge = TopEdge;
                return true;
            }
        //下辺と重なった場合
        }else if(m_rubberBand_->m_endPos_.y() - MARQUEE_WIDTH <= current_pos.y() && current_pos.y() <= m_rubberBand_->m_endPos_.y() + MARQUEE_WIDTH){
            if(m_rubberBand_->m_startPos_.x() + MARQUEE_WIDTH < current_pos.x() && current_pos.x() < m_rubberBand_->m_endPos_.x() - MARQUEE_WIDTH){
                QApplication::setOverrideCursor(Qt::SizeVerCursor);
                m_marqueeEdge = DownEdge;
                return true;
            }
        }
        //左上の点と重なった場合
        if((m_rubberBand_->m_startPos_.x() - MARQUEE_WIDTH <= current_pos.x() && current_pos.x() <= m_rubberBand_->m_startPos_.x() + MARQUEE_WIDTH)
                && (m_rubberBand_->m_startPos_.y() - MARQUEE_WIDTH <= current_pos.y() && current_pos.y() <= m_rubberBand_->m_startPos_.y() + MARQUEE_WIDTH)){
            QApplication::setOverrideCursor(Qt::SizeFDiagCursor);
            m_marqueeEdge = TopLeftPos;
            return true;
        //右上の点と重なった場合
        }else if((m_rubberBand_->m_endPos_.x() - MARQUEE_WIDTH <= current_pos.x() && current_pos.x() <= m_rubberBand_->m_endPos_.x() + MARQUEE_WIDTH)
                 && (m_rubberBand_->m_startPos_.y() - MARQUEE_WIDTH <= current_pos.y() && current_pos.y() <= m_rubberBand_->m_startPos_.y() + MARQUEE_WIDTH)){
            QApplication::setOverrideCursor(Qt::SizeBDiagCursor);
            m_marqueeEdge = TopRightPos;
            return true;
        //左下の点と重なった場合
        }else if((m_rubberBand_->m_startPos_.x() - MARQUEE_WIDTH <= current_pos.x() && current_pos.x() <= m_rubberBand_->m_startPos_.x() + MARQUEE_WIDTH)
                 && (m_rubberBand_->m_endPos_.y() - MARQUEE_WIDTH <= current_pos.y() && current_pos.y() <= m_rubberBand_->m_endPos_.y() + MARQUEE_WIDTH)){
            QApplication::setOverrideCursor(Qt::SizeBDiagCursor);
            m_marqueeEdge = DownLeftPos;
            return true;
        //右下の点と重なった場合
        }else if((m_rubberBand_->m_endPos_.x() - MARQUEE_WIDTH <= current_pos.x() && current_pos.x() <= m_rubberBand_->m_endPos_.x() + MARQUEE_WIDTH)
                 && (m_rubberBand_->m_endPos_.y() - MARQUEE_WIDTH <= current_pos.y() && current_pos.y() <= m_rubberBand_->m_endPos_.y() + MARQUEE_WIDTH)){
            QApplication::setOverrideCursor(Qt::SizeFDiagCursor);
            m_marqueeEdge = DownRightPos;
            return true;
        }
    }

    return false;
}

void DrawingArea::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)

    if(!m_MarqueeEnebled){
        return;
    }

    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

//マーキーサイズ表示ラベル関連処理
//マーキーサイズ表示ラベルをメンバ変数にセット
void DrawingArea::SetMarqueeSizeLabel(QLabel *width_label, QLabel *height_label)
{
    m_Width_label = width_label;
    m_Height_label = height_label;
}

#include "configuration.h"
#include "translationstring.h"
#include "common_utility.h"

//マーキーサイズ表示ラベルの表示を更新
void DrawingArea::UpdateMarqueeSizeLabel(int width_size, int height_size)
{
    Configuration& config = Configuration::GetConfiguration();
    Units units = config.config_para.units;

    QString units_str;
    if(units == unit_mm){
        units_str = TranslationString::GetString().translation_str.CUSTOMSIZE_008;
        QString size = QString::number(common_utility.PixelToMm(this->MagnificationConversion(width_size, true), config.config_para.preview_immediately_resolution)) + " " + units_str;
        m_Width_label->setText(size);
        size = QString::number(common_utility.PixelToMm(this->MagnificationConversion(height_size, true), config.config_para.preview_immediately_resolution)) + " " + units_str;
        m_Height_label->setText(size);
    }else if(units == unit_inchi){
        //誤差が出るため、一度mmに変換した後にinchiに変換する
        units_str = TranslationString::GetString().translation_str.CUSTOMSIZE_009;
        double temp_size = common_utility.PixelToMm(this->MagnificationConversion(width_size, true), config.config_para.preview_immediately_resolution);
        temp_size = common_utility.MmToInchi(temp_size, config.config_para.preview_immediately_resolution);
        QString size = QString::number(temp_size / 100) + " " + units_str;
        m_Width_label->setText(size);

        temp_size = common_utility.PixelToMm(this->MagnificationConversion(height_size, true), config.config_para.preview_immediately_resolution);
        temp_size = common_utility.MmToInchi(temp_size, config.config_para.preview_immediately_resolution);
        size = QString::number(temp_size / 100) + " " + units_str;
        m_Height_label->setText(size);
    }

    if(m_rubberBand_){
        if (m_mqrqueeCreatedCallBack)
        {
            m_mqrqueeCreatedCallBack(m_rubberBand_->m_existMarquee, QRect(this->MagnificationConversion(m_rubberBand_->geometry().x(), true),
                                                                          this->MagnificationConversion(m_rubberBand_->geometry().y(), true),
                                                                          this->MagnificationConversion(m_rubberBand_->geometry().width(), true),
                                                                          this->MagnificationConversion(m_rubberBand_->geometry().height(), true)));
        }
        if (m_mqrqueeCreatedForDelButtonCallBack)
        {
            m_mqrqueeCreatedForDelButtonCallBack(m_rubberBand_->m_existMarquee);
        }
    }
}

//プレビュー画像表示用に倍率を変更していた画像サイズの倍率を変更する
double DrawingArea::MagnificationConversion(double input_num, bool toOriginal)
{
    int temp;
    if(toOriginal){
        input_num = input_num / m_adjustment_value;
    }else {
        input_num = input_num * m_adjustment_value;
    }
    temp = input_num * 10 + 5;
    return temp / 10;
}

QRect DrawingArea::GetMarqueeSize()
{
    if(m_rubberBand_ && m_rubberBand_->m_existMarquee){
        return m_rubberBand_->geometry();
    }else {
        return QRect(0, 0, 0, 0);
    }
}

bool DrawingArea::isExistedMarquee(void)
{
    if(!m_rubberBand_){
        return false;
    }
    return m_rubberBand_->m_existMarquee;
}

void DrawingArea::DeleteMarquee(void)
{
    if(m_rubberBand_){
        delete m_rubberBand_;
        m_rubberBand_ = nullptr;
    }
}

void DrawingArea::setMarqueeExist(bool existed)
{
    m_rubberBand_->m_existMarquee = existed;
}
