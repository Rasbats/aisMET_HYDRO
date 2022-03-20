// 8:367:22 Defined by an email from Greg Johnson representing the
// USCG, Fall 2012.  Breaks from the RTCM and IMO Circular 289.
// "Area Notice Message Release Version: 1" 13 Aug 2012
//
// http://www.e-navigation.nl/content/geographic-notice
// http://www.e-navigation.nl/sites/default/files/asm_files/GN%20Release%20Version%201b.pdf
//
// 8:367:23
// https://www.e-navigation.nl/content/satellite-ship-weather
// 8:367:24
// https://www.e-navigation.nl/content/satellite-ship-weather-small
// 8:367:25
// https://www.e-navigation.nl/content/satellite-ship-weather-tiny

#include <cmath>
#include <string>

#include "ais.h"

namespace mylibais {

const size_t SUB_AREA_BITS = 96;

static int scale_multipliers[4] = {1, 10, 100, 1000};




Ais8_367_33::Ais8_367_33(const char *nmea_payload, const size_t pad)
    : Ais8(nmea_payload, pad) {
  assert(dac == 367);
  assert(fi == 33);

  if (!CheckStatus()) {
    return;
  }
  if (num_bits  < 168 || num_bits > 952) {
    status = AIS_ERR_BAD_BIT_COUNT;
    return;
  }

  const size_t report_start = 56;
  const size_t num_sensor_reports = (num_bits - report_start) / AIS8_367_33_REPORT_SIZE;

  const size_t extra_bits = (num_bits - report_start) % AIS8_367_33_REPORT_SIZE;
  if (extra_bits > 0) {
    status = AIS_ERR_BAD_BIT_COUNT;
    return;
  }

  for (size_t report_idx = 0; report_idx < num_sensor_reports; report_idx++) {
    const size_t start = report_start + (report_idx * AIS8_367_33_REPORT_SIZE);
    bits.SeekTo(start);
    std::unique_ptr<Ais8_367_33_SensorReport> sensor =
        ais8_367_33_sensor_report_factory(bits, start);
    if (sensor) {
      reports.push_back(std::move(sensor));
    } else {
      status = AIS_ERR_BAD_SUB_MSG;
      return;
    }
  }

  status = AIS_OK;
}

ostream& operator<< (ostream &o, const Ais8_367_33 &msg) {
  const int num_reports = msg.reports.size();
  for (int report_idx = 0; report_idx < num_reports; report_idx++) {
    Ais8_367_33_SensorEnum report_type = msg.reports[report_idx]->report_type;
    switch(report_type) {
      case AIS8_367_33_SENSOR_LOCATION: {
        Ais8_367_33_Location *rpt = dynamic_cast<Ais8_367_33_Location *>(msg.reports[report_idx].get());

        o << " [report_type: " << rpt->report_type << " day: " << rpt->utc_day << " hour: " << rpt->utc_hr << " min: " << rpt->utc_min << " site: " << rpt->site_id;
        o << " version: " << rpt->version << " position: " << rpt->position << " precision: " << rpt->precision << " altitude: " << rpt->altitude << " owner: " << rpt->owner << "]";
        break;
      }
      case AIS8_367_33_SENSOR_STATION: {
        Ais8_367_33_Station *rpt = dynamic_cast<Ais8_367_33_Station *>(msg.reports[report_idx].get());

        o << " [report_type: " << rpt->report_type << " day: " << rpt->utc_day << " hour: " << rpt->utc_hr << " min: " << rpt->utc_min << " site: " << rpt->site_id;
        o << " name: " << rpt->name << "]";
        break;
      }
      case AIS8_367_33_SENSOR_WIND: {
        Ais8_367_33_Wind *rpt = dynamic_cast<Ais8_367_33_Wind *>(msg.reports[report_idx].get());

        o << " [report_type: " << rpt->report_type << " day: " << rpt->utc_day << " hour: " << rpt->utc_hr << " min: " << rpt->utc_min << " site: " << rpt->site_id;
        o << " speed: " << rpt->wind_speed << " gust: " << rpt->wind_gust << " dir: " << rpt->wind_dir;
        o << " wind_gust_dir: " << rpt->wind_gust_dir << " sensor_type: " << rpt->sensor_type;
        o << " wind_forecast: " << rpt->wind_forecast << " wind_gust_forecast: " << rpt->wind_gust_forecast << " wind_dir_forecast: " << rpt->wind_dir_forecast;
        o << " utc_day_forecast: " << rpt->utc_day_forecast << " utc_hour_forecast: " << rpt->utc_hour_forecast << " utc_min_forecast: " << rpt->utc_min_forecast << " duration: " << rpt->duration << "]";
        break;
      }
      case AIS8_367_33_SENSOR_WATER_LEVEL: {
        Ais8_367_33_WaterLevel *rpt = dynamic_cast<Ais8_367_33_WaterLevel *>(msg.reports[report_idx].get());

        o << " [report_type: " << rpt->report_type << " day: " << rpt->utc_day << " hour: " << rpt->utc_hr << " min: " << rpt->utc_min << " site: " << rpt->site_id;
        o << " type: " << rpt->type << " level: " << rpt->level << " trend: " << rpt->trend << " vdatum: " << rpt->vdatum;
        o << " sensor_type: " << rpt->sensor_type << " forecast_type: " << rpt->forecast_type;
        o << " level_forecast: " << rpt->level_forecast;
        o << " utc_day_forecast: " << rpt->utc_day_forecast << " utc_hour_forecast: " << rpt->utc_hour_forecast << " utc_min_forecast: " << rpt->utc_min_forecast << " duration: " << rpt->duration << "]";
        break;
      }

      default: break;
    } // switch
  } // for
  

}  // namespace libais
