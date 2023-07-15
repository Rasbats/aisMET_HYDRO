/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  aisMET_HYDRO Plugin
 * Author:   Mike Rossiter
 *
 ***************************************************************************
 *   Copyright (C) 2017 by Mike Rossiter                                *
 *   $EMAIL$                                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************
 */

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif // precompiled headers

#include "aisMET_HYDRO_pi.h"
#include "aisMET_HYDROgui.h"
#include "aisMET_HYDROgui_impl.h"
#include "ocpn_plugin.h"


class aisMET_HYDRO_pi;
class Dlg;

#define DATABASE_NAME "RIS.db"

using namespace std;

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void* ppimgr)
{
    return new aisMET_HYDRO_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p) { delete p; }

//---------------------------------------------------------------------------------------------------------
//
//    aisMET_HYDRO PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------

#include "icons.h"

//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

aisMET_HYDRO_pi::aisMET_HYDRO_pi(void* ppimgr)
    : opencpn_plugin_117(ppimgr)
{
    // Create the PlugIn icons
    initialize_images();

    wxFileName fn;

    auto path = GetPluginDataDir("aisMET_HYDRO_pi");
    fn.SetPath(path);
    fn.AppendDir("data");
    fn.SetFullName("aisMET_HYDRO_panel_icon.png");

    path = fn.GetFullPath();

    wxInitAllImageHandlers();

    wxLogDebug(wxString("Using icon path: ") + path);
    if (!wxImage::CanRead(path)) {
        wxLogDebug("Initiating image handlers.");
        wxInitAllImageHandlers();
    }
    wxImage panelIcon(path);
    if (panelIcon.IsOk())
        m_panelBitmap = wxBitmap(panelIcon);
    else
        wxLogWarning("aisMET_HYDRO panel icon has NOT been loaded");
    m_bShowaisMET_HYDRO = false;
}

aisMET_HYDRO_pi::~aisMET_HYDRO_pi(void)
{

    delete _img_aisMET_HYDROIcon;

}

int aisMET_HYDRO_pi::Init(void)
{
    AddLocaleCatalog(_T("opencpn-aisMET_HYDRO_pi"));

	      //      Establish the location of the database file

    // Set some default private member parameters
    m_hr_dialog_x = 40;
    m_hr_dialog_y = 80;
    m_hr_dialog_sx = 400;
    m_hr_dialog_sy = 300;
    ::wxDisplaySize(&m_display_width, &m_display_height);

    //    Get a pointer to the opencpn display canvas, to use as a parent for
    //    the POI Manager dialog
    m_parent_window = GetOCPNCanvasWindow();

    //    Get a pointer to the opencpn configuration object
    m_pconfig = GetOCPNConfigObject();

    //    And load the configuration items
    LoadConfig();

    //    This PlugIn needs a toolbar icon, so request its insertion
    if (m_baisMET_HYDROShowIcon) {
#ifdef aisMET_HYDRO_USE_SVG
        m_leftclick_tool_id = InsertPlugInToolSVG(_T( "aisMET_HYDRO" ),
            _svg_aisMET_HYDRO, _svg_aisMET_HYDRO, _svg_aisMET_HYDRO_toggled,
            wxITEM_CHECK, _("aisMET_HYDRO"), _T( "" ), NULL,
            aisMET_HYDRO_TOOL_POSITION, 0, this);
#else
        m_leftclick_tool_id = InsertPlugInTool(_T(""), _img_aisMET_HYDROIcon,
            _img_aisMET_HYDROIcon, wxITEM_CHECK, _("aisMET_HYDRO"), _T(""), NULL,
            aisMET_HYDRO_TOOL_POSITION, 0, this);
#endif
    }
   

    m_pDialog = NULL;

	wxMenu dummy_menu;
    m_position_menu_id = AddCanvasContextMenuItem(
        new wxMenuItem(&dummy_menu, -1, _("RIS Signal Status")),
        this);
    SetCanvasContextMenuItemViz(m_position_menu_id, true);

    return (WANTS_OVERLAY_CALLBACK | WANTS_OPENGL_OVERLAY_CALLBACK
        | WANTS_TOOLBAR_CALLBACK | INSTALLS_TOOLBAR_TOOL | WANTS_CURSOR_LATLON
         | WANTS_AIS_SENTENCES  | WANTS_NMEA_SENTENCES 
        | WANTS_PLUGIN_MESSAGING | WANTS_CONFIG);
}

