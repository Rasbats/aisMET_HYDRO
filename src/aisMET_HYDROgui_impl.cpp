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

#include "aisMET_HYDROgui_impl.h"
#include "aisMET_HYDRO_pi.h"
#include <wx/progdlg.h>
#include <wx/wx.h>

#include "wx/textfile.h"
#include <stdio.h>
#include <wx/timer.h>
#include "signal.h"
#include "widget.h"
#include <memory>

using mylibais::AisMsg;
using std::unique_ptr;
using namespace mylibais;


/**************************************************************************/
/*          Some assorted utilities                                       */
/**************************************************************************/

wxString toSDMM(int NEflag, double a)
{
	short neg = 0;
	int d;
	long m;

	if (a < 0.0)
	{
		a = -a;
		neg = 1;
	}
	d = (int)a;
	m = (long)((a - (double)d) * 60000.0);

	if (neg)
		d = -d;

	wxString s;

	if (!NEflag)
		s.Printf(_T("%d %02ld.%03ld'"), d, m / 1000, m % 1000);
	else
	{
		if (NEflag == 1)
		{
			char c = 'N';

			if (neg)
			{
				d = -d;
				c = 'S';
			}

			s.Printf(_T("%03d %02ld.%03ld %c"), d, m / 1000, (m % 1000), c);
		}
		else if (NEflag == 2)
		{
			char c = 'E';

			if (neg)
			{
				d = -d;
				c = 'W';
			}
			s.Printf(_T("%03d %02ld.%03ld %c"), d, m / 1000, (m % 1000), c);
		}
	}
	return s;
}




void assign(char* dest, char* arrTest2) { strcpy(dest, arrTest2); }

#define BUFSIZE 0x10000

Dlg::Dlg(wxWindow* parent, wxWindowID id, const wxString& title,
	const wxPoint& pos, const wxSize& size, long style)
	: aisMET_HYDROBase(parent, id, title, pos, size, style)
{
	this->Fit();
	dbg = false; // for debug output set to true

	m_bPaused = false;
	m_bUsingTest = false;

	bool m_bShowaisMET_HYDRO = true;
	m_bHaveMessageList = false;
	m_bHaveDisplay = false;
	m_bUpdateTarget = false; 

	AISTargetList = new AIS_Target_Hash;
	pTargetData = new AIS_Target_Data;

	m_pASMmessages1 = NULL;
	myAISdisplay = NULL;

	auto path = GetPluginDataDir("aisMET_HYDRO_pi");
	fn.SetPath(path);
	fn.AppendDir("data");
	fn.AppendDir("pins");
	fn.SetFullName("green-pin.png");
	path = fn.GetFullPath();
	wxImage panelIcon(path);
    wxBitmap* m_panelBitmap = new wxBitmap(panelIcon);

	AddCustomWaypointIcon(m_panelBitmap, "green-pin", "MET_HYDRO");
	
}

namespace mylibais {
	template <typename T, typename... Args>
	std::unique_ptr<T> MakeUnique(Args &&... args) {
		//wxMessageBox("here");
		return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
	}

	unique_ptr<AisMsg>CreateAisMsg8(const string &body, const int fill_bits) {
		mylibais::Ais8 msg(body.c_str(), fill_bits);
		int fi = msg.fi;
		wxString fis = wxString::Format("%i", fi);
		wxMessageBox(fis);
		return MakeUnique<mylibais::Ais8_1_11>(body.c_str(), fill_bits);
	}

	unique_ptr<AisMsg> CreateAisMsg(const string &body, const int fill_bits) {

		string mybody = body;
		//wxMessageBox(mybody);

		int fill = fill_bits;
		wxString fills = wxString::Format("%i", fill);
		//wxMessageBox(fills);

		Ais8 msg(body.c_str(), fill_bits);

		int dac = msg.dac;
		wxString dacs = wxString::Format("%i", dac);
		wxMessageBox(dacs);

		int fi = msg.fi;
		wxString fis = wxString::Format("%i", fi);
		wxMessageBox(fis);

		return CreateAisMsg8(body, fill_bits);
	}
} // mylibais


inline const char * const BoolToString(bool b)
{
	return b ? "true" : "false";
}


