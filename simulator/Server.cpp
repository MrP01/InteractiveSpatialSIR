// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#include "Server.h"

#include <QtCore>
#include <QtWebSockets>

#include <cstdio>
using namespace std;

QT_USE_NAMESPACE

static QString getIdentifier(QWebSocket *peer) {
  return QStringLiteral("%1:%2").arg(peer->peerAddress().toString(), QString::number(peer->peerPort()));
}

//! [constructor]
Server::Server(quint16 port, QObject *parent)
    : QObject(parent),
      m_pWebSocketServer(new QWebSocketServer(QStringLiteral("Server"), QWebSocketServer::NonSecureMode, this)) {
  if (m_pWebSocketServer->listen(QHostAddress::Any, port)) {
    QTextStream(stdout) << "Connecting to joysticks on port " << port << '\n';
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
  QStringList pieces = message.split(" ");
  // take in position and update
}
//! [processMessage]w

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
