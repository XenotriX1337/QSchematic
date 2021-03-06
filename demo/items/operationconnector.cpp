#include <QPainter>
#include <QRectF>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include "../qschematic/items/label.h"
#include "../qschematic/scene.h"
#include "../qschematic/commands/commanditemremove.h"
#include "../qschematic/commands/commanditemvisibility.h"
#include "../qschematic/commands/commandlabelrename.h"
#include "operationconnector.h"
#include "operation.h"
#include "itemtypes.h"

#define SIZE (_settings.gridSize/2)
#define RECT (QRectF(-SIZE, -SIZE, 2*SIZE, 2*SIZE))

const QColor COLOR_BODY_FILL   = QColor(Qt::white);
const QColor COLOR_BODY_BORDER = QColor(Qt::black);
const qreal PEN_WIDTH          = 1.5;

OperationConnector::OperationConnector(const QPoint& gridPoint, const QString& text, QGraphicsItem* parent) :
    QSchematic::Connector(::ItemType::OperationConnectorType, gridPoint, text, parent)
{
    label()->setVisible(true);
    setForceTextDirection(false);
}

Gpds::Container OperationConnector::toContainer() const
{
    // Root
    Gpds::Container root;
    addItemTypeIdToContainer(root);
    root.addValue("connector", QSchematic::Connector::toContainer());

    return root;
}

void OperationConnector::fromContainer(const Gpds::Container& container)
{
    // Root
    QSchematic::Connector::fromContainer( *container.getValue<Gpds::Container*>( "connector" ) );
}

std::shared_ptr<QSchematic::Item> OperationConnector::deepCopy() const
{
    auto clone = std::make_shared<OperationConnector>(gridPos(), text(), parentItem());
    copyAttributes(*(clone.get()));

    return clone;
}

void OperationConnector::copyAttributes(OperationConnector& dest) const
{
    QSchematic::Connector::copyAttributes(dest);
}

QRectF OperationConnector::boundingRect() const
{
    qreal adj = 1.5;

    return RECT.adjusted(-adj, -adj, adj, adj);
}

void OperationConnector::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    // Draw the bounding rect if debug mode is enabled
    if (_settings.debug) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(Qt::red));
        painter->drawRect(boundingRect());
    }

    // Body pen
    QPen bodyPen;
    bodyPen.setWidthF(PEN_WIDTH);
    bodyPen.setStyle(Qt::SolidLine);
    bodyPen.setColor(COLOR_BODY_BORDER);

    // Body brush
    QBrush bodyBrush;
    bodyBrush.setStyle(Qt::SolidPattern);
    bodyBrush.setColor(COLOR_BODY_FILL);

    // Draw the component body
    painter->setPen(bodyPen);
    painter->setBrush(bodyBrush);
    painter->drawEllipse(RECT);
}

void OperationConnector::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    // Create the menu
    QMenu menu;
    {
        // Label visibility
        QAction* labelVisibility = new QAction;
        labelVisibility->setCheckable(true);
        labelVisibility->setChecked(label()->isVisible());
        labelVisibility->setText("Label visible");
        connect(labelVisibility, &QAction::toggled, [this](bool enabled) {
            if (scene()) {
                scene()->undoStack()->push(new QSchematic::CommandItemVisibility(label(), enabled));
            } else {
                label()->setVisible(enabled);
            }
        });

        // Text
        QAction* text = new QAction;
        text->setText("Rename ...");
        connect(text, &QAction::triggered, [this] {
            const QString& newText = QInputDialog::getText(nullptr, "Rename Connector", "New connector text", QLineEdit::Normal, label()->text());

            if (scene()) {
                scene()->undoStack()->push(new QSchematic::CommandLabelRename(label().get(), newText));
            } else {
                label()->setText(newText);
            }
        });

        // Align label
        QAction* alignLabel = new QAction;
        alignLabel->setText("Align Label");
        connect(alignLabel, &QAction::triggered, [this] {
            this->alignLabel();
        });

        // Delete
        QAction* deleteFromModel = new QAction;
        deleteFromModel->setText("Delete");
        connect(deleteFromModel, &QAction::triggered, [this] {
            if (scene()) {
                // Retrieve smart pointer
                std::shared_ptr<QSchematic::Item> itemPointer;
                {
                    // Retrieve parent (Operation)
                    const Operation* operation = qgraphicsitem_cast<const Operation*>(parentItem());
                    if (!operation) {
                        return;
                    }

                    // Retrieve connector
                    for (auto& i : operation->connectors()) {
                        if (i.get() == this) {
                            itemPointer = i;
                            break;
                        }
                    }
                    if (!itemPointer) {
                        return;
                    }
                }

                // Issue command
                scene()->undoStack()->push(new QSchematic::CommandItemRemove(scene(), itemPointer));
            }
        });

        // Assemble
        menu.addAction(labelVisibility);
        menu.addAction(text);
        menu.addAction(alignLabel);
        menu.addAction(deleteFromModel);
    }

    // Sow the menu
    menu.exec(event->screenPos());
}
