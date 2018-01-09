#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/string.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/gbsizer.h>
#include <wx/tglbtn.h>

#include "LogbookDialog.h"
#include "Logbook.h"
#include "FastAccessDialog.h"



void sailsCallback(wxAnyButton *btn, LogbookDialog* logbookDialog) {
	if(btn->IsKindOf(wxCLASSINFO(wxToggleButton))) {
		wxToggleButton* button = static_cast<wxToggleButton*>(btn);
		Logbook* logbook = logbookDialog->logbook;

		if(button->GetValue()) {
			// we just set sail
			logbook->oldSailsState = 0;
			logbook->sailsState = 1;
		} else {
			// we just took the sails down
			logbook->oldSailsState = 1;
			logbook->sailsState = 0;
		}
		logbook->sailsMessage = true;
		logbook->opt->engineMessageSails = true;
		logbook->appendRow(true, false);
	}
}

void engineCallback(wxAnyButton *btn, LogbookDialog* logbook) {
	if(btn->IsKindOf(wxCLASSINFO(wxToggleButton))) {
		wxToggleButton* button = static_cast<wxToggleButton*>(btn);

		if(button->GetValue()) {
			// we just started the engine
			logbook->startEngine1( true, true, true );
		} else {
			// we just stopped the engine
			logbook->stopEngine1( true, true );
		}
	}
}

void LogbookDialog::OnClickButtonFastAccessDialog( wxCommandEvent& event ) {

	if(NULL == m_fastAccessDialog) {
        m_fastAccessDialog = new FastAccessDialog(this, wxID_ANY, _( "Log Event" ), wxDefaultPosition, wxSize(250, 700), wxCAPTION | wxSTAY_ON_TOP | wxRESIZE_BORDER );
        m_fastAccessDialog->AddButton(_("Sails"), true, sailsCallback);
        m_fastAccessDialog->AddButton(_("Engine"), true, engineCallback);
	}

    if(m_bpButtonFastAccessDialog->GetValue())  {
        m_fastAccessDialog->Show();
    } else {
        m_fastAccessDialog->Show(false);
    }
}

FastAccessDialog::FastAccessDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->logbookDialog = static_cast<LogbookDialog*>(parent);

	wxBoxSizer* bSizer50;
	bSizer50 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer53;
	bSizer53 = new wxBoxSizer( wxVERTICAL );

	container = new wxGridSizer( 0, 1, 4, 0 );

	bSizer53->Add( container, 1, wxEXPAND, 5 );


	bSizer50->Add( bSizer53, 5, wxALL|wxEXPAND, 5 );


	this->SetSizer( bSizer50 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( FastAccessDialog::FastAccessDialogOnInitDialog ) );
}

FastAccessDialog::~FastAccessDialog()
{
	// Disconnect Events
	this->Disconnect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( FastAccessDialog::FastAccessDialogOnInitDialog ) );
	wxSizerItemList list = container->GetChildren();

	for(size_t i = 0; i < list.GetCount(); i++) {
		wxSizerItem* sizeritem = list.Item(i)->GetData();

		wxWindow* button = sizeritem->GetWindow();

		if(button->IsKindOf(wxCLASSINFO(wxToggleButton)))
			button->Disconnect(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( FastAccessDialog::buttonCallbackHub ), NULL, this );
		else
			button->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FastAccessDialog::buttonCallbackHub ), NULL, this );
	}
}

class Transfer : public wxObject {
public:
	wxAnyButton* button;
	void (*callback)(wxAnyButton* button, LogbookDialog* logbookDialog);
	LogbookDialog* logbookDialog;
};

void FastAccessDialog::buttonCallbackHub( wxCommandEvent& event ) {

	Transfer* data = static_cast<Transfer*>(event.GetEventUserData());
	data->callback(data->button, data->logbookDialog);

}

void FastAccessDialog::AddButton(const wxString& title, bool toggleButton, void (*callback)(wxAnyButton* button, LogbookDialog* logbookDialog)) {
	Transfer* data = new Transfer();
	data->callback = callback;
	data->logbookDialog = this->logbookDialog;

	if(toggleButton)
	{
		data->button = new wxToggleButton(this, wxID_ANY, title);
		data->button->Connect(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( FastAccessDialog::buttonCallbackHub ), data, this);
	}
	else
	{
		data->button = new wxButton(this, wxID_ANY, title);
		data->button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FastAccessDialog::buttonCallbackHub ), data, this);
	}

	container->Add( data->button, 0, wxALL|wxEXPAND, 5);

	this->Layout();
}
