﻿#ifndef QASIOTCPSOCKETPARENT_H
#define QASIOTCPSOCKETPARENT_H

#include <QObject>
#include "qasiosocket.h"

class QAsioTcpSocketParentPrivate;
class QAsioTcpServerParentPrivate;
namespace boost {
    template<typename T>
    class shared_ptr;
}

class QASIOSOCKET_EXPORT QAsioTcpSocketParent : public QObject
{
    Q_OBJECT

    friend class QAsioTcpSocketParentPrivate;
    friend class QAsioTcpServerParentPrivate;
public:
    explicit QAsioTcpSocketParent(int byteSize = 4096,QObject *parent = 0);
    virtual ~QAsioTcpSocketParent();

    /// @brief socket的当前链接状态
    /// @note  UnconnectedState &emsp; = &emsp;0X00&emsp;&emsp;未连接 <br/>
    ///  ConnectingState &emsp;=&emsp; 0X01 &emsp;&emsp;正在连接<br/>
    ///  ConnectedState &emsp;=&emsp; 0X02 &emsp;&emsp; 已连接
    enum SocketState {
        UnconnectedState = 0X00,
        ConnectingState = 0X01,
        ConnectedState = 0X02
    };

    /// @brief socket出错的位置
    /// @note ConnectEorro &emsp;=&emsp; 0X00 &emsp;&emsp;链接过程出错<br/>
    /// ReadError &emsp;=&emsp; 0X01 &emsp;&emsp;读取错误<br/>
    /// WriteEorro &emsp;=&emsp; 0X02 &emsp;&emsp;写入错误<br/>
    /// FindHostError &emsp;=&emsp; 0X03 &emsp;&emsp;解析主机错误<br/>
    enum SocketErroSite{
        ConnectEorro = 0X00,
        ReadError = 0X01,
        WriteEorro = 0X02,
        FindHostError = 0X03,
        NoBufferSize = 0X04,
        NoError = 0X05
    };

signals:
    /// @brief Signal 信号：链接状态改变
    /// @param state 最新的状态
    void stateChange(SocketState state);

public slots:
    /// @brief 链接到主机
    /// @param hostName 连接到的主机，支持域名和ip的，自动dns
    /// @param port 链接的目标端口
    void connectToHost(const QString &hostName, quint16 port);
    /// @brief 从目标主机断开链接
    void disconnectFromHost();

    /// @brief 发送数据到服务器
    /// @param data 要发送的数据，内部有队列缓存的，为空是自动执行队列
    /// @note 链接是未连接状态，和队列里没有有效数据则返回false
    virtual bool write(const QByteArray & data) = 0;

public:

    /// @brief 获取socket的本地描述符
    int socketDescriptor() const ;

    /// @brief 获取错误
    int error() const ;

    /// @brief 当前的链接状态
    SocketState state() const ;

    /// @brief 获取错误位置
    SocketErroSite erroSite() const ;

    /// @brief 获取链接另一端的IP
    QString getPeerIp() const ;

    /// @brief 获取链接另一端的端口
    qint16 getPeerPort() const ;

    /// @brief 设置超时时间，如果小于10s或自动变为10s的、、设置等于或小于0，就是取消超时。
    /// 计算方法是，此次接受的数据开始计时，如果在你设定的时间内没有新数据到来，就是超时，单位秒（s）
    void setHeartTimeOut(int s);

    /// @brief 获取超时时间，为0是未启用
    int getHeartTimeOut() const;

protected:
    virtual void haveErro() = 0;
    virtual void hostConnected() = 0;
    virtual void readDataed(const char * data,std::size_t bytes_transferred) = 0;
    virtual bool writeDataed(std::size_t bytes_transferred) = 0;
    virtual void finedHosted() = 0;
    virtual void heartTimeOut(int /*timeout*/){}

protected:
    void wirteData(const char * data,std::size_t size);
    void willDelete();
private:
    //对引用计数指针还用其指针，不是好的习惯，但是为了让此库只是编译时需要boost的头文件，而使用时不需要，只有用此下策
    boost::shared_ptr<QAsioTcpSocketParentPrivate> * p;

    Q_DISABLE_COPY(QAsioTcpSocketParent)
};

#endif // QASIOTCPSOCKETPARENT_H
