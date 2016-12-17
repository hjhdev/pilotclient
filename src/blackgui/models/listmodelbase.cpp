/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

// Drag and drop docu:
// http://doc.qt.io/qt-5/model-view-programming.html#using-drag-and-drop-with-item-views

#include "blackgui/guiutility.h"
#include "blackgui/models/columnformatters.h"
#include "blackgui/models/listmodelbase.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/airport.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/compare.h"
#include "blackmisc/country.h"
#include "blackmisc/countrylist.h"
#include "blackmisc/identifier.h"
#include "blackmisc/identifierlist.h"
#include "blackmisc/namevariantpair.h"
#include "blackmisc/namevariantpairlist.h"
#include "blackmisc/network/client.h"
#include "blackmisc/network/clientlist.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/network/user.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/predicates.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/sequence.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/variant.h"
#include "blackmisc/verify.h"
#include "blackmisc/weather/cloudlayerlist.h"
#include "blackmisc/weather/temperaturelayerlist.h"
#include "blackmisc/weather/windlayerlist.h"
#include "blackmisc/worker.h"

#include <QFlags>
#include <QJsonDocument>
#include <QList>
#include <QMimeData>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackGui
{
    namespace Models
    {
        int CListModelBaseNonTemplate::columnCount(const QModelIndex &modelIndex) const
        {
            Q_UNUSED(modelIndex);
            int c = this->m_columns.size();
            return c;
        }

        QVariant CListModelBaseNonTemplate::headerData(int section, Qt::Orientation orientation, int role) const
        {
            if (orientation != Qt::Horizontal)
            {
                return QVariant();
            }
            bool handled = (role == Qt::DisplayRole || role == Qt::ToolTipRole || role == Qt::InitialSortOrderRole);
            if (!handled)
            {
                return QVariant();
            }
            if (section < 0 || section >= this->m_columns.size())
            {
                return QVariant();
            }

            if (role == Qt::DisplayRole)
            {
                QString header = this->m_columns.at(section).getColumnName(false);
                return QVariant(header);
            }
            else if (role == Qt::ToolTipRole)
            {
                QString header = this->m_columns.at(section).getColumnToolTip(false);
                return header.isEmpty() ? QVariant() : QVariant(header);
            }
            return QVariant();
        }

        QModelIndex CListModelBaseNonTemplate::index(int row, int column, const QModelIndex &parent) const
        {
            Q_UNUSED(parent);
            return QStandardItemModel::createIndex(row, column);
        }

        QModelIndex CListModelBaseNonTemplate::parent(const QModelIndex &child) const
        {
            Q_UNUSED(child);
            return QModelIndex();
        }

        BlackMisc::CPropertyIndex CListModelBaseNonTemplate::columnToPropertyIndex(int column) const
        {
            return this->m_columns.columnToPropertyIndex(column);
        }

        int CListModelBaseNonTemplate::propertyIndexToColumn(const CPropertyIndex &propertyIndex) const
        {
            return m_columns.propertyIndexToColumn(propertyIndex);
        }

        BlackMisc::CPropertyIndex CListModelBaseNonTemplate::modelIndexToPropertyIndex(const QModelIndex &index) const
        {
            return this->columnToPropertyIndex(index.column());
        }

        void CListModelBaseNonTemplate::sortByPropertyIndex(const CPropertyIndex &propertyIndex, Qt::SortOrder order)
        {
            const int column = this->propertyIndexToColumn(propertyIndex);
            this->sort(column, order);
        }

        void CListModelBaseNonTemplate::setSortColumnByPropertyIndex(const BlackMisc::CPropertyIndex &propertyIndex)
        {
            this->m_sortColumn = this->m_columns.propertyIndexToColumn(propertyIndex);
        }

        void CListModelBaseNonTemplate::setSorting(const CPropertyIndex &propertyIndex, Qt::SortOrder order)
        {
            this->setSortColumnByPropertyIndex(propertyIndex);
            this->m_sortOrder = order;
        }

        bool CListModelBaseNonTemplate::hasValidSortColumn() const
        {

            if (!(this->m_sortColumn >= 0 && this->m_sortColumn < this->m_columns.size())) { return false; }
            return this->m_columns.isSortable(this->m_sortColumn);
        }

        Qt::ItemFlags CListModelBaseNonTemplate::flags(const QModelIndex &index) const
        {
            Qt::ItemFlags f = QStandardItemModel::flags(index);
            if (!index.isValid()) { return f; }
            const bool editable = this->m_columns.isEditable(index);
            f = editable ? (f | Qt::ItemIsEditable) : (f & ~Qt::ItemIsEditable);

            // flags from formatter
            const CDefaultFormatter *formatter = this->m_columns.getFormatter(index);
            if (formatter) { f = formatter->flags(f, editable); }

            // drag and drop
            f = f | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
            return f;
        }

        const QString &CListModelBaseNonTemplate::getTranslationContext() const
        {
            return m_columns.getTranslationContext();
        }

        Qt::DropActions CListModelBaseNonTemplate::supportedDragActions() const
        {
            return isOrderable() ? Qt::CopyAction | Qt::MoveAction : Qt::CopyAction;
        }

        Qt::DropActions CListModelBaseNonTemplate::supportedDropActions() const
        {
            return this->m_dropActions;
        }

        QStringList CListModelBaseNonTemplate::mimeTypes() const
        {
            static const QStringList mimes({ "application/swift.container.json" });
            return mimes;
        }

        void CListModelBaseNonTemplate::markDestroyed()
        {
            this->m_modelDestroyed = true;
        }

        bool CListModelBaseNonTemplate::isModelDestroyed()
        {
            return m_modelDestroyed;
        }

        void CListModelBaseNonTemplate::clearHighlighting()
        {
            // can be overridden to delete highlighting
        }

        void CListModelBaseNonTemplate::emitDataChanged(int startRowIndex, int endRowIndex)
        {
            BLACK_VERIFY_X(startRowIndex <= endRowIndex, Q_FUNC_INFO, "check rows");
            BLACK_VERIFY_X(startRowIndex >= 0 &&  endRowIndex >= 0, Q_FUNC_INFO, "check rows");

            const int columns = columnCount();
            const int rows = rowCount();
            if (columns < 1) { return; }
            if (startRowIndex < 0) { startRowIndex = 0; }
            if (endRowIndex >= rows) { endRowIndex = rows - 1; }
            const QModelIndex topLeft(createIndex(startRowIndex, 0));
            const QModelIndex bottomRight(createIndex(endRowIndex, columns - 1));
            emit this->dataChanged(topLeft, bottomRight);
        }

        CListModelBaseNonTemplate::CListModelBaseNonTemplate(const QString &translationContext, QObject *parent)
            : QStandardItemModel(parent), m_columns(translationContext), m_sortColumn(-1), m_sortOrder(Qt::AscendingOrder)
        {
            // non unique default name, set translation context as default
            this->setObjectName(translationContext);

            // connect
            connect(this, &CListModelBaseNonTemplate::dataChanged, this, &CListModelBaseNonTemplate::ps_onDataChanged);
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        CListModelBase<ObjectType, ContainerType, UseCompare>::CListModelBase(const QString &translationContext, QObject *parent)
            : CListModelBaseNonTemplate(translationContext, parent)
        { }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        int CListModelBase<ObjectType, ContainerType, UseCompare>::rowCount(const QModelIndex &parentIndex) const
        {
            Q_UNUSED(parentIndex);
            return this->containerOrFilteredContainer().size();
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        bool CListModelBase<ObjectType, ContainerType, UseCompare>::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
        {
            Q_UNUSED(action);
            Q_UNUSED(row);
            Q_UNUSED(column);
            Q_UNUSED(parent);
            if (!this->isDropAllowed()) { return false; }
            if (!this->acceptDrop(data)) { return false; }
            return true;
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        bool CListModelBase<ObjectType, ContainerType, UseCompare>::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
        {
            Q_UNUSED(row);
            Q_UNUSED(column);
            if (!this->isOrderable() || !this->acceptDrop(data)) { return false; }
            const CVariant valueVariant(this->toCVariant(data));
            if (valueVariant.isValid())
            {
                if (action == Qt::MoveAction)
                {
                    const ContainerType container(valueVariant.value<ContainerType>());
                    if (container.isEmpty()) { return false; }
                    const int position = parent.row();
                    this->moveItems(container, position);
                }
            }
            return true;
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        bool CListModelBase<ObjectType, ContainerType, UseCompare>::isValidIndex(const QModelIndex &index) const
        {
            if (!index.isValid()) { return false; }
            return (index.row() >= 0 && index.row() < this->rowCount(index) &&
                    index.column() >= 0 && index.column() < this->columnCount(index));
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        QVariant CListModelBase<ObjectType, ContainerType, UseCompare>::data(const QModelIndex &index, int role) const
        {
            // check / init
            if (!this->isValidIndex(index)) { return QVariant(); }

            if (role == Qt::BackgroundRole)
            {
                return CListModelBaseNonTemplate::data(index, role);
            }

            // Formatter
            const CDefaultFormatter *formatter = this->m_columns.getFormatter(index);
            Q_ASSERT_X(formatter, Q_FUNC_INFO, "Missing formatter");

            // Upfront checking avoids unnecessary data fetching
            if (!formatter || !formatter->supportsRole(role)) { return CListModelBaseNonTemplate::data(index, role); }

            // index, updront checking
            const int row = index.row();
            const int col = index.column();
            const BlackMisc::CPropertyIndex propertyIndex = this->columnToPropertyIndex(col);
            if (static_cast<int>(CPropertyIndex::GlobalIndexLineNumber) == propertyIndex.frontCasted<int>())
            {
                return QVariant::fromValue(row + 1);
            }

            // Formatted data
            const ObjectType obj = this->containerOrFilteredContainer()[row];
            return formatter->data(role, obj.propertyByIndex(propertyIndex)).getQVariant();
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        bool CListModelBase<ObjectType, ContainerType, UseCompare>::setData(const QModelIndex &index, const QVariant &value, int role)
        {
            Qt::ItemDataRole dataRole = static_cast<Qt::ItemDataRole>(role);
            if (!(dataRole == Qt::UserRole || dataRole == Qt::EditRole)) { return false; }

            // check / init
            if (!this->isValidIndex(index)) { return false; }
            if (!this->m_columns.isEditable(index)) { return false; }
            const CDefaultFormatter *formatter = this->m_columns.getFormatter(index);
            Q_ASSERT(formatter);
            if (!formatter) { return false; }

            ObjectType obj = this->m_container[index.row()];
            ObjectType currentObject(obj);
            BlackMisc::CPropertyIndex propertyIndex = this->columnToPropertyIndex(index.column());
            obj.setPropertyByIndex(propertyIndex, value);

            if (obj != currentObject)
            {
                const QModelIndex topLeft = index.sibling(index.row(), 0);
                const QModelIndex bottomRight = index.sibling(index.row(), this->columnCount() - 1);
                this->m_container[index.row()] = obj;
                const CVariant co = CVariant::fromValue(obj);
                emit objectChanged(co, propertyIndex);
                emit this->dataChanged(topLeft, bottomRight);
                this->updateFilteredContainer();
                return true;
            }
            return false;
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        bool CListModelBase<ObjectType, ContainerType, UseCompare>::setInContainer(const QModelIndex &index, const ObjectType &obj)
        {
            if (!index.isValid()) { return false; }
            int row = index.row();
            if (row < 0 || row >= this->container().size()) { return false; }
            m_container[row] = obj;
            return true;
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        int CListModelBase<ObjectType, ContainerType, UseCompare>::update(const ContainerType &container, bool sort)
        {
            if (m_modelDestroyed) { return 0; }

            // Keep sorting out of begin/end reset model
            ContainerType sortedContainer;
            ContainerType selection;
            if (this->m_selectionModel)
            {
                selection = this->m_selectionModel->selectedObjects();
            }
            const int oldSize = this->m_container.size();
            const bool performSort = sort && container.size() > 1 && this->hasValidSortColumn();
            if (performSort)
            {
                const int sortColumn = this->getSortColumn();
                sortedContainer = this->sortContainerByColumn(container, sortColumn, this->m_sortOrder);
            }

            this->beginResetModel();
            this->m_container = performSort ? sortedContainer : container;
            this->updateFilteredContainer();
            this->endResetModel();

            if (!selection.isEmpty())
            {
                this->m_selectionModel->selectObjects(selection);
            }

            const int newSize = this->m_container.size();
            Q_UNUSED(oldSize);
            // I have to update even with same size because I cannot tell what/if data are changed
            this->emitModelDataChanged();
            return newSize;
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::update(const QModelIndex &index, const ObjectType &object)
        {
            if (m_modelDestroyed) { return; }
            if (index.row() >= this->m_container.size()) { return; }
            this->m_container[index.row()] = object;

            QModelIndex i1 = index.sibling(index.row(), 0);
            QModelIndex i2 = index.sibling(index.row(), this->columnCount(index) - 1);
            emit this->dataChanged(i1, i2); // which range has been changed
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::update(int rowIndex, const ObjectType &object)
        {
            this->update(this->index(rowIndex, 0), object);
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        CWorker *CListModelBase<ObjectType, ContainerType, UseCompare>::updateAsync(const ContainerType &container, bool sort)
        {
            Q_UNUSED(sort);
            if (m_modelDestroyed) { return nullptr; }
            auto sortColumn = this->getSortColumn();
            auto sortOrder = this->getSortOrder();
            CWorker *worker = BlackMisc::CWorker::fromTask(this, "ModelSort", [this, container, sortColumn, sortOrder]()
            {
                return this->sortContainerByColumn(container, sortColumn, sortOrder);
            });
            worker->thenWithResult<ContainerType>(this, [this](const ContainerType & sortedContainer)
            {
                if (this->m_modelDestroyed) { return;  }
                this->update(sortedContainer, false);
            });
            worker->then(this, &CListModelBase::asyncUpdateFinished);
            return worker;
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::updateContainerMaybeAsync(const ContainerType &container, bool sort)
        {
            if (m_modelDestroyed) { return; }
            if (container.size() > asyncThreshold && sort)
            {
                // larger container with sorting
                updateAsync(container, sort);
            }
            else
            {
                update(container, sort);
            }
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        bool CListModelBase<ObjectType, ContainerType, UseCompare>::hasFilter() const
        {
            const bool f =  m_filter && m_filter->isValid();
            return f;
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::removeFilter()
        {
            if (!this->hasFilter()) { return; }
            this->m_filter.reset(nullptr);
            this->beginResetModel();
            this->updateFilteredContainer();
            this->endResetModel();
            this->emitModelDataChanged();
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::takeFilterOwnership(std::unique_ptr<IModelFilter<ContainerType> > &filter)
        {
            if (!filter)
            {
                this->removeFilter(); // clear filter
                return;
            }
            if (filter->isValid())
            {
                this->m_filter = std::move(filter);
                this->beginResetModel();
                this->updateFilteredContainer();
                this->endResetModel();
                this->emitModelDataChanged();
            }
            else
            {
                // invalid filter, so clear filter
                this->removeFilter();
            }
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        const ObjectType &CListModelBase<ObjectType, ContainerType, UseCompare>::at(const QModelIndex &index) const
        {
            if (index.row() < 0 || index.row() >= this->rowCount())
            {
                static const ObjectType def {}; // default object
                return def;
            }
            else
            {
                return this->containerOrFilteredContainer()[index.row()];
            }
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        const ContainerType &CListModelBase<ObjectType, ContainerType, UseCompare>::container() const
        {
            return this->m_container;
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        const ContainerType &CListModelBase<ObjectType, ContainerType, UseCompare>::containerFiltered() const
        {
            return this->m_containerFiltered;
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        const ContainerType &CListModelBase<ObjectType, ContainerType, UseCompare>::containerOrFilteredContainer() const
        {
            if (this->hasFilter())
            {
                return this->m_containerFiltered;
            }
            else
            {
                return this->m_container;
            }
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::push_back(const ObjectType &object)
        {
            beginInsertRows(QModelIndex(), this->m_container.size(), this->m_container.size());
            this->m_container.push_back(object);
            endInsertRows();
            this->updateFilteredContainer();
            this->emitModelDataChanged();
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::insert(const ObjectType &object)
        {
            beginInsertRows(QModelIndex(), 0, 0);
            this->m_container.insert(this->m_container.begin(), object);
            endInsertRows();

            if (this->hasFilter())
            {
                this->beginResetModel();
                this->updateFilteredContainer();
                this->endResetModel();
            }
            this->emitModelDataChanged();
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::insert(const ContainerType &container)
        {
            if (container.isEmpty()) { return; }
            beginInsertRows(QModelIndex(), 0, 0);
            this->m_container.insert(container);
            endInsertRows();

            if (this->hasFilter())
            {
                this->beginResetModel();
                this->updateFilteredContainer();
                this->endResetModel();
            }
            this->emitModelDataChanged();
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::remove(const ObjectType &object)
        {
            int oldSize = this->m_container.size();
            beginRemoveRows(QModelIndex(), 0, 0);
            this->m_container.remove(object);
            endRemoveRows();
            int newSize = this->m_container.size();
            if (oldSize != newSize)
            {
                this->emitModelDataChanged();
                if (this->hasFilter())
                {
                    this->beginResetModel();
                    this->updateFilteredContainer();
                    this->endResetModel();
                }
            }
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::clear()
        {
            beginResetModel();
            this->m_container.clear();
            this->m_containerFiltered.clear();
            endResetModel();
            this->emitModelDataChanged();
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        bool CListModelBase<ObjectType, ContainerType, UseCompare>::isEmpty() const
        {
            return this->m_container.isEmpty();
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::updateFilteredContainer()
        {
            if (this->hasFilter())
            {
                this->m_containerFiltered = this->m_filter->filter(this->m_container);
            }
            else
            {
                this->m_containerFiltered.clear();
            }
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::emitModelDataChanged()
        {
            const int n = this->containerOrFilteredContainer().size();
            emit this->modelDataChanged(n, this->hasFilter());
            emit this->changed();
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::ps_onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
        {
            // underlying base class changed
            Q_UNUSED(topLeft);
            Q_UNUSED(bottomRight);
            Q_UNUSED(roles);
            this->emitModelDataChanged();
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::ps_onChangedDigest()
        {
            const int n = this->containerOrFilteredContainer().size();
            emit this->changedDigest();
            emit this->modelDataChangedDigest(n, this->hasFilter());
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::moveItems(const ContainerType &items, int position)
        {
            // overridden in specialized class
            Q_UNUSED(items);
            Q_UNUSED(position);
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::sort()
        {
            this->sort(this->getSortColumn(), this->getSortOrder());
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::sort(int column, Qt::SortOrder order)
        {
            if (column == this->m_sortColumn && order == this->m_sortOrder) { return; }

            // new order
            this->m_sortColumn = column;
            this->m_sortOrder  = order;
            if (this->m_container.size() < 2)
            {
                return; // nothing to do
            }

            // sort the values
            this->updateContainerMaybeAsync(this->m_container, true);
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::truncate(int maxNumber, bool forceSort)
        {
            if (this->rowCount() <= maxNumber) { return; }
            if (forceSort)
            {
                this->sort();    // make sure container is sorted
            }
            ContainerType container(this->container());
            container.truncate(maxNumber);
            this->updateContainerMaybeAsync(container, false);
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        ContainerType CListModelBase<ObjectType, ContainerType, UseCompare>::sortContainerByColumn(const ContainerType &container, int column, Qt::SortOrder order) const
        {
            if (m_modelDestroyed) { return container; }
            if (container.size() < 2 || !this->m_columns.isSortable(column))
            {
                return container;    // nothing to do
            }

            // this is the only part not really thread safe, but columns do not change so far
            BlackMisc::CPropertyIndex propertyIndex = this->m_columns.columnToSortPropertyIndex(column);
            Q_ASSERT(!propertyIndex.isEmpty());
            if (propertyIndex.isEmpty())
            {
                return container;    // at release build do nothing
            }

            // sort the values
            std::integral_constant<bool, UseCompare> marker {};
            const auto p = [ = ](const ObjectType & a, const ObjectType & b) -> bool
            {
                return Private::compareForModelSort<ObjectType>(a, b, order, propertyIndex, marker);
            };

            return container.sorted(p);
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        QMimeData *CListModelBase<ObjectType, ContainerType, UseCompare>::mimeData(const QModelIndexList &indexes) const
        {
            QMimeData *mimeData = new QMimeData();
            if (indexes.isEmpty()) { return mimeData; }

            ContainerType container;
            QList<int> rows; // avoid redundant objects

            // Indexes are per row and column
            for (const QModelIndex &index : indexes)
            {
                if (!index.isValid()) { continue; }
                int r = index.row();
                if (rows.contains(r)) { continue; }
                container.push_back(this->at(index));
                rows.append(r);
            }

            // to JSON via CVariant
            const QJsonDocument containerJson(CVariant::fromValue(container).toJson());
            const QString jsonString(containerJson.toJson(QJsonDocument::Compact));

            mimeData->setData(CGuiUtility::swiftJsonDragAndDropMimeType(), jsonString.toUtf8());
            return mimeData;
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        QJsonObject CListModelBase<ObjectType, ContainerType, UseCompare>::toJson() const
        {
            return container().toJson();
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        QString CListModelBase<ObjectType, ContainerType, UseCompare>::toJsonString(QJsonDocument::JsonFormat format) const
        {
            return container().toJsonString(format);
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        bool CListModelBase<ObjectType, ContainerType, UseCompare>::isOrderable() const
        {
            return false;
        }

        // see here for the reason of thess forward instantiations
        // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
        template class CListModelBase<BlackMisc::Aviation::CLivery, BlackMisc::Aviation::CLiveryList, true>;
        template class CListModelBase<BlackMisc::CIdentifier, BlackMisc::CIdentifierList, false>;
        template class CListModelBase<BlackMisc::CStatusMessage, BlackMisc::CStatusMessageList, true>;
        template class CListModelBase<BlackMisc::CNameVariantPair, BlackMisc::CNameVariantPairList, false>;
        template class CListModelBase<BlackMisc::CCountry, BlackMisc::CCountryList, true>;
        template class CListModelBase<BlackMisc::Aviation::CAtcStation, BlackMisc::Aviation::CAtcStationList, true>;
        template class CListModelBase<BlackMisc::Aviation::CAirport, BlackMisc::Aviation::CAirportList, true>;
        template class CListModelBase<BlackMisc::Aviation::CAircraftIcaoCode, BlackMisc::Aviation::CAircraftIcaoCodeList, true>;
        template class CListModelBase<BlackMisc::Aviation::CAirlineIcaoCode, BlackMisc::Aviation::CAirlineIcaoCodeList, true>;
        template class CListModelBase<BlackMisc::Network::CServer, BlackMisc::Network::CServerList, false>;
        template class CListModelBase<BlackMisc::Network::CUser, BlackMisc::Network::CUserList, true>;
        template class CListModelBase<BlackMisc::Network::CTextMessage, BlackMisc::Network::CTextMessageList, false>;
        template class CListModelBase<BlackMisc::Network::CClient, BlackMisc::Network::CClientList, false>;
        template class CListModelBase<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList, true>;
        template class CListModelBase<BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::Simulation::CSimulatedAircraftList, true>;
        template class CListModelBase<BlackMisc::Simulation::CDistributor, BlackMisc::Simulation::CDistributorList, true>;
        template class CListModelBase<BlackMisc::Weather::CCloudLayer, BlackMisc::Weather::CCloudLayerList, false>;
        template class CListModelBase<BlackMisc::Weather::CTemperatureLayer, BlackMisc::Weather::CTemperatureLayerList, false>;
        template class CListModelBase<BlackMisc::Weather::CWindLayer, BlackMisc::Weather::CWindLayerList, false>;

    } // namespace
} // namespace
