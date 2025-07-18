#ifndef VIBESGRAPHICSITEM_H
#define VIBESGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QJsonObject>
#include <QJsonValue>

//#include <QBitArray>
#include "vibesscene2d.h"

// VibesDefaults includes
#include <QHash>
#include <QPen>

#include <QGraphicsSimpleTextItem>

// Singleton class to hold Vibes defaults and constants
class VibesDefaults {
    QHash<QString, QBrush> _brushes;
    QHash<QString, QPen> _pens;
public:
    static VibesDefaults & instance() { return _instance; }

    const QColor parseColorName(const QString& name){
        Q_ASSERT(name.size() == 7 || name.size() == 9);
        // Suported format #RRGGBB and #RRGGBBAA
        QColor color;
        color.setNamedColor(name.mid(0,7));
        // if len of name > 7 the 2 last caracters are the alpha value
        if(name.size() > 7){
            color.setAlpha(name.mid(7,2).toUInt(0,16));
        }
        return color;
    }
    const Qt::PenStyle parsePenStyle(const QString& style){
        if(style == QString("-")) return Qt::SolidLine;
        if(style == QString("--")) return Qt::DashLine;
        if(style == QString("-.")) return Qt::DashDotLine;
        if(style == QString("-..")) return Qt::DashDotDotLine;
        if(style == QString("..")) return Qt::DotLine;
        // by Default SolidLine
        return Qt::SolidLine;
    }
    const qreal parsePenWidth(const QString& width){
        // Pen cannot have negative width
        return std::max(0.,width.toDouble());
    }

    //>[VBS_VR_000002]
    // Hexadecimal/Short color name -> Qt Predefined color name
    bool toPredefinedColorName(QString &color){

        QString colorU = color.toUpper();
        QString colorL = color.toLower();

        if     (colorL.compare("none")   ==0){ color="transparent"; }
        else if(colorU.compare("#0000FF")==0){ color="blue"; }
        else if(colorL.compare("b")      ==0){ color="blue"; }
        else if(colorU.compare("#00FFFF")==0){ color="cyan"; }
        else if(colorL.compare("c")      ==0){ color="cyan"; }
        else if(colorU.compare("#00FF00")==0){ color="green"; }
        else if(colorL.compare("g")      ==0){ color="green"; }
        else if(colorU.compare("#FFFF00")==0){ color="yellow"; }
        else if(colorL.compare("y")      ==0){ color="yellow"; }
        else if(colorU.compare("#FF0000")==0){ color="red"; }
        else if(colorL.compare("r")      ==0){ color="red"; }
        else if(colorU.compare("#FF00FF")==0){ color="magenta"; }
        else if(colorL.compare("m")      ==0){ color="magenta"; }
        else if(colorU.compare("#FFFFFF")==0){ color="white"; }
        else if(colorL.compare("w")      ==0){ color="white"; }
        else if(colorU.compare("#000000")==0){ color="black"; }
        else if(colorL.compare("k")      ==0){ color="black"; }
        else if(colorU.compare("#C0C0C0")==0){ color="lightGray"; }
        else if(colorU.compare("#A0A0A4")==0){ color="gray"; }
        else if(colorU.compare("#808080")==0){ color="darkGray"; }
        else if(colorU.compare("#000080")==0){ color="darkBlue"; }
        else if(colorU.compare("#008080")==0){ color="darkCyan"; }
        else if(colorU.compare("#008000")==0){ color="darkGreen"; }
        else if(colorU.compare("#808000")==0){ color="darkYellow"; }
        else if(colorU.compare("#800000")==0){ color="darkRed"; }
        else if(colorU.compare("#800080")==0){ color="darkMagenta"; }
        else                                 { return false; }

        return true;
    }
    //<[VBS_VR_000002]

    const QBrush brush(const QString & name = QString()) {
        if( !_brushes.contains(name)){
            _brushes[name] = QBrush(parseColorName(name));
        }
        return _brushes[name];
    }

