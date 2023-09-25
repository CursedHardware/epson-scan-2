#include "imageeditview.h"
#include "imageeditdialog.h"

#include <QImageReader>
#include <QImageIOPlugin>
#include <QTimer>
#include <string>
#include <cstdio>
#include <cstring>

using namespace std;

ImageEditView::ImageEditView(QWidget *parent) :
    QGraphicsView(parent)
{
    setMouseTracking(true);
}

void ImageEditView::Initialize(ImageEditDialog* main)
{
    m_MainWindow  = main;
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

//背景設定 sceneをimageEditViewに設定
void ImageEditView::setBackGround()
{
    m_viewWidth = this->frameSize().width();
    m_viewHeight = this->frameSize().height();
    m_initialViewHeight = m_viewHeight;

    QPixmap pixmap;
    setScene(&m_scene);
    m_scene.setSceneRect(0,0,m_viewWidth,m_viewHeight);

    m_scene.clear();
    m_scene.addPixmap(pixmap);
}

//移動先をしめす垂直の棒を設定
void ImageEditView::makeVerticalBar(QPoint mouse)
{
    QPolygonF upperTriangle, lowerTriangle;
    QRectF rectangle;
    int thicknessOfBar = 2;

    for(int i=0; i < (int)m_range.size(); i++){
        if(i == 0 && mouse.x() > m_range[i][0] && mouse.x() < m_range[i][1]
                && mouse.y() > m_range[i][2] && mouse.y() < m_range[i][3])
        {
            rectangle = QRectF(0, m_range[i][2], thicknessOfBar, m_scaleValue);
            upperTriangle << QPointF(0 + thicknessOfBar / 2, m_range[i][2] + thicknessOfBar)
                          << QPointF(0 - thicknessOfBar, m_range[i][2])
                          << QPointF(0 + thicknessOfBar * 2, m_range[i][2]);
            lowerTriangle << QPointF(0 + thicknessOfBar / 2,m_range[i][3] - thicknessOfBar)
                          << QPointF(0 - thicknessOfBar, m_range[i][3])
                          << QPointF(0 + thicknessOfBar * 2, m_range[i][3]);
        }

        else if(m_range[i].size() > 4 && i % m_NumberOfRow == 0 && mouse.x() > m_range[i][4] && mouse.x() < m_range[i][5]
                                       && mouse.y() > m_range[i][6] && mouse.y() < m_range[i][7])
        {
            rectangle = QRectF(0, m_range[i][6], thicknessOfBar, m_scaleValue);
            upperTriangle << QPointF(0 + thicknessOfBar / 2, m_range[i][6] + thicknessOfBar)
                          << QPointF(0 - thicknessOfBar, m_range[i][6])
                          << QPointF(0 + thicknessOfBar * 2, m_range[i][6]);
            lowerTriangle << QPointF(0 + thicknessOfBar / 2, m_range[i][7] - thicknessOfBar)
                          << QPointF(0 - thicknessOfBar, m_range[i][7])
                          << QPointF(0 + thicknessOfBar * 2, m_range[i][7]);
        }

        else if(mouse.x()>m_range[i][0] && mouse.x()<m_range[i][1]
                && mouse.y()>m_range[i][2] && mouse.y()<m_range[i][3])
        {
            rectangle = QRectF((m_range[i][0]+m_range[i][1])/2,m_range[i][2],thicknessOfBar,m_scaleValue);
            upperTriangle << QPointF((m_range[i][0]+m_range[i][1])/2+thicknessOfBar/2,m_range[i][2]+thicknessOfBar)
                          << QPointF((m_range[i][0]+m_range[i][1])/2-thicknessOfBar,m_range[i][2])
                          << QPointF((m_range[i][0]+m_range[i][1])/2+thicknessOfBar*2,m_range[i][2]);
            lowerTriangle << QPointF((m_range[i][0]+m_range[i][1])/2+thicknessOfBar/2,m_range[i][3]-thicknessOfBar)
                          << QPointF((m_range[i][0]+m_range[i][1])/2-thicknessOfBar,m_range[i][3])
                          << QPointF((m_range[i][0]+m_range[i][1])/2+thicknessOfBar*2,m_range[i][3]);
        }
    }
    m_rectangle = m_scene.addRect(rectangle, QPen(Qt::white), QBrush(Qt::white));
    m_upperTriangle = m_scene.addPolygon(upperTriangle, QPen(Qt::white), QBrush(Qt::white));
    m_lowerTriangle = m_scene.addPolygon(lowerTriangle, QPen(Qt::white), QBrush(Qt::white));
}

//棒を移動させる関数
void ImageEditView::moveVerticalBar(QPoint mouse)
{
    int thicknessOfBar = 2;
    QPolygonF upperTriangle, lowerTriangle;
    QRectF rectangle;


    for(int i=0; i< (int)m_range.size(); i++){

            if(i==0 && mouse.x()>m_range[i][0] && mouse.x()<m_range[i][1]
                    && mouse.y()>m_range[i][2] && mouse.y()<m_range[i][3])
            {
                rectangle = QRectF(0,m_range[i][2],thicknessOfBar,m_scaleValue);
                 upperTriangle << QPointF(0+thicknessOfBar/2,m_range[i][2]+thicknessOfBar)
                               << QPointF(0-thicknessOfBar,m_range[i][2])
                               << QPointF(0+thicknessOfBar*2,m_range[i][2]);
                 lowerTriangle << QPointF(0+thicknessOfBar/2,m_range[i][3]-thicknessOfBar)
                               << QPointF(0-thicknessOfBar,m_range[i][3])
                               << QPointF(0+thicknessOfBar*2,m_range[i][3]);
            }

            else if(m_range[i].size() > 4 && i % m_NumberOfRow == 0 && mouse.x()>m_range[i][4] && mouse.x()<m_range[i][5]
                                           && mouse.y()>m_range[i][6] && mouse.y()<m_range[i][7])
            {
                rectangle = QRectF(0,m_range[i][6],thicknessOfBar,m_scaleValue);
                upperTriangle << QPointF(0+thicknessOfBar/2,m_range[i][6]+thicknessOfBar)
                              << QPointF(0-thicknessOfBar,m_range[i][6])
                              << QPointF(0+thicknessOfBar*2,m_range[i][6]);
                lowerTriangle << QPointF(0+thicknessOfBar/2,m_range[i][7]-thicknessOfBar)
                              << QPointF(0-thicknessOfBar,m_range[i][7])
                              << QPointF(0+thicknessOfBar*2,m_range[i][7]);
            }

            else if(mouse.x()>m_range[i][0] && mouse.x()<m_range[i][1]
                    && mouse.y()>m_range[i][2] && mouse.y()<m_range[i][3])
            {
                rectangle = QRectF((m_range[i][0]+m_range[i][1])/2,m_range[i][2],thicknessOfBar,m_scaleValue);
                upperTriangle << QPointF((m_range[i][0]+m_range[i][1])/2+thicknessOfBar/2,m_range[i][2]+thicknessOfBar)
                              << QPointF((m_range[i][0]+m_range[i][1])/2-thicknessOfBar,m_range[i][2])
                              << QPointF((m_range[i][0]+m_range[i][1])/2+thicknessOfBar*2,m_range[i][2]);
                lowerTriangle << QPointF((m_range[i][0]+m_range[i][1])/2+thicknessOfBar/2,m_range[i][3]-thicknessOfBar)
                              << QPointF((m_range[i][0]+m_range[i][1])/2-thicknessOfBar,m_range[i][3])
                              << QPointF((m_range[i][0]+m_range[i][1])/2+thicknessOfBar*2,m_range[i][3]);
            }

    }
    //qDebug() << "mousepos:" << mouse;
    //qDebug() << "this->width()" << this->width();
    //qDebug() << "this->height()" << this->height();
    if(mouse.x() >= 0 && mouse.y() >= 0 && mapFromScene(mouse).x() < this->width() && mapFromScene(mouse).y() < this->height()){
        m_rectangle->setRect(rectangle);
        m_rectangle->setSelected(false);
        m_upperTriangle->setPolygon(upperTriangle);
        m_lowerTriangle->setPolygon(lowerTriangle);
        m_rectangle->show();
        m_upperTriangle->show();
        m_lowerTriangle->show();
    }
    else {
        m_rectangle->hide();
        m_upperTriangle->hide();
        m_lowerTriangle->hide();
    }
}

//使い終わった棒を削除
void ImageEditView::removeVerticalBar()
{
    if(m_rectangle->scene() != NULL){
        m_scene.removeItem(m_rectangle);
    }
    if(m_upperTriangle->scene() != NULL){
        m_scene.removeItem(m_upperTriangle);
    }
    if(m_lowerTriangle->scene() != NULL){
        m_scene.removeItem(m_lowerTriangle);
    }
}

//選択した画像を青く囲う四角形の表示。
void ImageEditView::makeSelectedItemRectangle()
{
    if(m_rectangleIsExist == false && m_scene.selectedItems().isEmpty() == false)
    {
        QRectF rectangle;
        rectangle = QRectF(0, 0, m_scaleValue, m_scaleValue);

        QColor brushColor;
        brushColor.setRgb(0,0,255,90);

        QColor penColor;
        penColor.setRgb(255,255,255);
        rectangleItem = m_scene.addRect(rectangle,penColor,brushColor);
        rectangleItem->setPos(((QGraphicsPixmapItem*)m_scene.selectedItems()[0])->offset().x() - (m_scaleValue - ((QGraphicsPixmapItem*)m_scene.selectedItems()[0])->pixmap().width())/2,
                ((QGraphicsPixmapItem*)m_scene.selectedItems()[0])->offset().y() - (m_scaleValue - ((QGraphicsPixmapItem*)m_scene.selectedItems()[0])->pixmap().height())/2
                );
        rectangleItem->setSelected(false);
        m_rectangleIsExist = true;
    }
}

void ImageEditView::moveSelectedItemRectangle()
{
    rectangleItem->setPos(setImagePosition(m_selectedItemNumber));
}

void ImageEditView::removeSelectedItemRectangle()
{
    if(m_rectangleIsExist == true)
    {
        m_scene.removeItem(rectangleItem);
        m_rectangleIsExist = false;
        //m_itemCount++;
    }
}

/*
//loadボタンが押されたとき、この関数から呼び出しを行う
void imageEditView::integrateLoadFunction()
{
    loadImage();
    if(m_loadSuccess == true)
    {
        imageScaled();
        setImageScene();
        makeAreas();
        makeFrontAndBack();
        numberingImage();
    }
}
*/

//loadボタンが押されたとき、この関数から呼び出しを行う
void ImageEditView::integrateLoadFunction(std::list<imageEditInfo> list, std::list<imageEditInfo> *returnInfoList)
{
    /*
    QList<QString> stringList;
    for(int i = 0; i < (int)list.size(); i++)
    {
        stringList.append(QString::fromStdString(string((char*)*next(list.begin(), i)) ));
    }
    */
    auto save_path = list.begin();

    for(int i = 0; i < (int)list.size(); i++)
    {
        imageEditInfo temp_edit_info = (imageEditInfo)*save_path;
        std::string temp_str = temp_edit_info.file_path;
        loadImage(QString::fromStdString(string(temp_str)));
        if(m_loadSuccess == true)
        {
            imageScaled();
            setImageScene();
            makeAreas();
            makeFrontAndBack();
            numberingImage();
            rotateImage(temp_edit_info.angle);
            //qDebug() << "loadSuccess:" << QString::fromStdString(string(*next(list.begin(), i)));
        }
        ++save_path;
    }
    m_returnInfoList = returnInfoList;
}


/*
//画像のロード
void ImageEditView::loadImage()
{
    //qDebug() << "supportedImageFormats:" << QImageReader::supportedImageFormats();
    try
    {
        QFileDialog fileDialog;
        m_fileName = fileDialog.getOpenFileName();
        qDebug() << "imageLoad:" << m_image.load(m_fileName);
        if(m_image.isNull() == true){
            m_loadSuccess = false;
            throw 1;
        }
        m_originalImage = m_image;
        storeOriginalImages();
        m_loadSuccess = true;
    } catch (int i)
    {
        if(i == 1){
            //showErrorMessage();
            m_loadSuccess = false;
        }
    }
}
*/

//画像のロード
void ImageEditView::loadImage(QString filepath)
{
    //qDebug() << "supportedImageFormats:" << QImageReader::supportedImageFormats();
    try
    {
        //QFileDialog fileDialog;
        //m_fileName = fileDialog.getOpenFileName();
        m_fileName = filepath;
        m_image.load(m_fileName);
        if(m_image.isNull() == true){
            m_loadSuccess = false;
            throw 1;
        }
        m_originalImage = m_image;
        //storeOriginalImages();
        storeOriginalImages(filepath);
        m_loadSuccess = true;
    } catch (int i)
    {
        if(i == 1){
            //showErrorMessage();
            m_loadSuccess = false;
        }
    }
}

//解像度が変更されていないオリジナル画像の格納用関数
void ImageEditView::storeOriginalImages()
{
    m_originalImages.append(m_originalImage);
    //imageInfo info;
    //info.name = *next(list.begin(), m_originalImages.size());//list.front(m_originalImages.size());
    //info.angle = 0;
}

void ImageEditView::storeOriginalImages(QString filepath)
{
    m_originalImages.append(m_originalImage);

    imageEditInfo info;
    info.file_path = filepath.toStdString();
    info.angle = 0;
    m_imageInfo.append(info);
    //qDebug("string:%s", (m_imageInfo.last().file_path).c_str());
}

void ImageEditView::imageScaled()
{
    m_image = m_image.scaledToWidth(m_scaleValue);
    if(m_image.height() > 200)
    {
        m_image = m_image.scaledToHeight(m_scaleValue);
    }
}

//画像をm_sceneに登録. m_itemCountに読み込まれた画像の枚数が入る(３枚表示されていればm_itemCount == 3)
void ImageEditView::setImageScene()
{
    //int i=0;
    m_pixmap=QPixmap::fromImage(m_image);
    try {
        m_item = m_scene.addPixmap(m_pixmap);
        m_item->setOffset(setImagePosition(getItemCount() - 1));
        m_item->setFlags(QGraphicsItem::ItemIsSelectable);

        /*
        i = (getItemCount() - 1) / m_NumberOfRow;
        if(i >= 2) //デフォルトの行数が２なので、２より大きくなったら行を追加する
        {
            m_scene.setSceneRect(0,0,m_viewWidth,m_viewHeight + ((i-1) * 200));
        }
        */
        resizeScene();
        if(m_item == nullptr){
            throw 1;
        }
    } catch (int i) {
        //qDebug() << "m_item is empty.";
    }
}

/*
void ImageEditView::resizeScene()
{
    qDebug() << "resizeScene() getItemCount:" << getItemCount();
    int i=0;
    i = (getItemCount() - 1) / m_NumberOfRow;
    qDebug() << "i:" << i;
    qDebug() << "m_NumberOfRow:" << m_NumberOfRow;
    if(i >= 1) //デフォルトの行数が２なので、２より大きくなったら行を追加する
    {
        m_viewHeight = this->frameSize().height() + ((i-1) * 200);
        m_scene.setSceneRect(0,0,m_viewWidth, m_viewHeight);
    }
    qDebug() << "m_scene.height():" << m_scene.height();
}
*/

void ImageEditView::resizeScene()
{
    //qDebug() << "resizeScene() getItemCount:" << getItemCount();
    int numberOfLines = ((getItemCount() - 1) / m_NumberOfRow) + 1;

    //qDebug() << "i:" << numberOfLines;
    //qDebug() << "m_NumberOfRow:" << m_NumberOfRow;
    //qDebug() << "m_viewHeight:" << m_viewHeight;
    if(numberOfLines <= 2) //デフォルトの行数が２なので、２より大きくなったら行を追加する
    {
        m_viewHeight = m_initialViewHeight;
    }else {
        m_viewHeight = m_initialViewHeight + ((numberOfLines-2) * (m_scaleValue + m_margin)) - m_margin;
    }
    m_scene.setSceneRect(0,0,m_viewWidth, m_viewHeight);
    //qDebug() << "m_scene.height():" << m_scene.height();
}


//画像のオフセット位置を決める　画像を配置するときはsceneの座標を基準に置かれるので、scene基準の座標を取得する.
QPoint ImageEditView::setImagePosition(int imgCount)
{
    int i = 0;
    while (1)
    {
        //m_numberOfRowは画面の横に何枚画像がおけるかの計算をしている
        if(((m_scaleValue + m_margin) * i) + (m_scaleValue - m_item->pixmap().width()) / 2 + m_scaleValue > m_viewWidth)
        {
            m_NumberOfRow = i;
            break;
        }
        i++;
    }

    QPoint ImagePos;
    if(m_NumberOfRow != 0)
    {
        ImagePos.setX(((m_scaleValue+m_margin)*(imgCount % m_NumberOfRow))+(m_scaleValue - m_item->pixmap().width())/2);
        ImagePos.setY(((m_scaleValue+m_margin)*(imgCount / m_NumberOfRow))+(m_scaleValue - m_item->pixmap().height())/2);
    }
    return ImagePos;
}

/*
//画像のオフセット位置を決める　画像を配置するときはsceneの座標を基準に置かれるので、scene基準の座標を取得する.
QPoint imageEditView::setImagePosition(int imgCount)
{
    int i = 0;
    while (1)
    {
        //m_numberOfRowは画面の横に何枚画像がおけるかの計算をしている
        if(((m_scaleValue + m_margin) * i) + (m_scaleValue - m_item->pixmap().width()) / 2 + m_scaleValue + m_margin * 2 > m_viewWidth)
        {
            m_NumberOfRow = i;
            break;
        }
        i++;
    }
    QPoint ImagePos;
    ImagePos.setX(((m_scaleValue+m_margin)*(imgCount % m_NumberOfRow))+(m_scaleValue - m_item->pixmap().width())/2+m_margin);
    ImagePos.setY(((m_scaleValue+m_margin)*(imgCount / m_NumberOfRow))+(m_scaleValue - m_item->pixmap().height())/2+m_margin);

    return ImagePos;
}
*/

//マウスから画像の位置を取得するときは、view上の位置が必要なので、こっちを使う. mapFromSceneでsceneの座標をviewに変換できる.
QPoint ImageEditView::getImagePosition(int imgCount)
{
    QPoint ImagePos;
    ImagePos = setImagePosition(imgCount);
    ImagePos.setX(mapFromScene(ImagePos).x());
    ImagePos.setY(mapFromScene(ImagePos).y());

    return ImagePos;
}

//ここでエリアの分割範囲を決めている
void ImageEditView::makeAreas()
{
    m_range.resize(getItemCount() + 1); // +1
    for(int i =0; i < (getItemCount() + 1); i++) // +1
    {
        m_range[i].resize(4);
        if(i == 0)
        {
            //x座標が0 ~ m_scaleValue / 2の範囲、 y座標が0 ~ m_scaleValueの範囲.
            m_range[i][0] = 0;
            m_range[i][1] = m_scaleValue / 2;
            m_range[i][2] = 0;
            m_range[i][3] = m_scaleValue;
        }

        //m_NumberOfRowは一行に何個の画像が置けるか示している. 画像の右端と次の段の左端画像の範囲を取得する.
        if(i % m_NumberOfRow == 0 && i / m_NumberOfRow > 0)
        {
            m_range[i].resize(8);

            m_range[i][0] = m_range[(i-1) % (m_NumberOfRow + 1)][1];
            m_range[i][1] = m_range[i % (m_NumberOfRow + 1)][0] + m_scaleValue + m_margin;
            m_range[i][2] = (m_scaleValue + m_margin)* ((i) / (m_NumberOfRow + 1));
            m_range[i][3] = m_range[i][2] + m_scaleValue;
            m_range[i][4] = 0;
            m_range[i][5] = m_scaleValue / 2;
            m_range[i][6] = (m_scaleValue + m_margin)* ((i) / (m_NumberOfRow));
            m_range[i][7] = m_range[i][6] + m_scaleValue;
        }

        //画像が左から２番めのとき、上記のようにm_range[i][4] ~ m_range[i][7]まで取得
        if(i % m_NumberOfRow == 1 && i / m_NumberOfRow > 0)
        {
            m_range[i][0] = m_range[i-1][5];
            m_range[i][1] = m_range[i][0] + m_scaleValue + m_margin;
            m_range[i][2] = m_range[i-1][6];
            m_range[i][3] = m_range[i][2] + m_scaleValue;
        }

        //その他の場合.
        else if(i > 0)
        {
            m_range[i][0] = m_range[i-1][1];
            m_range[i][1] = m_range[i][0] + m_scaleValue + m_margin;
            m_range[i][2] = m_range[i-1][2];
            m_range[i][3] = m_range[i][2] + m_scaleValue;
        }
    }
/*
    for(int i =0; i < (getItemCount() + 1); i++) // +1
    {
        if(i % m_NumberOfRow == 0 && i / m_NumberOfRow > 0)
        {
            for(int j=0; j<8; j++)
            {
                qDebug() << "m_range[" << i << "]"  << m_range[i][j];
            }
        }
        else{
            for(int j=0; j<4; j++)
            {
                qDebug() << "m_range[" << i << "]"  << m_range[i][j];
            }
        }
    }
    */
}

//二次元のvector配列 上で取得したrangeとは別に、画像の前と後ろで並び替える場所が変わる.
void ImageEditView::makeFrontAndBack()
{
    m_array.resize((getItemCount() + 1) * 2);
    for( int i = 0; i < (getItemCount() + 1) * 2; i++)
    {
        m_array[i].resize(2);
    }
    for (int i = 0; i < (getItemCount() + 1) * 2; i++)
    {
        if(i == 0)
        {
            m_array[i][0] = 0;
            m_array[i][1] = m_scaleValue / 2;
        }
        else if(i > 0)
        {
            m_array[i][0] = m_array[i-1][1];
            m_array[i][1] = m_array[i][0] + (m_scaleValue + m_margin) / 2;
        }
    }
}

bool ImageEditView::selectedFrontAndBack(QPoint mousePos)
{
    bool PressPointIsFront = false; //大丈夫か不明
    try {
        if(m_scene.selectedItems().isEmpty() == true)
        {
            throw 1;
        }
        for(int i=0;i < getItemCount() * 2;i++)
        {
            /*
            qDebug() << "i:" << i;
            qDebug() << "getItemCount():" << getItemCount();

            qDebug() << "mousePos:" << mousePos;
            qDebug() << "m_array[i][0],[1]:" << m_array[i][0] << "," << m_array[i][1];
            */
            if(mousePos.x() > m_array[i][0] && mousePos.x() < m_array[i][1] && i % 2 == 0)
            {
                PressPointIsFront = true;
            }
            else if(mousePos.x() < m_array[i][1] && mousePos.x() > m_array[i+1][0] && i % 2 == 1)
            {
                PressPointIsFront = false;
            }
        }
    } catch (int i) {
        //showErrorMessage();
    }
    return PressPointIsFront;
}

void ImageEditView::setSmallImage(QPoint mousePos)
{
    imageTransparent(80); //画像の縮小・透過を行う. m_smallImageに格納される.
    m_smallItem = m_scene.addPixmap(m_smallImage);
    m_smallItem->setOffset(mousePos);
}

void ImageEditView::setBigImage(QPoint mousePos)
{
    imageTransparent(150); //画像の縮小・透過を行う. m_smallImageに格納される.
    m_smallItem = m_scene.addPixmap(m_smallImage);
    m_smallItem->setOffset(mousePos);
}

//選択したrangeを返す
int ImageEditView::selectedArea(QPoint mouse){
    int SelectedRange = -1;
    for(int i =0; i< (int)m_range.size(); i++){
        if(mouse.x() > m_range[i][0] && mouse.x() <= m_range[i][1]
                && mouse.y() > m_range[i][2] && mouse.y() <= m_range[i][3])
        {
            SelectedRange = i;
            //break;
        }
        if(i % m_NumberOfRow == 0 && i / m_NumberOfRow > 0
            && mouse.x() > m_range[i][4] && mouse.x() <= m_range[i][5]
            && mouse.y() > m_range[i][6] && mouse.y() <= m_range[i][7])
        {
            SelectedRange = i;
        }
    }
    return SelectedRange;
}



//マウス位置から画像の選択(点線で囲まれる)をしている
void ImageEditView::detectImage(QPoint mousePos)
{
    //qDebug() << "detectImage(QPoint mousePos):" << mousePos;


    for(int i=0;i<getItemCount();i++)
    {
        m_scene.items()[i]->setSelected(false);
    }
    if(this->items(mapFromScene(mousePos).x(), mapFromScene(mousePos).y(),1,1).count() > 0)
    {
        ((QGraphicsPixmapItem*)this->items(mapFromScene(mousePos).x(), mapFromScene(mousePos).y(),1,1)[0])->setSelected(true);
        m_item = ((QGraphicsPixmapItem*)this->items(mapFromScene(mousePos).x(), mapFromScene(mousePos).y(),1,1)[0]);
        for(int i=0;i<getItemCount();i++){
            //qDebug() << "getImagePosition(" << i << "):" << getImagePosition(i);

            if((int)(m_item->offset().x()) ==
                    ((int)((QGraphicsPixmapItem*)this->items(getImagePosition(i).x()+100,getImagePosition(i).y()+100,1,1)[0])->offset().x()) &&
                    (int)(m_item->offset().y()) ==
                    ((int)((QGraphicsPixmapItem*)this->items(getImagePosition(i).x()+100,getImagePosition(i).y()+100,1,1)[0])->offset().y())
                    )
            {
                m_selectedItemNumber = i;
            }
        }
    }

    //qDebug() << "m_itemCount:" << m_itemCount;
    //qDebug() << "detectImageSelectedNumber:" << m_selectedItemNumber;
}

//viewに表示された画像をm_orderItemに格納していく。一番左の画像が０番目になる。
void ImageEditView::numberingImage()
{
    m_orderItem.clear();
    for(int i=0;i<getItemCount();i++)
    {
        if(this->items(getImagePosition(i).x()+(m_scaleValue/2),getImagePosition(i).y()+(m_scaleValue/2),1,1).isEmpty() == false)
        {
            m_orderItem.append(this->items(getImagePosition(i).x()+(m_scaleValue/2),getImagePosition(i).y()+(m_scaleValue/2),1,1)[0]);
        }
        //qDebug() << "i:" << i;
    }
}

//画像の順番を変更する. 画像を実際に入れ替えるのはsortImage()で行う. ここでは画像の番号の入れ替えを行うのみ.
//NumberOfItems:画像の枚数
//pushedRange:マウスを押したところの範囲
//releaseRange:マウスを離したところの範囲
//selectedItemNumber:
void ImageEditView::sortImageOrder(int NumberOfItems, int pushedRange, int releaseRange, int selectedItemNumber, int numberOfRow)
{
    bool once = true;
    //qDebug() <<"NumberOfItems:" << NumberOfItems;
    //qDebug() <<"pushedRange:" << pushedRange;
    //qDebug() <<"releaseRange:" << releaseRange;
    //qDebug() <<"selectedItemNumber:" << selectedItemNumber;
    //qDebug() <<"numberOfRow:" << numberOfRow;

    int sortOrder[NumberOfItems];
    for(int i=0; i< NumberOfItems; i++)
    {
        sortOrder[i] = i;
    }

    for(int i=0; i< NumberOfItems; i++)
    {
        if(releaseRange > pushedRange)
        {
            for(i=0;i<selectedItemNumber;i++)
            {
                sortOrder[i] = i;
            }
            for(i=selectedItemNumber;i<releaseRange-1;i++)
            {
                sortOrder[i] = i+1;
            }
            if(i==releaseRange-1)
            {
                sortOrder[i] = selectedItemNumber;
            }
            for(i=releaseRange;i<NumberOfItems;i++)
            {
                sortOrder[i] = i;
            }
        }
        else if(releaseRange < pushedRange)
        {
            for(i=0;i<releaseRange;i++)
            {
                sortOrder[i] = i;
            }
            if(i==releaseRange)
            {
                sortOrder[i] = selectedItemNumber;
            }
            for(i=releaseRange+1;i<selectedItemNumber+1;i++)
            {
                sortOrder[i] = i-1;
            }
            for(i=selectedItemNumber+1;i<NumberOfItems;i++)
            {
                sortOrder[i] = i;
            }
        }
    }

    m_sortOrder.clear();
    m_sortOrder.resize(NumberOfItems);
    for(int i=0; i<NumberOfItems; i++)
    {
        m_sortOrder[i] = sortOrder[i];
        //qDebug() << "sortOrder[" << i <<"]" << sortOrder[i];
        if(m_selectedItemNumber == sortOrder[i] && once == true)
        {
            m_selectedItemNumber = i;
            //qDebug() << "m_selectedItemNumber = i:" << m_selectedItemNumber;
            once = false;
        }
    }
    sortOriginalImageOrder();
    sortImage();
}

void ImageEditView::sortOriginalImageOrder()
{
    QList<QImage> copy_originalImages;
    copy_originalImages = m_originalImages;

    //qDebug() << "m_originalImage.size():" << m_originalImages.size();
    for(int i=0; i<m_originalImages.size();i++)
    {
        m_originalImages[i] = copy_originalImages[m_sortOrder[i]];
    }

    QList<imageEditInfo> copy_imageInfo;
    copy_imageInfo = m_imageInfo;

    for(int i=0; i<m_imageInfo.size();i++)
    {
        m_imageInfo[i] = copy_imageInfo[m_sortOrder[i]];
    }
}

void ImageEditView::sortImage()
{
    numberingImage();
    for(int i = 0; i < getItemCount(); i++)
    {
        m_item = ((QGraphicsPixmapItem*) m_orderItem[m_sortOrder[i]]);
        ((QGraphicsPixmapItem*) m_orderItem[m_sortOrder[i]])->setOffset(setImagePosition(i));
        ((QGraphicsPixmapItem*) m_orderItem[m_sortOrder[i]])->setTransformOriginPoint(setImagePosition(i).x()
                                         + ((QGraphicsPixmapItem*)m_orderItem[m_sortOrder[i]])->pixmap().width()/2
                ,setImagePosition(i).y() + ((QGraphicsPixmapItem*)m_orderItem[m_sortOrder[i]])->pixmap().height()/2);
    }
}

void ImageEditView::removeItem()
{
    int sortOrder[getItemCount()];
    removeSelectedItemRectangle();
    numberingImage();

    try
    {
        if(m_scene.selectedItems().isEmpty() == true)
        {
            throw 1;
        }

        m_scene.removeItem(m_orderItem[m_selectedItemNumber]);
        m_scene.clearSelection();

        for(int i = 0; i < getItemCount() +1; i++)
        {
            if(i < m_selectedItemNumber)
            {
                sortOrder[i] = i;
            }
            else if(i >= m_selectedItemNumber)
            {
                m_item = (QGraphicsPixmapItem*)m_orderItem[i];
                m_item->setOffset(setImagePosition(i - 1));
                m_item->setTransformOriginPoint(m_item->offset().x() + m_item->pixmap().width() / 2,
                                                m_item->offset().y() + m_item->pixmap().height() / 2);
            }
            //qDebug() << "m_sortOrder[" << i << "]" << m_sortOrder[i];
        }

        m_sortOrder.clear();
        m_sortOrder.resize(getItemCount());
        for(int i = 0; i < getItemCount(); i++)
        {
            m_sortOrder[i] = sortOrder[i];
            //qDebug() << "sortOrder[" << i <<"]" << sortOrder[i];
        }

        m_range.resize(m_range.size() - 1);
        m_originalImages.removeAt(m_selectedItemNumber);
        m_imageInfo.removeAt(m_selectedItemNumber);


        resizeScene();
    } catch (int i) {
        //qDebug("Couldn't remove a image.");
    }
}

//画像の透過と縮小
QPixmap ImageEditView::imageTransparent(int imageSize)
{
    QGraphicsPixmapItem* originalPixmapItem = (QGraphicsPixmapItem*)m_scene.selectedItems()[0];
    QPixmap originalPixmap = originalPixmapItem->pixmap();
    QImage originalImage = originalPixmap.toImage();

    int viewWidth = originalImage.width();
    int viewHeight = originalImage.height();
    //QImageクラスで画像を生成。フォーマットを32ビット画像にする。
    QImage image(viewWidth, viewHeight,QImage::Format_ARGB32);


    //ix, iyの2重ルーブで、全ての画素の色を設定する。
    for(int ix=0;ix<viewWidth;ix++)
    {
        for(int iy=0; iy<viewHeight; iy++)
        {
            //int red = originalImage.pixelColor(ix,iy).red();
            int red = qRed(originalImage.pixel(ix,iy));
            //int green = originalImage.pixelColor(ix,iy).green();
            int green = qGreen(originalImage.pixel(ix,iy));
            //int blue = originalImage.pixelColor(ix,iy).blue();
            int blue = qBlue(originalImage.pixel(ix,iy));
            //透明度の設定
            int alpha = 40;

            //Qtの色のクラスに色を設定する。
            QColor color;
            color.setRgb(red,green,blue,alpha);

            //座標(ix, iy)の画素の色を設定する。
            //image.setPixelColor(ix,iy,color);
            image.setPixel(ix, iy, qRgba(red, green, blue, alpha));
            image.pixel(ix,iy);

        }
    }
    m_transparentedPixmap=QPixmap::fromImage(image);
    m_smallImage = m_transparentedPixmap.scaledToHeight(imageSize);
    return m_smallImage;
}

/*
void imageEditView::testTimer()
{
    myTimer* mytimer;
    mytimer->startTimer(500);
}
*/

void ImageEditView::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << "Press_event->button():" << event->button();
    if(event->button() == Qt::LeftButton)
    {
        removeSelectedItemRectangle();
    }

    if(event->button() == Qt::LeftButton)
    {
        m_mouseScenePos = mapToScene(event->pos()).toPoint();
        //qDebug() << "Press_mousePos:" << event->pos();
        try {
            m_pushedPosition = m_mouseScenePos;
            m_pushSelectedRange = selectedArea(m_mouseScenePos);
            //qDebug() << "m_pushSelectedRange:" << m_pushSelectedRange;
            //selectedFrontAndBack(m_mouseScenePos);
            //qDebug() << "itemcount" << getItemCount();
            detectImage(m_mouseScenePos);
            if(m_scene.selectedItems().isEmpty() == true || getItemCount() == 0)
            {
                bool i = m_scene.selectedItems().isEmpty();
                //qDebug() << "selecteditems is empty." << i;
                throw 1;
            }
            setSmallImage(m_mouseScenePos);
            makeVerticalBar(m_pushedPosition);
        } catch (int i) {
            //qDebug() << "Error";
        }
    }
    makeSelectedItemRectangle();
}

