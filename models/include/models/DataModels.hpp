#ifndef DATAMODELS_H
#define DATAMODELS_H

#include <QObject>
#include <QAbstractListModel>

#include "dataclient/DataClientInterface.hpp"

//------------------------------------------------------------------------------------

class ConnectionModels : public QObject
{
    Q_OBJECT

    public:
        enum ConnState
        {
           not_connected,
           connected
        };

        ConnectionModels(QObject *parent = 0);

        void setConnectionState(ConnState new_state);

    signals:
        void connectionChanged(bool connection_state);

    public slots:
        void dataServerEvent(bool connection_state);

    private:
        ConnState m_conn_state;
};

//------------------------------------------------------------------------------------

class ConnectionButton : public QObject
{
    Q_OBJECT

    public:

        ConnectionButton(QObject *parent = 0);

    signals:
        void connectionRequest(bool connection_request, QString srv, int port);
        void connectionChanged(bool connection_state);

public slots:
        void setConnectionRequest(bool connection_request, QString srv, int port);
        void dataServerEvent(bool connection_state);

    private:
        bool m_conn_state;
};

//------------------------------------------------------------------------------------

// Create an Item so we have something to put in the model:
struct DiscretItem
{
    QString name;
    QString status;
    QString value;
};

class CDiscretPointsModel : public QAbstractListModel
{
    Q_OBJECT

    public slots:
        void dataChanged(int strt_point, int amouint_point);
        void dataServerEvent(bool connection_state);

    public:
        // enum DataRoles for QAbstractListModel
        enum DataRoles {
            NameRole = Qt::UserRole + 1,
            StatusRole,
            ValueRole
        };

        CDiscretPointsModel(const CDataClientInterface& srv, QObject *parent = 0);
        ~CDiscretPointsModel(){}

        void addData(const DiscretItem& entry);
        int rowCount(const QModelIndex & parent = QModelIndex())const;
        QVariant data(const QModelIndex & index, int role)const;
        QHash<int,QByteArray> roleNames()const;

    private:
        // Below are the model items:
        QList<DiscretItem> m_model;
        CDataClientInterface& m_srv;
        bool m_data_connection_state;
};

//------------------------------------------------------------------------------------

// Create an Item so we have something to put in the model:
struct ItemAnalog
{
    QString name;
    QString status;
    QString value;
};

class CAnalogPointsModel : public QAbstractListModel
{
    Q_OBJECT

    public slots:
        void dataChanged(int strt_point, int amouint_point);
        void dataServerEvent(bool connection_state);

    public:
        // enum DataRoles for QAbstractListModel
        enum DataRoles {
            NameRole = Qt::UserRole + 1,
            StatusRole,
            ValueRole
        };

        CAnalogPointsModel(const CDataClientInterface& srv, QObject *parent = 0);
        ~CAnalogPointsModel(){}

        void addData(const ItemAnalog& entry);
        int rowCount(const QModelIndex & parent = QModelIndex())const;
        QVariant data(const QModelIndex & index, int role)const;
        QHash<int,QByteArray> roleNames()const;

    private:
        // Below are the model items
        QList<ItemAnalog> m_model;
        CDataClientInterface& m_srv;
        bool m_data_connection_state;
};

//------------------------------------------------------------------------------------

class CPointsUpdateModel : public QObject
{
    Q_OBJECT

    signals:
        void updateDigitalPoint_signal(QString number, QString value);
        void updateAnalogPoint_signal(QString number, QString value);

    public slots:
        void dataServerEvent(bool connection_state);
        void updateDigitalPoint_slot(QString number, QString value);
        void updateAnalogPoint_slot(QString number, QString value);
    public:
        CPointsUpdateModel(const CDataClientInterface& srv, QObject *parent = 0);
        ~CPointsUpdateModel(){}

    private:
        // Below are the model items:
        CDataClientInterface& m_srv;
        bool m_data_connection_state;
};

#endif // DATAMODELS_H
