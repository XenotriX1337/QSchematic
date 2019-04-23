#include "../../../lib/items/label.h"
#include "operationdemo1.h"
#include "operationconnector.h"
#include "itemtypes.h"

struct ConnectorAttribute {
    QPoint point;
    QString name;
};

OperationDemo1::OperationDemo1(QGraphicsItem* parent) :
    Operation(::ItemType::OperationDemo1Type, parent)
{
    setSize(8, 8);
    label()->setText(QStringLiteral("Demo 1"));

    QVector<ConnectorAttribute> connectorAttributes = {
        { QPoint(0, 2), QStringLiteral("in 1") },
        { QPoint(0, 4), QStringLiteral("in 2") },
        { QPoint(0, 6), QStringLiteral("in 3") },
        { QPoint(8, 4), QStringLiteral("out") }
    };

    for (const auto& c : connectorAttributes) {
        auto connector = std::make_shared<OperationConnector>(c.point, c.name);
        connector->label()->setVisible(true);
        addConnector(connector);
    }
}

Gds::Container OperationDemo1::toContainer() const
{
    // Root
    Gds::Container root;
    root.addEntry("operation", Operation::toContainer());

    return root;
}

void OperationDemo1::fromContainer(const Gds::Container& container)
{
    // Root
    Operation::fromContainer( container.getEntry<Gds::Container>( "operation" ) );
}

std::unique_ptr<QSchematic::Item> OperationDemo1::deepCopy() const
{
    auto clone = std::make_unique<OperationDemo1>(parentItem());
    copyAttributes(*(clone.get()));

    return clone;
}

void OperationDemo1::copyAttributes(OperationDemo1& dest) const
{
    Operation::copyAttributes(dest);
}
