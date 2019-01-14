/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#include "models/DataModels.hpp"

#include <QtCore>
#include <QtNetwork>
#include <QString>
#include <QDebug>

#include "models/Conditions.hpp"

//-----------------------------------------------------------------------------------------------------------------

ConnectionModels::ConnectionModels(QObject *parent)
   : QObject(parent)
   , m_conn_state(ConnState::not_connected)
{
   ConditionsBroker& conditions = ConditionsBroker::getInstance();
   QObject::connect(&conditions, SIGNAL(connectionChanged_signal(bool)), this, SLOT(dataServerEvent(bool)));
}

void ConnectionModels::setConnectionState(ConnState new_state)
{
   m_conn_state = new_state;

   if (ConnState::not_connected == m_conn_state)
   {
      emit connectionChanged(false);
   }
   else
   {
      emit connectionChanged(true);
   }
}

void ConnectionModels::dataServerEvent(bool connection_state)
{
   if (false == connection_state)
   {
      m_conn_state = ConnState::not_connected;
   }
   else
   {
      m_conn_state = ConnState::connected;
   }

   emit connectionChanged(connection_state);
}

//-----------------------------------------------------------------------------------------------------------------

ConnectionButton::ConnectionButton(QObject *parent)
   : QObject(parent)
   , m_conn_state(false)
{
   ConditionsBroker& conditions = ConditionsBroker::getInstance();
   QObject::connect(&conditions, SIGNAL(connectionChanged_signal(bool)), this, SLOT(dataServerEvent(bool)));
   QObject::connect(this, SIGNAL(connectionRequest(bool, QString, int)), &conditions, SLOT(connectionRequest_slot(bool, QString, int)));
}

void ConnectionButton::setConnectionRequest(bool connection_request, QString srv, int port)
{
   emit connectionRequest(connection_request, srv, port);
}

void ConnectionButton::dataServerEvent(bool connection_state)
{
   if (false == connection_state)
   {
      emit connectionChanged(false);
   }
   else
   {
      emit connectionChanged(true);
   }
}

//-----------------------------------------------------------------------------------------------------------------

void CDiscretPointsModel::dataChanged(int strt_point, int amouint_point)
{
    // Create some items and then add to the model:
    if (true == m_data_connection_state)
    {
        for (int idx= strt_point; idx < strt_point + amouint_point; idx++)
        {
            boost::optional<uint16_t> d_point_val  = m_srv.getDPoint(idx);
            boost::optional<int8_t> d_point_stat = m_srv.getDStatus(idx);
            if(d_point_val && d_point_stat)
            {
                m_model[idx].status = QString::number(d_point_stat.get());
                m_model[idx].value   = QString::number(d_point_val.get());
            }
        }
        emit QAbstractItemModel::dataChanged(index(strt_point), index(strt_point + amouint_point-1));
        qDebug() << __func__ << ": DPOINT updated first=" << strt_point << " / last=" << (strt_point + amouint_point-1);
    }
}

void CDiscretPointsModel::dataServerEvent(bool connection_state)
{
    m_data_connection_state = connection_state;

    if (false == m_data_connection_state)
    {
        qDebug() << __func__ << ": data server disconnected";

        for (int idx= 0; idx < m_srv.getDPointAmount(); idx++)
        {
            m_model[idx].status = "";
            m_model[idx].value  = "";
        }
        emit QAbstractItemModel::dataChanged(index(0), index(m_srv.getDPointAmount()-1));
    }
}

CDiscretPointsModel::CDiscretPointsModel(const CDataClientInterface& srv, QObject *parent)
    : QAbstractListModel(parent)
    , m_srv(const_cast<CDataClientInterface&>(srv))
    , m_data_connection_state(false)
{
    // Create some items and then add to the model:
    int N = srv.getDPointAmount();
    DiscretItem* items = new DiscretItem[N];
    for (int i = 0; i < N; i++)
    {
        items[i].name =  QString::number(i);
        m_model<<items[i];
    }

   QObject::connect(&srv, SIGNAL(dataConnection(bool)), this, SLOT(dataServerEvent(bool)));
   QObject::connect(&srv, SIGNAL(dpoint_updated(int, int)), this, SLOT(dataChanged(int, int)));
}

// addData() method for QAbstractListModel
void CDiscretPointsModel::addData(const DiscretItem& entry)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_model << entry;
    endInsertRows();
}

// rowCount() method for QAbstractListModel
int CDiscretPointsModel::rowCount(const QModelIndex & parent)const
{
    return m_model.count();
}

// data() required for QAbstractListModel
QVariant CDiscretPointsModel::data(const QModelIndex & index, int role)const
{
    if ( !index.isValid() || index.row() < 0 || index.row() >= m_model.count() )
        return QVariant();
    DiscretItem modelEntry = m_model[index.row()];
    if (role == NameRole)   {return modelEntry.name;}
    if (role == StatusRole) {return modelEntry.status;}
    if (role == ValueRole)  {return modelEntry.value;}
    return QVariant();
}

