#ifndef COMMON_HPP
#define COMMON_HPP


namespace kq::irc
{

struct settings
{
    QString host;
    quint16 port;
    QString nick;
};

struct message_data
{
    QString who;
    QString type;
    QString where;
    QString message;
};

}


#endif // COMMON_HPP
