#ifndef IRC_HPP
#define IRC_HPP

#include <QByteArray>
#include <QObject>

#include "common.hpp"

class QTcpSocket;

namespace kq::irc
{

class client : public QObject
{
    Q_OBJECT
public:
    explicit client(
        settings const&,
        QObject *parent = nullptr
    );

    settings const& config() const { return cfg; }

public slots:
    void say(QByteArray const&, QByteArray const&);
    void join(QByteArray const&);

    void write_line(QByteArray const&);

signals:
    void message(message_data const&);

private:
    void connect_to_irc();
    void on_read(QByteArray const&);
    void parse_lines();

    void identify();
    void handle_ping(message_data const&);

    settings cfg;
    QTcpSocket* connection;
    QByteArray input_buffer;
};

}

#endif // IRC_HPP
