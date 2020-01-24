#include <QDebug>
#include <QRegularExpression>
#include <QTcpSocket>
#include <QTextStream>

#include "irc.hpp"

namespace kq::irc
{

client::client(settings const& config, QObject *parent):
    QObject(parent),
    cfg{config},
    connection{}
{
    connect_to_irc();

    connect(this, &client::message,
            this, &client::handle_ping,
            Qt::QueuedConnection);
}

void client::connect_to_irc()
{
    if(connection) {
        connection->disconnect();
        connection->disconnectFromHost();
        delete connection;
    }

    qDebug() << "Connecting to: " << cfg.host << cfg.port;

    input_buffer.clear();

    connection = new QTcpSocket(this);

    connect(connection, &QTcpSocket::connected, [this]{
        qDebug() << "Connected.";
        identify();
    });

    connect(connection, &QTcpSocket::disconnected, [this]{
        qDebug() << "Disconnected.";
        connect_to_irc();
    });

    connect(connection, &QTcpSocket::readyRead, [this]{
        on_read(connection->readAll());
    });

    connection->connectToHost(cfg.host, cfg.port);
}

void client::on_read(QByteArray const& buf)
{
    input_buffer += buf;
    parse_lines();
}

void client::parse_lines()
{
    static auto constexpr server_message =
        R"((?::([^@!\ ]*(?:(?:![^@]*)?@[^\ ]*)?)\ ))"
        R"(?([^\ ]+)((?:[^:\ ][^\ ]*)?(?:\ [^:\ ][^\ ]*))"
        R"({0,14})(?:\ :?(.*))?)";
    QRegularExpression re{server_message};

    while(true) {
        auto new_line_index = input_buffer.indexOf("\r\n");
        if(new_line_index == -1)
            break;

        QString line = input_buffer.left(new_line_index);
        input_buffer.remove(0, new_line_index + 2);

        auto m = re.match(line);
        if (m.hasMatch()) {
            message_data msg = {
                m.captured(1),
                m.captured(2),
                m.captured(3),
                m.captured(4)
            };
            qDebug() << m;
            emit message(msg);
        }
    }
}

void client::identify()
{
    QByteArray msg;
    QTextStream(&msg) << "USER " << cfg.nick << " foo bar :"
        << cfg.nick;
    write_line(msg);
    msg.clear();
    QTextStream(&msg) << "NICK :" + cfg.nick;
    write_line(msg);
}

void client::handle_ping(message_data const& msg)
{
    if(msg.type != "PING")
        return;

    qDebug() << msg.type << msg.message;
    write_line("PONG: " + msg.message.toLatin1());
}

void client::write_line(QByteArray const& line)
{
    connection->write(line);
    connection->write("\r\n");
}

void client::say(
    QByteArray const& recipient,
    QByteArray const& message
) {
    QByteArray msg;
    QTextStream(&msg) << "PRIVMSG " << recipient << " :"
        << message;
    write_line(msg);
}

void client::join(QByteArray const& channel)
{
    QByteArray msg;
    QTextStream(&msg) << "JOIN " << channel;
    write_line(msg);
}

}
