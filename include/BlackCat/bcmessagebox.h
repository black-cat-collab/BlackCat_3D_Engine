#ifndef BCMessageBox_H
#define BCMessageBox_H

#include <QDialog>
#include <QMouseEvent>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QHBoxLayout>
#include <QCheckBox>
#include "blackcat_global.h"

class BLACKCATSHARED_EXPORT BCMessageBox : public QDialog
{
    Q_OBJECT

public:
    explicit BCMessageBox(QString text, QString title = QString(), QWidget *parent = 0);
    virtual ~BCMessageBox();

    virtual void setText(QString text)
    {
        if(m_pTextLabel)
        {
            m_pTextLabel->setText(text);
        }
    }

    virtual QLabel* titleLabel() const
    {
        return m_pTitleLabel;
    }

    virtual QLabel* textLabel() const
    {
        return m_pTextLabel;
    }

    virtual QPushButton* clickedButton() const
    {
        return m_pClickedButton;
    }

    virtual QPushButton* addButton(QString text = QString());

    virtual void setMoveEnable(bool bEnable)
    {
        m_bMoveEnable = bEnable;
    }
protected:
   void init();
   void mouseMoveEvent(QMouseEvent* event);
   void mousePressEvent(QMouseEvent* event);
   void mouseReleaseEvent(QMouseEvent* event);


protected:
   bool         m_bMoveEnable;
   QPushButton* m_pClickedButton;
   QLabel*      m_pTitleLabel;
   QLabel*      m_pTextLabel;
   QHBoxLayout* m_pButtonLayout;
   QVBoxLayout* m_pWidgetLayout;
   QString      m_strObjectName;
private:
   QPoint       m_LastMousePosition;
   bool         m_bMoving;

private slots:
   void         slotClose();
};

#endif // BCMessageBox_H
