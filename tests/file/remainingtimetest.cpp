/*
 * This file is part of the KDE Baloo Project
 * Copyright (C) 2015  Pinak Ahuja <pinak.ahuja@gmail.com>
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

#include "database.h"
#include "firstrunindexer.h"
#include "filecontentindexerprovider.h"
#include "filecontentindexer.h"
#include "fileindexerconfig.h"
#include "timeestimator.h"

#include <QCoreApplication>
#include <QThreadPool>
#include <QTemporaryDir>
#include <QObject>
#include <QDBusConnection>
#include <QTextStream>
#include <QString>

#include <iostream>

class Scheduler : public QObject
{
    Q_OBJECT

public:
    Scheduler(QObject* parent = 0);
    void startIndexing();

private Q_SLOTS:
    void startContentIndexer();
    void printTime(QString url);
    void finished();

private:
    QTemporaryDir m_dir;

    Baloo::Database m_db;
    Baloo::FileContentIndexerProvider m_provider;
    Baloo::FileIndexerConfig m_config;
    Baloo::FileContentIndexer* m_contentRunnable;

    QThreadPool m_pool;

    int m_count;
    int m_batchSize;

    QTextStream m_out;
};

Scheduler::Scheduler(QObject* parent)
    : QObject(parent)
    , m_db(m_dir.path())
    , m_provider(&m_db)
    , m_contentRunnable(0)
    , m_count(0)
    , m_batchSize(40)
    , m_out(stdout)
{
    m_db.open(Baloo::Database::CreateDatabase);
    m_pool.setMaxThreadCount(1);
    QDBusConnection bus = QDBusConnection::sessionBus();
    bus.connect("", "/contentindexer", "org.kde.baloo", "startedWithFile", this, SLOT(printTime(QString)));

    // Set test path
    qputenv("BALOO_DB_PATH", m_dir.path().toUtf8());
}

void Scheduler::startIndexing()
{
    auto firstRunnable = new Baloo::FirstRunIndexer(&m_db, &m_config, m_config.includeFolders());
    connect(firstRunnable, &Baloo::FirstRunIndexer::done, this, &Scheduler::startContentIndexer);
    m_pool.start(firstRunnable);
}

void Scheduler::startContentIndexer()
{
    m_contentRunnable = new Baloo::FileContentIndexer(&m_provider);
    connect(m_contentRunnable, &Baloo::FileContentIndexer::done, this, &Scheduler::finished);
    m_pool.start(m_contentRunnable);
}

void Scheduler::printTime(QString url)
{
    Q_ASSERT(m_contentRunnable != 0);

    if (++m_count == 10 * m_batchSize) {
        Baloo::TimeEstimator estimator;
        estimator.setAverageTimePerBatch(m_contentRunnable->averageTimePerBatch());
        estimator.setFilesLeft(m_provider.size());
        // print Remaining time after every 10 batches
        m_out <<  "Remaining Time: " << estimator.calculateTimeLeft() << endl;
        m_count = 0;
    }
}

void Scheduler::finished()
{
    m_out << "done!" << endl;
    QCoreApplication::exit();
}

int main (int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    Scheduler sched;
    sched.startIndexing();
    app.exec();
}

#include "remainingtimetest.moc"