void Dlg::OnLogging(wxCommandEvent& event) {
	
	if ( NULL == myAISdisplay) {		
		myAISdisplay = new AISdisplay(this, wxID_ANY, _T("AIS Logging"), wxPoint(20, 20), wxSize(350, 400), wxCAPTION|wxCLOSE_BOX|wxRESIZE_BORDER);
		myAISdisplay->Show();
		m_bHaveDisplay = true;
	}

	if (m_bHaveDisplay) {
		myAISdisplay->Show();
	}
}

void Dlg::OnToggleButton(wxCommandEvent& event) {
	m_bPaused = !m_bPaused;
}

void Dlg::SetAISMessage(wxString &msg , wxString &sentence)
{
		
	if (m_bPaused) return;
	
	m_message = sentence;

	bool m_bGotDAC = DecodeForDAC(msg);

	if (m_bGotDAC) {
		Decode(msg);
	}
}

wxString Dlg::SetaisMET_HYDROMessage(string &msg) {
	unique_ptr<AisMsg> myMsg;

	//AIS_Bitstring* myBits;

	string payload;
	//   !AIVDM,1,1,,A,E>j4e>@LtqHIpHHLh@@@@@@@@@@0Vei<=iWL000000N2P0,4*05
	//payload = "E>j4e>@LtqHIpHHLh@@@@@@@@@@0Vei<=iWL000000N2P0"; //aisMET_HYDRO
	payload = msg;
	//string output;

	// myMsg = CreateAisMsg(payload,0);
	int mm = myMsg->mmsi;
	int ms = myMsg->message_id;

	wxString outstring(wxString::Format(("%i"), mm));
	return outstring;
	
}

void Dlg::OnClose(wxCloseEvent& event)
{	
	plugin->OnaisMET_HYDRODialogClose();
}


wxString Dlg::DateTimeToTimeString(wxDateTime myDT)
{
	wxString sHours, sMinutes, sSecs;
	sHours = myDT.Format(_T("%H"));
	sMinutes = myDT.Format(_T("%M"));
	sSecs = myDT.Format(_T("%S"));
	wxString dtss = sHours + sMinutes + sSecs;
	return dtss;
}

wxString Dlg::DateTimeToDateString(wxDateTime myDT)
{

	wxString sDay, sMonth, sYear;
	sDay = myDT.Format(_T("%d"));
	sMonth = myDT.Format(_T("%m"));
	sYear = myDT.Format(_T("%y"));

	return sDay + sMonth + sYear;
}


bool Dlg::DecodeForDAC(wxString insentence)
{

	string myMsg = std::string(insentence.mb_str());
    
	const char* payload1 = myMsg.c_str();
	mylibais::Ais8 myDacFi(payload1, 0);

	int n = strlen(payload1);
	wxString len = wxString::Format("%i", n);
	//wxMessageBox(len);

	int dac0 = myDacFi.dac;
	wxString outdac0 = wxString::Format("%i", dac0);
	//wxMessageBox(outdac0);
	
	int fi0 = myDacFi.fi;
	wxString outfi0 = wxString::Format("%i", fi0);
    //wxMessageBox(outfi0);
	if (fi0 == 31 && dac0 == 1) {
		//wxMessageBox("here");

		return true;
	}

	if (fi0 == 25 || fi0 == 26 || fi0 == 41) {
		return false;
	}
	else
		return false;


}

void Dlg::Decode(wxString sentence)
{

	string myMsg = std::string(sentence.mb_str());

	const char* payload1 = myMsg.c_str();
	mylibais::Ais8 myDacFi(payload1, 0);

	int dac0 = myDacFi.dac;
	int fi0;
	if (dac0 == 1) {
		//wxString outdac0 = wxString::Format("%i", dac0);
		//wxMessageBox(outdac0);


		fi0 = myDacFi.fi;
		if (fi0 == 31 && dac0 == 1) {
			wxString outfi0 = wxString::Format("%i", fi0);
			//wxMessageBox(outfi0);
			if (m_bHaveDisplay) {
				if (myAISdisplay->m_tbAISPause->GetValue()) {

					int mm = 0;
					int ms = 0;

					myAISdisplay->m_tcAIS->AppendText(mySentence);
				}
			}
			getAis8_1_31(myMsg);
		}
	}
	
}

