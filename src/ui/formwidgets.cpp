/***************************************************************************
 *   Copyright (C) 2007 by Pino Toscano <pino@kde.org>                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "formwidgets.h"

#include <qbuttongroup.h>
#include <klineedit.h>
#include <klocale.h>

// local includes
#include "core/form.h"

FormWidgetsController::FormWidgetsController( QObject *parent )
    : QObject( parent )
{
}

FormWidgetsController::~FormWidgetsController()
{
}

void FormWidgetsController::signalChanged( FormWidgetIface *w )
{
    emit changed( w );
}

void FormWidgetsController::signalAction( Okular::Action *a )
{
    emit action( a );
}

QButtonGroup* FormWidgetsController::registerRadioButton( FormWidgetIface* widget, const QList< int >& siblings )
{
    if ( !widget->button() )
        return 0;

    QList< RadioData >::iterator it = m_radios.begin(), itEnd = m_radios.end();
    const int id = widget->formField()->id();
    for ( ; it != itEnd; ++it )
    {
        const QList< int >::const_iterator idsIt = qFind( (*it).ids, id );
        if ( idsIt != (*it).ids.constEnd() )
        {
            (*it).group->addButton( widget->button() );
            return (*it).group;
        }
    }
    RadioData newdata;
    newdata.ids = siblings;
    newdata.ids.append( id );
    newdata.group = new QButtonGroup();
    newdata.group->addButton( widget->button() );
    connect( newdata.group, SIGNAL(buttonClicked(QAbstractButton*)),
             this, SLOT(slotButtonClicked(QAbstractButton*)) );
    m_radios.append( newdata );
    return newdata.group;
}

void FormWidgetsController::dropRadioButtons()
{
    QList< RadioData >::iterator it = m_radios.begin(), itEnd = m_radios.end();
    for ( ; it != itEnd; ++it )
    {
        delete (*it).group;
    }
    m_radios.clear();
}

void FormWidgetsController::slotButtonClicked( QAbstractButton *button )
{
    if ( CheckBoxEdit *check = qobject_cast< CheckBoxEdit * >( button ) )
        emit changed( check );
    else if ( RadioButtonEdit *radio = qobject_cast< RadioButtonEdit * >( button ) )
        emit changed( radio );
}


FormWidgetIface * FormWidgetFactory::createWidget( Okular::FormField * ff, QWidget * parent )
{
    FormWidgetIface * widget = 0;
    switch ( ff->type() )
    {
        case Okular::FormField::FormButton:
        {
            Okular::FormFieldButton * ffb = static_cast< Okular::FormFieldButton * >( ff );
            switch ( ffb->buttonType() )
            {
                case Okular::FormFieldButton::Push:
                    widget = new PushButtonEdit( ffb, parent );
                    break;
                case Okular::FormFieldButton::CheckBox:
                    widget = new CheckBoxEdit( ffb, parent );
                    break;
                case Okular::FormFieldButton::Radio:
                    widget = new RadioButtonEdit( ffb, parent );
                    break;
                default: ;
            }
            break;
        }
        case Okular::FormField::FormText:
        {
            Okular::FormFieldText * fft = static_cast< Okular::FormFieldText * >( ff );
            switch ( fft->textType() )
            {
                case Okular::FormFieldText::Multiline:
                    widget = new TextAreaEdit( fft, parent );
                    break;
                case Okular::FormFieldText::Normal:
                    widget = new FormLineEdit( fft, parent );
                    break;
                case Okular::FormFieldText::FileSelect:
                    widget = new FileEdit( fft, parent );
                    break;
            }
            break;
        }
        case Okular::FormField::FormChoice:
        {
            Okular::FormFieldChoice * ffc = static_cast< Okular::FormFieldChoice * >( ff );
            switch ( ffc->choiceType() )
            {
                case Okular::FormFieldChoice::ListBox:
                    widget = new ListEdit( ffc, parent );
                    break;
                case Okular::FormFieldChoice::ComboBox:
                    widget = new ComboEdit( ffc, parent );
                    break;
            }
            break;
        }
        default: ;
    }
    return widget;
}


FormWidgetIface::FormWidgetIface( QWidget * w, Okular::FormField * ff )
    : m_controller( 0 ), m_widget( w ), m_ff( ff ), m_pageItem( 0 )
{
}

FormWidgetIface::~FormWidgetIface()
{
}

Okular::NormalizedRect FormWidgetIface::rect() const
{
    return m_ff->rect();
}

void FormWidgetIface::setWidthHeight( int w, int h )
{
    m_widget->resize( w, h );
}

void FormWidgetIface::moveTo( int x, int y )
{
    m_widget->move( x, y );
}

bool FormWidgetIface::setVisibility( bool visible )
{
    if ( !m_ff->isVisible() )
        return false;

    bool hadfocus = m_widget->hasFocus();
    if ( hadfocus )
        m_widget->clearFocus();
    m_widget->setVisible( visible );
    return hadfocus;
}

void FormWidgetIface::setCanBeFilled( bool fill )
{
    if ( m_widget->isEnabled() )
    {
        m_widget->setEnabled( fill );
    }
}

void FormWidgetIface::setPageItem( PageViewItem *pageItem )
{
    m_pageItem = pageItem;
}

Okular::FormField* FormWidgetIface::formField() const
{
    return m_ff;
}

PageViewItem* FormWidgetIface::pageItem() const
{
    return m_pageItem;
}

void FormWidgetIface::setFormWidgetsController( FormWidgetsController *controller )
{
    m_controller = controller;
}

QAbstractButton* FormWidgetIface::button()
{
    return 0;
}


PushButtonEdit::PushButtonEdit( Okular::FormFieldButton * button, QWidget * parent )
    : QPushButton( parent ), FormWidgetIface( this, button ), m_form( button )
{
    setText( m_form->caption() );
    setEnabled( !m_form->isReadOnly() );
    setVisible( m_form->isVisible() );
    setCursor( Qt::ArrowCursor );

    connect( this, SIGNAL(clicked()), this, SLOT(slotClicked()) );
}

void PushButtonEdit::slotClicked()
{
    if ( m_form->activationAction() )
        m_controller->signalAction( m_form->activationAction() );
}


CheckBoxEdit::CheckBoxEdit( Okular::FormFieldButton * button, QWidget * parent )
    : QCheckBox( parent ), FormWidgetIface( this, button ), m_form( button )
{
    setText( m_form->caption() );
    setEnabled( !m_form->isReadOnly() );

    setVisible( m_form->isVisible() );
    setCursor( Qt::ArrowCursor );
}

void CheckBoxEdit::setFormWidgetsController( FormWidgetsController *controller )
{
    FormWidgetIface::setFormWidgetsController( controller );

    const QList< int > siblings = m_form->siblings();
    if ( !siblings.isEmpty() )
        m_controller->registerRadioButton( this, siblings );

    setCheckState( m_form->state() ? Qt::Checked : Qt::Unchecked );

    connect( this, SIGNAL(stateChanged(int)), this, SLOT(slotStateChanged(int)) );
}

QAbstractButton* CheckBoxEdit::button()
{
    return this;
}

void CheckBoxEdit::slotStateChanged( int state )
{
    m_form->setState( state == Qt::Checked );

    if ( !group() )
        m_controller->signalChanged( this );
    
    if ( state == Qt::Checked && m_form->activationAction() )
        m_controller->signalAction( m_form->activationAction() );
}


RadioButtonEdit::RadioButtonEdit( Okular::FormFieldButton * button, QWidget * parent )
    : QRadioButton( parent ), FormWidgetIface( this, button ), m_form( button )
{
    setText( m_form->caption() );
    setEnabled( !m_form->isReadOnly() );

    setVisible( m_form->isVisible() );
    setCursor( Qt::ArrowCursor );
}

void RadioButtonEdit::setFormWidgetsController( FormWidgetsController *controller )
{
    FormWidgetIface::setFormWidgetsController( controller );

    m_controller->registerRadioButton( this, m_form->siblings() );

    setChecked( m_form->state() );

    connect( this, SIGNAL(toggled(bool)), this, SLOT(slotToggled(bool)) );
}

QAbstractButton* RadioButtonEdit::button()
{
    return this;
}

void RadioButtonEdit::slotToggled( bool checked )
{
    m_form->setState( checked );

    if ( !group() )
        m_controller->signalChanged( this );
}


FormLineEdit::FormLineEdit( Okular::FormFieldText * text, QWidget * parent )
    : QLineEdit( parent ), FormWidgetIface( this, text ), m_form( text )
{
    int maxlen = m_form->maximumLength();
    if ( maxlen >= 0 )
        setMaxLength( maxlen );
    setAlignment( m_form->textAlignment() );
    setText( m_form->text() );
    if ( m_form->isPassword() )
        setEchoMode( QLineEdit::Password );
    setReadOnly( m_form->isReadOnly() );

    connect( this, SIGNAL(textEdited(QString)), this, SLOT(textEdited(QString)) );
    setVisible( m_form->isVisible() );
}

void FormLineEdit::textEdited( const QString& )
{
    m_form->setText( text() );

    m_controller->signalChanged( this );
}


TextAreaEdit::TextAreaEdit( Okular::FormFieldText * text, QWidget * parent )
    : KTextEdit( parent ), FormWidgetIface( this, text ), m_form( text )
{
    setAcceptRichText( m_form->isRichText() );
    setCheckSpellingEnabled( m_form->canBeSpellChecked() );
    setAlignment( m_form->textAlignment() );
    setPlainText( m_form->text() );
    setReadOnly( m_form->isReadOnly() );

    connect( this, SIGNAL(textChanged()), this, SLOT(slotChanged()) );
    setVisible( m_form->isVisible() );
}

void TextAreaEdit::slotChanged()
{
    m_form->setText( toPlainText() );

    m_controller->signalChanged( this );
}


FileEdit::FileEdit( Okular::FormFieldText * text, QWidget * parent )
    : KUrlRequester( parent ), FormWidgetIface( this, text ), m_form( text )
{
    setMode( KFile::File | KFile::ExistingOnly | KFile::LocalOnly );
    setFilter( i18n( "*|All Files" ) );
    setUrl( KUrl( m_form->text() ) );
    lineEdit()->setAlignment( m_form->textAlignment() );
    setEnabled( !m_form->isReadOnly() );

    connect( this, SIGNAL(textChanged(QString)), this, SLOT(slotChanged(QString)) );
    setVisible( m_form->isVisible() );
}

void FileEdit::slotChanged( const QString& )
{
    m_form->setText( url().toLocalFile() );

    m_controller->signalChanged( this );
}


ListEdit::ListEdit( Okular::FormFieldChoice * choice, QWidget * parent )
    : QListWidget( parent ), FormWidgetIface( this, choice ), m_form( choice )
{
    addItems( m_form->choices() );
    setSelectionMode( m_form->multiSelect() ? QAbstractItemView::ExtendedSelection : QAbstractItemView::SingleSelection );
    setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );
    QList< int > selectedItems = m_form->currentChoices();
    if ( m_form->multiSelect() )
    {
        foreach ( int index, selectedItems )
            if ( index >= 0 && index < count() )
                item( index )->setSelected( true );
    }
    else
    {
        if ( selectedItems.count() == 1 && selectedItems.at(0) >= 0 && selectedItems.at(0) < count() )
        {
            setCurrentRow( selectedItems.at(0) );
            scrollToItem( item( selectedItems.at(0) ) );
        }
    }
    setEnabled( !m_form->isReadOnly() );

    connect( this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()) );
    setVisible( m_form->isVisible() );
    setCursor( Qt::ArrowCursor );
}

void ListEdit::slotSelectionChanged()
{
    QList< QListWidgetItem * > selection = selectedItems();
    QList< int > rows;
    foreach( const QListWidgetItem * item, selection )
        rows.append( row( item ) );
    m_form->setCurrentChoices( rows );

    m_controller->signalChanged( this );
}


ComboEdit::ComboEdit( Okular::FormFieldChoice * choice, QWidget * parent )
    : QComboBox( parent ), FormWidgetIface( this, choice ), m_form( choice )
{
    addItems( m_form->choices() );
    setEditable( true );
    setInsertPolicy( NoInsert );
    lineEdit()->setReadOnly( !m_form->isEditable() );
    QList< int > selectedItems = m_form->currentChoices();
    if ( selectedItems.count() == 1 && selectedItems.at(0) >= 0 && selectedItems.at(0) < count() )
        setCurrentIndex( selectedItems.at(0) );
    setEnabled( !m_form->isReadOnly() );

    if ( m_form->isEditable() && !m_form->editChoice().isEmpty() )
        lineEdit()->setText( m_form->editChoice() );

    connect( this, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()) );
    connect( this, SIGNAL(editTextChanged(QString)), this, SLOT(slotValueChanged()) );
    setVisible( m_form->isVisible() );
    setCursor( Qt::ArrowCursor );
}

void ComboEdit::slotValueChanged()
{
    const int index = currentIndex();
    const QString text = currentText();
    const bool isCustomValue = ( index == -1 || itemText( index ) != text );

    if ( isCustomValue )
    {
        m_form->setEditChoice( text );
    }
    else
    {
        m_form->setCurrentChoices( QList< int >() << index );
    }

    m_controller->signalChanged( this );
}


#include "formwidgets.moc"
