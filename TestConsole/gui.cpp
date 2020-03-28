///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "gui.h"

///////////////////////////////////////////////////////////////////////////

MainFrame::MainFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	Button1 = new wxBitmapButton( this, wxID_ANY, wxBitmap( wxT("2.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer1->Add( Button1, 0, wxALL, 5 );
	
	Button2 = new wxBitmapButton( this, wxID_ANY, wxBitmap( wxT("2.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer1->Add( Button2, 0, wxALL, 5 );
	
	Button3 = new wxBitmapButton( this, wxID_ANY, wxBitmap( wxT("2.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer1->Add( Button3, 0, wxALL, 5 );
	
	Button4 = new wxBitmapButton( this, wxID_ANY, wxBitmap( wxT("2.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer1->Add( Button4, 0, wxALL, 5 );
	
	
	mainSizer->Add( bSizer1, 1, wxEXPAND, 0 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	Button5 = new wxBitmapButton( this, wxID_ANY, wxBitmap( wxT("2.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer2->Add( Button5, 0, wxALL, 5 );
	
	Button6 = new wxBitmapButton( this, wxID_ANY, wxBitmap( wxT("2.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer2->Add( Button6, 0, wxALL, 5 );
	
	Button7 = new wxBitmapButton( this, wxID_ANY, wxBitmap( wxT("2.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer2->Add( Button7, 0, wxALL, 5 );
	
	Button8 = new wxBitmapButton( this, wxID_ANY, wxBitmap( wxT("2.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer2->Add( Button8, 0, wxALL, 5 );
	
	
	mainSizer->Add( bSizer2, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
}

MainFrame::~MainFrame()
{
}
