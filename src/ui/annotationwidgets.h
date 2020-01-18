/***************************************************************************
 *   Copyright (C) 2006 by Pino Toscano <pino@kde.org>                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _ANNOTATIONWIDGETS_H_
#define _ANNOTATIONWIDGETS_H_

#include <qwidget.h>

#include "core/annotations.h"

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QWidget;
class KColorButton;
class KFontRequester;
class AnnotationWidget;

class PixmapPreviewSelector
  : public QWidget
{
    Q_OBJECT

public:
    explicit PixmapPreviewSelector( QWidget * parent = 0 );
    virtual ~PixmapPreviewSelector();

    void setIcon( const QString& icon );
    QString icon() const;

    void addItem( const QString& item, const QString& id );

    void setPreviewSize( int size );
    int previewSize() const;

    void setEditable( bool editable );

signals:
    void iconChanged( const QString& );

private slots:
    void iconComboChanged( const QString& icon );

private:
    QString m_icon;
    QLabel * m_iconLabel;
    QComboBox * m_comboItems;
    int m_previewSize;
};


/**
 * A factory to create AnnotationWidget's.
 */
class AnnotationWidgetFactory
{
public:
    static AnnotationWidget * widgetFor( Okular::Annotation * ann );
};

class AnnotationWidget
  : public QObject
{
    Q_OBJECT

public:
    virtual ~AnnotationWidget();

    virtual Okular::Annotation::SubType annotationType() const;

    QWidget * styleWidget();
    QWidget * extraWidget();

    virtual void applyChanges() = 0;

signals:
    void dataChanged();

protected:
    AnnotationWidget( Okular::Annotation * ann );

    virtual QWidget * createStyleWidget() = 0;
    virtual QWidget * createExtraWidget();

    Okular::Annotation * m_ann;
    QWidget * m_styleWidget;
    QWidget * m_extraWidget;
};

class TextAnnotationWidget
  : public AnnotationWidget
{
    Q_OBJECT

public:
    TextAnnotationWidget( Okular::Annotation * ann );

    virtual void applyChanges();

protected:
    virtual QWidget * createStyleWidget();

private:
    Okular::TextAnnotation * m_textAnn;
    PixmapPreviewSelector * m_pixmapSelector;
    KFontRequester * m_fontReq;
};

class StampAnnotationWidget
  : public AnnotationWidget
{
    Q_OBJECT

public:
    StampAnnotationWidget( Okular::Annotation * ann );

    virtual void applyChanges();

protected:
    virtual QWidget * createStyleWidget();

private:
    Okular::StampAnnotation * m_stampAnn;
    PixmapPreviewSelector * m_pixmapSelector;
};

class LineAnnotationWidget
  : public AnnotationWidget
{
    Q_OBJECT

public:
    LineAnnotationWidget( Okular::Annotation * ann );

    virtual void applyChanges();

protected:
    virtual QWidget * createStyleWidget();

private:
    Okular::LineAnnotation * m_lineAnn;
    int m_lineType;
    QDoubleSpinBox * m_spinLL;
    QDoubleSpinBox * m_spinLLE;
    QDoubleSpinBox * m_spinSize;
};

class HighlightAnnotationWidget
  : public AnnotationWidget
{
    Q_OBJECT

public:
    HighlightAnnotationWidget( Okular::Annotation * ann );

    virtual void applyChanges();

protected:
    virtual QWidget * createStyleWidget();

private:
    Okular::HighlightAnnotation * m_hlAnn;
    QComboBox * m_typeCombo;
};

class GeomAnnotationWidget
  : public AnnotationWidget
{
    Q_OBJECT

public:
    GeomAnnotationWidget( Okular::Annotation * ann );

    virtual void applyChanges();

protected:
    virtual QWidget * createStyleWidget();

private:
    Okular::GeomAnnotation * m_geomAnn;
    QComboBox * m_typeCombo;
    QCheckBox * m_useColor;
    KColorButton * m_innerColor;
    QDoubleSpinBox * m_spinSize;
};

class FileAttachmentAnnotationWidget
  : public AnnotationWidget
{
    Q_OBJECT

public:
    FileAttachmentAnnotationWidget( Okular::Annotation * ann );

    virtual void applyChanges();

protected:
    virtual QWidget * createStyleWidget();
    virtual QWidget * createExtraWidget();

private:
    Okular::FileAttachmentAnnotation * m_attachAnn;
    PixmapPreviewSelector * m_pixmapSelector;
};

class CaretAnnotationWidget
  : public AnnotationWidget
{
    Q_OBJECT

public:
    CaretAnnotationWidget( Okular::Annotation * ann );

    virtual void applyChanges();

protected:
    virtual QWidget * createStyleWidget();

private:
    Okular::CaretAnnotation * m_caretAnn;
    PixmapPreviewSelector * m_pixmapSelector;
};

#endif