void ImageEditView::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::NoButton){
    }

    if(event->buttons() == Qt::LeftButton){
        m_mouseScenePos = mapToScene(event->pos()).toPoint();
        m_presentSelectedRange = selectedArea(m_mouseScenePos);
        try {
            if(m_scene.selectedItems().isEmpty() == true || getItemCount() == 0)
            {
                throw 1;
            }
            m_smallItem->setOffset(m_mouseScenePos.x(), m_mouseScenePos.y());
            moveVerticalBar(m_mouseScenePos);
        } catch (int i) {
            //qDebug("Couldn't load the image.");
        }
    }
    //qDebug() << "eventPos:" << m_mouseScenePos;
    //qDebug() << "m_scene.width:" << m_scene.width();
    //qDebug() << "m_scene.height:" << m_scene.height();
    //qDebug() << "this.width:" << this->width();
    //qDebug() << "this.height:" << this->height();

    /*
    if(0 < event->pos().y() && event->pos().y() <30){
        //scroll(0,1);
        m_scene.setSceneRect(0,1,m_viewWidth,m_viewHeight);
        qDebug("event->pos().y() < 30");
    }
    if(event->pos().y() > this->frameSize().height() - 30){
        //scroll(0,-1);
        m_scene.setSceneRect(0,-1,m_viewWidth,m_viewHeight);
        qDebug("event->pos().y() > this->frameSize().height() - 30");
    }
    */
}

