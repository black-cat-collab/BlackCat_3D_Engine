/*
@Brief:			Qt multi-threaded network module
@Author:		vilas wang
@Contact:		QQ451930733

The Qt multi-threaded network module is a wrapper of Qt Network module, and combine with thread-pool to realize multi-threaded networking.
- Multi-task concurrent(Each request task is executed in different threads).
- Both single request and batch request mode are supported.
- Large file multi-thread downloading supported. (The thread here refers to the download channel. Download speed is faster.)
- HTTP(S)/FTP protocol supported.
- Multiple request methods supported. (GET/POST/PUT/DELETE/HEAD)
- Asynchronous API.
- Thread-safe.

Note: You must call NetworkManager::initialize() before use, and call NetworkManager::unInitialize() before application quit.
That must be called in the main thread.
*/

#ifndef IHCNETWORK_H
#define IHCNETWORK_H

#include "source/networkdefs.h"
#include "source/networkmanager.h"
#include "source/networkreply.h"
#include "source/networkevent.h"

#endif ///IHCNETWORK_H
