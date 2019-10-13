#include "source/rvc.h"
#include <QFile>
#include <QDirIterator>
OKVC::OKVC()
{

}

void OKVC::InsertString(QString s, int pos)
{
    for (int i=0;i<s.length();i++)
        state.m_pram->set(pos++,s[i].toLatin1());
    state.m_pram->set(pos,0);
}

void OKVC::ResetFileList()
{
    m_listFiles.clear();
    m_currentFile = 0;
    InsertString("               ",p_fileLocation);
    QDirIterator it(m_currentDir, QStringList() << "*.prg", QDir::Files);
    while (it.hasNext()) {
        QString s = it.next();
        m_listFiles.append(s.split("/").last());
//        qDebug() << it.next();
    }
    InsertString(m_listFiles[m_currentFile],p_fileLocation);
}

void OKVC::ReadNextFile()
{
    m_currentFile++;
    if (m_currentFile<m_listFiles.count())
        InsertString(m_listFiles[m_currentFile],p_fileLocation);
    else InsertString("",p_fileLocation);
}

void OKVC::LoadFile()
{

}

void OKVC::LoadRom(QString file, int pos)
{
    QFile f(file);
    f.open(QFile::ReadOnly);
    QByteArray blob = f.readAll();
    f.close();
    for (int i=0;i<blob.size();i++) {
        int p = i+pos;
        if (p<0x10000)
            state.m_pram->set(p,blob[i]);
        else
            state.m_vram->set(p-0x10000,blob[i]);
    }


}

void OKVC::Init(OKMemory* pram, OKMemory* vram)
{
    state.m_pram = pram;
    state.m_vram = vram;
    m_img = QImage(256,256,QImage::Format_RGB32);
    m_backbuffer = QImage(256,256,QImage::Format_RGB32);
    m_screen = QImage(256,256,QImage::Format_RGB32);

    state.m_palette.resize(256);
    for (int color=0;color<256;color++) {
        int k = (color&0b00011000)>>3;
        int dd=0;
//        if ((k&1)==1) dd=15;
        state.m_palette[color] = QColor((color&0b00000111)*32,((color&0b00011000))*8+dd,(color&0b11100000));
    }
//    qDebug() << QString::number(state.m_pram->get(p_fontBank));
    LoadRom(":resources/rom/font.bin",0xF0000);

    state.m_pram->set(p_fontBank,0x0F);
    state.m_pram->set(p_fontSizeX,0x08);
    state.m_pram->set(p_fontSizeY,0x08);
    state.m_pram->set(p_borderColor,0x0);
    state.m_pram->set(p_borderWidth,0x10);
    state.m_pram->set(p_borderHeight,0x10);

}

void OKVC::PutPixel(int x, int y, uchar color)
{
//    m_img.setPixelColor(x,y,QColor((color&0b00000111)*32,(color&0b00011000>>3)*64,(color&0b1110000)));
    m_img.setPixelColor(x,y,QColor(color,0,0));
}

