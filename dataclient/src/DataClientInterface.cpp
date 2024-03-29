/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#include "dataclient/DataClientInterface.hpp"

#include <QtCore>
#include <QtNetwork>
#include <QDebug>

#include <iostream>
#include <chrono>
#include <string>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>

void CDataClientInterface::startDataConnection_slot(bool conn_request, QString srv, int port)
{
    if (true == conn_request)
    {
        startDataConnection(srv.toStdString(), port);
    }
    else
    {
        stopDataConnection();
    }
}

CDataClientInterface::CDataClientInterface(QObject *parent)
   : QObject(parent)
   , m_raw_buf(m_read_buffer.prepare(0))
   , m_connection_state(false)
{
    m_io_dummy_work = std::make_shared<boost::asio::io_service::work>(m_io_service);
    m_srv_thread.reset(new boost::scoped_thread<>(boost::bind(&CDataClientInterface::serviceLoop, this)));
    m_boost_socket.reset(new boost::asio::ip::tcp::socket(m_io_service));
}

CDataClientInterface::~CDataClientInterface()
{
    m_io_service.stop();
    m_srv_thread->join();
}

void CDataClientInterface::startDataConnection(std::string host, int server_port)
{
    try
    {
        boost::asio::ip::tcp::endpoint server;
        server.address(boost::asio::ip::address::from_string(host));
        server.port(server_port);

        qDebug() << __func__ << ": try to connect to " << QString(host.c_str()) << ":" << server_port;
        m_boost_socket->async_connect(server, boost::bind(&CDataClientInterface::connectionHandler, this, boost::asio::placeholders::error));
    }
    catch(boost::system::system_error e)
    {
        qDebug() << __func__ << ":" << e.code().message().c_str();
    }
}

void CDataClientInterface::stopDataConnection()
{
    m_boost_socket->cancel();
    m_boost_socket->close();
}

// --------------------------------------------------------

boost::optional<uint16_t> CDataClientInterface::getDPoint( uint16_t number )
{
    std::lock_guard<std::mutex> lock(m_dpoint_mtx);

    return (true == m_boost_socket->is_open()) ? DPOINT[number].value : boost::optional<uint16_t>{};
}

boost::optional<double> CDataClientInterface::getAPoint( uint16_t number )
{
    std::lock_guard<std::mutex> lock(m_apoint_mtx);

    return (true == m_boost_socket->is_open()) ? APOINT[number].value : boost::optional<double>{};
}

boost::optional<int8_t> CDataClientInterface::getDStatus( uint16_t number )
{
    std::lock_guard<std::mutex> lock(m_dpoint_mtx);

    return (true == m_boost_socket->is_open()) ? DPOINT[number].status : boost::optional<int8_t>{};
}

boost::optional<int8_t> CDataClientInterface::getAStatus( uint16_t number )
{
    std::lock_guard<std::mutex> lock(m_apoint_mtx);

    return (true == m_boost_socket->is_open()) ? APOINT[number].status : boost::optional<int8_t>{};
}


bool CDataClientInterface::setDPoint( uint16_t number, uint16_t value )
{
    bool result = false;
    TDPOINT d_point = {number, 16, value};

    if (true == m_boost_socket->is_open())
    {
        if (value != DPOINT[number].value)
        {
           sendSetDpointReq(number, d_point);
        }
        result = true;
    }
    return result;
}

bool CDataClientInterface::setAPoint( uint16_t number, double value )
{
    bool result = false;
    TAPOINT a_point = {number, 16, value};

    if (true == m_boost_socket->is_open())
    {
        if (value != APOINT[number].value)
        {
           sendSetApointReq(number, a_point);
        }
        result = true;
    }
    return result;
}

// --------------------------------------------------------

void CDataClientInterface::connectionHandler(const boost::system::error_code& error)
{
    if (boost::system::errc::success == error)
    {
        updateConnectionState(true);

        qDebug() << __func__ << ": connected...";

        // allocate new subbuffer & wait for response package
        m_raw_buf = m_read_buffer.prepare(sizeof(THeader));

        // wait for read request package
        async_read(*m_boost_socket, m_raw_buf, boost::asio::transfer_all(),
                                               boost::bind(&CDataClientInterface::readHeader,
                                                           this,
                                                           boost::asio::placeholders::error,
                                                           boost::asio::placeholders::bytes_transferred));
        // send start requests
        m_comm_thread.reset(new boost::scoped_thread<>([this](){ sendGetDpointsReq(0, d_point_total);
                                                                 std::this_thread::sleep_for(std::chrono::milliseconds(10));
                                                                 sendGetApointsReq(0, a_point_total);
                                                                 std::this_thread::sleep_for(std::chrono::milliseconds(10));}));
    }
    else
    {
        qDebug() <<  __func__ << ": error(" << error.value() << ") message: " << QString(error.message().c_str());
    }
}

