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
#include <wx/tglbtn.h>
#include <wx/arrstr.h>

using mylibais::AisMsg;

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
    m_bStop = false;
	m_bUsingTest = false;
    m_bActive = false;

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
    if (!myAISdisplay) {
		myAISdisplay = new AISdisplay(this, wxID_ANY, _T("AIS Logging"), wxPoint(20, 20), wxSize(-1, -1), wxCAPTION | wxCLOSE_BOX | wxRESIZE_BORDER);		
    }
    myAISdisplay->Show();
    m_bHaveDisplay = true;	 

}

void Dlg::OnToggleButton(wxCommandEvent& event) {
	m_bPaused = !m_bPaused;
}

void Dlg::SetAISMessage(wxString &msg , wxString &sentence)
{
    bool m_bGotDAC = false;	
	if (m_bPaused) return;
    if (m_bStop) return;

    //plugin->m_pDialog->m_textCtrlTest->SetValue(m_message);        

	m_bGotDAC = DecodeForDAC(msg);

	if (m_bGotDAC) {

		m_message = sentence;

		const char* payload1 = msg.c_str();
		mylibais::Ais8 myDacFi(payload1, 0);
		int dac0 = myDacFi.dac;
		int fi0 = myDacFi.fi;
        int id = myDacFi.message_id;

		//myDacFi.mmsi
        //wxString myId = wxString::Format("%i", id);


							string myMsg = msg.ToStdString();

							if (fi0 == 31 && dac0 == 1)  {
									// wxMessageBox("1_31");
									getAis8_1_31(myMsg);
							}

							if (fi0 == 26 && dac0 == 1) {
									// wxMessageBox("1_26");
									getAis8_1_26(myMsg);
							}

							if (fi0 == 33 && dac0 == 367) {
									// wxMessageBox("367_33");
									getAis8_367_33(myMsg);
							}
			
		}
	
}
/*
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
*/

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
/*
wxArrayInt Dlg::DecodeForDACFI(wxString payload)
{
	wxArrayInt myDACFI;
	string myMsg = std::string(payload.mb_str());
    
	const char* payload1 = myMsg.c_str();
	mylibais::Ais8 myDacFi(payload1, 0);

	int n = strlen(payload1);
	wxString len = wxString::Format("%i", n);
	//wxMessageBox(len);

	int dac0 = myDacFi.dac;
	myDACFI.Add(dac0);
	int fi0 = myDacFi.fi;
	myDACFI.Add(fi0);
}
*/

