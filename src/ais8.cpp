// Binary Broadcast Message (BBM) - 8

// See also: http://www.e-navigation.nl/asm

#include <iomanip>

#include "ais.h"
using namespace std;

namespace mylibais {

	Ais8::Ais8(const char *nmea_payload, const size_t pad)
		: AisMsg(nmea_payload, pad), spare(0), dac(0), fi(0) {
		if (!CheckStatus()) {
			return;
		}

		// in bits w/o DAC/FI
		// TODO(schwehr): Verify if this is 46 or 56 (accumulated bits below).
		const int payload_len = num_bits - 56;
		if (payload_len < 0 || payload_len > 952) {
			status = AIS_ERR_BAD_BIT_COUNT;
			return;
		}

		assert(message_id == 8);

		bits.SeekTo(38);
		spare = bits.ToUnsignedInt(38, 2);
		dac = bits.ToUnsignedInt(40, 10);
		fi = bits.ToUnsignedInt(50, 6);
	}

	// IMO Circ 289 - Meteorological and Hydrographic data
// See also Circ 236
	Ais8_1_31::Ais8_1_31(const char *nmea_payload, const size_t pad)
		: Ais8(nmea_payload, pad), position_accuracy(0), utc_day(0), utc_hour(0),
		  utc_min(0), wind_ave(0), wind_gust(0), wind_dir(0), wind_gust_dir(0),
		  air_temp(0.0), rel_humid(0), dew_point(0.0), air_pres(0.0),
		  air_pres_trend(0), horz_vis(0.0), water_level(0.0),
		  water_level_trend(0), surf_cur_speed(0.0), surf_cur_dir(0),
		  cur_speed_2(0.0), cur_dir_2(0), cur_depth_2(0), cur_speed_3(0.0),
		  cur_dir_3(0), cur_depth_3(0), wave_height(0.0), wave_period(0),
		  wave_dir(0), swell_height(0.0), swell_period(0), swell_dir(0),
		  sea_state(0), water_temp(0.0), precip_type(0), salinity(0.0),
		  ice(0), spare2(0) {
	  assert(dac == 1);
	  assert(fi == 31);
	  /*
	  if (!CheckStatus()) {
		return;
	  }
	  if (num_bits != 360) {
		status = AIS_ERR_BAD_BIT_COUNT;
		return;
	  }*/

	  bits.SeekTo(56);
	  position = bits.ToAisPoint(56, 49);
	  position_accuracy = bits[105];
	  utc_day = bits.ToUnsignedInt(106, 5);
	  utc_hour = bits.ToUnsignedInt(111, 5);
	  utc_min = bits.ToUnsignedInt(116, 6);

	  wind_ave = bits.ToUnsignedInt(122, 7);  // kts
	  wind_gust = bits.ToUnsignedInt(129, 7);  // kts
	  wind_dir = bits.ToUnsignedInt(136, 9);
	  wind_gust_dir = bits.ToUnsignedInt(145, 9);
	  air_temp = bits.ToInt(154, 11) / 10.;  // C
	  rel_humid = bits.ToUnsignedInt(165, 7);
	  dew_point = bits.ToInt(172, 10)/ 10.;  // TODO(schwehr): How is this mapped?
	  air_pres = (bits.ToUnsignedInt(182, 9) + 800);  // Pa
	  air_pres_trend = bits.ToUnsignedInt(191, 2);

	  horz_vis = bits.ToUnsignedInt(193, 8) / 10.;  // NM
	  water_level = bits.ToUnsignedInt(201, 12);  // m
	  water_level_trend = bits.ToUnsignedInt(213, 2);

	  surf_cur_speed = bits.ToUnsignedInt(215, 8);
	  surf_cur_dir = bits.ToUnsignedInt(223, 9);
	  cur_speed_2 = bits.ToUnsignedInt(232, 8) / 10.;  // kts
	  cur_dir_2 = bits.ToUnsignedInt(240, 9);
	  cur_depth_2 = bits.ToUnsignedInt(249, 5);  // m
	  cur_speed_3 = bits.ToUnsignedInt(254, 8) / 10.;  // kts
	  cur_dir_3 = bits.ToUnsignedInt(262, 9);
	  cur_depth_3 = bits.ToUnsignedInt(271, 5);  // m

	  wave_height = bits.ToUnsignedInt(276, 8);  // m

	  wave_period = bits.ToUnsignedInt(284, 6);
	  wave_dir = bits.ToUnsignedInt(290, 9);
	  swell_height = bits.ToUnsignedInt(299, 8) / 10.;
	  swell_period = bits.ToUnsignedInt(307, 6);
	  swell_dir = bits.ToUnsignedInt(313, 9);
	  sea_state = bits.ToUnsignedInt(322, 4);  // beaufort scale - Table 1.2
	  water_temp = bits.ToInt(326, 10) / 10.;
	  precip_type = bits.ToUnsignedInt(336, 3);
	  salinity = bits.ToUnsignedInt(339, 9) / 10.;
	  ice = bits.ToUnsignedInt(348, 2);  // yes/no/undef/unknown
	  spare2 = bits.ToUnsignedInt(350, 10);

	 // assert(bits.GetRemaining() == 0);
	 // status = AIS_OK;
	}

	//********** Weather FI 11 ************************

