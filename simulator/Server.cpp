#include "Server.h"
#include <stdio.h>

#include <QtCore>
#include <QtWebSockets>

#include <cstdio>
using namespace std;

QT_USE_NAMESPACE

static QString getIdentifier(QWebSocket *peer) {
  return QStringLiteral("%1:%2").arg(peer->peerAddress().toString(), QString::number(peer->peerPort()));
}

//! [constructor]
Server::Server(quint16 port, BoxSimulator *simulator, QObject *parent)
    : QObject(parent), simulator(simulator),
      webSocketServer(new QWebSocketServer(QStringLiteral("Server"), QWebSocketServer::NonSecureMode, this)) {
  if (webSocketServer->listen(QHostAddress::Any, port)) {
    QTextStream(stdout) << "Listening for joysticks on port " << port << '\n';
    connect(webSocketServer, &QWebSocketServer::newConnection, this, &Server::onNewConnection);
  }
}

Server::~Server() { webSocketServer->close(); }
//! [constructor]

//! [onNewConnection]
void Server::onNewConnection() {
  auto pSocket = webSocketServer->nextPendingConnection();
  QTextStream(stdout) << getIdentifier(pSocket) << " connected!\n";
  pSocket->setParent(this);

  connect(pSocket, &QWebSocket::textMessageReceived, this, &Server::processMessage);
  connect(pSocket, &QWebSocket::disconnected, this, &Server::socketDisconnected);

  clients << pSocket;
}
//! [onNewConnection]

//! [processMessage]
void Server::processMessage(const QString &message) {
  QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());
  // std::cout << pSender->localPort() << " and " << pSender->peerPort() << std::endl;
  // QTextStream(stdout) << "Incoming message: " << message << "\n";
  QStringList pieces = message.split(";");
  size_t my_index = pSender->peerPort() % simulator->people.size();
  simulator->people[my_index].npc = false;
  simulator->people[my_index].setVelocity(
      pieces[0].toDouble() * INITIAL_MAX_SPEED * 10, pieces[1].toDouble() * INITIAL_MAX_SPEED * 10);
}
//! [processMessage]

//! [socketDisconnected]
void Server::socketDisconnected() {
  QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
  QTextStream(stdout) << getIdentifier(pClient) << " disconnected!\n";
  if (pClient) {
    clients.removeAll(pClient);
    pClient->deleteLater();
  }
}
//! [socketDisconnected]