void Dlg::OnTest(wxCommandEvent& event)
{
	m_bUsingTest = true;

	wxString mySentence = plugin->m_pDialog->m_textCtrlTest->GetValue();

	if (mySentence.IsEmpty() || mySentence.IsNull()) {
		wxMessageBox("No sentence has been entered");
		return;
	}

	if (mySentence.Mid(0, 6) != "!AIVDM") {
		wxMessageBox("Invalid sentence");
		return;
	}

	string myMsg = parseNMEASentence(mySentence).ToStdString();

	const char* payload1 = myMsg.c_str();
	mylibais::Ais8 myDacFi(payload1, 0);

	int dac0 = myDacFi.dac;
	wxString outdac0 = wxString::Format("%i", dac0);
	//wxMessageBox(outdac0, "DAC");

	int fi0 = myDacFi.fi;
	wxString outfi0 = wxString::Format("%i", fi0);
	//wxMessageBox(outfi0, "FI");

	bool m_bGotDAC = DecodeForDAC(myMsg);

	if (m_bGotDAC) {
		Decode(myMsg);
	}
	

}

wxString Dlg::parseNMEASentence(wxString& sentence)
{

	wxString token[40];
	wxString s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
	token[0] = _T("");

	wxStringTokenizer tokenizer(sentence, wxT(","));
	
	int i = 0;

	while (tokenizer.HasMoreTokens()) {
		token[i] = tokenizer.GetNextToken();
		i++;
	}
	if (token[0].Right(3) == _T("VDM")) {
		return token[5];		
	}
	
	return "999";
}

wxString Dlg::MakeDescription(mylibais::Ais8_1_31 myData) {

	wxString myDescription;
	wxString cr = "\n";
	int idefault  = 0;
	
	wxString sMMSI = wxString::Format("%i", pTargetData->MMSI);
	myDescription += "MMSI: " + sMMSI + cr;
	
	int dd = myData.utc_day;
	wxString outdd = wxString::Format("%i", dd);
	int hr = myData.utc_hour;
	wxString outhr = wxString::Format("%i", hr);
	int min = myData.utc_min;
	wxString outmin = wxString::Format("%i", min);

	wxString ddhhmm = outdd + outhr + outmin;
	string DayHourMinute = std::string(ddhhmm.mb_str());

	myDescription += DayHourMinute + cr;

	int wind_ave = myData.wind_ave;
	wxString oWind_ave = wxString::Format("%i", wind_ave);
	idefault = (int)wind_ave;
	if (idefault == 127) {}
		//myDescription += "Average wind speed: n/a" + cr;
	else
		myDescription += "Average wind speed: " + oWind_ave + " knots" + cr;

	int wind_dir = myData.wind_dir;
	wxString oWind_dir = wxString::Format("%i", wind_dir);
	idefault = (int)wind_dir;
	if (idefault == 360) {}
		//myDescription += "Average wind speed: n/a" + cr;
	else
		myDescription += "Average wind direction: " + oWind_dir + " deg" + cr;

	float air_press = myData.air_pres;
	idefault = (int)air_press;
	wxString outpress = wxString::Format("%i", idefault);
	if (idefault == 1311 || idefault == 800) {}
		//myDescription += "Air pressure: n/a" + cr;
	else
		myDescription += "Air pressure:" + outpress + cr;

	float surf_cur_speed = myData.surf_cur_speed;
	wxString osurf_cur_speed = wxString::Format("%f", surf_cur_speed);
	idefault = (int)surf_cur_speed;
	if (idefault != 255) 
		//myDescription +=  "Surface current spd: n/a" + cr;
		myDescription += "Surface current spd: " + osurf_cur_speed + cr;

	int water_level = myData.water_level;	
	if (water_level == 4001) {}
		//myDescription += "Water level: n/a" + cr;
	else {
		double wl = (1.0 * water_level / 100) -10.;
		wxString oWater_level = wxString::Format("%f", wl);
		myDescription += "Water level: " + oWater_level + cr;
	}
	
	int ice = myData.ice;
	wxString oIce = wxString::Format("%i", ice);
	switch (ice) {
	case 0:
		//myDescription += "Ice: NO ice present" + cr;
		break;
	case 1:
		myDescription += "Ice: YES, ice is present" + cr;
		break;
	case 3:
		//myDescription += "Ice: n/a" + cr;
		break;
	}

	return myDescription;

}

