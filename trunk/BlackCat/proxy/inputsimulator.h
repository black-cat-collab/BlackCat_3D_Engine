#ifndef INPUTSIMULATOR_H
#define INPUTSIMULATOR_H

#include <QObject>
#include <QMap>
#include <QPoint>
#include "BlackCat/glwidget.h"
#include "BlackCat/corecommon.h"

class InputSimulator : public QObject
{
    Q_OBJECT
public:
    explicit InputSimulator(QObject *parent = nullptr);

    void setInputWidget(GLWidget *pWidget=NULL){m_pWidget=pWidget;};

private:

    QMap<quint16,quint16> m_keysMap;

    QPoint       m_screenPosition;

    GLWidget*    m_pWidget;
    QPoint       m_currPosition;

signals:

public slots:
    void simulateKeyboard(quint16 keyCode, bool state);
    void simulateMouseKeys(quint16 keyCode, bool state);
    void simulateMouseMove(quint16 posX, quint16 posY);
    void simulateWheelEvent(bool deltaPos);
    void setMouseDelta(qint16 deltaX, qint16 deltaY);
    void setScreenPosition(const QPoint &pos){m_screenPosition = pos;}

private slots:
    void createKeysMap();
};

#endif // INPUTSIMULATOR_H