    const QPen pen(const QString & name = QString(),const QString & style = QString(),const QString & width = QString()) {
        if( !_pens.contains(name)){
            _pens[name] = QPen(parseColorName(name),0);
        }
        _pens[name].setStyle(parsePenStyle(style));
        _pens[name].setWidthF(parsePenWidth(width));
        return _pens[name];
    }

private:
    VibesDefaults();
    static VibesDefaults _instance;
    void initDefaultBrushesAndPens();
};
// Helper macro to access the VibesDefaults instance
#define vibesDefaults VibesDefaults::instance()

class VibesGraphicsItem
{
    // Pointer to the QGraphicsItem object. Used for casting.
    QGraphicsItem * _qGraphicsItem;
    QString _name;
    int _dimX, _dimY;
public:
    enum { VibesGraphicsItemType = QGraphicsItem::UserType,
           // Group of item
           VibesGraphicsGroupType,
           // Primitive types
           VibesGraphicsBoxType,
           VibesGraphicsEllipseType,
           VibesGraphicsPolygonType,
           VibesGraphicsArrowType,
           VibesGraphicsPointType,
           VibesGraphicsPieType,
           VibesGraphicsRingType,
           // Complex types based on primitive types
           VibesGraphicsVehicleType,
           VibesGraphicsVehicleAUVType,
           VibesGraphicsVehicleTankType,
           VibesGraphicsVehicleMotorBoatType,
           // List based types
           VibesGraphicsLineType,
           VibesGraphicsBoxesType,
           VibesGraphicsBoxesUnionType,
           VibesGraphicsPointsType,
           VibesGraphicsTextType,
           // Do not remove the following value! It signals the end of VibesGraphicsItem types
           VibesGraphicsLastType,
           VibesGraphicsRasterType,
           // Happy birthday
           VibesGraphicsCakeType
         };
    // Constructor
    VibesGraphicsItem(QGraphicsItem * qGraphicsItem);
    // Destructor (virtual)
    virtual ~VibesGraphicsItem() { setName(QString()); }

    bool setJson(QJsonObject json, int dimX, int dimY);
    bool setJson(QJsonObject json) { return setJson(json, _dimX, _dimY); }
    const QJsonObject & json() const { return _json; }
    QJsonValue jsonValue(const QString &key) const;
    void setJsonValue(const QString &key, const QJsonValue &value);
    void setJsonValues(const QJsonObject &values);

    bool existsInProj(int dimX, int dimY) const { return hasDim(dimX) && hasDim(dimY); }
    bool setProj(int dimX, int dimY);
    bool updateProj() { return setProj(_dimX,_dimY); }
    int dimension() const { return maxDim(); }


    QString name() const { return _name; }
    void setName(QString name) { if (name != this->name()) { _name=name; if (scene()) scene()->setItemName(this, this->name()); } }
    VibesScene2D* scene() const { if (_qGraphicsItem) return static_cast<VibesScene2D*>( _qGraphicsItem->scene() ); else return 0;}

    operator QGraphicsItem& () { Q_ASSERT(_qGraphicsItem!=0); return *_qGraphicsItem; }
    operator const QGraphicsItem& () const { Q_ASSERT(_qGraphicsItem!=0); return *_qGraphicsItem; }

    static VibesGraphicsItem *newWithType(const QString type);

protected:
    bool parseJson(QJsonObject &json);
    virtual bool parseJsonGraphics(const QJsonObject &json) = 0;
    virtual bool computeProjection(int dimX, int dimY) = 0;
    virtual bool hasDim(int n) const { return n>=0 && n<_nbDim; }
    virtual int maxDim() const { return _nbDim; }
    // Utility
    static bool isJsonMatrix(const QJsonValue json, int &nbRows, int &nbCols);
    static bool isJsonMatrix(const QJsonValue json) { int r,c; return isJsonMatrix(json, r, c); }
    // Json Properties categories
    virtual bool propertyIsReadOnly(const QString & key) { if (key=="type") return true; else return false; }
    virtual bool propertyChangesGeometry(const QString & key) { return false; }

protected:
    QJsonObject _json;
    int _nbDim;
};