void ImageEditView::mouseReleaseEvent(QMouseEvent *event)
{
    m_mouseScenePos = mapToScene(event->pos()).toPoint();
    removeSelectedItemRectangle();

    try {
        if(m_scene.selectedItems().isEmpty() == true || getItemCount() == 0 ||
                mapFromScene(m_mouseScenePos).x() > this->width() || mapFromScene(m_mouseScenePos).y() > this->height() ||
                mapFromScene(m_mouseScenePos).x() < 0 || mapFromScene(m_mouseScenePos).y() < 0)
        {
            //if(m_mouseScenePos.x() > this->width() || m_mouseScenePos.y() > this->height()){
            if(m_mouseScenePos.x() > this->width() || m_mouseScenePos.y() > this->height() ||
                    m_mouseScenePos.x() < 0 || m_mouseScenePos.y() < 0){
                m_scene.removeItem(m_smallItem);
                removeVerticalBar();
            }
            //qDebug() << "throw";
            throw 1;
        }
        m_releasedPosition = m_mouseScenePos;
        if(m_smallItem->scene() != NULL){
            m_scene.removeItem(m_smallItem);
        }
        removeVerticalBar();
        m_releaseSelectedRange = selectedArea(m_releasedPosition);

        if(m_releaseSelectedRange == -1){
            m_releaseSelectedRange = m_pushSelectedRange;
        }

        //qDebug() << "m_pushSelectedRange:" << m_pushSelectedRange;
        //qDebug() << "m_releaseSelectedRange:" << m_releaseSelectedRange;
        //qDebug() << "m_selectedItemNumber:" << m_selectedItemNumber;
        //qDebug() << "m_NumberOfRow:" << m_NumberOfRow;
        //qDebug() << "getItemCount:" << getItemCount();
        //qDebug() << "m_itemCount:" << m_itemCount;
        sortImageOrder(getItemCount(), m_pushSelectedRange, m_releaseSelectedRange, m_selectedItemNumber, m_NumberOfRow);

        //qDebug() << "m_orderItem.count():" << m_orderItem.count();
    } catch (int i) {
        //showErrorMessage();
    }

    makeSelectedItemRectangle();
    /*
    if(m_scene.selectedItems().isEmpty() == false)
    {

        int PositionNumberOfReleasePoint =0;

        if(selectedFrontAndBack(event->pos()) == true){
            PositionNumberOfReleasePoint = m_releaseSelectedRange;
        }
        else if(selectedFrontAndBack(event->pos()) == false){
            PositionNumberOfReleasePoint = m_releaseSelectedRange - 1;
        }

        makeSelectedItemRectangle();
    }
    */

    //回転・削除ボタンのグレーアウト機能
    if(m_scene.selectedItems().isEmpty() == false){
        m_MainWindow->grayOut(true);
    }
    else{
        m_MainWindow->grayOut(false);
    }

    //セーブボタンのグレーアウト機能
    if(getItemCount() == 0){
        m_MainWindow->saveButtonGrayOut(false);
    }
    else {
        m_MainWindow->saveButtonGrayOut(true);
    }
}