void OKVC::DrawCircle(int x, int y, int radius, uchar color, bool fill)
{
    if (radius<1)
        return;
    QPainter painter;
    painter.begin(&m_img);
    painter.setPen(QPen(QColor(color,0,0), 1, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
    painter.setBrush(QColor(color,0,0));
    painter.drawEllipse(x,y,radius,radius);
    painter.end();
}


void OKVC::DrawLine(int x1, int y1, int x2, int y2, uchar color)
{
    QPainter painter;
    painter.begin(&m_img);
    painter.setPen(QPen(QColor(color,0,0), 1, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
    painter.drawLine(x1,y1,x2,y2);
    painter.end();
}

void OKVC::Blit(int x1, int y1, int x2, int y2, int w, int h)
{
    int px1 = x1;
    int py1 = y1*16;
    int px2 = x2;
    int py2 = y2;
//#pragma omp parallel for
    for (int yy=0;yy<h;yy++)
        for (int xx=0;xx<w;xx++) {
            int sx = xx+px1;
            int sy = yy+py1;
            int tx = xx+px2;
            int ty = yy+py2;
            uchar c = 0;

            c = state.m_vram->get(sx+sy*256);
//            if (sy<256) // in Screen ram
            if (sx>=0 && sx<256 && sy>=0 && sy<256)
                c = m_img.pixelColor(sx,sy).red();

            state.m_vram->set(tx+ty*256,c);

//            if (ty<256) // in Screen ram
            if (tx>=0 && tx<256 && ty>=0 && ty<256)
                m_img.setPixelColor(tx,ty,QColor(c,0,0));
        }
}

void OKVC::Rect(int x1, int y1, int w, int h,uchar c)
{
    int px1 = x1;
    int py1 = y1;
    for (int yy=0;yy<h;yy++)
        for (int xx=0;xx<w;xx++) {
            int sx = xx+px1;
            int sy = yy+py1;

//            state.m_vram->set(tx+ty*256,c);

//            if (ty<256) // in Screen ram
  //          if (tx>=0 && tx<256 && ty>=0 && ty<256)
                m_img.setPixelColor(sx,sy,QColor(c,0,0));
        }
}

void OKVC::BlitFont(int fontsizeX, int fontsizeY, int chr, int col, int px, int py)
{
    int base = (state.m_pram->get(p_fontBank)-1)*0x10000;
    int width = 256/fontsizeX;
    int x = chr % (width);
    int y = (int)(chr/width);
    int px1 = x*fontsizeX;
    int py1 = y*fontsizeY;
    int px2 = px;
    int py2 = py;
//    qDebug() << "Base: "<<QString::number(base,16);
//#pragma omp parallel for
    for (int yy=0;yy<fontsizeY;yy++)
        for (int xx=0;xx<fontsizeX;xx++) {
            int sx = xx+px1;
            int sy = yy+py1;

            int tx = xx+px2;
            int ty = yy+py2;
            uchar c = 0;

            c = state.m_vram->get(base + sx+sy*256);


            if (c!=0) {
                state.m_vram->set(tx+ty*256,col);

//            if (ty<256) // in Screen ram
                if (tx>=0 && tx<256 && ty>=0 && ty<256)
                    m_img.setPixelColor(tx,ty,QColor(col,0,0));
            }
        }

}



void OKVC::Update()
{
    state.m_pram->set(p_time,rand()%255);
    if (get(p_exec)==p_pixel) {
        PutPixel(get(p_p1_x), get(p_p1_y),get(p_p1_c));
    }
    if (get(p_exec)==p_line) {
        DrawLine(get(p_p1_x), get(p_p1_y),get(p_p2_x), get(p_p2_y),    get(p_p1_c));
    }
    if (get(p_exec)==p_clearscreen) {
        m_img.fill(QColor(get(p_p1_c),0,0));
    }
    if (get(p_exec)==p_circlefill) {
        DrawCircle(get(p_p1_x), get(p_p1_y),get(p_p1_c),get(p_p1_3),true);
    }
    if (get(p_exec)==p_palette) {
        state.m_palette[get(p_p1_x)] = QColor(get(p_p1_y), get(p_p1_c), get(p_p1_3));
    }
    if (get(p_exec)==p_blit) {
        Blit(get(p_p1_x), get(p_p1_y), get(p_p1_c), get(p_p1_3),get(p_p2_x),get(p_p2_y));
    }
    if (get(p_exec)==p_blitFont) {
        BlitFont(get(p_fontSizeX), get(p_fontSizeY), get(p_p1_x), get(p_p1_y),get(p_p1_c),get(p_p1_3));
    }
    if (get(p_exec)==p_rect) {
        Rect(get(p_p1_x), get(p_p1_y), get(p_p1_c),get(p_p1_3),get(p_p2_x) );
    }
    if (get(p_exec)==p_resetFileList)
        ResetFileList();

    if (get(p_exec)==p_nextFile)
        ReadNextFile();

    if (get(p_exec)==p_loadFile)
        LoadFile();

    set(p_exec,0);

    state.m_waitForVSYNC = (get(p_vsync)==1);
}

void OKVC::VRAMtoScreen()
{
//    QRgb* s =
    for (int y=0;y<256;y++)
        for (int x=0;x<256;x++)
            m_img.setPixelColor(x,y,QColor(state.m_vram->get(x+y*256),0,0));

}

void OKVC::GenerateOutputSignal()
{
    int bw  = state.m_pram->get(p_borderWidth);
    int bh  = state.m_pram->get(p_borderHeight);
    int bc  = state.m_pram->get(p_borderColor);
    /*   for (int y=0;y<m_screen.height();y++)
//#pragma omp parallel for
        for (int x=0;x<m_screen.width();x++) {
            int col = m_img.pixelColor(x,y).red();
            if (y<bh || y>m_screen.height()-bh)
                col = bc;
            if (x<bw || x>m_screen.width()-bw)
                col = bc;
            m_screen.setPixelColor(x,y,state.m_palette[col]);
        }
        */

    QRgb *screen = (QRgb *) m_screen.bits();
    QRgb *back = (QRgb *) m_img.bits();
    int w = m_screen.width();
    for (int y=0;y<m_screen.height();y++)
//        #pragma omp parallel for
        for (int x=0;x<w;x++) {
            int col = QColor(back[y*w+x]).red();
            if (y<bh || y>m_screen.height()-bh)
                col = bc;
            if (x<bw || x>w-bw)
                col = bc;

            screen[y*w+x] = state.m_palette[col].rgba();
        }
}

uchar OKVC::get(uint i)
{
    return state.m_pram->get(i);;
}

uchar OKVC::set(uint i, uchar j)
{
    state.m_pram->set(i,j);
}
