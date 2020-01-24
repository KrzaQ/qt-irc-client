#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QMetaType>

#include "irc.hpp"
#include "greeter.hpp"

void say_time(
    kq::irc::client&,
    kq::irc::message_data const&
);

int main(int argc, char *argv[])
{
    qRegisterMetaType<kq::irc::message_data>(
        "message_data"
    );

    QCoreApplication a(argc, argv);

    kq::irc::settings settings{
        "irc.freenode.org",
        6667,
        "ProgMag"
    };

    kq::irc::client irc{settings};
    auto greeter = new kq::irc::greeter(&irc, &settings);

    QObject::connect(&irc, &kq::irc::client::message,
        [&](kq::irc::message_data const& md) {
            if(md.type == "001")
                irc.join("#progmag");
    });

    QObject::connect(&irc, &kq::irc::client::message,
        greeter, &kq::irc::greeter::handle_message,
        Qt::QueuedConnection);

    QObject::connect(greeter, &kq::irc::greeter::say,
        &irc, &kq::irc::client::say,
        Qt::QueuedConnection);

    QObject::connect(&irc, &kq::irc::client::message,
        [&](kq::irc::message_data const& md) {
            say_time(irc, md);
    });

    return a.exec();
}

void say_time(
    kq::irc::client& irc,
    kq::irc::message_data const& msg
) {
    if(msg.type != "PRIVMSG")
        return;

    if(msg.message != "!time")
        return;

    auto date = QDateTime::currentDateTime().toString();
    irc.say(msg.where.toLatin1(), date.toLatin1());
}