// Specialization of qgraphicsitem_cast to VibesGraphicsItem* base class (uses dynamic_cast)
template<> inline VibesGraphicsItem * qgraphicsitem_cast(QGraphicsItem *item)
{
    return (item && item->type() >= VibesGraphicsItem::VibesGraphicsItemType
            && item->type() < VibesGraphicsItem::VibesGraphicsLastType) ? dynamic_cast<VibesGraphicsItem*>(item) : 0;
}

template<> inline const VibesGraphicsItem * qgraphicsitem_cast(const QGraphicsItem *item)
{
    return (item && item->type() >= VibesGraphicsItem::VibesGraphicsItemType
            && item->type() < VibesGraphicsItem::VibesGraphicsLastType) ? dynamic_cast<const VibesGraphicsItem*>(item) : 0;
}

// Provides a cast to GraphicsItem classes from VibesGraphicsItem base class, without using dynamic_cast.
template <class T> inline T vibesgraphicsitem_cast(VibesGraphicsItem *item)
{
    return item ? qgraphicsitem_cast<T>(&(static_cast<QGraphicsItem&>(*item))) : 0;
}

template <class T> inline const T vibesgraphicsitem_cast(const VibesGraphicsItem *item)
{
    return item ? qgraphicsitem_cast<T>(&(static_cast<const QGraphicsItem&>(*item))) : 0;
}