	Ais8_1_11::Ais8_1_11(const char *nmea_payload, const size_t pad)
		: Ais8(nmea_payload, pad), day(0), hour(0), minute(0), wind_ave(0),
		wind_gust(0), wind_dir(0), wind_gust_dir(0), air_temp(0.0), rel_humid(0),
		dew_point(0.0), air_pres(0.0), air_pres_trend(0), horz_vis(0.0),
		water_level(0.0), water_level_trend(0), surf_cur_speed(0.0),
		surf_cur_dir(0), cur_speed_2(0.0), cur_dir_2(0), cur_depth_2(0),
		cur_speed_3(0.0), cur_dir_3(0), cur_depth_3(0), wave_height(0.0),
		wave_period(0), wave_dir(0), swell_height(0.0), swell_period(0),
		swell_dir(0), sea_state(0), water_temp(0.0), precip_type(0),
		salinity(0.0), ice(0), spare2(0), extended_water_level(0) {

		assert(dac == 1);
		assert(fi == 11);
		bits.SeekTo(56);
		// Reverse order lat/lng!
		// TODO(schwehr): Reverse order, or just reverse bit count? Compare 6.1.14!
		double y = bits.ToInt(56, 24) / 60000.0;
		double x = bits.ToInt(80, 25) / 60000.0;
		position = AisPoint(x, y);

		day = bits.ToUnsignedInt(105, 5);
		hour = bits.ToUnsignedInt(110, 5);
		minute = bits.ToUnsignedInt(115, 6);
		wind_ave = bits.ToUnsignedInt(121, 7);
		wind_gust = bits.ToUnsignedInt(128, 7);
		wind_dir = bits.ToUnsignedInt(135, 9);
		wind_gust_dir = bits.ToUnsignedInt(144, 9);
		air_temp = bits.ToUnsignedInt(153, 11) / 10. - 60;
		rel_humid = bits.ToUnsignedInt(164, 7);
		dew_point = bits.ToUnsignedInt(171, 10) / 10. - 20;  // TODO(schwehr): verify
		air_pres = bits.ToUnsignedInt(181, 9) + 800;
		air_pres_trend = bits.ToUnsignedInt(190, 2);
		horz_vis = bits.ToUnsignedInt(192, 8) / 10.;
		// TODO(schwehr): verify for -10.0 to 30.0
		water_level = bits.ToUnsignedInt(200, 9) / 10. - 10;
		water_level_trend = bits.ToUnsignedInt(209, 2);
		surf_cur_speed = bits.ToUnsignedInt(211, 8) / 10.;
		surf_cur_dir = bits.ToUnsignedInt(219, 9);
		cur_speed_2 = bits.ToUnsignedInt(228, 8) / 10.;
		cur_dir_2 = bits.ToUnsignedInt(236, 9);
		cur_depth_2 = bits.ToUnsignedInt(245, 5);
		cur_speed_3 = bits.ToUnsignedInt(250, 8) / 10.;
		cur_dir_3 = bits.ToUnsignedInt(258, 9);
		cur_depth_3 = bits.ToUnsignedInt(267, 5);

		wave_height = bits.ToUnsignedInt(272, 8) / 10.;
		wave_period = bits.ToUnsignedInt(280, 6);
		wave_dir = bits.ToUnsignedInt(286, 9);
		swell_height = bits.ToUnsignedInt(295, 8) / 10.;
		swell_period = bits.ToUnsignedInt(303, 6);
		swell_dir = bits.ToUnsignedInt(309, 9);

		sea_state = bits.ToUnsignedInt(318, 4);
		// TODO(schwehr): verify for -10.0 to +50.0
		water_temp = bits.ToUnsignedInt(322, 10) / 10. - 10;
		precip_type = bits.ToUnsignedInt(332, 3);
		salinity = bits.ToUnsignedInt(335, 9) / 10.0;  // Part per mil (1/1000).
		ice = bits.ToUnsignedInt(344, 2);
		// There is no way to know which meaning to attach to the following 6 bits
		// TODO(schwehr): how to treat this spare vrs water level?
		spare2 = bits.ToUnsignedInt(346, 6);
		bits.SeekRelative(-6);
		extended_water_level = bits.ToUnsignedInt(346, 6);

		//assert(bits.GetRemaining() == 0);
		//status = AIS_OK;

	}
// Environmental Message
// Environmental Message
Ais8_1_26_Location::Ais8_1_26_Location(const AisBitset &bits,
                                       const size_t offset) {
  position = bits.ToAisPoint(offset, 55);
  z = bits.ToUnsignedInt(offset + 55, 11) / 10.;
  owner = bits.ToUnsignedInt(offset + 66, 4);
  timeout = bits.ToUnsignedInt(offset + 70, 3);
  spare = bits.ToUnsignedInt(offset + 73, 12);
}

Ais8_1_26_Station::Ais8_1_26_Station(const AisBitset &bits,
                                     const size_t offset) {
  name = bits.ToString(offset, 84);
  spare = bits.ToUnsignedInt(offset + 84, 1);
}

Ais8_1_26_Wind::Ais8_1_26_Wind(const AisBitset &bits,
                               const size_t offset) {
  wind_speed = bits.ToUnsignedInt(offset, 7);
  wind_gust  = bits.ToUnsignedInt(offset + 7, 7);  // knots
  wind_dir = bits.ToUnsignedInt(offset + 14, 9);
  wind_gust_dir = bits.ToUnsignedInt(offset + 23, 9);
  sensor_type = bits.ToUnsignedInt(offset + 32, 3);
  wind_forecast = bits.ToUnsignedInt(offset + 35, 7);
  wind_gust_forecast = bits.ToUnsignedInt(offset + 42, 7);  // knots
  wind_dir_forecast = bits.ToUnsignedInt(offset + 49, 9);
  utc_day_forecast = bits.ToUnsignedInt(offset + 58, 5);
  utc_hour_forecast = bits.ToUnsignedInt(offset + 63, 5);
  utc_min_forecast = bits.ToUnsignedInt(offset + 68, 6);
  duration = bits.ToUnsignedInt(offset + 74, 8);
  spare = bits.ToUnsignedInt(offset + 82, 3);
}

Ais8_1_26_WaterLevel::Ais8_1_26_WaterLevel(const AisBitset &bits,
                                           const size_t offset) {
  type = bits[offset];
  level = bits.ToInt(offset + 1, 16) / 100.;
  trend = bits.ToUnsignedInt(offset + 17, 2);
  vdatum = bits.ToUnsignedInt(offset + 19, 5);
  sensor_type = bits.ToUnsignedInt(offset + 24, 3);
  forecast_type = bits[offset + 27];
  level_forecast = bits.ToInt(offset + 28, 16) / 100.;
  utc_day_forecast = bits.ToUnsignedInt(offset + 44, 5);
  utc_hour_forecast = bits.ToUnsignedInt(offset + 49, 5);
  utc_min_forecast = bits.ToUnsignedInt(offset + 54, 6);
  duration = bits.ToUnsignedInt(offset + 60, 8);
  spare = bits.ToUnsignedInt(offset + 68, 17);
}

Ais8_1_26_Curr2D::Ais8_1_26_Curr2D(const AisBitset &bits,
                                   const size_t offset) {
  for (size_t idx = 0; idx < 3; idx++) {
    size_t start = offset + idx * 26;
    currents[idx].speed = bits.ToUnsignedInt(start, 8) / 10.;
    currents[idx].dir = bits.ToUnsignedInt(start + 8, 9);
    currents[idx].depth = bits.ToUnsignedInt(start + 17, 9);
  }
  type = bits.ToUnsignedInt(offset + 78, 3);
  spare = bits.ToUnsignedInt(offset + 81, 4);
}

Ais8_1_26_Curr3D::Ais8_1_26_Curr3D(const AisBitset &bits,
                                   const size_t offset) {
  for (size_t idx = 0; idx < 2; idx++) {
    size_t start = offset + idx * 33;
    currents[idx].north = bits.ToUnsignedInt(start, 8) / 10.;
    currents[idx].east = bits.ToUnsignedInt(start + 8, 8) / 10.;
    currents[idx].up = bits.ToUnsignedInt(start + 16, 8) / 10.;
    currents[idx].depth = bits.ToUnsignedInt(start + 24, 9);
  }
  type = bits.ToUnsignedInt(offset + 66, 3);
  spare = bits.ToUnsignedInt(offset + 69, 16);
}

Ais8_1_26_HorzFlow::Ais8_1_26_HorzFlow(const AisBitset &bits,
                                       const size_t offset) {
  for (size_t idx = 0; idx < 2; idx++) {
    size_t start = offset + idx * 42;
    currents[idx].bearing = bits.ToUnsignedInt(start, 9);
    currents[idx].dist = bits.ToUnsignedInt(start + 9, 7);
    currents[idx].speed = bits.ToUnsignedInt(start + 16, 8) / 10.;
    currents[idx].dir = bits.ToUnsignedInt(start + 24, 9);
    currents[idx].level = bits.ToUnsignedInt(start + 33, 9);
  }
  spare = bits[offset + 84];
}

Ais8_1_26_SeaState::Ais8_1_26_SeaState(const AisBitset &bits,
                                       const size_t offset) {
  swell_height = bits.ToUnsignedInt(offset, 8) / 10.;
  swell_period = bits.ToUnsignedInt(offset + 8, 6);
  swell_dir = bits.ToUnsignedInt(offset + 14, 9);
  sea_state = bits.ToUnsignedInt(offset + 23, 4);
  swell_sensor_type = bits.ToUnsignedInt(offset + 27, 3);
  water_temp = bits.ToInt(offset + 30, 10) / 10.;
  water_temp_depth = bits.ToUnsignedInt(offset + 40, 7) / 10.;
  water_sensor_type = bits.ToUnsignedInt(offset + 47, 3);
  wave_height = bits.ToUnsignedInt(offset + 50, 8) / 10.;
  wave_period = bits.ToUnsignedInt(offset + 58, 6);
  wave_dir = bits.ToUnsignedInt(offset + 64, 9);
  wave_sensor_type = bits.ToUnsignedInt(offset + 73, 3);
  salinity = bits.ToUnsignedInt(offset + 76, 9) / 10.;
}

Ais8_1_26_Salinity::Ais8_1_26_Salinity(const AisBitset &bits,
                                       const size_t offset) {
  water_temp = bits.ToUnsignedInt(offset, 10) / 10. - 10;
  conductivity = bits.ToUnsignedInt(offset + 10, 10) / 100.;
  pressure = bits.ToUnsignedInt(offset + 20, 16) / 10.;
  salinity = bits.ToUnsignedInt(offset + 36, 9) / 10.;
  salinity_type = bits.ToUnsignedInt(offset + 45, 2);
  sensor_type = bits.ToUnsignedInt(offset + 47, 3);
  spare[0] = bits.ToUnsignedInt(offset + 50, 32);
  spare[1] = bits.ToUnsignedInt(offset + 82, 3);
}

Ais8_1_26_Wx::Ais8_1_26_Wx(const AisBitset &bits,
                           const size_t offset) {
  air_temp = bits.ToInt(offset, 11) / 10.;
  air_temp_sensor_type = bits.ToUnsignedInt(offset + 11, 3);
  precip = bits.ToUnsignedInt(offset + 14, 2);
  horz_vis = bits.ToUnsignedInt(offset + 16, 8) / 10.;
  dew_point = bits.ToInt(offset + 24, 10) / 10.;
  dew_point_type = bits.ToUnsignedInt(offset + 34, 3);
  air_pressure = (bits.ToUnsignedInt(offset + 37, 9) + 800) / 100.0;  // Pa.
  air_pressure_trend = bits.ToUnsignedInt(offset + 46, 2);
  air_pressor_type = bits.ToUnsignedInt(offset + 48, 3);
  salinity = bits.ToUnsignedInt(offset + 51, 9) / 10.;
  spare = bits.ToUnsignedInt(offset + 60, 25);
}

Ais8_1_26_AirDraught::Ais8_1_26_AirDraught(const AisBitset &bits,
                                           const size_t offset) {
  draught = bits.ToUnsignedInt(offset, 13) / 100.;
  gap = bits.ToUnsignedInt(offset + 13, 13) / 10.;
  trend = bits.ToUnsignedInt(offset + 26, 2);
  forecast_gap = bits.ToUnsignedInt(offset + 28, 13) / 10.;
  utc_day_forecast = bits.ToUnsignedInt(offset + 41, 5);
  utc_hour_forecast = bits.ToUnsignedInt(offset + 46, 5);
  utc_min_forecast = bits.ToUnsignedInt(offset + 51, 6);
  spare = bits.ToUnsignedInt(offset + 57, 28);
}

// TODO(schwehr): Refactor to be like the 8:367:22 factory.
Ais8_1_26_SensorReport*
ais8_1_26_sensor_report_factory(const AisBitset &bits,
                                const size_t offset) {
  const Ais8_1_26_SensorEnum rpt_type =
      (Ais8_1_26_SensorEnum)bits.ToUnsignedInt(offset, 4);

  // WARNING: out of order decoding
  // Only get the report header if we can decode the type
  const size_t rpt_start = offset + 27;  // skip tp after site_id
  bits.SeekTo(rpt_start);
  Ais8_1_26_SensorReport *rpt = nullptr;
  switch (rpt_type) {
  case AIS8_1_26_SENSOR_LOCATION:
    rpt = new Ais8_1_26_Location(bits, rpt_start);
    break;
  case AIS8_1_26_SENSOR_STATION:
    rpt = new Ais8_1_26_Station(bits, rpt_start);
    break;
  case AIS8_1_26_SENSOR_WIND:
    rpt = new Ais8_1_26_Wind(bits, rpt_start);
    break;
  case AIS8_1_26_SENSOR_WATER_LEVEL:
    rpt = new Ais8_1_26_WaterLevel(bits, rpt_start);
    break;
  case AIS8_1_26_SENSOR_CURR_2D:
    rpt = new Ais8_1_26_Curr2D(bits, rpt_start);
    break;
  case AIS8_1_26_SENSOR_CURR_3D:
    rpt = new Ais8_1_26_Curr3D(bits, rpt_start);
    break;
  case AIS8_1_26_SENSOR_HORZ_FLOW:
    rpt = new Ais8_1_26_HorzFlow(bits, rpt_start);
    break;
  case AIS8_1_26_SENSOR_SEA_STATE:
    rpt = new Ais8_1_26_SeaState(bits, rpt_start);
    break;
  case AIS8_1_26_SENSOR_SALINITY:
    rpt = new Ais8_1_26_Salinity(bits, rpt_start);
    break;
  case AIS8_1_26_SENSOR_WX:
    rpt = new Ais8_1_26_Wx(bits, rpt_start);
    break;
  case AIS8_1_26_SENSOR_AIR_DRAUGHT:
    rpt = new Ais8_1_26_AirDraught(bits, rpt_start);
    break;
  case AIS8_1_26_SENSOR_RESERVED_11: break;  // Leave rpt == 0 to indicate error
  case AIS8_1_26_SENSOR_RESERVED_12: break;
  case AIS8_1_26_SENSOR_RESERVED_13: break;
  case AIS8_1_26_SENSOR_RESERVED_14: break;
  case AIS8_1_26_SENSOR_RESERVED_15: break;
  default:
    {}  // Leave rpt == 0 to indicate error
  }

  if (!rpt)
    return rpt;

  rpt->report_type = rpt_type;
  bits.SeekTo(offset + 4);
  rpt->utc_day = bits.ToUnsignedInt(offset + 4, 5);
  rpt->utc_hr = bits.ToUnsignedInt(offset + 9, 5);
  rpt->utc_min = bits.ToUnsignedInt(offset + 14, 6);
  rpt->site_id = bits.ToUnsignedInt(offset + 20, 7);
  return rpt;
}

Ais8_1_26::Ais8_1_26(const char *nmea_payload, const size_t pad)
    : Ais8(nmea_payload, pad) {
  assert(dac == 1);
  assert(fi == 26);

  if (!CheckStatus()) {
    return;
  }
  if (168 > num_bits || num_bits > 1098) {
    status = AIS_ERR_BAD_BIT_COUNT;
    return;
  }

  const size_t num_sensor_reports = (num_bits - 56) / AIS8_1_26_REPORT_SIZE;

  // TODO(schwehr): what to do about extra data in sensor report msg 8_1_26?
  // if ((num_bits - 56) % AIS8_1_26_REPORT_SIZE)

  for (size_t report_idx = 0; report_idx < num_sensor_reports; report_idx++) {
    const size_t start = 56 + report_idx * AIS8_1_26_REPORT_SIZE;
    bits.SeekTo(start);
    Ais8_1_26_SensorReport *sensor =
        ais8_1_26_sensor_report_factory(bits, start);
    if (sensor) {
      reports.push_back(sensor);
    } else {
      status = AIS_ERR_BAD_SUB_SUB_MSG;
      return;
    }
  }

  // TODO(schwehr): Enable this assert after fixing the message.
  // assert(bits.GetRemaining() == 0);
  status = AIS_OK;
}

// TODO(schwehr): Use unique_ptr to manage memory.
Ais8_1_26::~Ais8_1_26() {
  for (size_t i = 0; i < reports.size(); i++) {
    delete reports[i];
    reports[i] = nullptr;
  }
}





// Environmental Message
// Environmental Message
Ais8_367_33_Location::Ais8_367_33_Location(const AisBitset &bits,
                                           const size_t offset) {
  version = bits.ToUnsignedInt(offset, 6);
  position = bits.ToAisPoint(offset + 6, 55);

  precision = bits.ToUnsignedInt(offset + 61, 3);

  altitude_raw = bits.ToUnsignedInt(offset + 64, 12);
  altitude = altitude_raw / 10.;  // meters

  owner = bits.ToUnsignedInt(offset + 76, 4);
  timeout = bits.ToUnsignedInt(offset + 80, 3);
  spare2 = bits.ToUnsignedInt(offset + 83, 2);
}

Ais8_367_33_Station::Ais8_367_33_Station(const AisBitset &bits,
                                         const size_t offset) {
  name = bits.ToString(offset, 84);
  // Remove any trailing '@' characters.  Spec says they are not to be shown in the presentation.
  while (!name.empty() && name.back() == '@') {
    name.pop_back();
  }
  spare2 = bits.ToUnsignedInt(offset + 84, 1);
}

Ais8_367_33_Wind::Ais8_367_33_Wind(const AisBitset &bits,
                                   const size_t offset) {
  wind_speed = bits.ToUnsignedInt(offset, 7);
  wind_gust  = bits.ToUnsignedInt(offset + 7, 7);  // knots
  wind_dir = bits.ToUnsignedInt(offset + 14, 9);
  wind_gust_dir = bits.ToUnsignedInt(offset + 23, 9);
  sensor_type = bits.ToUnsignedInt(offset + 32, 3);
  wind_forecast = bits.ToUnsignedInt(offset + 35, 7);
  wind_gust_forecast = bits.ToUnsignedInt(offset + 42, 7);  // knots
  wind_dir_forecast = bits.ToUnsignedInt(offset + 49, 9);
  utc_day_forecast = bits.ToUnsignedInt(offset + 58, 5);
  utc_hour_forecast = bits.ToUnsignedInt(offset + 63, 5);
  utc_min_forecast = bits.ToUnsignedInt(offset + 68, 6);
  duration = bits.ToUnsignedInt(offset + 74, 8);
  spare2 = bits.ToUnsignedInt(offset + 82, 3);
}

Ais8_367_33_WaterLevel::Ais8_367_33_WaterLevel(const AisBitset &bits,
                                               const size_t offset) {
  type = bits[offset];
  level = bits.ToInt(offset + 1, 16);
  trend = bits.ToUnsignedInt(offset + 17, 2);
  vdatum = bits.ToUnsignedInt(offset + 19, 5);
  sensor_type = bits.ToUnsignedInt(offset + 24, 3);
  forecast_type = bits[offset + 27];
  level_forecast = bits.ToInt(offset + 28, 16);
  utc_day_forecast = bits.ToUnsignedInt(offset + 44, 5);
  utc_hour_forecast = bits.ToUnsignedInt(offset + 49, 5);
  utc_min_forecast = bits.ToUnsignedInt(offset + 54, 6);
  duration = bits.ToUnsignedInt(offset + 60, 8);
  spare2 = bits.ToUnsignedInt(offset + 68, 17);
}

Ais8_367_33_Curr2D::Ais8_367_33_Curr2D(const AisBitset &bits,
                                       const size_t offset) {
  for (size_t idx = 0; idx < 3; idx++) {
    const size_t start = offset + idx * 26;
    currents[idx].speed_raw = bits.ToUnsignedInt(start, 8);
    currents[idx].speed = currents[idx].speed_raw / 10.;
    currents[idx].dir = bits.ToUnsignedInt(start + 8, 9);
    currents[idx].depth = bits.ToUnsignedInt(start + 17, 9);
  }
  type = bits.ToUnsignedInt(offset + 78, 3);
  spare2 = bits.ToUnsignedInt(offset + 81, 4);
}

Ais8_367_33_Curr3D::Ais8_367_33_Curr3D(const AisBitset &bits,
                                       const size_t offset) {
  for (size_t idx = 0; idx < 2; idx++) {
    const size_t start = offset + idx * 36;

    currents[idx].north_raw = bits.ToUnsignedInt(start, 9);
    currents[idx].north = currents[idx].north_raw / 10.;

    currents[idx].east_raw = bits.ToUnsignedInt(start + 9, 9);
    currents[idx].east = currents[idx].east_raw / 10.;

    currents[idx].up_raw = bits.ToUnsignedInt(start + 18, 9);
    currents[idx].up = currents[idx].up_raw / 10.;

    currents[idx].depth = bits.ToUnsignedInt(start + 27, 9);
  }
  type = bits.ToUnsignedInt(offset + 72, 3);
  spare2 = bits.ToUnsignedInt(offset + 75, 10);
}

Ais8_367_33_HorzFlow::Ais8_367_33_HorzFlow(const AisBitset &bits,
                                           const size_t offset) {
  bearing = bits.ToUnsignedInt(offset, 9);
  for (size_t idx = 0; idx < 2; idx++) {
    const size_t start = offset + 9 + (idx * 35);

    currents[idx].dist = bits.ToUnsignedInt(start, 9);

    currents[idx].speed_raw = bits.ToUnsignedInt(start + 9, 8);
    currents[idx].speed = currents[idx].speed_raw / 10.;

    currents[idx].dir = bits.ToUnsignedInt(start + 17, 9);
    currents[idx].level = bits.ToUnsignedInt(start + 26, 9);
  }
  type = bits.ToUnsignedInt(offset + 79, 3);
  spare2 = bits.ToUnsignedInt(offset + 82, 3);
}

Ais8_367_33_SeaState::Ais8_367_33_SeaState(const AisBitset &bits,
                                           const size_t offset) {
  swell_height_raw = bits.ToUnsignedInt(offset, 8);
  swell_height = swell_height_raw / 10.;

  swell_period = bits.ToUnsignedInt(offset + 8, 6);
  swell_dir = bits.ToUnsignedInt(offset + 14, 9);
  sea_state = bits.ToUnsignedInt(offset + 23, 4);
  swell_sensor_type = bits.ToUnsignedInt(offset + 27, 3);

  water_temp_raw = bits.ToInt(offset + 30, 10);
  water_temp = (water_temp_raw / 10.) - 10.0;

  water_temp_depth_raw = bits.ToUnsignedInt(offset + 40, 7);
  water_temp_depth = water_temp_depth_raw / 10.;

  water_sensor_type = bits.ToUnsignedInt(offset + 47, 3);

  wave_height_raw = bits.ToUnsignedInt(offset + 50, 8);
  wave_height = wave_height_raw / 10.;

  wave_period = bits.ToUnsignedInt(offset + 58, 6);
  wave_dir = bits.ToUnsignedInt(offset + 64, 9);
  wave_sensor_type = bits.ToUnsignedInt(offset + 73, 3);

  salinity_raw = bits.ToUnsignedInt(offset + 76, 9);
  salinity = salinity_raw / 10.;
}

Ais8_367_33_Salinity::Ais8_367_33_Salinity(const AisBitset &bits,
                                           const size_t offset) {
  water_temp_raw = bits.ToUnsignedInt(offset, 10);
  water_temp = water_temp_raw / 10. - 10;

  conductivity_raw = bits.ToUnsignedInt(offset + 10, 10);
  conductivity = conductivity_raw / 100.;

  pressure_raw = bits.ToUnsignedInt(offset + 20, 16);
  pressure = pressure_raw / 10.;

  salinity_raw = bits.ToUnsignedInt(offset + 36, 9);
  salinity = salinity_raw / 10.;

  salinity_type = bits.ToUnsignedInt(offset + 45, 2);
  sensor_type = bits.ToUnsignedInt(offset + 47, 3);
  spare2[0] = bits.ToUnsignedInt(offset + 50, 32);
  spare2[1] = bits.ToUnsignedInt(offset + 82, 3);
}

Ais8_367_33_Wx::Ais8_367_33_Wx(const AisBitset &bits,
                               const size_t offset) {
  air_temp_raw = bits.ToInt(offset, 11);
  air_temp = air_temp_raw / 10.;

  air_temp_sensor_type = bits.ToUnsignedInt(offset + 11, 3);
  precip = bits.ToUnsignedInt(offset + 14, 2);

  horz_vis_raw = bits.ToUnsignedInt(offset + 16, 8);
  horz_vis = horz_vis_raw / 10.;

  dew_point_raw = bits.ToUnsignedInt(offset + 24, 10);
  dew_point = (dew_point_raw / 10.) - 20.0;

  dew_point_type = bits.ToUnsignedInt(offset + 34, 3);

  air_pressure_raw = bits.ToUnsignedInt(offset + 37, 9);
  air_pressure = air_pressure_raw + 799;  // hPa.

  air_pressure_trend = bits.ToUnsignedInt(offset + 46, 2);
  air_pressure_sensor_type = bits.ToUnsignedInt(offset + 48, 3);

  salinity_raw = bits.ToUnsignedInt(offset + 51, 9);
  salinity = salinity_raw / 10.;

  spare2 = bits.ToUnsignedInt(offset + 60, 25);
}

Ais8_367_33_AirGap::Ais8_367_33_AirGap(const AisBitset &bits,
                                       const size_t offset) {
  air_draught = bits.ToUnsignedInt(offset, 13);
  air_gap = bits.ToUnsignedInt(offset + 13, 13);
  air_gap_trend = bits.ToUnsignedInt(offset + 26, 2);
  predicted_air_gap = bits.ToUnsignedInt(offset + 28, 13);
  utc_day_forecast = bits.ToUnsignedInt(offset + 41, 5);
  utc_hour_forecast = bits.ToUnsignedInt(offset + 46, 5);
  utc_min_forecast = bits.ToUnsignedInt(offset + 51, 6);
  type = bits.ToUnsignedInt(offset + 57, 3);
  spare2 = bits.ToUnsignedInt(offset + 60, 25);
}

Ais8_367_33_Wind_V2::Ais8_367_33_Wind_V2(const AisBitset &bits,
                                         const size_t offset) {
  wind_speed = bits.ToUnsignedInt(offset, 7);
  wind_gust = bits.ToUnsignedInt(offset + 7, 7);
  wind_dir = bits.ToUnsignedInt(offset + 14, 9);
  averaging_time = bits.ToUnsignedInt(offset + 23, 6);
  sensor_type = bits.ToUnsignedInt(offset + 29, 3);
  wind_speed_forecast = bits.ToUnsignedInt(offset + 32, 7);
  wind_gust_forecast = bits.ToUnsignedInt(offset + 39, 7);
  wind_dir_forecast = bits.ToUnsignedInt(offset + 46, 9);
  utc_hour_forecast = bits.ToUnsignedInt(offset + 55, 5);
  utc_min_forecast = bits.ToUnsignedInt(offset + 60, 6);
  duration = bits.ToUnsignedInt(offset + 66, 8);
  spare2 = bits.ToUnsignedInt(offset + 74, 11);
}

unique_ptr<Ais8_367_33_SensorReport>
ais8_367_33_sensor_report_factory(const AisBitset &bits,
                                  const size_t offset) {
  const Ais8_367_33_SensorEnum rpt_type =
      (Ais8_367_33_SensorEnum)bits.ToUnsignedInt(offset, 4);

  // WARNING: out of order decoding
  // Only get the report header if we can decode the type

  const size_t rpt_start = offset + 27;  // Skip to after site_id
  bits.SeekTo(rpt_start);
  Ais8_367_33_SensorReport *rpt = nullptr;

  switch (rpt_type) {
  case AIS8_367_33_SENSOR_LOCATION:
    rpt = new Ais8_367_33_Location(bits, rpt_start);
    break;
  case AIS8_367_33_SENSOR_STATION:
    rpt = new Ais8_367_33_Station(bits, rpt_start);
    break;
  case AIS8_367_33_SENSOR_WIND:
    rpt = new Ais8_367_33_Wind(bits, rpt_start);
    break;
  case AIS8_367_33_SENSOR_WATER_LEVEL:
    rpt = new Ais8_367_33_WaterLevel(bits, rpt_start);
    break;
  case AIS8_367_33_SENSOR_CURR_2D:
    rpt = new Ais8_367_33_Curr2D(bits, rpt_start);
    break;
  case AIS8_367_33_SENSOR_CURR_3D:
    rpt = new Ais8_367_33_Curr3D(bits, rpt_start);
    break;
  case AIS8_367_33_SENSOR_HORZ_FLOW:
    rpt = new Ais8_367_33_HorzFlow(bits, rpt_start);
    break;
  case AIS8_367_33_SENSOR_SEA_STATE:
    rpt = new Ais8_367_33_SeaState(bits, rpt_start);
    break;
  case AIS8_367_33_SENSOR_SALINITY:
    rpt = new Ais8_367_33_Salinity(bits, rpt_start);
    break;
  case AIS8_367_33_SENSOR_WX:
    rpt = new Ais8_367_33_Wx(bits, rpt_start);
    break;
  case AIS8_367_33_SENSOR_AIR_GAP:
    rpt = new Ais8_367_33_AirGap(bits, rpt_start);
    break;
  case AIS8_367_33_SENSOR_WIND_REPORT_2:
    rpt = new Ais8_367_33_Wind_V2(bits, rpt_start);
    break;
  // Leave rpt == 0 to indicate error
  case AIS8_367_33_SENSOR_RESERVED_12: break;
  case AIS8_367_33_SENSOR_RESERVED_13: break;
  case AIS8_367_33_SENSOR_RESERVED_14: break;
  case AIS8_367_33_SENSOR_RESERVED_15: break;
  case AIS8_367_33_SENSOR_ERROR: break;
  default:
    assert(false);
  }
  if (!rpt) {
    return {};
  }

  // Parse header
  rpt->report_type = rpt_type;
  bits.SeekTo(offset + 4);
  rpt->utc_day = bits.ToUnsignedInt(offset + 4, 5);
  rpt->utc_hr = bits.ToUnsignedInt(offset + 9, 5);
  rpt->utc_min = bits.ToUnsignedInt(offset + 14, 6);
  rpt->site_id = bits.ToUnsignedInt(offset + 20, 7);

  return unique_ptr<Ais8_367_33_SensorReport>(rpt);
}

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
    unique_ptr<Ais8_367_33_SensorReport> sensor =
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
      case AIS8_367_33_SENSOR_CURR_2D: {
        Ais8_367_33_Curr2D *rpt = dynamic_cast<Ais8_367_33_Curr2D *>(msg.reports[report_idx].get());

        o << " [report_type: " << rpt->report_type << " day: " << rpt->utc_day << " hour: " << rpt->utc_hr << " min: " << rpt->utc_min << " site: " << rpt->site_id;
        for (size_t idx = 0; idx < 3; idx++) {
          o << " [speed: " << rpt->currents[idx].speed << " dir: " << rpt->currents[idx].dir << " depth: " << rpt->currents[idx].depth << "]";
        }
        o << " type: " << rpt->type << "]";
        break;
      }
      case AIS8_367_33_SENSOR_CURR_3D: {
        Ais8_367_33_Curr3D *rpt = dynamic_cast<Ais8_367_33_Curr3D *>(msg.reports[report_idx].get());

        o << " [report_type: " << rpt->report_type << " day: " << rpt->utc_day << " hour: " << rpt->utc_hr << " min: " << rpt->utc_min << " site: " << rpt->site_id;
        for (size_t idx = 0; idx < 2; idx++) {
          o << " [north: " << rpt->currents[idx].north << " east: " << rpt->currents[idx].east << " up: " << rpt->currents[idx].up << " depth: " << rpt->currents[idx].depth << "]";
        }
        o << " type: " << rpt->type << "]";
        break;
      }
      case AIS8_367_33_SENSOR_HORZ_FLOW: {
        Ais8_367_33_HorzFlow *rpt = dynamic_cast<Ais8_367_33_HorzFlow *>(msg.reports[report_idx].get());

        o << " [report_type: " << rpt->report_type << " day: " << rpt->utc_day << " hour: " << rpt->utc_hr << " min: " << rpt->utc_min << " site: " << rpt->site_id;
        o << " bearing: " << rpt->bearing;
        for (size_t idx = 0; idx < 2; idx++) {
          o << " [dist: " << rpt->currents[idx].dist << " speed: " << rpt->currents[idx].speed << " dir: " << rpt->currents[idx].dir << " level: " << rpt->currents[idx].level << "]";
        o << " type: " << rpt->type << "]";
        }
        break;
      }
      case AIS8_367_33_SENSOR_SEA_STATE: {
        Ais8_367_33_SeaState *rpt = dynamic_cast<Ais8_367_33_SeaState *>(msg.reports[report_idx].get());

        o << " [report_type: " << rpt->report_type << " day: " << rpt->utc_day << " hour: " << rpt->utc_hr << " min: " << rpt->utc_min << " site: " << rpt->site_id;
        o << " swell_height: " << rpt->swell_height << " swell_period: " << rpt->swell_period << " swell_dir: " << rpt->swell_dir;
        o << " sea_state: " << rpt->sea_state << " sensor_type: " << rpt->swell_sensor_type;
        o << " water_temp: " << rpt->water_temp << " water_temp_depth: " << rpt->water_temp_depth << " water_sensor_type: " << rpt->water_sensor_type;
        o << " wave_height: " << rpt->wave_height << " wave_period: " << rpt->wave_period << " wave_dir: " << rpt->wave_dir << " wave_sensor_type: " << rpt->wave_sensor_type;
        o << " salinity: " << rpt->salinity << "]";
        break;
      }
      case AIS8_367_33_SENSOR_SALINITY: {
        Ais8_367_33_Salinity *rpt = dynamic_cast<Ais8_367_33_Salinity *>(msg.reports[report_idx].get());

        o << " [report_type: " << rpt->report_type << " day: " << rpt->utc_day << " hour: " << rpt->utc_hr << " min: " << rpt->utc_min << " site: " << rpt->site_id;
        o << " water_temp: " << rpt->water_temp << " conductivity: " << rpt->conductivity << " water_pressure: " << rpt->pressure;
        o << " salinity: " << rpt->salinity << " salinity_type: " << rpt->salinity_type << " sensor_type: " << rpt->sensor_type << "]";
        break;
      }
      case AIS8_367_33_SENSOR_WX: {
        Ais8_367_33_Wx *rpt = dynamic_cast<Ais8_367_33_Wx *>(msg.reports[report_idx].get());

        o << " [report_type: " << rpt->report_type << " day: " << rpt->utc_day << " hour: " << rpt->utc_hr << " min: " << rpt->utc_min << " site: " << rpt->site_id;
        o << " air_temp: " << rpt->air_temp << " air_temp_sensor_type: " << rpt->air_temp_sensor_type << " precip: " << rpt->precip;
        o << " horz_vis: " << rpt->horz_vis << " dew_point: " << rpt->dew_point << " dew_point_type: " << rpt->dew_point_type;
        o << " air_pressure: " << rpt->air_pressure << " air_pressure_trend: " << rpt->air_pressure_trend << " air_pressure_sensor_type: " << rpt->air_pressure_sensor_type;
        o << " salinity: " << rpt->salinity << "]";
        break;
      }
      case AIS8_367_33_SENSOR_AIR_GAP: {
        Ais8_367_33_AirGap *rpt = dynamic_cast<Ais8_367_33_AirGap *>(msg.reports[report_idx].get());

        o << " [report_type: " << rpt->report_type << " day: " << rpt->utc_day << " hour: " << rpt->utc_hr << " min: " << rpt->utc_min << " site: " << rpt->site_id;
        o << " air_draught: " << rpt->air_draught << " air_gap: " << rpt->air_gap << " air_gap_trend: " << rpt->air_gap_trend;
        o << " predicted_air_gap: " << rpt->predicted_air_gap;
        o << " utc_day_forecast: " << rpt->utc_day_forecast << " utc_hour_forecast: " << rpt->utc_hour_forecast << " utc_min_forecast: " << rpt->utc_min_forecast << " type: " << rpt->type << "]";
        break;
      }
      case AIS8_367_33_SENSOR_WIND_REPORT_2: {
        Ais8_367_33_Wind_V2 *rpt = dynamic_cast<Ais8_367_33_Wind_V2 *>(msg.reports[report_idx].get());

        o << " [report_type: " << rpt->report_type << " day: " << rpt->utc_day << " hour: " << rpt->utc_hr << " min: " << rpt->utc_min << " site: " << rpt->site_id;
        o << " wind_speed: " << rpt->wind_speed << " wind_gust: " << rpt->wind_gust << " wind_dir: " << rpt->wind_dir;
        o << " averaging_time: " << rpt->averaging_time << " sensor_type: " << rpt->sensor_type;
        o << " wind_speed_forecast: " << rpt->wind_speed_forecast << " wind_gust_forecast: " << rpt->wind_gust_forecast;
        o << " wind_dir_forecast: " << rpt->wind_dir_forecast;

        o << " utc_hour_forecast: " << rpt->utc_hour_forecast << " utc_min_forecast: " << rpt->utc_min_forecast << " duration: " << rpt->duration << "]";
        break;
      }
      default: break;
    } // switch
  } // for

  return o;
}



	
} //mylibais

