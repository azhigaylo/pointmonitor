/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#ifndef CONDITIONS_H
#define CONDITIONS_H

#include <QObject>

class ConditionsBroker : public QObject
{
    Q_OBJECT

    public:
        static ConditionsBroker& getInstance();

    signals:
        void connectionChanged_signal(bool connection_state);
        void connectionRequest_signal(bool connection_req, QString srv, int port);

    public slots:
        void connectionChanged_slot(bool connection_state);
        void connectionRequest_slot(bool connection_req, QString srv, int port);

    private:
        ConditionsBroker(QObject *parent = 0){}
        ~ConditionsBroker(){}

        ConditionsBroker(const ConditionsBroker&);                 // Prevent copy-construction
        ConditionsBroker& operator=(const ConditionsBroker&);      // Prevent assignment
};

#endif // CONDITIONS_H