// A macro for defining the type method of VibesGraphicsItem subclasses
#define VIBES_GRAPHICS_ITEM_TYPE_DECL(class_name) \
    enum { Type = class_name ## Type }; \
    int type() const { return Type; }
// A macro for defining the constructor of VibesGraphicsItem subclasses
#define VIBES_GRAPHICS_ITEM_CTOR_DECL(class_name, base_class) \
    class_name(QGraphicsItem * parent = 0) : base_class(parent), VibesGraphicsItem(this) {} \
    class_name(const QJsonObject &json, QGraphicsItem * parent = 0) : base_class(parent), VibesGraphicsItem(this) { setJson(json); }
// A macro for defining VibesGraphicsItem subclasses
#define VIBES_GRAPHICS_ITEM(class_name, base_class) \
public: \
    VIBES_GRAPHICS_ITEM_TYPE_DECL(class_name) \
    VIBES_GRAPHICS_ITEM_CTOR_DECL(class_name, base_class)

#define VIBES_READ_ONLY_PROPERTIES(...) \
protected: \
inline bool propertyIsReadOnly(const QString& key) { \
    if (QStringList({__VA_ARGS__}).contains(key)) return true; \
    else return VibesGraphicsItem::propertyIsReadOnly(key); }
#define VIBES_GEOMETRY_CHANGING_PROPERTIES(...) \
protected: \
inline bool propertyChangesGeometry(const QString& key) { \
    if (QStringList({__VA_ARGS__}).contains(key)) return true; \
    else return VibesGraphicsItem::propertyChangesGeometry(key); }

/// A group of objects (a layer)

class VibesGraphicsGroup : public QGraphicsItemGroup, public VibesGraphicsItem
{
    VIBES_GRAPHICS_ITEM(VibesGraphicsGroup, QGraphicsItemGroup)
public:
    void addToGroup(VibesGraphicsItem *item);
    void clear();
protected:
    bool parseJsonGraphics(const QJsonObject &json);
    bool computeProjection(int dimX, int dimY) { return true; }
};

/// A box

class VibesGraphicsBox : public QGraphicsRectItem, public VibesGraphicsItem
{
    VIBES_GRAPHICS_ITEM(VibesGraphicsBox, QGraphicsRectItem)
    VIBES_GEOMETRY_CHANGING_PROPERTIES("bounds")
protected:
    bool parseJsonGraphics(const QJsonObject &json);
    bool computeProjection(int dimX, int dimY);
};

/// A set of boxes

class VibesGraphicsBoxes : public QGraphicsItemGroup, public VibesGraphicsItem
{
    VIBES_GRAPHICS_ITEM(VibesGraphicsBoxes, QGraphicsItemGroup)
    VIBES_GEOMETRY_CHANGING_PROPERTIES("bounds")
protected:
    bool parseJsonGraphics(const QJsonObject &json);
    bool computeProjection(int dimX, int dimY);
};

/// The union of a set of boxes

class VibesGraphicsBoxesUnion : public QGraphicsPathItem, public VibesGraphicsItem
{
    VIBES_GRAPHICS_ITEM(VibesGraphicsBoxesUnion, QGraphicsPathItem)
    VIBES_GEOMETRY_CHANGING_PROPERTIES("bounds")
protected:
    bool parseJsonGraphics(const QJsonObject &json);
    bool computeProjection(int dimX, int dimY);
};


/// An ellipse or confidence ellipse

class VibesGraphicsEllipse : public QGraphicsEllipseItem, public VibesGraphicsItem
{
    VIBES_GRAPHICS_ITEM(VibesGraphicsEllipse, QGraphicsEllipseItem)
    VIBES_GEOMETRY_CHANGING_PROPERTIES("center","axis","orientation","covariance","sigma")
protected:
    bool parseJsonGraphics(const QJsonObject &json);
    bool computeProjection(int dimX, int dimY);
    void axisAngleFromCovarianceK(const double &sxx, const double &syy, const double &sxy, const double &k,
                                  double &wx, double &wy, double &angle);
};


/// A line (defined by a set of points)

class VibesGraphicsLine : public QGraphicsPathItem, public VibesGraphicsItem
{
    VIBES_GRAPHICS_ITEM(VibesGraphicsLine, QGraphicsPathItem)
    VIBES_GEOMETRY_CHANGING_PROPERTIES("points")
protected:
    bool parseJsonGraphics(const QJsonObject &json);
    bool computeProjection(int dimX, int dimY);
};

/// A polygon

class VibesGraphicsPolygon : public QGraphicsPolygonItem, public VibesGraphicsItem
{
    VIBES_GRAPHICS_ITEM(VibesGraphicsPolygon, QGraphicsPolygonItem)
    VIBES_GEOMETRY_CHANGING_PROPERTIES("bounds")
protected:
    bool parseJsonGraphics(const QJsonObject &json);
    bool computeProjection(int dimX, int dimY);
};

/// A text

class VibesGraphicsText : public QGraphicsSimpleTextItem, public VibesGraphicsItem
{
    VIBES_GRAPHICS_ITEM(VibesGraphicsText, QGraphicsSimpleTextItem)
    VIBES_GEOMETRY_CHANGING_PROPERTIES("position","text")
protected:
    bool parseJsonGraphics(const QJsonObject &json);
    bool computeProjection(int dimX, int dimY);
    // void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
};

/// A simple vehicle (triangle)

class VibesGraphicsVehicle : public QGraphicsItemGroup, public VibesGraphicsItem
{
    VIBES_GRAPHICS_ITEM(VibesGraphicsVehicle, QGraphicsItemGroup)
    VIBES_GEOMETRY_CHANGING_PROPERTIES("center","length","orientation")
protected:
    bool parseJsonGraphics(const QJsonObject &json);
    bool computeProjection(int dimX, int dimY);
};

/// A submarine vehicle type AUV (torpedo)

class VibesGraphicsVehicleAUV : public QGraphicsItemGroup, public VibesGraphicsItem
{
    VIBES_GRAPHICS_ITEM(VibesGraphicsVehicleAUV, QGraphicsItemGroup)
    VIBES_GEOMETRY_CHANGING_PROPERTIES("center","length","orientation")
protected:
    bool parseJsonGraphics(const QJsonObject &json);
    bool computeProjection(int dimX, int dimY);
};

/// A tank vehicle type

class VibesGraphicsVehicleTank : public QGraphicsItemGroup, public VibesGraphicsItem
{
    VIBES_GRAPHICS_ITEM(VibesGraphicsVehicleTank, QGraphicsItemGroup)
    VIBES_GEOMETRY_CHANGING_PROPERTIES("center","length","orientation")
protected:
    bool parseJsonGraphics(const QJsonObject &json);
    bool computeProjection(int dimX, int dimY);
};

/// A tank vehicle type

class VibesGraphicsVehicleMotorBoat : public QGraphicsItemGroup, public VibesGraphicsItem
{
    VIBES_GRAPHICS_ITEM(VibesGraphicsVehicleMotorBoat, QGraphicsItemGroup)
    VIBES_GEOMETRY_CHANGING_PROPERTIES("center","length","orientation")
protected:
    bool parseJsonGraphics(const QJsonObject &json);
    bool computeProjection(int dimX, int dimY);
};

/// An arrow

class VibesGraphicsArrow : public QGraphicsItemGroup, public VibesGraphicsItem
{
    VIBES_GRAPHICS_ITEM(VibesGraphicsArrow, QGraphicsItemGroup)
    VIBES_GEOMETRY_CHANGING_PROPERTIES("points","tip_length")
protected:
    bool parseJsonGraphics(const QJsonObject &json);
    bool computeProjection(int dimX, int dimY);
};

/// A Pie

class VibesGraphicsPie : public QGraphicsItemGroup, public VibesGraphicsItem
{
    VIBES_GRAPHICS_ITEM(VibesGraphicsPie, QGraphicsItemGroup)
    VIBES_GEOMETRY_CHANGING_PROPERTIES("center","rho", "theta")
protected:
    bool parseJsonGraphics(const QJsonObject &json);
    bool computeProjection(int dimX, int dimY);
};

/// A Point
class VibesGraphicsPoint : public QGraphicsEllipseItem, public VibesGraphicsItem
{
    VIBES_GRAPHICS_ITEM(VibesGraphicsPoint, QGraphicsEllipseItem)
    VIBES_GEOMETRY_CHANGING_PROPERTIES("point");
protected:
    bool parseJsonGraphics(const QJsonObject &json);
    bool computeProjection(int dimX, int dimY);
};

/// A group of points
class VibesGraphicsPoints : public QGraphicsItemGroup, public VibesGraphicsItem
{
    VIBES_GRAPHICS_ITEM(VibesGraphicsPoints, QGraphicsItemGroup)
    VIBES_GEOMETRY_CHANGING_PROPERTIES("centers")
protected:
    bool parseJsonGraphics(const QJsonObject &json);
    bool computeProjection(int dimX, int dimY);
};

class VibesGraphicsRing : public QGraphicsItemGroup, public VibesGraphicsItem
{
    VIBES_GRAPHICS_ITEM(VibesGraphicsRing, QGraphicsItemGroup)
    VIBES_GEOMETRY_CHANGING_PROPERTIES("center","rho")
protected:
    bool parseJsonGraphics(const QJsonObject &json);
    bool computeProjection(int dimX, int dimY);
};
/// An Image Rasters VibesGraphicsRasterType
class VibesGraphicsRaster : public QGraphicsItemGroup, public VibesGraphicsItem
{
    VIBES_GRAPHICS_ITEM(VibesGraphicsRaster, QGraphicsItemGroup)
    VIBES_GEOMETRY_CHANGING_PROPERTIES("x","y")
protected:
    bool parseJsonGraphics(const QJsonObject &json);
    bool computeProjection(int dimX, int dimY);
};

class VibesGraphicsCake : public QGraphicsItemGroup, public VibesGraphicsItem
{
    VIBES_GRAPHICS_ITEM(VibesGraphicsCake, QGraphicsItemGroup)
    VIBES_GEOMETRY_CHANGING_PROPERTIES("center","length","orientation")
protected:
    bool parseJsonGraphics(const QJsonObject &json);
    bool computeProjection(int dimX, int dimY);
};

#endif // VIBESGRAPHICSITEM_H
