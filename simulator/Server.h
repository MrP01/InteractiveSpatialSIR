
#ifndef Server_H
#define Server_H

#include "Simulator.h"
#include <QtCore/QList>
#include <QtCore/QObject>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(QString)

class Server : public QObject {
  Q_OBJECT
 public:
  explicit Server(quint16 port, BoxSimulator *simulator, QObject *parent = nullptr);
  ~Server() override;

 private slots:
  void onNewConnection();
  void processMessage(const QString &message);
  void socketDisconnected();

 private:
  BoxSimulator *simulator;
  QWebSocketServer *webSocketServer;
  QList<QWebSocket *> clients;
};

#endif // Server_H
