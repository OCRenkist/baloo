/*
 * This file is part of the KDE Baloo Project
 * Copyright (C) 2013-2015  Vishesh Handa <vhanda@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _BALOO_SEARCHSTORE_H
#define _BALOO_SEARCHSTORE_H

#include <QString>

namespace Baloo {

class Query;
class Database;

class SearchStore
{
public:
    SearchStore();
    ~SearchStore();

    QStringList types();

    QVector<quint64> exec(const Query& query);
    QString filePath(quint64 id);

private:
    Database* m_db;
};

}

#endif // _BALOO_SEARCHSTORE_H