void CDataClientInterface::readHeader(const boost::system::error_code& error, size_t bytes_transferred)
{
    if(boost::system::errc::success == error.value())
    {
        qDebug() << __func__ << ": we got " << bytes_transferred << "bytes";

        m_read_buffer.commit(bytes_transferred);

        processHeader();
    }
    else
    {
        switch (error.value())
        {
            // operation canceled - it's ok for us
            case boost::system::errc::operation_canceled :
            {
                updateConnectionState(false);
                break;
            }
            case boost::system::errc::connection_reset:
            case boost::system::errc::no_such_file_or_directory :
            {
                qDebug() <<  __func__ << ": error(" << error.value() << ") message: " << QString(error.message().c_str());
                updateConnectionState(false);
                break;
            }
            default :
            {
                qDebug() <<  __func__ << ": error(" << error.value() << ") message: " << QString(error.message().c_str());
                break;
            }
        }
    }
}

void CDataClientInterface::readData(const boost::system::error_code& error, size_t bytes_transferred)
{
    if(boost::system::errc::success == error.value())
    {
        qDebug() << __func__ << ": we got " << bytes_transferred << "bytes";

        m_read_buffer.commit(bytes_transferred);

        processIncomingData();
    }
    else
    {
        switch (error.value())
        {
            // operation canceled - it's ok for us
            case boost::system::errc::operation_canceled :
            {
                updateConnectionState(false);
                break;
            }
            case boost::system::errc::connection_reset:
            case boost::system::errc::no_such_file_or_directory :
            {
                qDebug() <<  __func__ << ": error(" << error.value() << ") message: " << QString(error.message().c_str());
                updateConnectionState(false);
                break;
            }
            default :
            {
                qDebug() <<  __func__ << ": error(" << error.value() << ") message: " << QString(error.message().c_str());
                break;
            }
        }
    }
}

void CDataClientInterface::processHeader()
{
    std::istream is_data(&m_read_buffer);

    // check magic header 'magicHeader'
    if (magicHeader == is_data.peek())
    {
         // read header
         is_data.read(reinterpret_cast<char*>(&m_package.header), sizeof(m_package.header));
         if (is_data.gcount() >= sizeof(m_package.header))
         {
             qDebug() << __func__ << ": we got a header: head_strt=" << m_package.header.head_strt << ", data_size=" << m_package.header.data_size
                      << ", cmd=" << m_package.header.cmd << ", start_point=" << m_package.header.start_point << ", number_point=" << m_package.header.number_point;

             // allocate new subbuffer & wait for response package
             m_raw_buf = m_read_buffer.prepare(m_package.header.data_size - sizeof(m_package.header));

             // wait for read request package
             async_read(*m_boost_socket, m_raw_buf, boost::asio::transfer_all(),
                                                    boost::bind(&CDataClientInterface::readData,
                                                                this,
                                                                boost::asio::placeholders::error,
                                                                boost::asio::placeholders::bytes_transferred));
         }
         else
         {
            // wait for new header
            // allocate new subbuffer & wait for response package
            m_raw_buf = m_read_buffer.prepare(sizeof(THeader));

            // wait for read request package
            async_read(*m_boost_socket, m_raw_buf, boost::asio::transfer_all(),
                                                   boost::bind(&CDataClientInterface::readHeader,
                                                               this,
                                                               boost::asio::placeholders::error,
                                                               boost::asio::placeholders::bytes_transferred));
         }
    }
    else
    {
        qDebug() << __func__ << ": bad package, skip it...";
        m_read_buffer.consume(m_read_buffer.size());

        // wait for new header
        // allocate new subbuffer & wait for response package
        m_raw_buf = m_read_buffer.prepare(sizeof(THeader));

        // wait for read request package
        async_read(*m_boost_socket, m_raw_buf, boost::asio::transfer_all(),
                                               boost::bind(&CDataClientInterface::readHeader,
                                                           this,
                                                           boost::asio::placeholders::error,
                                                           boost::asio::placeholders::bytes_transferred));
    }
}

