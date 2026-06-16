#ifndef BASICDEFINE_H
#define BASICDEFINE_H

#include <QWidget>
#include <QListView>
#include <QComboBox>
#include <QString>
#include <QThread>
#include "blackcat_global.h"
#include "corecommon.h"

namespace bc
{
    // 单例
    template<typename T>
    class BLACKCATSHARED_EXPORT CommonSingleton
    {
    public:
        static T *GetInstance()
        {
            if(m_pInstance == NULL)
            {
                m_pInstance = new T;
            }

            return m_pInstance;
        }

        static void Destroy()
        {
            delete m_pInstance;
        }

    protected:
        CommonSingleton() { }
    private:
        static T *m_pInstance;
    };


    enum MainStackIndex
    {
        EmptyStackIndex = 0,
        SceneStackIndex
    };
}



#endif // BASICDEFINE_H
