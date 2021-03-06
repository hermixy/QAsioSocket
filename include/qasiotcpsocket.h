﻿/// ********************************************************************************************************************
/// @file qasiotcpsocket.h
/// @brief TcpSocket的封装
/// @version 0.00.01
/// @date 2015.02.03
/// @author 渡世白玉(www.dushibaiyu.com)
///
/// ====================================================================================================================

#ifndef QASIOTCPSOCKET_H
#define QASIOTCPSOCKET_H

#include <QObject>
#include <QBuffer>
#include <QMutex>
#include <QMutexLocker>
#include <QQueue>
#include <QEvent>
#include <QVariant>
#include "qasiotcpsocketparent.h"

class QAsioTcpServer;

/// @brief TcpSocket的封装,接口按照QTcpsocket的设计的
class QASIOSOCKET_EXPORT QAsioTcpSocket : public QAsioTcpSocketParent
{
    Q_OBJECT
public:

    /// @brief 构造函数
    explicit QAsioTcpSocket(int bytesize = 4096,QObject *parent = 0);

    /// @brief 析构
    virtual ~QAsioTcpSocket();

signals:

    /// @brief Signal 信号：数据就绪的信号
    void readReadly();

    /// @brief Signal 信号：链接成功的信号
    void connected();

    /// @brief Signal 信号：断开链接的信号
    void disconnected();

    /// @brief Signal 信号：发生错误信号
    /// @param site 发生错误的位置
    /// @param erro_code 错误码
    void sentError(SocketErroSite site,int erro_code);

    /// @brief Signal 信号：解析主机成功的信号
    void hostFound();

    void heartTimeOuted(int i);

public slots:
    /// @brief 发送数据到服务器
    /// @param data 要发送的数据，内部有队列缓存的，为空是自动执行队列
    /// @note 链接是未连接状态，和队列里没有有效数据则返回false
    bool write(const QByteArray & data);

public:

    /// @brief 当前可读字节数
    qint64 bytesAvailable() const {return buffer.size() - buffer.pos();}

    /// @brief 读取数据
    /// @param maxsize 最多读取多少字节
    QByteArray read(qint64 maxsize)
    {
        QMutexLocker mutexLocker(&bufferMutex);
        return buffer.read(maxsize);
    }

    /// @brief 读取全部数据
    QByteArray readAll()
    {
        QMutexLocker mutexLocker(&bufferMutex);
        return buffer.readAll();
    }

    /// @brief 读取一行数据
    QByteArray readLine()
    {
        QMutexLocker mutexLocker(&bufferMutex);
        return buffer.readLine();
    }

    /// @brief 是否读取到最后了
    bool atEnd() {
        QMutexLocker mutexLocker(&bufferMutex);
        if (buffer.isOpen())
            return buffer.atEnd();
        else
            return true;
    }

    /// @brief 设置是不是断开链接就立刻清除用户缓存，并停止发送数据就绪信号
    /// @note 默认是false，不清除和停止信号的。<br/>
    ///  但是在QAsioTcpServer里发送出来的是true，既清除和停止信号
    void setDisconnecdDeleteBuffer(bool isdel) {isDisconDelData = isdel;}
    /// @brief 当前的断开就清楚数据的设置
    bool disconnecdDeleteBuffer() const {return isDisconDelData;}

protected:
    virtual void haveErro();
    virtual void hostConnected();
    virtual void readDataed(const char * data,std::size_t bytes_transferred);
    virtual bool writeDataed(std::size_t bytes_transferred);
    virtual void finedHosted();
    virtual void heartTimeOut(int timeout);
    // 处理自定义事件，就是asio事件循环发送过来的事件
    virtual void customEvent(QEvent * event);

private:
    //发送的队列和队列的锁
    QMutex writeMutex;
    QQueue<QByteArray> writeQueue;

    //接受数据的buffer和buffr锁
    QBuffer buffer;
    QMutex bufferMutex;
    bool isDisconDelData;
    Q_DISABLE_COPY(QAsioTcpSocket)
};

class QASIOSOCKET_EXPORT QAsioEvent : public QEvent
{
public:
    enum ConnectedEvent{//socket事件代表的状态
        Connected, //已连接
        DisConnect,//断开链接
        HaveEorro,//发现错误错误
        ReadReadly, //数据可供读取
        HostFined, //找到主机
        HeartTimeOut //心跳时间到了，但是没有数据
    };

    /// @brief 默认构造函数
    explicit QAsioEvent():QEvent(QAsioSocketEventType){}

    /// @brief 构造函数，并传入需要携带的信息
    /// @param ctype 事件代表的状态
    /// @param code 如果是错误，错误信息是什么
    QAsioEvent(ConnectedEvent ctype)
        :QEvent(QAsioSocketEventType),cType_(ctype){}

    /// @brief 设置事件代表的状态
    /// @param ctype 事件代表的状态
    void setSocketState(ConnectedEvent ctype){cType_ = ctype;}
    void setData(const QVariant & data) {data_ = data;}

    /// @brief 获取事件代表的状态
    ConnectedEvent getConnectedType() const {return cType_;}

    QVariant getData() const {return data_;}
private:
    ConnectedEvent cType_;
    QVariant data_;
public:
    /// @brief 事件的类型
    static const QEvent::Type QAsioSocketEventType;

};

#endif // QASIOTCPSOCKET_H