// roleNames() method for QAbstractListModel:
QHash<int,QByteArray> CDiscretPointsModel::roleNames()const
{
    QHash<int, QByteArray> roles;
    roles[NameRole]   = "Name";
    roles[StatusRole] = "Status";
    roles[ValueRole]  = "Value";
    return roles;
}


//-----------------------------------------------------------------------------------------------------------------

void CAnalogPointsModel::dataChanged(int strt_point, int amouint_point)
{
    // Create some items and then add to the model:
    if (true == m_data_connection_state)
    {
        for (int idx= strt_point; idx < strt_point + amouint_point; idx++)
        {
            boost::optional<double> a_point_val  = m_srv.getAPoint(idx);
            boost::optional<int8_t> a_point_stat = m_srv.getAStatus(idx);
            if(a_point_val && a_point_stat)
            {
                m_model[idx].status = QString::number(a_point_stat.get());
                m_model[idx].value   = QString::number(a_point_val.get());
            }
        }
        emit QAbstractItemModel::dataChanged(index(strt_point), index(strt_point + amouint_point-1));
        qDebug() << __func__ << ": APOINT updated first=" << strt_point << " / last=" << (strt_point + amouint_point-1);
    }
}

void CAnalogPointsModel::dataServerEvent(bool connection_state)
{
    m_data_connection_state = connection_state;

    if (false == m_data_connection_state)
    {
        qDebug() << __func__ << ": data server disconnected";

        for (int idx= 0; idx < m_srv.getAPointAmount(); idx++)
        {
            m_model[idx].status = "";
            m_model[idx].value  = "";
        }
        emit QAbstractItemModel::dataChanged(index(0), index(m_srv.getDPointAmount()-1));
    }
}


CAnalogPointsModel::CAnalogPointsModel(const CDataClientInterface& srv, QObject *parent)
    : QAbstractListModel(parent)
    , m_srv(const_cast<CDataClientInterface&>(srv))
    , m_data_connection_state(false)
{
    // Create some items and then add to the model:
    int N = srv.getAPointAmount();
    ItemAnalog* items = new ItemAnalog[N];
    for (int i = 0; i < N; i++)
    {
        items[i].name =  QString::number(i);
        m_model<<items[i];
    }

   QObject::connect(&srv, SIGNAL(dataConnection(bool)), this, SLOT(dataServerEvent(bool)));
   QObject::connect(&srv, SIGNAL(apoint_updated(int, int)), this, SLOT(dataChanged(int, int)));
}


// addData() method for QAbstractListModel
void CAnalogPointsModel::addData(const ItemAnalog& entry)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_model << entry;
    endInsertRows();
}

// rowCount() method for QAbstractListModel
int CAnalogPointsModel::rowCount(const QModelIndex & parent)const
{
    return m_model.count();
}

// data() required for QAbstractListModel
QVariant CAnalogPointsModel::data(const QModelIndex & index, int role)const
{
    if ( !index.isValid() || index.row() < 0 || index.row() >= m_model.count() )
        return QVariant();
    ItemAnalog modelEntry = m_model[index.row()];
    if (role == NameRole)   {return modelEntry.name;}
    if (role == StatusRole) {return modelEntry.status;}
    if (role == ValueRole)  {return modelEntry.value;}
    return QVariant();
}

// roleNames() method for QAbstractListModel
QHash<int,QByteArray> CAnalogPointsModel::roleNames()const
{
    QHash<int, QByteArray> roles;
    roles[NameRole]   = "Name";
    roles[StatusRole] = "Status";
    roles[ValueRole]  = "Value";
    return roles;
}


//-----------------------------------------------------------------------------------------------------------------

CPointsUpdateModel::CPointsUpdateModel(const CDataClientInterface& srv, QObject *parent)
    : m_srv(const_cast<CDataClientInterface&>(srv))
    , m_data_connection_state(false)
{
   QObject::connect(&srv, SIGNAL(dataConnection(bool)), this, SLOT(dataServerEvent(bool)));

   QObject::connect(this, SIGNAL(updateDigitalPoint_signal(QString, QString)), this, SLOT(updateDigitalPoint_slot(QString, QString)));
   QObject::connect(this, SIGNAL(updateAnalogPoint_signal(QString, QString)), this, SLOT(updateAnalogPoint_slot(QString, QString)));
}

void CPointsUpdateModel::dataServerEvent(bool connection_state)
{
    m_data_connection_state = connection_state;
}

void CPointsUpdateModel::updateDigitalPoint_slot(QString number, QString value)
{
    if (true == m_data_connection_state)
    {
        m_srv.setDPoint ( number.toUShort(), value.toUShort());
       qDebug() << __func__ << ": DPOINT " << number << " / new value = " << value;
    }
    else
    {
       qDebug() << __func__ << ": data server is not connected !!!";
    }
}

void CPointsUpdateModel::updateAnalogPoint_slot(QString number, QString value)
{
    if (true == m_data_connection_state)
    {
        m_srv.setAPoint ( number.toUShort(), value.toDouble() );
       qDebug() << __func__ << ": APOINT " << number << " / new value = " << value;
    }
    else
    {
       qDebug() << __func__ << ": data server is not connected !!!";
    }
}

