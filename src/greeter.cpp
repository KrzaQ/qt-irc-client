#include <QByteArray>
#include <QTextStream>

#include "greeter.hpp"
#include "common.hpp"

namespace kq::irc
{

greeter::greeter(QObject* parent, settings* config):
    QObject(parent),
    config{config}
{}

void greeter::handle_message(message_data const& msg)
{
    if(msg.type != "JOIN")
        return;

    auto nick = msg.who.split("!").front();
    if(nick == config->nick)
        return;

    QByteArray txt;
    QTextStream(&txt) << "Hello " << nick << "!";

    emit say(msg.where.toLatin1(), txt);
}

}