void ImageEditView::mouseDoubleClickEvent(QMouseEvent *event)
{
    //qDebug() << "double click:" << event->pos();
}



bool ImageEditView::imageIsSelected()
{
    //qDebug() << "m_scene.selectedItems().isEmpty():" << m_scene.selectedItems().isEmpty(); //m_item->isSelected();
    //qDebug() << "m_orderItem.count():" << m_orderItem.count();
    if(m_scene.selectedItems().isEmpty() == true)
    {
        //qDebug() << "false";
        return false;
    }
    else
    {
        //qDebug() << "true";
        return true;
    }
}

void ImageEditView::rotateImage(int angle)
{
    if(angle != 0){
        m_myQGraphicsItem = this->items()[0];

        m_myQGraphicsItem->setTransformOriginPoint(((QGraphicsPixmapItem*)m_myQGraphicsItem)->offset().x() +
                ((QGraphicsPixmapItem*)m_myQGraphicsItem)->pixmap().width()/2
                , ((QGraphicsPixmapItem*)m_myQGraphicsItem)->offset().y() +
                ((QGraphicsPixmapItem*)m_myQGraphicsItem)->pixmap().height()/2);
        m_myQGraphicsItem->setRotation(angle);
    }
}

void ImageEditView::rotateLeft()
{
    if(m_scene.selectedItems().isEmpty() == false){
        m_myQGraphicsItem = m_scene.selectedItems()[0];

        m_myQGraphicsItem->setTransformOriginPoint(((QGraphicsPixmapItem*)m_scene.selectedItems()[0])->offset().x() +
                ((QGraphicsPixmapItem*)m_scene.selectedItems()[0])->pixmap().width()/2
                , ((QGraphicsPixmapItem*)m_scene.selectedItems()[0])->offset().y() +
                ((QGraphicsPixmapItem*)m_scene.selectedItems()[0])->pixmap().height()/2);
        m_myQGraphicsItem->setRotation(m_myQGraphicsItem->rotation() - 90);
    }
}