void CDataClientInterface::processIncomingData()
{
    std::istream is_data(&m_read_buffer);

    // read data
    is_data.read(reinterpret_cast<char*>(&m_package.array), m_package.header.data_size - sizeof(m_package.header));
    if (is_data.gcount() >=  m_package.header.data_size - sizeof(m_package.header))
    {
        switch(m_package.header.cmd)
        {
          case GetDiscretPoint :
          {
             std::lock_guard<std::mutex> lock(m_dpoint_mtx);
             for (int i=0; i < m_package.header.number_point; i++)
             {
                DPOINT[m_package.header.start_point + i] = m_package.array.digital[i];
             }
             dpoint_updated(m_package.header.start_point, m_package.header.number_point);
             break;
          }
          case GetAnalogPoint :
          {
             std::lock_guard<std::mutex> lock(m_apoint_mtx);
             for (int i=0; i < m_package.header.number_point; i++)
             {
                APOINT[m_package.header.start_point + i] = m_package.array.analog[i];
             }
             apoint_updated(m_package.header.start_point, m_package.header.number_point);
             break;
          }
          case NotifyDiscretPoint :
          {
             std::lock_guard<std::mutex> lock(m_dpoint_mtx);
             for (int i=0; i < m_package.header.number_point; i++)
             {
                DPOINT[m_package.header.start_point + i] = m_package.array.digital[i];
             }
             dpoint_updated(m_package.header.start_point, m_package.header.number_point);
             break;
          }
          case NotifyAnalogPoint :
          {
             std::lock_guard<std::mutex> lock(m_apoint_mtx);
             for (int i=0; i < m_package.header.number_point; i++)
             {
                APOINT[m_package.header.start_point + i] = m_package.array.analog[i];
             }
             apoint_updated(m_package.header.start_point, m_package.header.number_point);
             break;
          }
          default :
          {
              qDebug() << __func__ << ": unknown command !";
              break;
          }
        }
        qDebug() << __func__ << ": processed" << m_package.header.data_size << "bytes";
    }
    else
    {
        qDebug() << __func__ << ": bad package, skip it...";
        m_read_buffer.consume(m_read_buffer.size());
    }

    // wait for new header
    // allocate new subbuffer & wait for response package
    m_raw_buf = m_read_buffer.prepare(sizeof(THeader));

    // wait for read request package
    async_read(*m_boost_socket, m_raw_buf, boost::asio::transfer_all(),
                                           boost::bind(&CDataClientInterface::readHeader,
                                                       this,
                                                       boost::asio::placeholders::error,
                                                       boost::asio::placeholders::bytes_transferred));
}

void CDataClientInterface::updateConnectionState(bool state)
{
    m_connection_state = state;
    emit dataConnection(m_connection_state);
}

void CDataClientInterface::sendGetDpointsReq(uint16_t start_point, uint16_t number_point)
{
    TClientRequest m_request;
    m_request.header.data_size    = sizeof(THeader);
    m_request.header.cmd          = GetDiscretPoint;
    m_request.header.start_point  = start_point;
    m_request.header.number_point = number_point;

    boost::asio::streambuf out_buffer;
    std::ostream output_strm(&out_buffer);
    output_strm.write(reinterpret_cast<const char*>(&m_request), sizeof(m_request));
    boost::asio::write(*(m_boost_socket.get()), out_buffer.data(), boost::asio::transfer_all());
}

void CDataClientInterface::sendGetApointsReq(uint16_t start_point, uint16_t number_point)
{
    TClientRequest m_request;
    m_request.header.data_size    = sizeof(THeader);
    m_request.header.cmd          = GetAnalogPoint;
    m_request.header.start_point  = start_point;
    m_request.header.number_point = number_point;

    boost::asio::streambuf out_buffer;
    std::ostream output_strm(&out_buffer);
    output_strm.write(reinterpret_cast<const char*>(&m_request), sizeof(m_request));
    boost::asio::write(*(m_boost_socket.get()), out_buffer.data(), boost::asio::transfer_all());
}

void CDataClientInterface::sendSetDpointReq(uint16_t point_number, const TDPOINT& value)
{
    TClientRequest m_request;
    m_request.header.data_size    = sizeof(m_request);
    m_request.header.cmd          = SetDiscretPoint;
    m_request.header.start_point  = point_number;
    m_request.header.number_point = 1;
    m_request.point.digital = value;

    boost::asio::streambuf out_buffer;
    std::ostream output_strm(&out_buffer);
    output_strm.write(reinterpret_cast<const char*>(&m_request), sizeof(m_request));
    boost::asio::write(*(m_boost_socket.get()), out_buffer.data(), boost::asio::transfer_all());
}

void CDataClientInterface::sendSetApointReq(uint16_t point_number, const TAPOINT& value)
{
    TClientRequest m_request;
    m_request.header.data_size    = sizeof(m_request);
    m_request.header.cmd          = SetAnalogPoint;
    m_request.header.start_point  = point_number;
    m_request.header.number_point = 1;
    m_request.point.analog = value;

    boost::asio::streambuf out_buffer;
    std::ostream output_strm(&out_buffer);
    output_strm.write(reinterpret_cast<const char*>(&m_request), sizeof(m_request));
    boost::asio::write(*(m_boost_socket.get()), out_buffer.data(), boost::asio::transfer_all());
}

void CDataClientInterface::serviceLoop()
{
    try
    {
        qDebug() << __func__ << ": io service was started";
        m_io_service.run();
        qDebug() << __func__ << ": io service shutdown...";
    }
    catch(const std::exception& e)
    {
        qDebug() << __func__ << ": input Err: " << e.what();
    }
}
