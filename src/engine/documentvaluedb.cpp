/*
   This file is part of the KDE Baloo project.
 * Copyright (C) 2015  Vishesh Handa <vhanda@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "documentvaluedb.h"

using namespace Baloo;

DocumentValueDB::DocumentValueDB(MDB_txn* txn)
    : m_txn(txn)
{
    Q_ASSERT(txn != 0);

    int rc = mdb_dbi_open(txn, "documentvaluedb", MDB_CREATE | MDB_INTEGERKEY, &m_dbi);
    Q_ASSERT(rc == 0);
}

DocumentValueDB::~DocumentValueDB()
{
    mdb_dbi_close(mdb_txn_env(m_txn), m_dbi);
}

void DocumentValueDB::put(uint docId, uint slotNum, const QByteArray& value)
{
    Q_ASSERT(docId > 0);

    uint keyArr[2] = {docId, slotNum};
    MDB_val key;
    key.mv_size = sizeof(uint) + sizeof(uint);
    key.mv_data = static_cast<void*>(&keyArr);

    MDB_val val;
    val.mv_size = value.size();
    val.mv_data = static_cast<void*>(const_cast<char*>(value.constData()));

    int rc = mdb_put(m_txn, m_dbi, &key, &val, 0);
    if (rc == MDB_MAP_FULL) {
        Q_ASSERT_X(0, "", "Database is full. You need to increase the map size");
    }
    Q_ASSERT(rc == 0);
}

QByteArray DocumentValueDB::get(uint docId, uint slotNum)
{
    Q_ASSERT(docId > 0);

    uint keyArr[2] = {docId, slotNum};
    MDB_val key;
    key.mv_size = sizeof(uint) + sizeof(uint);
    key.mv_data = static_cast<void*>(&keyArr);

    MDB_val val;
    int rc = mdb_get(m_txn, m_dbi, &key, &val);
    if (rc == MDB_NOTFOUND) {
        return QByteArray();
    }
    Q_ASSERT(rc == 0);

    return QByteArray::fromRawData(static_cast<char*>(val.mv_data), val.mv_size);
}

void DocumentValueDB::del(uint docId)
{
    Q_ASSERT(docId > 0);

    uint keyArr[2] = {docId, 0};
    MDB_val key;
    key.mv_size = sizeof(uint) + sizeof(uint);
    key.mv_data = static_cast<void*>(&keyArr);

    MDB_cursor* cursor;
    mdb_cursor_open(m_txn, m_dbi, &cursor);
    while(1) {
        int rc = mdb_cursor_get(cursor, &key, 0, MDB_SET_RANGE);
        if (rc == MDB_NOTFOUND) {
            return;
        }
        Q_ASSERT(rc == 0);

        uint* keyArr = static_cast<uint*>(key.mv_data);
        uint fetchedId = keyArr[0];

        if (fetchedId != docId)
            break;

        rc = mdb_cursor_del(cursor, 0);
        Q_ASSERT(rc == 0);
    }
}