bool Dlg::DecodeForDAC(wxString insentence)
{

	string myMsg = std::string(insentence.mb_str());

	//m_textCtrlTest->SetValue(myMsg);
    
	const char* payload1 = myMsg.c_str();
	mylibais::Ais8 myDacFi(payload1, 0);	

	//int n = strlen(payload1);
	//wxString len = wxString::Format("%i", n);
	//wxMessageBox(len);

	int dac0 = myDacFi.dac;
	wxString outdac0 = wxString::Format("%i", dac0);
	//wxMessageBox(outdac0);
	
	int fi0 = myDacFi.fi;
	wxString outfi0 = wxString::Format("%i", fi0);
    //wxMessageBox(outfi0);

	if (fi0 == 31 && dac0 == 1) {
		return true;
	} 

	else

	if (fi0 == 26 && dac0 == 1) {
		return true;
	} else

	if (fi0 == 33 && dac0 == 367) {
		return true;
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
	int fi0 = myDacFi.fi;
	if (dac0 == 1) {
		//wxString outdac0 = wxString::Format("%i", dac0);
		//wxMessageBox(outdac0);

		if (fi0 == 31 && dac0 == 1) {
			//wxString outfi0 = wxString::Format("%i", fi0);
			getAis8_1_31(myMsg);
		}
	}else
        if (dac0 == 367) {
                if (fi0 == 33) {
                        getAis8_367_33(myMsg);
                }
        }
	
}

/*
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
*/
void Dlg::OnStopReading(wxCommandEvent& event) { 

    m_bStop = m_buttonStop->GetValue();
    if (m_bStop) { 
		if (m_bActive) {
            m_buttonRemoveWPT->SetBackgroundColour(wxColour(0, 255, 0));
            m_buttonRemoveWPT->SetValue(false);
		}
		m_buttonStop->SetBackgroundColour(wxColour(255, 0, 0));		
		m_bPaused = true;
    } else {
        if (m_bActive) {
            m_buttonRemoveWPT->SetBackgroundColour(wxColour(0, 255, 0));
            m_buttonRemoveWPT->SetValue(false);
        }
        m_buttonStop->SetBackgroundColour(wxColour(0, 255, 0));
        m_bPaused = false;	
	}
}

void Dlg::OnRemoveWPT(wxCommandEvent& event) {
    
	m_bActive = m_buttonRemoveWPT->GetValue();
	
	if (m_bActive) {
                m_buttonRemoveWPT->SetBackgroundColour(wxColour(255, 0, 0));
                wxString str[1000];
                wxString guid;
				m_bPaused = true;
                for (unsigned int i = 0; i <= m_aMMSI.size(); i++) {

                        str[i] = m_aMMSI[i];
                        PlugIn_Waypoint_Ex wayPoint;
                        bool b = GetSingleWaypointEx(str[i], &wayPoint);                        
                        guid = wayPoint.m_GUID;
                        AISTargetList->erase(guid);
                        bool d = DeleteSingleWaypoint(guid);
                }
        } else {
                m_buttonRemoveWPT->SetBackgroundColour(wxColour(0, 255, 0));
				m_bPaused = false;
        }
}

wxString Dlg::parseNMEASentence(wxString& sentence)
{

	wxString token[40];
	wxString s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
	token[0] = "";

	wxStringTokenizer tokenizer(sentence, wxT(","));
	
	int i = 0;

	while (tokenizer.HasMoreTokens()) {
		token[i] = tokenizer.GetNextToken();
		i++;
	}
	if (token[0].Right(3) == "VDM") {
		return token[5];		
	}
	
	return "999";
}

wxString Dlg::MakeDescription(AIS_Data myData) {

	wxString myDescription;
	wxString cr = "\n";
	int idefault  = 0;

	myDescription = myData.dacFI + cr;

	if (myData.dacFI == "DAC_FI: 1_26" || myData.dacFI == "DAC_FI: 367_33") {
		wxString s_SiteID = wxString::Format("%i", myData.site_id);
		myDescription += "SiteID: " + s_SiteID + cr;
	}
	
	wxString sMMSI = wxString::Format("%i", myData.MMSI);
	myDescription += "MMSI: " + sMMSI + cr;
	
	int dd = myData.utc_day;
	wxString outdd;
	if (dd < 10) outdd = "0" + wxString::Format("%i", dd);
	else outdd = wxString::Format("%i", dd);

	int hr = myData.utc_hour;
	wxString outhr;
	if (hr < 10) outhr = "0" + wxString::Format("%i", hr);
	else outhr = wxString::Format("%i", hr);

	int min = myData.utc_min;
	wxString outmin;
	if(min < 10) outmin = "0" + wxString::Format("%i", min);
	else outmin = wxString::Format("%i", min);
	

	wxString ddhhmm = "Day: " + outdd + "  " + outhr + outmin + " UTC";
	string DayHourMinute = std::string(ddhhmm.mb_str());

	myDescription += DayHourMinute + cr;

	int iwind_ave = myData.wind_speed;
	wxString wind_speed;
	if (iwind_ave == 999 || iwind_ave == 127)
		wind_speed = "n/a";
	else
		wind_speed = wxString::Format("%i", iwind_ave);

	myDescription += "Average wind speed: " + wind_speed + " knots" + cr;

	int iwind_dir = myData.wind_dir;
	wxString wind_dir;
	if (iwind_dir == 999 || iwind_dir == 360)
		wind_dir = "n/a";
	else
		wind_dir = wxString::Format("%i", iwind_dir);

	myDescription += "Average wind direction: " + wind_dir + " deg" + cr;

	float air_press = myData.air_press;
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
	if (water_level == 4001 || water_level == 999) {}
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
		
	AIS_Data myData;
	myData.dacFI = "DAC_FI: 1_31";
	myData.MMSI = myMetHydro.mmsi;
	myData.lat = myMetHydro.position.lat_deg;
	myData.lon = myMetHydro.position.lng_deg;

	myData.utc_day = myMetHydro.utc_day;
	myData.utc_hour = myMetHydro.utc_hour;
	myData.utc_min = myMetHydro.utc_min;

	myData.water_level = myMetHydro.water_level;
	myData.wind_dir = myMetHydro.wind_dir;
	myData.wind_speed = myMetHydro.wind_ave;
	myData.air_press = myMetHydro.air_pres;

	myData.site_id = 999;

	wxArrayString myWaypoints = GetWaypointGUIDArray();
	
	wxString str[1000];
    wxString guid = "";
	for (unsigned int i = 1; i < myWaypoints.size(); i++) {
		str[i] = myWaypoints[i];
		PlugIn_Waypoint_Ex  wayPoint;
		GetSingleWaypointEx(str[i], &wayPoint);
        wxString sMMSI = wxString::Format("%i", myData.MMSI);
		if (wayPoint.m_GUID == sMMSI) {			
			AISTargetList->erase(guid);
			bool d = DeleteSingleWaypoint(guid);
			break;
		}
	}
		
	myData.description = MakeDescription(myData);	

	wxString myMMSI = wxString::Format("%i", myData.MMSI);
	
    PlugIn_Waypoint_Ex* wayPoint = new PlugIn_Waypoint_Ex(myData.lat, myData.lon, "", "", myMMSI);	
	wayPoint->m_MarkDescription = myData.description;
	wayPoint->IsNameVisible = false;
	
	wayPoint->m_MarkName = myMMSI;
	wayPoint->IconName = "green-pin";

	AddSingleWaypointEx(wayPoint, false);       
    m_aMMSI.Add(myMMSI);

    plugin->m_pDialog->m_textCtrlTest->SetValue(m_message);

    if (m_bHaveDisplay) {
        if (myAISdisplay->m_tbAISPause->GetValue()) {
             plugin->m_pDialog->myAISdisplay->m_tcAIS->AppendText(m_message);
        }
    }

	GetParent()->Refresh();			
}

void Dlg::getAis8_1_26(string rawPayload) {
  
	const char* payload = rawPayload.c_str();	
	mylibais::Ais8_1_26 my26MetHydro(payload, 0);

	AIS_Data myData;
	myData.dacFI = "DAC_FI: 1_26";
	myData.MMSI = my26MetHydro.mmsi;
	
	myData.water_level = 999;
	myData.wind_dir = 999;
	myData.wind_speed = 999;
	myData.site_id = 999;

	vector<mylibais::Ais8_1_26_SensorReport *> reports = my26MetHydro.reports;

	for (std::vector <mylibais::Ais8_1_26_SensorReport*>::iterator it = reports.begin();		
		it != reports.end(); it++) {	

		int type = (*it)->getType();
		wxString ty = wxString::Format("%i", type);
		//wxMessageBox(ty);
		if ((*it)->report_type == AIS8_1_26_SENSOR_WIND) {
			mylibais::Ais8_1_26_Wind *rpt = dynamic_cast<Ais8_1_26_Wind *>((*it));	
			myData.wind_dir = rpt->wind_dir;
			myData.wind_speed = rpt->wind_speed;
		}

		if ((*it)->report_type == AIS8_1_26_SENSOR_WATER_LEVEL){
		    mylibais::Ais8_1_26_WaterLevel *rpt = dynamic_cast<Ais8_1_26_WaterLevel *>((*it));	
			myData.water_level = rpt->level;
		}

		if ((*it)->report_type == AIS8_1_26_SENSOR_LOCATION) {
			
			mylibais::Ais8_1_26_Location *rpt = dynamic_cast<Ais8_1_26_Location *>((*it));
			AisPoint myLocation = rpt->position;
			myData.site_id = rpt->site_id;

			myData.utc_day = rpt->utc_day;
			myData.utc_hour = rpt->utc_hr;
			myData.utc_min = rpt->utc_min;

			myData.lat = myLocation.lat_deg;
			myData.lon = myLocation.lng_deg;

			wxArrayString myWaypoints = GetWaypointGUIDArray();
	
			wxString str[1000];
			wxString guid = "";
			for (unsigned int i = 1; i < myWaypoints.size(); i++) {
				str[i] = myWaypoints[i];
				PlugIn_Waypoint_Ex  wayPoint;
				GetSingleWaypointEx(str[i], &wayPoint);                                
                wxString sMMSI = wxString::Format("%i", myData.MMSI);
				if (wayPoint.m_GUID == sMMSI) {
					guid = wayPoint.m_GUID;
					AISTargetList->erase(guid);
					bool d = DeleteSingleWaypoint(guid);
					break;
				}
			}
		}		
	}
	
	myData.description = MakeDescription(myData);
    wxString myMMSI = wxString::Format("%i", myData.MMSI);
	
	PlugIn_Waypoint_Ex*  wayPoint = new PlugIn_Waypoint_Ex(myData.lat, myData.lon, "", "", myMMSI);
	wayPoint->m_MarkDescription = myData.description;
	wayPoint->IsNameVisible = false;

	wayPoint->m_MarkName = myMMSI;
	wayPoint->IconName = "green-pin";

	AddSingleWaypointEx(wayPoint, false);
    m_aMMSI.Add(myMMSI);

    plugin->m_pDialog->m_textCtrlTest->SetValue(m_message);

    if (m_bHaveDisplay) {
                if (myAISdisplay->m_tbAISPause->GetValue()) {
                        plugin->m_pDialog->myAISdisplay->m_tcAIS->AppendText(
                            m_message);
                }
    }


	GetParent()->Refresh();

}


void Dlg::getAis8_367_33(string rawPayload) {

	const char* payload = rawPayload.c_str();
	mylibais::Ais8_367_33 my366MetHydro(payload, 0);

	AIS_Data myData;
	myData.dacFI = "DAC_FI: 367_33";
	myData.MMSI = my366MetHydro.mmsi;

	myData.water_level = 999;
	myData.wind_dir = 999;
	myData.wind_speed = 999;
	myData.site_id = 999;

	for (std::vector<unique_ptr <mylibais::Ais8_367_33_SensorReport>>::iterator it = my366MetHydro.reports.begin();
		it != my366MetHydro.reports.end(); it++) {

		if ((*it)->report_type == AIS8_367_33_SENSOR_WIND) {
			mylibais::Ais8_367_33_Wind *rpt = dynamic_cast<Ais8_367_33_Wind *>((*it).get());	
			myData.wind_dir = rpt->wind_dir;
			myData.wind_speed = rpt->wind_speed;
		}

		if ((*it)->report_type == AIS8_367_33_SENSOR_WATER_LEVEL){
		    mylibais::Ais8_367_33_WaterLevel *rpt = dynamic_cast<Ais8_367_33_WaterLevel *>((*it).get());	
			myData.water_level = rpt->level;
		}

		
		if ((*it)->report_type == AIS8_367_33_SENSOR_LOCATION) {
			mylibais::Ais8_367_33_Location *rpt = dynamic_cast<Ais8_367_33_Location *>((*it).get());
			AisPoint myLocation = rpt->position;
			
			myData.lat = myLocation.lat_deg;
			myData.lon = myLocation.lng_deg;
			myData.site_id = rpt->site_id;

			myData.utc_day = rpt->utc_day;
			myData.utc_hour = rpt->utc_hr;
			myData.utc_min = rpt->utc_min;


			wxArrayString myWaypoints = GetWaypointGUIDArray();
	
			wxString str[1000];
			wxString guid = "";
			for (unsigned int i = 1; i < myWaypoints.size(); i++) {
				str[i] = myWaypoints[i];
				PlugIn_Waypoint_Ex  wayPoint;
				GetSingleWaypointEx(str[i], &wayPoint);
                wxString sMMSI = wxString::Format("%i", myData.MMSI);
				if (wayPoint.m_GUID == sMMSI) {
					guid = wayPoint.m_GUID;
					AISTargetList->erase(guid);
					bool d = DeleteSingleWaypoint(guid);
					break;
				}
			}			
		}
	}

	myData.description = MakeDescription(myData);		
	wxString myMMSI = wxString::Format("%i", myData.MMSI);

	PlugIn_Waypoint_Ex*  wayPoint = new PlugIn_Waypoint_Ex(myData.lat, myData.lon, "", "", myMMSI);
	wayPoint->m_MarkDescription = myData.description;
	wayPoint->IsNameVisible = false;

	wayPoint->m_MarkName = myMMSI;
	wayPoint->IconName = "green-pin";

	AddSingleWaypointEx(wayPoint, false);
    m_aMMSI.Add(myMMSI);

    plugin->m_pDialog->m_textCtrlTest->SetValue(m_message);

    if (m_bHaveDisplay) {
                if (myAISdisplay->m_tbAISPause->GetValue()) {
                        plugin->m_pDialog->myAISdisplay->m_tcAIS->AppendText(
                            m_message);
                }
    }


	GetParent()->Refresh();

}



void Dlg::CreateControlsMessageList()
{
	int width;
	int dx = 20;

	width = dx * 4;
	if (m_pASMmessages1) {
		m_pASMmessages1->m_pListCtrlAISTargets->InsertColumn(tlTRK, _("AIS messages"), wxLIST_FORMAT_LEFT, width);
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

