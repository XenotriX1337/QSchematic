#pragma once

#include "../../../lib/items/node.h"

class FlowStart : public QSchematic::Node
{
    Q_OBJECT
    Q_DISABLE_COPY(FlowStart)

public:
    FlowStart();
    virtual ~FlowStart() override = default;

    virtual Gds::Container toContainer() const override;
    virtual void fromContainer(const Gds::Container& container) override;
    virtual std::unique_ptr<Item> deepCopy() const override;
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

protected:
    void copyAttributes(FlowStart& dest) const;

private:
    QPolygon _symbolPolygon;
};
