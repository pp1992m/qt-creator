/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#pragma once

#include "clangtoolsdiagnostic.h"
#include "clangtoolsprojectsettings.h"

#include <debugger/analyzer/detailederrorview.h>
#include <utils/fileutils.h>
#include <utils/treemodel.h>

#include <QPointer>
#include <QSortFilterProxyModel>

#include <functional>

namespace ProjectExplorer { class Project; }

namespace ClangTools {
namespace Internal {

class DiagnosticItem : public Utils::TreeItem
{
public:
    using OnCheckedFixit = std::function<void(bool)>;
    DiagnosticItem(const Diagnostic &diag, const OnCheckedFixit &onCheckedFixit);

    Diagnostic diagnostic() const { return m_diagnostic; }
    bool applyFixits() const { return m_applyFixits; }

private:
    Qt::ItemFlags flags(int column) const override;
    QVariant data(int column, int role) const override;
    bool setData(int column, const QVariant &data, int role) override;

private:
    const Diagnostic m_diagnostic;
    bool m_applyFixits = false;
    OnCheckedFixit m_onCheckedFixit;
};

class ClangToolsDiagnosticModel : public Utils::TreeModel<>
{
    Q_OBJECT

public:
    ClangToolsDiagnosticModel(QObject *parent = nullptr);

    virtual void addDiagnostics(const QList<Diagnostic> &diagnostics);
    virtual QList<Diagnostic> diagnostics() const;

    enum ItemRole {
        DiagnosticRole = Debugger::DetailedErrorView::FullTextRole + 1
    };

signals:
    void fixItsToApplyCountChanged(int count);

private:
    int m_fixItsToApplyCount = 0;
};

class DiagnosticFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    DiagnosticFilterModel(QObject *parent = nullptr);

    void setProject(ProjectExplorer::Project *project);
    void addSuppressedDiagnostic(const SuppressedDiagnostic &diag);
    ProjectExplorer::Project *project() const { return m_project; }

private:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    void handleSuppressedDiagnosticsChanged();

    QPointer<ProjectExplorer::Project> m_project;
    Utils::FileName m_lastProjectDirectory;
    SuppressedDiagnosticsList m_suppressedDiagnostics;
};

} // namespace Internal
} // namespace ClangTools