bool aisMET_HYDRO_pi::DeInit(void)
{
   
	

		//    Record the dialog position
	if (NULL != m_pDialog) {


        // Capture dialog position
        wxPoint p = m_pDialog->GetPosition();
        wxRect r = m_pDialog->GetRect();
        SetaisMET_HYDRODialogX(p.x);
        SetaisMET_HYDRODialogY(p.y);
        SetaisMET_HYDRODialogSizeX(r.GetWidth());
        SetaisMET_HYDRODialogSizeY(r.GetHeight());
		

		if(m_pDialog) {
			m_pDialog->Close();
			delete m_pDialog;
			m_pDialog = NULL;
		}

    
    m_bShowaisMET_HYDRO = false;
    SetToolbarItemState(m_leftclick_tool_id, m_bShowaisMET_HYDRO);

	}
    		
	SaveConfig();
    
    RequestRefresh(m_parent_window); // refresh main window

    return true;
}

int aisMET_HYDRO_pi::GetAPIVersionMajor()
{
    return atoi(API_VERSION);
}

int aisMET_HYDRO_pi::GetAPIVersionMinor()
{
    std::string v(API_VERSION);
    size_t dotpos = v.find('.');
    return atoi(v.substr(dotpos + 1).c_str());
}

int aisMET_HYDRO_pi::GetPlugInVersionMajor()
{
    return PLUGIN_VERSION_MAJOR;
}

int aisMET_HYDRO_pi::GetPlugInVersionMinor() { return PLUGIN_VERSION_MINOR; }

wxBitmap* aisMET_HYDRO_pi::GetPlugInBitmap() { return &m_panelBitmap; }

wxString aisMET_HYDRO_pi::GetCommonName() { return _("aisMET_HYDRO"); }

wxString aisMET_HYDRO_pi::GetShortDescription() { return _("aisMET_HYDRO"); }

wxString aisMET_HYDRO_pi::GetLongDescription() { return _("Displays ais met_hydro data"); }

int aisMET_HYDRO_pi::GetToolbarToolCount(void) { return 1; }

void aisMET_HYDRO_pi::SetColorScheme(PI_ColorScheme cs)
{
    if (NULL == m_pDialog)
        return;

    DimeWindow(m_pDialog);
}


void aisMET_HYDRO_pi::OnToolbarToolCallback(int id)
{

    if (NULL == m_pDialog) {
        m_pDialog = new Dlg(m_parent_window);
        m_pDialog->plugin = this;
        
        m_pDialog->Move(wxPoint(m_hr_dialog_x, m_hr_dialog_y));
        m_pDialog->SetSize(m_hr_dialog_sx, m_hr_dialog_sy);


    }

    // m_pDialog->Fit();
    // Toggle
    m_bShowaisMET_HYDRO = !m_bShowaisMET_HYDRO;

    //    Toggle dialog?
    if (m_bShowaisMET_HYDRO) {
        m_pDialog->Move(wxPoint(m_hr_dialog_x, m_hr_dialog_y));
        m_pDialog->SetSize(m_hr_dialog_sx, m_hr_dialog_sy);
        m_pDialog->Show();

    } else {
        m_pDialog->Hide();
    }

    // Toggle is handled by the toolbar but we must keep plugin manager b_toggle
    // updated to actual status to ensure correct status upon toolbar rebuild
    SetToolbarItemState(m_leftclick_tool_id, m_bShowaisMET_HYDRO);

    // Capture dialog position
    wxPoint p = m_pDialog->GetPosition();
    wxRect r = m_pDialog->GetRect();
    SetaisMET_HYDRODialogX(p.x);
    SetaisMET_HYDRODialogY(p.y);
    SetaisMET_HYDRODialogSizeX(r.GetWidth());
    SetaisMET_HYDRODialogSizeY(r.GetHeight());

    RequestRefresh(m_parent_window); // refresh main window
}

