#ifndef HCDOCKTITLEWIDGET_H
#define HCDOCKTITLEWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include "../blackcat_global.h"
#include "../corecommon.h"

class BLACKCATSHARED_EXPORT DockTitleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DockTitleWidget(QString title, QWidget *parent = 0);
    virtual ~DockTitleWidget();

protected:
    void init();

    QFrame *m_pBackgroundFrame;
    QFrame *m_pTitleFrame;
    QLabel *m_pTitleLabel;
    QPushButton *m_pCloseButton;

signals:
    void sigClose();
};

#endif // HCDOCKTITLEWIDGET_H
