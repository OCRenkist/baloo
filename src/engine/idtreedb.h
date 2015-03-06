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

#ifndef BALOO_IDTREEDB_H
#define BALOO_IDTREEDB_H

#include "engine_export.h"
#include <lmdb.h>
#include <QVector>

namespace Baloo {

class BALOO_ENGINE_EXPORT IdTreeDB
{
public:
    explicit IdTreeDB(MDB_txn* txn);
    ~IdTreeDB();

    void put(quint64 docId, const QVector<quint64> subDocIds);
    QVector<quint64> get(quint64 docId);
    void del(quint64 docId);

    void setTransaction(MDB_txn* txn) {
        m_txn = txn;
    }
private:
    MDB_txn* m_txn;
    MDB_dbi m_dbi;
};
}

#endif // BALOO_IDTREEDB_H