void ImageEditView::rotateRight()
{
    if(m_scene.selectedItems().isEmpty() == false){
        m_myQGraphicsItem = m_scene.selectedItems()[0];
        m_myQGraphicsItem->setTransformOriginPoint(((QGraphicsPixmapItem*)m_scene.selectedItems()[0])->offset().x() +
                ((QGraphicsPixmapItem*)m_scene.selectedItems()[0])->pixmap().width()/2
                , ((QGraphicsPixmapItem*)m_scene.selectedItems()[0])->offset().y() +
                ((QGraphicsPixmapItem*)m_scene.selectedItems()[0])->pixmap().height()/2);
        m_myQGraphicsItem->setRotation(m_myQGraphicsItem->rotation() + 90);
    }
}

void ImageEditView::showErrorMessage(){
    QMessageBox msgBox(this);
    msgBox.setText(tr("画像ファイルを読み込めませんでした"));
    msgBox.setWindowTitle(tr("Couldn't open the image"));
    msgBox.exec();
}

/*
int ImageEditView::showRemoveMessage(){
    QMessageBox msgBox(this);
    msgBox.setText(tr("選択画像を削除しますか？"));
    msgBox.setWindowTitle(tr("画像削除確認"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int res = msgBox.exec();
    return res;
}
*/