void aisMET_HYDRO_pi::SetAISSentence(wxString &sentence) {

wxString myMsg;
    wxString mySentence = sentence;
    wxString token[40];
    wxString s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
    token[0] = _T("");
    bool isFI = false;

    wxStringTokenizer tokenizer(sentence, wxT(","));

    int i = 0;
    wxString dd;

    while (tokenizer.HasMoreTokens()) {
        token[i] = tokenizer.GetNextToken();
        i++;
    }
    if (token[0].Right(3) == _T("VDM")) {


        if (token[1] == "2" && token[2] == "1") {
                        if (m_pDialog) {
                            isFI = m_pDialog->DecodeForDAC(token[5]);
                            if (isFI)
                                prevMsg = token[5];
                        }
                        return;
        } else if (token[1] == "2" && token[2] == "2") {
                        myMsg = prevMsg + token[5];
                        if (m_pDialog) {
                            isFI = m_pDialog->DecodeForDAC(myMsg);
                            if (isFI) 
                                m_pDialog->SetAISMessage(myMsg, mySentence);                            
                        }
                        return;
        } else if (token[1] == "1" && token[2] == "1") {
                        s5 = token[5];
                        if (m_pDialog) {
                            m_pDialog->DecodeForDAC(token[5]);
                            m_pDialog->SetAISMessage(s5, mySentence);
                            m_pDialog->mySentence = mySentence;
                        }

                        return;
        }
    }
    return;
}

bool aisMET_HYDRO_pi::LoadConfig(void)
{
    wxFileConfig* pConf = (wxFileConfig*)m_pconfig;

    if (pConf) {
        pConf->SetPath(_T( "/Settings/aisMET_HYDRO_pi" ));
        pConf->Read(_T( "ShowaisMET_HYDROIcon" ), &m_baisMET_HYDROShowIcon, 1);
        m_hr_dialog_x = pConf->Read(_T ( "DialogPosX" ), 40L);
        m_hr_dialog_y = pConf->Read(_T ( "DialogPosY" ), 140L);
        m_hr_dialog_sx = pConf->Read(_T ( "DialogSizeX"), 330L);
#ifdef __WXOSX__
        m_hr_dialog_sy = pConf->Read(_T ( "DialogSizeY"), 250L);
#else
        m_hr_dialog_sy = pConf->Read(_T ( "DialogSizeY"), 300L);
#endif
        if ((m_hr_dialog_x < 0) || (m_hr_dialog_x > m_display_width))
            m_hr_dialog_x = 40;
        if ((m_hr_dialog_y < 0) || (m_hr_dialog_y > m_display_height))
            m_hr_dialog_y = 140;

        return true;
    } else
        return false;
}

bool aisMET_HYDRO_pi::SaveConfig(void)
{
    wxFileConfig* pConf = (wxFileConfig*)m_pconfig;

    if (pConf) {
        pConf->SetPath(_T ( "/Settings/aisMET_HYDRO_pi" ));
        pConf->Write(_T ( "ShowaisMET_HYDROIcon" ), m_baisMET_HYDROShowIcon);
        pConf->Write(_T ( "DialogPosX" ), m_hr_dialog_x);
        pConf->Write(_T ( "DialogPosY" ), m_hr_dialog_y);
        pConf->Write(_T ( "DialogSizeX"), m_hr_dialog_sx);
        pConf->Write(_T ( "DialogSizeY"), m_hr_dialog_sy);

        return true;
    } else
        return false;
}

void aisMET_HYDRO_pi::OnaisMET_HYDRODialogClose()
{
    m_bShowaisMET_HYDRO = false;
    SetToolbarItemState(m_leftclick_tool_id, m_bShowaisMET_HYDRO);
    m_pDialog->Hide();

	RequestRefresh(m_parent_window); // refresh main window
	
    SaveConfig();   
}