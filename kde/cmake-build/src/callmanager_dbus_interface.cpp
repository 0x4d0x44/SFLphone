/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -m -c CallManagerInterface -i dbus/metatypes.h -p callmanager_dbus_interface /home/emmanuel/sflphone/sflphone-client-kde/src/dbus/callmanager-introspec.xml
 *
 * qdbusxml2cpp is Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#include "callmanager_dbus_interface.h"

/*
 * Implementation of interface class CallManagerInterface
 */

CallManagerInterface::CallManagerInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
}

CallManagerInterface::~CallManagerInterface()
{
}


#include "callmanager_dbus_interface.moc"
