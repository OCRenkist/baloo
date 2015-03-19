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

#ifndef BALOO_POSITIONDB_H
#define BALOO_POSITIONDB_H

#include "engine_export.h"
#include "postingiterator.h"

#include <QByteArray>
#include <QVector>
#include <QDebug>
#include <lmdb.h>

namespace Baloo {

struct PositionInfo {
    quint64 docId;
    QVector<uint> positions;

    PositionInfo(quint64 id = 0, const QVector<uint> posList = QVector<uint>())
        : docId(id), positions(posList) {}

    bool operator ==(const PositionInfo& rhs) const {
        return docId == rhs.docId;
    }
    bool operator !=(const PositionInfo& rhs) const {
        return docId != rhs.docId;
    }

    bool operator <(const PositionInfo& rhs) const {
        return docId < rhs.docId;
    }
};

class BALOO_ENGINE_EXPORT PositionDB
{
public:
    explicit PositionDB(MDB_txn* txn);
    ~PositionDB();

    void put(const QByteArray& term, const QVector<PositionInfo>& list);
    QVector<PositionInfo> get(const QByteArray& term);

    PostingIterator* iter(const QByteArray& term);

    void setTransaction(MDB_txn* txn) {
        m_txn = txn;
    }
private:
    MDB_txn* m_txn;
    MDB_dbi m_dbi;
};

inline QDebug operator<<(QDebug dbg, const PositionInfo &pos) {
    dbg << "(" << pos.docId << "-->" << pos.positions << ")";
    return dbg;
}

}

Q_DECLARE_TYPEINFO(Baloo::PositionInfo, Q_MOVABLE_TYPE);

#endif // BALOO_POSITIONDB_H