int ImageEditView::getItemCount()
{
    int itemCount = 0;
    if(m_rectangleIsExist == true)
    {
        itemCount = m_scene.items().count() - 2;
    }
    else {
        itemCount = m_scene.items().count() - 1;
    }
    return itemCount;
}

/*
void imageEditView::setValue()
{
    m_itemIsEmpty = m_scene.selectedItems().isEmpty();
}
*/

void ImageEditView::getImageInfo()
{
    m_returnInfoList->clear();
    for (int i=0;i<getItemCount();i++)
    {
        m_returnInfoList->push_back(m_imageInfo.at(i));
        //qDebug() << "m_returnInfoList name:" << (m_returnInfoList->back().name).c_str();
        //qDebug() << "m_returnInfoList angle:" << m_returnInfoList->back().angle;
    }
}

/*
void imageEditView::sample_getlist(){
    imageInfo info;
    info.name = (unsigned char*)"/home/shokaku/workspace/format_test/jpeg_color";
    info.angle = 90;
    m_returnInfoList->push_back(info);
    qDebug("m_returnInfoList name:%s", m_returnInfoList->front().name);
    qDebug("m_returnInfoList angle:%d", m_returnInfoList->front().angle);
}
*/

int ImageEditView::getImageRotation(int i){
    //qDebug() << "itemAt" << "[" << i << "]:" << (int)(((QGraphicsPixmapItem*)this->items(getImagePosition(i).x()+100,getImagePosition(i).y()+100,1,1)[0])->rotation());
    //qDebug() << "itemPos" << "[" << i << "]:" << getImagePosition(i).x()+100 << "," << getImagePosition(i).y()+100;
    //qDebug() << "itemAt" << "[" << i << "]:" << (((QGraphicsPixmapItem*)this->items(getImagePosition(i).x()+100,getImagePosition(i).y()+100,1,1)[0]));

    if((int)itemAt(getImagePosition(i).x() +100,getImagePosition(i).y()+100)->flags() != QGraphicsItem::ItemIsSelectable){
        //qDebug() << "itemIsUnselectable";
        return items( getImagePosition(i).x() +100, getImagePosition(i).y()+100)[1]->rotation();
    } else {
        //qDebug() << "itemIsselectable" << itemAt( getImagePosition(i).x() +100,getImagePosition(i).y()+100)->ItemIsSelectable;
        return items( getImagePosition(i).x() +100, getImagePosition(i).y()+100)[0]->rotation();
    }
    /*
    if((int)itemAt(getImagePosition(i).x() +100,getImagePosition(i).y()+100)->flags() != QGraphicsItem::ItemIsSelectable){
        qDebug() << "itemUnselectable";
    }
    //qDebug("string:%d", itemAt(getImagePosition(i).x() +100,getImagePosition(i).y()+100)->flags());
    //qDebug() << "itemIsselectable" << QGraphicsItem::ItemIsSelectable;
    qDebug() << "itemIsselectable" << itemAt( getImagePosition(i).x() +100,getImagePosition(i).y()+100)->flags();
    return items( getImagePosition(i).x() +100, getImagePosition(i).y()+100)[0]->rotation();
    */
}

