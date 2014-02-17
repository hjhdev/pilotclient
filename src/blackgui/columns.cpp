/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "columns.h"
#include <QModelIndex>
#include <QCoreApplication>

namespace BlackGui
{
    CColumn::CColumn(const QString &headerName, int propertyIndex, int alignment, bool editable) :
        m_columnName(headerName), m_alignment(alignment), m_propertyIndex(propertyIndex), m_editable(editable)
    {}

    CColumn::CColumn(const QString &headerName, int propertyIndex, bool editable) :
        m_columnName(headerName), m_alignment(-1), m_propertyIndex(propertyIndex), m_editable(editable)
    {}

    const char *CColumn::getTranslationContextChar() const
    {
        return this->m_translationContext.toUtf8().constData();
    }

    const char *CColumn::getColumnNameChar() const
    {
        return this->m_columnName.toUtf8().constData();
    }

    QVariant CColumn::aligmentAsQVariant() const
    {
        if (this->hasAlignment()) return QVariant(this->m_alignment);
        return QVariant(Qt::AlignVCenter | Qt::AlignLeft); // default
    }

    QString CColumn::getColumnName(bool i18n) const
    {
        if (!i18n || this->m_translationContext.isEmpty()) return this->m_columnName;
        return QCoreApplication::translate(this->getTranslationContextChar(), this->getColumnNameChar());
    }

    /*
     * Header
     */
    CColumns::CColumns(const QString &translationContext, QObject *parent) :
        QObject(parent), m_translationContext(translationContext)
    {
        // void
    }

    /*
     * Add column
     */
    void CColumns::addColumn(CColumn column)
    {
        Q_ASSERT(!this->m_translationContext.isEmpty());
        column.setTranslationContext(this->m_translationContext);
        this->m_columns.push_back(column);
    }

    /*
     * Property index to name
     */
    QString CColumns::propertyIndexToColumnName(int propertyIndex, bool i18n) const
    {
        int column = this->propertyIndexToColumn(propertyIndex);
        return this->m_columns.at(column).getColumnName(i18n);
    }

    /*
     * Index to name
     */
    QString CColumns::columnToName(int column, bool i18n) const
    {
        Q_ASSERT(column >= 0 && column < this->m_columns.size());
        return this->m_columns.at(column).getColumnName(i18n);
    }

    /*
     * Get property index
     */
    int CColumns::columnToPropertyIndex(int column) const
    {
        Q_ASSERT(column >= 0 && column < this->m_columns.size());
        return this->m_columns.at(column).getPropertyIndex();
    }

    /*
     * Property index to column
     */
    int CColumns::propertyIndexToColumn(int propertyIndex) const
    {
        for (int i = 0; i < this->m_columns.size(); i++)
        {
            if (this->m_columns.at(i).getPropertyIndex() == propertyIndex)
                return i;
        }
        return -1;
    }

    /*
     * Name to property index
     */
    int CColumns::nameToPropertyIndex(const QString &name) const
    {
        for (int i = 0; i < this->m_columns.size(); i++)
        {
            if (this->m_columns.at(i).getColumnName(false) == name)
                return i;
        }
        return -1;
    }

    /*
     * Size
     */
    int CColumns::size() const
    {
        return this->m_columns.size();
    }

    /*
     * Alignment?
     */
    bool CColumns::hasAlignment(const QModelIndex &index) const
    {
        if (index.column() < 0 || index.column() >= this->m_columns.size()) return false;
        return this->m_columns.at(index.column()).hasAlignment();
    }

    /*
     * Editable?
     */
    bool CColumns::isEditable(const QModelIndex &index) const
    {
        if (index.column() < 0 || index.column() >= this->m_columns.size()) return false;
        return this->m_columns.at(index.column()).isEditable();
    }

    /*
     * Aligment as QVariant
     */
    QVariant CColumns::aligmentAsQVariant(const QModelIndex &index) const
    {
        if (index.column() < 0 || index.column() >= this->m_columns.size()) return QVariant();
        return this->m_columns.at(index.column()).aligmentAsQVariant();
    }

    /*
     * Context
     */
    const char *CColumns::getTranslationContextChar() const
    {
        return this->m_translationContext.toUtf8().constData();
    }

} // namespace BlackGui
