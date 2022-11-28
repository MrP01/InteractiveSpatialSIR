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
    : QObject(parent), m_simulator(simulator),
      m_pWebSocketServer(new QWebSocketServer(QStringLiteral("Server"), QWebSocketServer::NonSecureMode, this)) {
  if (m_pWebSocketServer->listen(QHostAddress::Any, port)) {
    QTextStream(stdout) << "Listening for joysticks on port " << port << '\n';
    connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, &Server::onNewConnection);
  }
}

Server::~Server() { m_pWebSocketServer->close(); }
//! [constructor]

//! [onNewConnection]
void Server::onNewConnection() {
  auto pSocket = m_pWebSocketServer->nextPendingConnection();
  QTextStream(stdout) << getIdentifier(pSocket) << " connected!\n";
  pSocket->setParent(this);

  connect(pSocket, &QWebSocket::textMessageReceived, this, &Server::processMessage);
  connect(pSocket, &QWebSocket::disconnected, this, &Server::socketDisconnected);

  m_clients << pSocket;
}
//! [onNewConnection]

//! [processMessage]
void Server::processMessage(const QString &message) {
  QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());
  // std::cout << pSender->localPort() << " and " << pSender->peerPort() << std::endl;
  QTextStream(stdout) << "Incoming message: " << message << "\n";
  QStringList pieces = message.split(";");
  size_t my_index = pSender->peerPort() % m_simulator->people.size();
  m_simulator->people[my_index].setVelocity(pieces[0].toDouble(), pieces[1].toDouble());
  // take in position and update
}
//! [processMessage]

//! [socketDisconnected]
void Server::socketDisconnected() {
  QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
  QTextStream(stdout) << getIdentifier(pClient) << " disconnected!\n";
  if (pClient) {
    m_clients.removeAll(pClient);
    pClient->deleteLater();
  }
}
//! [socketDisconnected]
