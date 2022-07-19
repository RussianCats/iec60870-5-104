#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "cs104_slave.h"

#include "hal_thread.h"
#include "hal_time.h"

static bool running = true;

void sigint_handler(int signalId)
{
    running = false;
}

static bool
connectionRequestHandler(void *parameter, const char *ipAddress)
{
    printf("New connection request from %s\n", ipAddress);
    return true;
}

static void
connectionEventHandler(void *parameter, IMasterConnection con, CS104_PeerConnectionEvent event)
{
    if (event == CS104_CON_EVENT_CONNECTION_OPENED)
    {
        printf("Connection opened (%p)\n", con);
    }
    else if (event == CS104_CON_EVENT_CONNECTION_CLOSED)
    {
        printf("Connection closed (%p)\n", con);
    }
    else if (event == CS104_CON_EVENT_ACTIVATED)
    {
        printf("Connection activated (%p)\n", con);
    }
    else if (event == CS104_CON_EVENT_DEACTIVATED)
    {
        printf("Connection deactivated (%p)\n", con);
    }
}

int main(int argc, char **argv)
{
    /* Add Ctrl-C handler */
    signal(SIGINT, sigint_handler);

    /* create a new slave/server instance with default connection parameters and
     * default message queue size */
    CS104_Slave slave = CS104_Slave_create(10, 10);

    CS104_Slave_setLocalAddress(slave, "0.0.0.0");

    /* Set mode to a single redundancy group
     * NOTE: library has to be compiled with CONFIG_CS104_SUPPORT_SERVER_MODE_SINGLE_REDUNDANCY_GROUP enabled (=1)
     */
    CS104_Slave_setServerMode(slave, CS104_MODE_SINGLE_REDUNDANCY_GROUP);

    /* /* получить параметры подключения - они нужны нам для создания правильных ASDUS -
     * вы также можете изменить параметры здесь, когда параметры по умолчанию не должны использоваться */
    CS101_AppLayerParameters alParams = CS104_Slave_getAppLayerParameters(slave);

    /* установить обработчик для обработки запросов на подключение. New connection request from [ip]*/
    CS104_Slave_setConnectionRequestHandler(slave, connectionRequestHandler, NULL);

    /* установить обработчик для отслеживания событий подключения. activated/closed */
    CS104_Slave_setConnectionEventHandler(slave, connectionEventHandler, NULL);

    CS104_Slave_start(slave);

    if (CS104_Slave_isRunning(slave) == false)
    {
        printf("Starting server failed!\n");
        goto exit_program;
    }

    int16_t scaledValue = 0;
    float scaledValue1 = 1000.1;

    while (running)
    {

        Thread_sleep(500);

        CS101_ASDU newAsdu = CS101_ASDU_create(alParams, false, CS101_COT_PERIODIC, 0, 1, false, false);

        InformationObject io = (InformationObject)MeasuredValueScaled_create(NULL, 11, scaledValue, IEC60870_QUALITY_GOOD); // какой-то интовый тип данных

        scaledValue += 2;

        CS101_ASDU_addInformationObject(newAsdu, io);

        InformationObject_destroy(io);

        /* Add ASDU to slave event queue */
        CS104_Slave_enqueueASDU(slave, newAsdu);

        CS101_ASDU_destroy(newAsdu);

        //------------------------------//

        CS101_ASDU newAsdu1 = CS101_ASDU_create(alParams, false, CS101_COT_PERIODIC, 0, 1, false, false);

        io = (InformationObject)ParameterFloatValue_create(NULL, 111, scaledValue1, IEC60870_QUALITY_GOOD); // тип данных float

        scaledValue1 += 2.2;

        CS101_ASDU_addInformationObject(newAsdu1, io);

        InformationObject_destroy(io);

        /* Add ASDU to slave event queue */
        CS104_Slave_enqueueASDU(slave, newAsdu1);

        CS101_ASDU_destroy(newAsdu1);
    }

    CS104_Slave_stop(slave);

exit_program:
    CS104_Slave_destroy(slave);

    Thread_sleep(10);
}
