#ifndef GREETER_HPP
#define GREETER_HPP

#include <QObject>

namespace kq::irc
{

struct message_data;
struct settings;

class greeter : public QObject
{
    Q_OBJECT
public:
    greeter(QObject* parent, settings* config);

public slots:
    void handle_message(message_data const&);

signals:
    void say(QByteArray const&, QByteArray const&);

private:
    settings* config;
};

}

#endif // GREETER_HPP
