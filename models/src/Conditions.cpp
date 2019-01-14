/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#include "models/Conditions.hpp"

#include <QtCore>
#include <QDebug>

//------------------------------------------------------------------------------

ConditionsBroker& ConditionsBroker::getInstance()
{
     static ConditionsBroker inst;
     return inst;
}

void ConditionsBroker::connectionChanged_slot(bool connection_state)
{
    emit(connectionChanged_signal(connection_state));
}

void ConditionsBroker::connectionRequest_slot(bool connection_req, QString srv, int port)
{
    emit(connectionRequest_signal(connection_req, srv, port));

    if (true == connection_req)
    {
        qDebug() << __func__ << ": WE NEED CONNECTION ";
    }
    else
    {
        qDebug() << __func__ << ": WE NEED DISCONNECTION ";
    }
}
