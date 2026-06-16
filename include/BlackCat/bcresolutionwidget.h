#ifndef BCRESOLUTIONWIDGET_H
#define BCRESOLUTIONWIDGET_H

#include <QWidget>
#include <QFontDatabase>
#include <QDir>
#include <QDebug>

#define RESOLUTION_STANDARD_WIDTH   (1920)
#define RESOLUTION_STANDARD_HEIGHT   (1080)

template<typename T>
class BCResolutionWidget : public T
{

public:
    explicit BCResolutionWidget(float fHeight, QWidget *parent = nullptr) : T(parent)
    {
        init(fHeight);
    }

    virtual ~BCResolutionWidget() {}

    virtual int x(bool bNative = false)
    {
        return bNative ? T::x() : T::x() / m_XScale;
    }
    virtual int y(bool bNative = false)
    {
        return bNative ? T::y() : T::y() / m_YScale;
    }
    virtual int width(bool bNative = false)
    {
        return bNative ? T::width() : T::width() / m_XScale;
    }
    virtual int height(bool bNative = false)
    {
        return bNative ? T::height() : T::height() / m_YScale;
    }

    virtual int XScaleInt() { return qRound(m_XScale); }
    virtual int YScaleInt() { return qRound(m_YScale); }

    virtual const float& XScale() { return m_XScale; }
    virtual const float& YScale() { return m_YScale; }

    virtual void resize(QSize size, bool bNative = false)
    {
        if(bNative)
        {
            T::resize(size.width(), size.height());
        }
        else
        {
            T::resize(size.width()*m_XScale, size.height()*m_YScale);
        }

    }

    virtual void resize(int width, int height, bool bNative = false)
    {
        if(bNative)
        {
            T::resize(width, height);
        }
        else
        {
            T::resize(width*m_XScale, height*m_YScale);
        }
    }

    virtual void move(QPoint pos, bool bNative = false)
    {
        if(bNative)
        {
            T::move(pos.x(), pos.y());
        }
        else
        {
            T::move(pos.x()*m_XScale, pos.y()*m_YScale);
        }
    }

    virtual void move(int x, int y, bool bNative = false)
    {
        if(bNative)
        {
            T::move(x, y);
        }
        else
        {
            T::move(x*m_XScale, y*m_YScale);
        }
    }

    virtual void setGeometry(QRect rect, bool bNative = false)
    {
        resize(rect.width(), rect.height(), bNative);
        move(rect.x(), rect.y(), bNative);
    }

    virtual void AdaptChildToResolution()
    {
#ifdef _WIN32
#ifdef BCQT6
        QList<QWidget*> children    = T::template findChildren<QWidget*>();
#else
        QList<QWidget*> children    = T::findChildren<QWidget*>();
#endif
        foreach(QWidget *pWidget, children)
        {
            QFont srcFont = pWidget->font();
            QFont desFont = srcFont;

            QString str = srcFont.family();
            if(!str.contains("Source Han Sans CN") && !m_strFontFamily.isEmpty())
            {
                desFont.setFamily(m_strFontFamily);
            }
            int fontSize = desFont.pointSize();
            desFont.setPointSize(fontSize*m_XScale);
            fontSize = desFont.pointSize();
            pWidget->setFont(desFont);

            pWidget->resize(pWidget->width() * m_XScale, pWidget->height() * m_YScale);
            pWidget->move(pWidget->pos().x() * m_XScale, pWidget->pos().y() * m_YScale);
        }
#endif


    }
protected:
    virtual void init(float fHeight)
    {
        m_YScale    = fHeight / RESOLUTION_STANDARD_HEIGHT;
        m_XScale    = m_YScale;
    }

    /**
     *  获取外部字体
     * @brief loadFontFamilyFromFiles
     * @param fontFile
     * @return
     */
    virtual void loadFontFamilyFromFiles(const QString &fontDir)
    {
        QDir dir(fontDir);
        QStringList nameFilters;
        nameFilters << "*.otf" << "*.ttf";
        QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
        for(int i = 0 ; i < files.size() ; i++)
        {
            QString fileName = files.at(i);
            QFile file(fontDir + "/" + fileName);
            if(!file.open(QIODevice::ReadOnly))
            {
                qDebug()<<"Open font file error";
                continue;
            }
            int loadedFontID = QFontDatabase::addApplicationFontFromData(file.readAll());
            QStringList loadedFontFamilies = QFontDatabase::applicationFontFamilies(loadedFontID);
            if(!loadedFontFamilies.empty())
            {
                QString str = loadedFontFamilies.at(0);
                if(str.contains("Source Han Sans CN Regular"))
                {
                    m_strFontFamily = str;
                }
            }
            file.close();
        }
    }

protected:
    float   m_XScale;
    float   m_YScale;
    QString m_strFontFamily;

};



#endif // BCRESOLUTIONWIDGET_H
