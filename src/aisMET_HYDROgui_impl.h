/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  aisMET_HYDRO Plugin
 * Author:   Mike Rossiter
 *
 ***************************************************************************
 *   Copyright (C) 2017 by Mike Rossiter                                   *
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

#ifndef _HRGUI_IMPL_H_
#define _HRGUI_IMPL_H_

#ifdef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "AisMaker.h"

#include "aisMET_HYDROgui.h"
#include "ocpn_plugin.h"
#include "tinyxml.h"
#include "wx/process.h"
#include "json/reader.h"
#include "json/writer.h"
#include <cmath>
#include <sstream>
#include <wx/filedlg.h>
#include <wx/gdicmn.h>
#include <wx/listctrl.h>
#include <wx/stdpaths.h>
#include <wx/thread.h>
#include <wx/utils.h>

#include "AIS_Bitstring.h"
#include "ais.h"
#include "signal.h"
#include "widget.h"

#include <memory>
#include <wx/hashmap.h>

#include <wx/dynarray.h>
#include <wx/html/htmlwin.h>
#include <wx/tglbtn.h>
#include <wx/wxhtml.h>

#include "AISdisplay.h"
#include "ASMmessages.h"
#include "ocpn_plugin.h"
#include <sstream> 
#include <algorithm>
#include <vector>

#ifdef __WXOSX__
#define aisMET_HYDRO_DLG_STYLE                                                 \
    wxCLOSE_BOX | wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxSTAY_ON_TOP
#else
#define aisMET_HYDRO_DLG_STYLE                                                 \
    wxCLOSE_BOX | wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
#endif

using namespace std;

class aisMET_HYDRO_pi;
class AisMaker;
class AISdisplay;
class asmMessages;

class AIS_Target_Data;
class Signalling;

struct AIS_Data {
    int utc_day;
    int utc_hour;
    int utc_min;

    float air_press;
    float air_temp;

    int MMSI;
    int report_type;
    int reports_size;

    float water_level;

    int wind_dir;
    int wind_speed;

    float surf_cur_speed;
    float speed;
    int dir;

    int ice;

    double lat;
    double lon;

    wxString description;
    wxString dacFI;
    int site_id;
    wxString site_name;
    
    wxString site_ident;
};

struct site_data {

  int site_id;
    string site_string;
  double site_lat;
    double site_lon;
  

};

// lass rtept;
class AIS_Target_Data {
public:
    int MID;
    int HECT;
    int NavStatus;
    int SyncState;
    int SlotTO;
    double SOG;
    double COG;
    double HDG;
    double Lon;
    double Lat;
    int ROTAIS;
    int ROTIND;

    char ShipNameExtension[15];
    unsigned char ShipType;
    int IMO;

    int DimA;
    int DimB;
    int DimC;
    int DimD;

    double Euro_Length; // Extensions for European Inland AIS
    double Euro_Beam;
    double Euro_Draft;
    int UN_shiptype;
    bool b_isEuroInland;
    bool b_hasInlandDac; // intermediate storage for EU Inland. SignalK
    bool b_blue_paddle;
    int blue_paddle;

    int ETA_Mo;
    int ETA_Day;
    int ETA_Hr;
    int ETA_Min;

    int hect;
    int signalForm;
    int signalStatus;
    int bridgeClearance;
    string country;
    string RISindex;
    bool b_nameValid;
    bool b_nameFromCache;
    /***************** weather obs *****/
    double air_press;
    int MMSI;

    /***************** 8_1_31 *****/
    /********* provide an index*********/
    int utc_day;
    int utc_hour;
    int utc_min;
    wxString Description;
    string GUID;
};

// WX_DECLARE_HASH_MAP( int, AIS_Target_Data*, wxIntegerHash, wxIntegerEqual,
// AIS_Target_Hash ); WX_DECLARE_HASH_MAP( int, wxString, wxIntegerHash,
// wxIntegerEqual, AIS_Target_Name_Hash );

WX_DECLARE_HASH_MAP(
    wxString, AIS_Target_Data*, wxStringHash, wxStringEqual, AIS_Target_Hash);
WX_DECLARE_HASH_MAP(
    string, wxString, wxStringHash, wxStringEqual, AIS_Target_Name_Hash);

// An identifier to notify the application when the // work is done #define
// ID_COUNTED_COLORS    100

class AisMaker;

namespace mylibais {

// Decodes the payload of an AIS message and returns an AisMsg instance.
// Returns a nullptr on failure.
// The body is the armored text from 1 or more sentences that compose
// the encoded bits for an AIS message.
// The fill_bits are the number of pad bits in the last character of the
// body.  AIS messages are 8-bit aligned and the characters in the armored
// body are 6-bit aligned.
std::unique_ptr<mylibais::AisMsg> CreateAisMsg(
    const string& body, const int fill_bits);

} // namespace mylibais

class PlugIn_ViewPort;

class Dlg : public aisMET_HYDROBase {
public:
    Dlg(wxWindow* parent, wxWindowID id = wxID_ANY,
        const wxString& title = _("aisMET_HYDRO"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = aisMET_HYDRO_DLG_STYLE);

    aisMET_HYDRO_pi* plugin;

    wxString DateTimeToTimeString(wxDateTime myDT);
    wxString DateTimeToDateString(wxDateTime myDT);

    Signalling* signalling;

    AisMaker* myAIS;
    AisMaker* myaisMET_HYDRO;
    AISdisplay* m_pAISdialog;

    bool m_bPaused;
    void SetAISMessage(wxString& msg, wxString& sentence);
    // wxString SetaisMET_HYDROMessage(string &msg);

    wxTextFile* nmeafile;

    AIS_Target_Data* pTargetData;
    AIS_Target_Hash* AISTargetList;

    AIS_Target_Data myTestData;
    vector<AIS_Target_Data> myTestDataCollection;

    site_data site_report;
    vector<site_data> the_data;
    int count = 0;

    void CreateControlsMessageList();

    PlugIn_ViewPort* m_vp;
    asmMessages* m_pASMmessages1;
    AISdisplay* myAISdisplay;
    bool m_bHaveMessageList;
    bool m_bHaveDisplay;
    bool m_bUpdateTarget;

    wxString m_message;

    wxFileName fn;
    wxString station_icon_name;
    wxBitmap* wpIcon;
    wxString StandardPath();

    double initLat;
    double initLon;
    wxString mySentence;

    bool DecodeForDAC(wxString insentence);
    void Decode(wxString sentence);

protected:
private:
    bool m_bUsingTest;
    wxString s_GUID;

    wxString AIVDM;
    bool m_bShowaisMET_HYDRO;

    wxString m_sLogMsg;
    string m_sPayload;
    wxArrayString m_aMMSI;
    bool m_bActive;
    bool m_bStop;

    bool dbg;

    bool m_bUseStop;
    bool m_bUsePause;

    void OnClose(wxCloseEvent& event);

    void OnLogging(wxCommandEvent& event);
    void OnToggleButton(wxCommandEvent& event);

    // void OnTest(wxCommandEvent& event);
    void OnStopReading(wxCommandEvent& event);
    void OnRemoveWPT(wxCommandEvent& event);

    wxString parseNMEASentence(wxString& sentence);

    wxString MakeDescription(AIS_Data myData);
    void getAis8_1_11(string rawPayload);
    void getAis8_1_31(string rawPayload);
    // void SendAIS(double cse, double spd, double lat, double lon);
    void getAis8_1_26(string rawPayload);

    void getAis8_367_33(string rawPayload);
};

#endif