// ************ Weather (International) **************
void Dlg::getAis8_1_31(string rawPayload) {
		
	const char* payload = rawPayload.c_str();
	mylibais::Ais8_1_31 myMetHydro(payload, 0);
	
	int mm = myMetHydro.mmsi;

	AisPoint weatherPoint = myMetHydro.position;
	wxString outLat = wxString::Format("%f", weatherPoint.lat_deg);
	wxString outLon = wxString::Format("%f", weatherPoint.lng_deg);

	wxArrayString myWaypoints = GetWaypointGUIDArray();
	
	wxString str[1000];
    wxString guid = "";
	for (unsigned int i = 1; i < myWaypoints.size(); i++) {
		str[i] = myWaypoints[i];
		PlugIn_Waypoint  wayPoint;
		GetSingleWaypoint(str[i], &wayPoint);
		if (wayPoint.m_lat == weatherPoint.lat_deg && wayPoint.m_lon == weatherPoint.lng_deg) {
			guid = wayPoint.m_GUID;
			AISTargetList->erase(guid);
			bool d = DeleteSingleWaypoint(guid);
			break;
		}
	}
		
	pTargetData = new AIS_Target_Data;	

	pTargetData->Lat = weatherPoint.lat_deg;
	pTargetData->Lon = weatherPoint.lng_deg;
	pTargetData->Description = MakeDescription(myMetHydro);		
	pTargetData->MMSI = mm;

	double myLat = pTargetData->Lat;
	double myLon = pTargetData->Lon;
		
	plugin->m_pDialog->m_textCtrlTest->SetValue(m_message);
	PlugIn_Waypoint_Ex*  wayPoint = new PlugIn_Waypoint_Ex(myLat, myLon, "", "", "");		
	wayPoint->m_MarkDescription = pTargetData->Description;
	wayPoint->IsNameVisible = false;
	wxString s_MarkName = wxString::Format("%i", pTargetData->MMSI);
	wayPoint->m_MarkName = s_MarkName;
	wayPoint->IconName = "green-pin";

	s_GUID = wayPoint->m_GUID; 
	pTargetData->GUID = s_GUID;

	(*AISTargetList)[pTargetData->GUID] = pTargetData;  // update the hash table entry

	AddSingleWaypointEx(wayPoint, false);
	GetParent()->Refresh();		
	
}

void Dlg::CreateControlsMessageList()
{
	int width;
	long lwidth;

	int dx = 20;

	width = dx * 4;
	if (m_pASMmessages1) {
		m_pASMmessages1->m_pListCtrlAISTargets->InsertColumn(tlTRK, _("Hectomtr"), wxLIST_FORMAT_LEFT, width);
		m_pASMmessages1->m_pListCtrlAISTargets->InsertColumn(tlNAME, _("RISindex"), wxLIST_FORMAT_LEFT, width);

	}
}

wxString Dlg::StandardPath()
{
    wxString s = wxFileName::GetPathSeparator();
    wxString stdPath  = *GetpPrivateApplicationDataLocation();

    stdPath += s + _T("plugins");
    if (!wxDirExists(stdPath))
      wxMkdir(stdPath);

    stdPath += s + _T("aisMET_HYDRO");

#ifdef __WXOSX__
    // Compatibility with pre-OCPN-4.2; move config dir to
    // ~/Library/Preferences/opencpn if it exists
    {
        wxStandardPathsBase& std_path = wxStandardPathsBase::Get();
        wxString s = wxFileName::GetPathSeparator();
        // should be ~/Library/Preferences/opencpn
        wxString oldPath = (std_path.GetUserConfigDir() +s + _T("plugins") +s + _T("UKTides"));
        if (wxDirExists(oldPath) && !wxDirExists(stdPath)) {
            wxLogMessage("UKTides_pi: moving config dir %s to %s", oldPath, stdPath);
            wxRenameFile(oldPath, stdPath);
        }
    }
#endif

    if (!wxDirExists(stdPath))
      wxMkdir(stdPath);

    stdPath += s;
    return stdPath;
}