void ImageEditView::pdfDialog()
{
    /*
    QPrinter printer(QPrinter::ScreenResolution);
    QPrintDialog printDialog(&printer, this);
    printer.setOutputFormat(QPrinter::PdfFormat);
    if(printDialog.exec() == QDialog::Accepted)
    {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QImage rotateImages[getItemCount()];
        for(int i=0; i < getItemCount(); i++)
        {
            rotateImages[i] = m_originalImages[i].transformed(QMatrix().rotate(((QGraphicsPixmapItem*)m_orderItem[i])->rotation()));
            QSize size = rotateImages[i].size();
            size.scale(rect.size(), Qt::KeepAspectRatio);
            painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
            painter.setWindow(rotateImages[i].rect());
            painter.drawImage(0, 0, rotateImages[i]);
            if(i != getItemCount()-1)
            {
                printer.newPage();
            }
        }
    }
    */

    QList<imageEditInfo> copy_imageInfo;
    copy_imageInfo = m_imageInfo;
    m_imageInfo.clear();

    //qDebug() << "getItemCount():" << getItemCount();
    //std::list<imageInfo> ex_InfoList;
    for(int i=0; i < getItemCount(); i++)
    {
        imageEditInfo info;
        info.file_path = copy_imageInfo.at(i).file_path;
        info.angle = getImageRotation(i);
        //info.angle = ((QGraphicsPixmapItem*)m_orderItem[i])->rotation();
        m_imageInfo.append(info);
        //qDebug() << "m_imageInfo string:" << (m_imageInfo.last().file_path).c_str();
        //qDebug() << "m_imageInfo string:" << m_imageInfo.last().angle;
    }

    getImageInfo();
    for (int i=0;i<getItemCount();i++)
    {
        //qDebug("getImageInfo() string:%s", ((next(m_returnInfoList->begin(), i))->file_path).c_str());
        //qDebug("getImageInfo() string:%d", ((next(m_returnInfoList->begin(), i))->angle));
    }
}
