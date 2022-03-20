// -*- c++ -*-

// TODO(schwehr): Create an archive of messages that do not decode.

#ifndef LIBAIS_AIS_H_
#define LIBAIS_AIS_H_

#include <array>
#include <bitset>
#include <cassert>
#include <cstring>
#include <string>
#include <iostream>
#include <vector>
#include <memory>


using std::bitset;
using std::ostream;
using std::string;
using std::vector;
using namespace std;

namespace mylibais {




	// Returns the text in the nth field starting with the first field being 0.
	// Empty delim_str is not allowed.
	string GetNthField(const string &str, const size_t n, const string &delim_str);

	// Returns the number of pad bits in an AIS AIVDM NMEA string.
	// Returns -1 if there is an error.
	int GetPad(const string &nmea_str);

	// Returns the armored payload of an AIS AIVDM NMEA string.
	// Returns an empty string if there was an error.
	string GetBody(const string &nmea_str);


	// Note: Needs to be kept in sync with AIS_STATUS_STRINGS list in ais.cpp.
	enum AIS_STATUS {
		AIS_UNINITIALIZED,  // Message is not yet completely decoded.
		AIS_OK,
		AIS_ERR_BAD_BIT_COUNT,
		AIS_ERR_BAD_NMEA_CHR,
		AIS_ERR_BAD_PTR,
		AIS_ERR_UNKNOWN_MSG_TYPE,
		AIS_ERR_MSG_NOT_IMPLEMENTED,
		AIS_ERR_MSG_SUB_NOT_IMPLEMENTED,
		AIS_ERR_EXPECTED_STRING,
		AIS_ERR_BAD_MSG_CONTENT,
		AIS_ERR_MSG_TOO_LONG,
		AIS_ERR_BAD_SUB_MSG,
		AIS_ERR_BAD_SUB_SUB_MSG,
		AIS_STATUS_NUM_CODES
	};

	extern const char *const AIS_STATUS_STRINGS[AIS_STATUS_NUM_CODES];

	// Designated Area Codes (DAC) / Maritime Identification Digits define
	// which country controls a subset of the submessage spaces within
	// AIS "binary" messages 6, 8, 25, and 26.  See:
	//   http://www.itu.int/online/mms/glad/cga_mids.sh?lng=E
	//   http://en.wikipedia.org/w/index.php?title=Maritime_identification_digits
	// River Information System (RIS):
	//   ECE-TRANS-SC3-2006-10r-RIS.pdf

	enum Dac {
		AIS_DAC_0_TEST = 0,
		AIS_DAC_1_INTERNATIONAL,
		AIS_DAC_200_RIS = 200,
		AIS_DAC_201_ALBANIA = 201,
		AIS_DAC_202_ANDORRA = 202,
		AIS_DAC_203_AUSTRIA = 203,
		AIS_DAC_204_AZORES = 204,
		AIS_DAC_205_BELGIUM = 205,
		AIS_DAC_206_BELARUS = 206,
		AIS_DAC_207_BULGARIA = 207,
		AIS_DAC_208_VATICAN_CITY_STATE = 208,
		AIS_DAC_209_CYPRUS = 209,
		AIS_DAC_210_CYPRUS = 210,
		AIS_DAC_211_GERMANY = 211,
		AIS_DAC_212_CYPRUS = 212,
		AIS_DAC_213_GEORGIA = 213,
		AIS_DAC_214_MOLDOVA = 214,
		AIS_DAC_215_MALTA = 215,
		AIS_DAC_216_ARMENIA = 216,
		AIS_DAC_218_GERMANY = 218,
		AIS_DAC_219_DENMARK = 219,
		AIS_DAC_220_DENMARK = 220,
		AIS_DAC_224_SPAIN = 224,
		AIS_DAC_225_SPAIN = 225,
		AIS_DAC_226_FRANCE = 226,
		AIS_DAC_227_FRANCE = 227,
		AIS_DAC_228_FRANCE = 228,
		AIS_DAC_229_MALTA = 229,
		AIS_DAC_230_FINLAND = 230,
		AIS_DAC_231_FAROE_ISLANDS = 231,
		AIS_DAC_232_UNITED_KINGDOM_OF_GREAT_BRITAIN_AND_NORTHERN_IRELAND = 232,
		AIS_DAC_233_UNITED_KINGDOM_OF_GREAT_BRITAIN_AND_NORTHERN_IRELAND = 233,
		AIS_DAC_234_UNITED_KINGDOM_OF_GREAT_BRITAIN_AND_NORTHERN_IRELAND = 234,
		AIS_DAC_235_UNITED_KINGDOM_OF_GREAT_BRITAIN_AND_NORTHERN_IRELAND = 235,
		AIS_DAC_236_GIBRALTAR = 236,
		AIS_DAC_237_GREECE = 237,
		AIS_DAC_238_CROATIA = 238,
		AIS_DAC_239_GREECE = 239,
		AIS_DAC_240_GREECE = 240,
		AIS_DAC_241_GREECE = 241,
		AIS_DAC_242_MOROCCO = 242,
		AIS_DAC_243_HUNGARY = 243,
		AIS_DAC_244_NETHERLANDS = 244,
		AIS_DAC_245_NETHERLANDS = 245,
		AIS_DAC_246_NETHERLANDS = 246,
		AIS_DAC_247_ITALY = 247,
		AIS_DAC_248_MALTA = 248,
		AIS_DAC_249_MALTA = 249,
		AIS_DAC_250_IRELAND = 250,
		AIS_DAC_251_ICELAND = 251,
		AIS_DAC_252_LIECHTENSTEIN = 252,
		AIS_DAC_253_LUXEMBOURG = 253,
		AIS_DAC_254_MONACO = 254,
		AIS_DAC_255_MADEIRA = 255,
		AIS_DAC_256_MALTA = 256,
		AIS_DAC_257_NORWAY = 257,
		AIS_DAC_258_NORWAY = 258,
		AIS_DAC_259_NORWAY = 259,
		AIS_DAC_261_POLAND = 261,
		AIS_DAC_262_MONTENEGRO = 262,
		AIS_DAC_263_PORTUGAL = 263,
		AIS_DAC_264_ROMANIA = 264,
		AIS_DAC_265_SWEDEN = 265,
		AIS_DAC_266_SWEDEN = 266,
		AIS_DAC_267_SLOVAKIA = 267,
		AIS_DAC_268_SAN_MARINO = 268,
		AIS_DAC_269_SWITZERLAND = 269,
		AIS_DAC_270_CZECH_REPUBLIC = 270,
		AIS_DAC_271_TURKEY = 271,
		AIS_DAC_272_UKRAINE = 272,
		AIS_DAC_273_RUSSIAN_FEDERATION = 273,
		AIS_DAC_274_MACEDONIA = 274,
		AIS_DAC_275_LATVIA = 275,
		AIS_DAC_276_ESTONIA = 276,
		AIS_DAC_277_LITHUANIA = 277,
		AIS_DAC_278_SLOVENIA = 278,
		AIS_DAC_279_SERBIA = 279,
		AIS_DAC_301_ANGUILLA = 301,
		AIS_DAC_303_ALASKA = 303,
		AIS_DAC_304_ANTIGUA_AND_BARBUDA = 304,
		AIS_DAC_305_ANTIGUA_AND_BARBUDA = 305,
		AIS_DAC_306_NETHERLANDS_ANTILLES = 306,
		AIS_DAC_307_ARUBA = 307,
		AIS_DAC_308_BAHAMAS = 308,
		AIS_DAC_309_BAHAMAS = 309,
		AIS_DAC_310_BERMUDA = 310,
		AIS_DAC_311_BAHAMAS = 311,
		AIS_DAC_312_BELIZE = 312,
		AIS_DAC_314_BARBADOS = 314,
		AIS_DAC_316_CANADA = 316,
		AIS_DAC_319_CAYMAN_ISLANDS = 319,
		AIS_DAC_321_COSTA_RICA = 321,
		AIS_DAC_323_CUBA = 323,
		AIS_DAC_325_DOMINICA = 325,
		AIS_DAC_327_DOMINICAN_REPUBLIC = 327,
		AIS_DAC_329_GUADELOUPE = 329,
		AIS_DAC_330_GRENADA = 330,
		AIS_DAC_331_GREENLAND = 331,
		AIS_DAC_332_GUATEMALA = 332,
		AIS_DAC_334_HONDURAS = 334,
		AIS_DAC_336_HAITI = 336,
		AIS_DAC_338_UNITED_STATES_OF_AMERICA = 338,
		AIS_DAC_339_JAMAICA = 339,
		AIS_DAC_341_SAINT_KITTS_AND_NEVIS = 341,
		AIS_DAC_343_SAINT_LUCIA = 343,
		AIS_DAC_345_MEXICO = 345,
		AIS_DAC_347_MARTINIQUE = 347,
		AIS_DAC_348_MONTSERRAT = 348,
		AIS_DAC_350_NICARAGUA = 350,
		AIS_DAC_351_PANAMA = 351,
		AIS_DAC_352_PANAMA = 352,
		AIS_DAC_353_PANAMA = 353,
		AIS_DAC_354_PANAMA = 354,
		AIS_DAC_355_PANAMA = 355,
		AIS_DAC_356_PANAMA = 356,
		AIS_DAC_357_PANAMA = 357,
		AIS_DAC_358_PUERTO_RICO = 358,
		AIS_DAC_359_EL_SALVADOR = 359,
		AIS_DAC_361_SAINT_PIERRE_AND_MIQUELON = 361,
		AIS_DAC_362_TRINIDAD_AND_TOBAGO = 362,
		AIS_DAC_364_TURKS_AND_CAICOS_ISLANDS = 364,
		AIS_DAC_366_UNITED_STATES_OF_AMERICA = 366,
		AIS_DAC_367_UNITED_STATES_OF_AMERICA = 367,
		AIS_DAC_368_UNITED_STATES_OF_AMERICA = 368,
		AIS_DAC_369_UNITED_STATES_OF_AMERICA = 369,
		AIS_DAC_370_PANAMA = 370,
		AIS_DAC_371_PANAMA = 371,
		AIS_DAC_372_PANAMA = 372,
		AIS_DAC_373_PANAMA = 373,
		AIS_DAC_375_SAINT_VINCENT_AND_THE_GRENADINES = 375,
		AIS_DAC_376_SAINT_VINCENT_AND_THE_GRENADINES = 376,
		AIS_DAC_377_SAINT_VINCENT_AND_THE_GRENADINES = 377,
		AIS_DAC_378_BRITISH_VIRGIN_ISLANDS = 378,
		AIS_DAC_379_UNITED_STATES_VIRGIN_ISLANDS = 379,
		AIS_DAC_401_AFGHANISTAN = 401,
		AIS_DAC_403_SAUDI_ARABIA = 403,
		AIS_DAC_405_BANGLADESH = 405,
		AIS_DAC_408_BAHRAIN = 408,
		AIS_DAC_410_BHUTAN = 410,
		AIS_DAC_412_CHINA = 412,
		AIS_DAC_413_CHINA = 413,
		AIS_DAC_414_CHINA = 414,
		AIS_DAC_416_TAIWAN = 416,
		AIS_DAC_417_SRI_LANKA = 417,
		AIS_DAC_419_INDIA = 419,
		AIS_DAC_422_IRAN = 422,
		AIS_DAC_423_AZERBAIJAN = 423,
		AIS_DAC_425_IRAQ = 425,
		AIS_DAC_428_ISRAEL = 428,
		AIS_DAC_431_JAPAN = 431,
		AIS_DAC_432_JAPAN = 432,
		AIS_DAC_434_TURKMENISTAN = 434,
		AIS_DAC_436_KAZAKHSTAN = 436,
		AIS_DAC_437_UZBEKISTAN = 437,
		AIS_DAC_438_JORDAN = 438,
		AIS_DAC_440_KOREA = 440,
		AIS_DAC_441_KOREA = 441,
		AIS_DAC_443_STATE_OF_PALESTINE = 443,
		AIS_DAC_445_DEMOCRATIC_PEOPLES_REPUBLIC_OF_KOREA = 445,
		AIS_DAC_447_KUWAIT = 447,
		AIS_DAC_450_LEBANON = 450,
		AIS_DAC_451_KYRGYZ_REPUBLIC = 451,
		AIS_DAC_453_MACAO = 453,
		AIS_DAC_455_MALDIVES = 455,
		AIS_DAC_457_MONGOLIA = 457,
		AIS_DAC_459_NEPAL = 459,
		AIS_DAC_461_OMAN = 461,
		AIS_DAC_463_PAKISTAN = 463,
		AIS_DAC_466_QATAR = 466,
		AIS_DAC_468_SYRIAN_ARAB_REPUBLIC = 468,
		AIS_DAC_470_UNITED_ARAB_EMIRATES = 470,
		AIS_DAC_472_TAJIKISTAN = 472,
		AIS_DAC_473_YEMEN = 473,
		AIS_DAC_475_YEMEN = 475,
		AIS_DAC_477_HONG_KONG = 477,
		AIS_DAC_478_BOSNIA_AND_HERZEGOVINA = 478,
		AIS_DAC_501_ADELIE_LAND = 501,
		AIS_DAC_503_AUSTRALIA = 503,
		AIS_DAC_506_MYANMAR = 506,
		AIS_DAC_508_BRUNEI_DARUSSALAM = 508,
		AIS_DAC_510_MICRONESIA = 510,
		AIS_DAC_511_PALAU = 511,
		AIS_DAC_512_NEW_ZEALAND = 512,
		AIS_DAC_514_CAMBODIA = 514,
		AIS_DAC_515_CAMBODIA = 515,
		AIS_DAC_516_CHRISTMAS_ISLAND = 516,
		AIS_DAC_518_COOK_ISLANDS = 518,
		AIS_DAC_520_FIJI = 520,
		AIS_DAC_523_COCOS = 523,
		AIS_DAC_525_INDONESIA = 525,
		AIS_DAC_529_KIRIBATI = 529,
		AIS_DAC_531_LAO_PEOPLES_DEMOCRATIC_REPUBLIC = 531,
		AIS_DAC_533_MALAYSIA = 533,
		AIS_DAC_536_NORTHERN_MARIANA_ISLANDS = 536,
		AIS_DAC_538_MARSHALL_ISLANDS = 538,
		AIS_DAC_540_NEW_CALEDONIA = 540,
		AIS_DAC_542_NIUE = 542,
		AIS_DAC_544_NAURU = 544,
		AIS_DAC_546_FRENCH_POLYNESIA = 546,
		AIS_DAC_548_PHILIPPINES = 548,
		AIS_DAC_553_PAPUA_NEW_GUINEA = 553,
		AIS_DAC_555_PITCAIRN_ISLAND = 555,
		AIS_DAC_557_SOLOMON_ISLANDS = 557,
		AIS_DAC_559_AMERICAN_SAMOA = 559,
		AIS_DAC_561_SAMOA = 561,
		AIS_DAC_563_SINGAPORE = 563,
		AIS_DAC_564_SINGAPORE = 564,
		AIS_DAC_565_SINGAPORE = 565,
		AIS_DAC_566_SINGAPORE = 566,
		AIS_DAC_567_THAILAND = 567,
		AIS_DAC_570_TONGA = 570,
		AIS_DAC_572_TUVALU = 572,
		AIS_DAC_574_VIETNAM = 574,
		AIS_DAC_576_VANUATU = 576,
		AIS_DAC_577_VANUATU = 577,
		AIS_DAC_578_WALLIS_AND_FUTUNA_ISLANDS = 578,
		AIS_DAC_601_SOUTH_AFRICA = 601,
		AIS_DAC_603_ANGOLA = 603,
		AIS_DAC_605_ALGERIA = 605,
		AIS_DAC_607_SAINT_PAUL_AND_AMSTERDAM_ISLANDS = 607,
		AIS_DAC_608_ASCENSION_ISLAND = 608,
		AIS_DAC_609_BURUNDI = 609,
		AIS_DAC_610_BENIN = 610,
		AIS_DAC_611_BOTSWANA = 611,
		AIS_DAC_612_CENTRAL_AFRICAN_REPUBLIC = 612,
		AIS_DAC_613_CAMEROON = 613,
		AIS_DAC_615_CONGO = 615,
		AIS_DAC_616_COMOROS = 616,
		AIS_DAC_617_CABO_VERDE = 617,
		AIS_DAC_618_CROZET_ARCHIPELAGO = 618,
		AIS_DAC_619_COTE_DIVOIRE = 619,
		AIS_DAC_620_COMOROS = 620,
		AIS_DAC_621_DJIBOUTI = 621,
		AIS_DAC_622_EGYPT = 622,
		AIS_DAC_624_ETHIOPIA = 624,
		AIS_DAC_625_ERITREA = 625,
		AIS_DAC_626_GABONESE_REPUBLIC = 626,
		AIS_DAC_627_GHANA = 627,
		AIS_DAC_629_GAMBIA = 629,
		AIS_DAC_630_GUINEABISSAU = 630,
		AIS_DAC_631_EQUATORIAL_GUINEA = 631,
		AIS_DAC_632_GUINEA = 632,
		AIS_DAC_633_BURKINA_FASO = 633,
		AIS_DAC_634_KENYA = 634,
		AIS_DAC_635_KERGUELEN_ISLANDS = 635,
		AIS_DAC_636_LIBERIA = 636,
		AIS_DAC_637_LIBERIA = 637,
		AIS_DAC_638_SOUTH_SUDAN = 638,
		AIS_DAC_642_LIBYA = 642,
		AIS_DAC_644_LESOTHO = 644,
		AIS_DAC_645_MAURITIUS = 645,
		AIS_DAC_647_MADAGASCAR = 647,
		AIS_DAC_649_MALI = 649,
		AIS_DAC_650_MOZAMBIQUE = 650,
		AIS_DAC_654_MAURITANIA = 654,
		AIS_DAC_655_MALAWI = 655,
		AIS_DAC_656_NIGER = 656,
		AIS_DAC_657_NIGERIA = 657,
		AIS_DAC_659_NAMIBIA = 659,
		AIS_DAC_660_REUNION = 660,
		AIS_DAC_661_RWANDA = 661,
		AIS_DAC_662_SUDAN = 662,
		AIS_DAC_663_SENEGAL = 663,
		AIS_DAC_664_SEYCHELLES = 664,
		AIS_DAC_665_SAINT_HELENA = 665,
		AIS_DAC_666_SOMALIA = 666,
		AIS_DAC_667_SIERRA_LEONE = 667,
		AIS_DAC_668_SAO_TOME_AND_PRINCIPE = 668,
		AIS_DAC_669_SWAZILAND = 669,
		AIS_DAC_670_CHAD = 670,
		AIS_DAC_671_TOGOLESE_REPUBLIC = 671,
		AIS_DAC_672_TUNISIA = 672,
		AIS_DAC_674_TANZANIA = 674,
		AIS_DAC_675_UGANDA = 675,
		AIS_DAC_676_DEMOCRATIC_REPUBLIC_OF_THE_CONGO = 676,
		AIS_DAC_677_TANZANIA = 677,
		AIS_DAC_678_ZAMBIA = 678,
		AIS_DAC_679_ZIMBABWE = 679,
		AIS_DAC_701_ARGENTINE_REPUBLIC = 701,
		AIS_DAC_710_BRAZIL = 710,
		AIS_DAC_720_BOLIVIA = 720,
		AIS_DAC_725_CHILE = 725,
		AIS_DAC_730_COLOMBIA = 730,
		AIS_DAC_735_ECUADOR = 735,
		AIS_DAC_740_FALKLAND_ISLANDS = 740,
		AIS_DAC_745_GUIANA = 745,
		AIS_DAC_750_GUYANA = 750,
		AIS_DAC_755_PARAGUAY = 755,
		AIS_DAC_760_PERU = 760,
		AIS_DAC_765_SURINAME = 765,
		AIS_DAC_770_URUGUAY = 770,
		AIS_DAC_775_VENEZUELA = 775
	};

	class AisPoint {
	public:
		double lng_deg;
		double lat_deg;

		AisPoint();
		AisPoint(double lng_deg_, double lat_deg_);
	};
	ostream& operator<< (ostream &o, const AisPoint &position);

	//////////////////////////////////////////////////////////////////////
	// Support class for decoding
	//////////////////////////////////////////////////////////////////////
	static const int MAX_BITS = 1192;

	class AisBitset : protected bitset<MAX_BITS> {
	public:
		AisBitset();

		AIS_STATUS ParseNmeaPayload(const char *nmea_payload, int pad);

		int GetNumBits() const { return num_bits; }
		int GetNumChars() const { return num_chars; }
		int GetPosition() const { return current_position; }
		int GetRemaining() const { return num_bits - current_position; }

		const AisBitset& SeekRelative(int d) const;
		const AisBitset& SeekTo(size_t pos) const;

		bool operator[](size_t pos) const;

		unsigned int ToUnsignedInt(const size_t start, const size_t len) const;
		int ToInt(const size_t start, const size_t len) const;
		string ToString(const size_t start, const size_t len) const;

		const AisPoint ToAisPoint(const size_t start, const size_t point_size) const;

		// Visible for testing.
		static bitset<6> Reverse(const bitset<6> &bits);

	protected:
		int num_bits;
		int num_chars;

		static bool nmea_ord_initialized_;
		static bitset<6> nmea_ord_[128];
		static const char bits_to_char_tbl_[];

		static void InitNmeaOrd();

	private:
		// This will help uncover dicontinuities when querying sequential bits, i.e.
		// when we query a bit sequence that is not in direct succession of the
		// previous one. In the future, we may use this to automatically determine
		// the next read location.
		// This field is also used to determine the number of remaining bits after the
		// last read position.
		// That being said, the 'start' argument in all the 'To...' methods above is
		// redundant and the only purpose is to discover typos in the bit positions in
		// each message's parse method, i.e. debugging.
		mutable int current_position;
	};

	class AisMsg {
	public:
		int message_id;
		int repeat_indicator;
		int mmsi;
		

		// TODO(schwehr): make status private and have accessors.
		bool had_error() const { return status != AIS_OK; }
		AIS_STATUS get_error() const { return status; }

		virtual ~AisMsg() {}

	protected:
		AIS_STATUS status;  // AIS_OK or error code
		int num_chars;  // Number of characters in the nmea_payload.
		size_t num_bits;  // Number of bits in the nmea_payload.
		AisBitset bits;  // The bitset that was constructed out of the nmea_payload.

		AisMsg() : status(AIS_UNINITIALIZED), num_chars(0), num_bits(0), bits() {}
		AisMsg(const char *nmea_payload, const size_t pad);

		// Returns true if the msg is in a good state "so far", i.e. either AIS_OK or
		// AIS_UNINITIALIZED.
		bool CheckStatus() const;
	};

	// AIS Binary Broadcast message ... parent to many
	class Ais8 : public AisMsg {
	public:
		int spare;
		// TODO(schwehr): seq? // ITU M.R. 1371-3 Anex 2 5.3.1
		int dac;  // dac+fi = app id
		int fi;

		// TODO(schwehr): make Ais8 protected
		Ais8(const char *nmea_payload, const size_t pad);

	protected:
		Ais8() {}
	};



	// DAC 1 weather

	class Ais8_1_11 : public Ais8 {
	 public:
	  AisPoint position;
	  int day;
	  int hour;
	  int minute;
	  int wind_ave;  // kts
	  int wind_gust;  // kts
	  int wind_dir;
	  int wind_gust_dir;
	  float air_temp;  // C
	  int rel_humid;
	  float dew_point;
	  float air_pres;
	  int air_pres_trend;
	  float horz_vis;  // NM
	  float water_level;  // m
	  int water_level_trend;
	  float surf_cur_speed;
	  int surf_cur_dir;
	  float cur_speed_2;  // kts
	  int cur_dir_2;
	  int cur_depth_2;  // m
	  float cur_speed_3;  // kts
	  int cur_dir_3;
	  int cur_depth_3;  // m
	  float wave_height;  // m
	  int wave_period;
	  int wave_dir;
	  float swell_height;
	  int swell_period;
	  int swell_dir;
	  int sea_state;  // beaufort scale
	  float water_temp;
	  int precip_type;
	  float salinity;  // Part per mil (1/1000).
	  int ice;  // yes/no/undef/unknown
	  int spare2;
	  int extended_water_level;  // OHMEX uses this for extra water level precision

	  Ais8_1_11(const char *nmea_payload, size_t pad);
	};
	  
	  
		// IMO Circ 289 Meteorological and Hydrographic data
	// Section 1, Table 1.1
	// TODO(schwehr): is this exactly the same as 8_1_11 or has anything changed?
	//       x,y swapped.
	class Ais8_1_31 : public Ais8 {
	 public:
	  AisPoint position;  // Opposite the bit order of 8_1_11
	  int position_accuracy;  // New field
	  int utc_day;
	  int utc_hour;
	  int utc_min;
	  int wind_ave;  // kts
	  int wind_gust;  // kts
	  int wind_dir;
	  int wind_gust_dir;
	  float air_temp;  // C
	  int rel_humid;
	  float dew_point;
	  float air_pres;  // Pascals (Pa).
	  int air_pres_trend;
	  float horz_vis;  // NM
	  int water_level;  // m
	  int water_level_trend;

	  float surf_cur_speed;
	  int surf_cur_dir;
	  float cur_speed_2;  // kts
	  int cur_dir_2;
	  int cur_depth_2;  // m
	  float cur_speed_3;  // kts
	  int cur_dir_3;
	  int cur_depth_3;  // m

	  float wave_height;  // m
	  int wave_period;
	  int wave_dir;
	  float swell_height;  // m
	  int swell_period;
	  int swell_dir;
	  int sea_state;  // beaufort scale - Table 1.2
	  float water_temp;
	  int precip_type;
	  float salinity;
	  int ice;  // yes/no/undef/unknown
	  int spare2;

	  Ais8_1_31(const char *nmea_payload, const size_t pad);
	};

const size_t AIS8_1_26_REPORT_SIZE = 112;

enum Ais8_1_26_SensorEnum {
  AIS8_1_26_SENSOR_ERROR = -1,
  AIS8_1_26_SENSOR_LOCATION = 0,
  AIS8_1_26_SENSOR_STATION = 1,
  AIS8_1_26_SENSOR_WIND = 2,
  AIS8_1_26_SENSOR_WATER_LEVEL = 3,
  AIS8_1_26_SENSOR_CURR_2D = 4,
  AIS8_1_26_SENSOR_CURR_3D = 5,
  AIS8_1_26_SENSOR_HORZ_FLOW = 6,
  AIS8_1_26_SENSOR_SEA_STATE = 7,
  AIS8_1_26_SENSOR_SALINITY = 8,
  AIS8_1_26_SENSOR_WX = 9,
  AIS8_1_26_SENSOR_AIR_DRAUGHT = 10,
  AIS8_1_26_SENSOR_RESERVED_11 = 11,
  AIS8_1_26_SENSOR_RESERVED_12 = 12,
  AIS8_1_26_SENSOR_RESERVED_13 = 13,
  AIS8_1_26_SENSOR_RESERVED_14 = 14,
  AIS8_1_26_SENSOR_RESERVED_15 = 15,
};

class Ais8_1_26_SensorReport {
 public:
  int report_type;
  int utc_day;
  int utc_hr;
  int utc_min;
  int site_id;  // aka link_id

  virtual Ais8_1_26_SensorEnum getType() const = 0;
  virtual ~Ais8_1_26_SensorReport() {}
};

Ais8_1_26_SensorReport*
ais8_1_26_sensor_report_factory(const AisBitset &bs,
                                const size_t offset);

class Ais8_1_26_Location : public Ais8_1_26_SensorReport {
 public:
  AisPoint position;
  float z;  // alt in m from MSL
  int owner;
  int timeout;
  int spare;

  Ais8_1_26_Location(const AisBitset &bs, const size_t offset);
  Ais8_1_26_Location() {}
  Ais8_1_26_SensorEnum getType() const { return AIS8_1_26_SENSOR_LOCATION; }
};

class Ais8_1_26_Station : public Ais8_1_26_SensorReport {
 public:
  string name;
  int spare;

  Ais8_1_26_Station(const AisBitset &bs, const size_t offset);
  Ais8_1_26_Station() {}
  Ais8_1_26_SensorEnum getType() const {return AIS8_1_26_SENSOR_STATION;}
};

class Ais8_1_26_Wind : public Ais8_1_26_SensorReport {
 public:
  int wind_speed;  // knots
  int wind_gust;  // knots
  int wind_dir;
  int wind_gust_dir;
  int sensor_type;
  int wind_forecast;  // knots
  int wind_gust_forecast;  // knots
  int wind_dir_forecast;
  int utc_day_forecast;
  int utc_hour_forecast;
  int utc_min_forecast;
  int duration;
  int spare;

  Ais8_1_26_Wind(const AisBitset &bs, const size_t offset);
  Ais8_1_26_Wind() {}
  Ais8_1_26_SensorEnum getType() const {return AIS8_1_26_SENSOR_WIND;}
};

class Ais8_1_26_WaterLevel : public Ais8_1_26_SensorReport {
 public:
  int type;
  float level;  // m.  assuming it is being stored at 0.01 m inc.
  int trend;
  int vdatum;
  int sensor_type;
  int forecast_type;
  float level_forecast;
  int utc_day_forecast;
  int utc_hour_forecast;
  int utc_min_forecast;
  int duration;  // minutes
  int spare;

  Ais8_1_26_WaterLevel(const AisBitset &bs, const size_t offset);
  Ais8_1_26_WaterLevel() {}
  Ais8_1_26_SensorEnum getType() const {return AIS8_1_26_SENSOR_WATER_LEVEL;}
};

class Ais8_1_26_Curr2D_Current {
 public:
  float speed;  // knots
  int dir;
  int depth;  // m
};

class Ais8_1_26_Curr2D : public Ais8_1_26_SensorReport {
 public:
  Ais8_1_26_Curr2D_Current currents[3];
  int type;
  int spare;

  Ais8_1_26_Curr2D(const AisBitset &bs, const size_t offset);
  Ais8_1_26_Curr2D() {}
  Ais8_1_26_SensorEnum getType() const {return AIS8_1_26_SENSOR_CURR_2D;}
};

class Ais8_1_26_Curr3D_Current {
 public:
  float north;
  float east;
  float up;
  int depth;  // m
};

class Ais8_1_26_Curr3D : public Ais8_1_26_SensorReport {
 public:
  Ais8_1_26_Curr3D_Current currents[2];
  int type;
  int spare;

  Ais8_1_26_Curr3D(const AisBitset &bs, const size_t offset);
  Ais8_1_26_Curr3D() {}
  Ais8_1_26_SensorEnum getType() const {return AIS8_1_26_SENSOR_CURR_3D;}
};

class Ais8_1_26_HorzFlow_Current {
 public:
  int bearing;  // deg
  int dist;  // m
  float speed;  // knots
  int dir;  // deg
  int level;  // m
};

class Ais8_1_26_HorzFlow : public Ais8_1_26_SensorReport {
 public:
  Ais8_1_26_HorzFlow_Current currents[2];
  int spare;

  Ais8_1_26_HorzFlow(const AisBitset &bs, const size_t offset);
  Ais8_1_26_HorzFlow() {}
  Ais8_1_26_SensorEnum getType() const {return AIS8_1_26_SENSOR_HORZ_FLOW;}
};

class Ais8_1_26_SeaState : public Ais8_1_26_SensorReport {
 public:
  float swell_height;
  int swell_period;  // seconds
  int swell_dir;  // deg
  int sea_state;
  int swell_sensor_type;
  float water_temp;  // C
  float water_temp_depth;  // m
  int water_sensor_type;
  float wave_height;
  int wave_period;  // seconds
  int wave_dir;  // deg
  int wave_sensor_type;
  float salinity;

  Ais8_1_26_SeaState(const AisBitset &bs, const size_t offset);
  Ais8_1_26_SeaState() {}
  Ais8_1_26_SensorEnum getType() const {return AIS8_1_26_SENSOR_SEA_STATE;}
};

class Ais8_1_26_Salinity : public Ais8_1_26_SensorReport {
 public:
  float water_temp;  // C
  float conductivity;  // siemens/m
  float pressure;  // decibars
  float salinity;  // 0/00 ppt
  int salinity_type;
  int sensor_type;
  int spare[2];

  Ais8_1_26_Salinity(const AisBitset &bs, const size_t offset);
  Ais8_1_26_Salinity() {}
  Ais8_1_26_SensorEnum getType() const {return AIS8_1_26_SENSOR_SALINITY;}
};

class Ais8_1_26_Wx : public Ais8_1_26_SensorReport {
 public:
  float air_temp;  // C
  int air_temp_sensor_type;
  int precip;
  float horz_vis;  // nm
  float dew_point;  // C
  int dew_point_type;
  float air_pressure;  // Pascals (Pa).
  int air_pressure_trend;
  int air_pressor_type;
  float salinity;  // 0/00 ppt
  int spare;

  Ais8_1_26_Wx(const AisBitset &bs, const size_t offset);
  Ais8_1_26_Wx() {}
  Ais8_1_26_SensorEnum getType() const {return AIS8_1_26_SENSOR_WX;}
};

class Ais8_1_26_AirDraught : public Ais8_1_26_SensorReport {
 public:
  float draught;
  float gap;
  float forecast_gap;
  int trend;
  int utc_day_forecast;
  int utc_hour_forecast;
  int utc_min_forecast;
  int spare;

  Ais8_1_26_AirDraught(const AisBitset &bs, const size_t offset);
  Ais8_1_26_AirDraught() {}
  Ais8_1_26_SensorEnum getType() const {return AIS8_1_26_SENSOR_AIR_DRAUGHT;}
};

// IMO Circ 289 Environmental
class Ais8_1_26 : public Ais8 {
 public:
  vector<Ais8_1_26_SensorReport *> reports;

  Ais8_1_26(const char *nmea_payload, const size_t pad);
  ~Ais8_1_26();
};
ostream& operator<< (ostream &o, const Ais8_1_26 &msg);


// Environmental Message
// https://www.e-navigation.nl/content/environmental-message-0
// https://www.e-navigation.nl/sites/default/files/asm_files/em_version_release_3-23mar15_0.pdf
const size_t AIS8_367_33_REPORT_SIZE = 112;

enum Ais8_367_33_SensorEnum {
  AIS8_367_33_SENSOR_ERROR = -1,
  AIS8_367_33_SENSOR_LOCATION = 0,
  AIS8_367_33_SENSOR_STATION = 1,
  AIS8_367_33_SENSOR_WIND = 2,
  AIS8_367_33_SENSOR_WATER_LEVEL = 3,
  AIS8_367_33_SENSOR_CURR_2D = 4,
  AIS8_367_33_SENSOR_CURR_3D = 5,
  AIS8_367_33_SENSOR_HORZ_FLOW = 6,
  AIS8_367_33_SENSOR_SEA_STATE = 7,
  AIS8_367_33_SENSOR_SALINITY = 8,
  AIS8_367_33_SENSOR_WX = 9,
  AIS8_367_33_SENSOR_AIR_GAP = 10,
  AIS8_367_33_SENSOR_WIND_REPORT_2 = 11,
  AIS8_367_33_SENSOR_RESERVED_12 = 12,
  AIS8_367_33_SENSOR_RESERVED_13 = 13,
  AIS8_367_33_SENSOR_RESERVED_14 = 14,
  AIS8_367_33_SENSOR_RESERVED_15 = 15,
};

class Ais8_367_33_SensorReport {
 public:
  Ais8_367_33_SensorEnum report_type = AIS8_367_33_SENSOR_ERROR;
  int utc_day = 0;
  int utc_hr = 0;
  int utc_min = 0;
  int site_id = 0;

  virtual Ais8_367_33_SensorEnum getType() const = 0;
  virtual ~Ais8_367_33_SensorReport() {}
};

unique_ptr<Ais8_367_33_SensorReport>
ais8_367_33_sensor_report_factory(const AisBitset &bs,
                                  const size_t offset);

class Ais8_367_33_Location : public Ais8_367_33_SensorReport {
 public:
  int version = 0;
  AisPoint position = {0.0, 0.0};
  int precision = 0;
  int altitude_raw = 0;  // 0.1 Meter
  float altitude = 0.0;  // Meters
  int owner = 0;
  int timeout = 0;
  int spare2 = 0;

  Ais8_367_33_Location(const AisBitset &bs, const size_t offset);
  ~Ais8_367_33_Location() {}
  Ais8_367_33_SensorEnum getType() const override { return AIS8_367_33_SENSOR_LOCATION; }
};

class Ais8_367_33_Station : public Ais8_367_33_SensorReport {
 public:
  std::string name;
  int spare2 = 0;

  Ais8_367_33_Station(const AisBitset &bs, const size_t offset);
  ~Ais8_367_33_Station() {}
  Ais8_367_33_SensorEnum getType() const override {return AIS8_367_33_SENSOR_STATION;}
};

class Ais8_367_33_Wind : public Ais8_367_33_SensorReport {
 public:
  int wind_speed = 0;  // Knots
  int wind_gust = 0;  // Knots
  int wind_dir = 0;  // Degrees
  int wind_gust_dir = 0;  // Degrees
  int sensor_type = 0;
  int wind_forecast = 0;  // Knots
  int wind_gust_forecast = 0;  // Knots
  int wind_dir_forecast = 0;  // Degrees
  int utc_day_forecast = 0;
  int utc_hour_forecast = 0;
  int utc_min_forecast = 0;
  int duration = 0;  // Minutes
  int spare2 = 0;

  Ais8_367_33_Wind(const AisBitset &bs, const size_t offset);
  ~Ais8_367_33_Wind() {}
  Ais8_367_33_SensorEnum getType() const override {return AIS8_367_33_SENSOR_WIND;}
};

class Ais8_367_33_WaterLevel : public Ais8_367_33_SensorReport {
 public:
  int type = 0;
  int level = 0;  // Centimeters
  int trend = 0;
  int vdatum = 0;
  int sensor_type = 0;
  int forecast_type = 0;
  int level_forecast = 0;  // Centimeters
  int utc_day_forecast = 0;
  int utc_hour_forecast = 0;
  int utc_min_forecast = 0;
  int duration = 0;  // Minutes
  int spare2 = 0;

  Ais8_367_33_WaterLevel(const AisBitset &bs, const size_t offset);
  ~Ais8_367_33_WaterLevel() {}
  Ais8_367_33_SensorEnum getType() const override {return AIS8_367_33_SENSOR_WATER_LEVEL;}
};

class Ais8_367_33_Curr2D_Current {
 public:
  int speed_raw = 0;  // 0.1 Knots
  float speed = 0.0;  // Knots
  int dir = 0;  // Degrees
  int depth = 0;  // Meters
};

class Ais8_367_33_Curr2D : public Ais8_367_33_SensorReport {
 public:
  std::array<Ais8_367_33_Curr2D_Current, 3> currents;
  int type = 0;
  int spare2 = 0;

  Ais8_367_33_Curr2D(const AisBitset &bs, const size_t offset);
  ~Ais8_367_33_Curr2D() {}
  Ais8_367_33_SensorEnum getType() const override {return AIS8_367_33_SENSOR_CURR_2D;}
};

class Ais8_367_33_Curr3D_Current {
 public:
  int north_raw = 0;  // 0.1 knots
  float north = 0.0;  // Knots

  int east_raw = 0;  // 0.1 knots
  float east = 0.0;  // Knots

  int up_raw = 0;  // 0.1 knots
  float up = 0.0;  // Knots

  int depth = 0;  // Meters
};

class Ais8_367_33_Curr3D : public Ais8_367_33_SensorReport {
 public:
  std::array<Ais8_367_33_Curr3D_Current, 2> currents;
  int type = 0;
  int spare2 = 0;

  Ais8_367_33_Curr3D(const AisBitset &bs, const size_t offset);
  ~Ais8_367_33_Curr3D() {}
  Ais8_367_33_SensorEnum getType() const override {return AIS8_367_33_SENSOR_CURR_3D;}
};

class Ais8_367_33_HorzFlow_Current {
 public:
  int dist = 0;  // Meters
  int speed_raw = 0;  // 0.1 Knots
  float speed = 0.0;  // Knots
  int dir = 0;  // Degrees
  int level = 0;  // Meters
};

class Ais8_367_33_HorzFlow : public Ais8_367_33_SensorReport {
 public:
  std::array<Ais8_367_33_HorzFlow_Current, 2> currents;
  int bearing = 0;  // Degrees
  int type = 0;
  int spare2 = 0;

  Ais8_367_33_HorzFlow(const AisBitset &bs, const size_t offset);
  ~Ais8_367_33_HorzFlow() {}
  Ais8_367_33_SensorEnum getType() const override {return AIS8_367_33_SENSOR_HORZ_FLOW;}
};

class Ais8_367_33_SeaState : public Ais8_367_33_SensorReport {
 public:
  int swell_height_raw = 0;  // 0.1 Meter increments
  float swell_height = 0.0;  // Meters

  int swell_period = 0;  // Seconds
  int swell_dir = 0;  // Degrees
  int sea_state = 0;
  int swell_sensor_type = 0;

  int water_temp_raw = 0;  // 0.1 degrees C
  float water_temp = 0.0;  // Celsius

  int water_temp_depth_raw = 0;  // 0.1 Meters
  float water_temp_depth = 0.0;  // Meters

  int water_sensor_type = 0;

  int wave_height_raw = 0;  // 0.1 Meters
  float wave_height = 0.0;  // Meters

  int wave_period = 0;  // Seconds
  int wave_dir = 0;  // Degrees
  int wave_sensor_type = 0;

  int salinity_raw = 0;  // 0.1%
  float salinity = 0.0;  // %

  Ais8_367_33_SeaState(const AisBitset &bs, const size_t offset);
  ~Ais8_367_33_SeaState() {}
  Ais8_367_33_SensorEnum getType() const override {return AIS8_367_33_SENSOR_SEA_STATE;}
};

class Ais8_367_33_Salinity : public Ais8_367_33_SensorReport {
 public:
  int water_temp_raw = 0;  // 0.1 degrees Celsius
  float water_temp = 0.0;  // Celsius

  int conductivity_raw = 0;  // 0.01 Siemens/Meter
  float conductivity = 0.0;  // Siemens/Meter

  int pressure_raw = 0;  // 0.1 decibars
  float pressure = 0.0;  // decibars

  int salinity_raw = 0;  // 0.1%
  float salinity = 0.0;  // 0/00 ppt

  int salinity_type = 0;
  int sensor_type = 0;
  int spare2[2] = {0, 0};

  Ais8_367_33_Salinity(const AisBitset &bs, const size_t offset);
  ~Ais8_367_33_Salinity() {}
  Ais8_367_33_SensorEnum getType() const override {return AIS8_367_33_SENSOR_SALINITY;}
};

class Ais8_367_33_Wx : public Ais8_367_33_SensorReport {
 public:
  int air_temp_raw = 0;  // 0.1 degrees Celsius
  float air_temp = 0.0;  // Celsius

  int air_temp_sensor_type = 0;
  int precip = 0;

  int horz_vis_raw = 0;  // 0.1 Nautical Miles
  float horz_vis = 0.0;  // Nautical Miles

  int dew_point_raw = 0;  // 0.1 degrees Celsius
  float dew_point = 0.0;  // Celsius

  int dew_point_type = 0;

  int air_pressure_raw = 0;  // 1 hPA
  int air_pressure = 0;  // Pascals (Pa).

  int air_pressure_trend = 0;
  int air_pressure_sensor_type = 0;

  int salinity_raw = 0;  // 0.1%
  float salinity = 0.0;  // %

  int spare2 = 0;

  Ais8_367_33_Wx(const AisBitset &bs, const size_t offset);
  ~Ais8_367_33_Wx() {}
  Ais8_367_33_SensorEnum getType() const override {return AIS8_367_33_SENSOR_WX;}
};

class Ais8_367_33_AirGap : public Ais8_367_33_SensorReport {
 public:
  int air_draught = 0;  // Centimeters
  int air_gap = 0;  // Centimeters
  int air_gap_trend = 0;
  int predicted_air_gap = 0;  // Centimeters

  int utc_day_forecast = 0;
  int utc_hour_forecast = 0;
  int utc_min_forecast = 0;

  int type = 0;
  int spare2 = 0;

  Ais8_367_33_AirGap(const AisBitset &bs, const size_t offset);
  ~Ais8_367_33_AirGap() {}
  Ais8_367_33_SensorEnum getType() const override {return AIS8_367_33_SENSOR_AIR_GAP;}
};

class Ais8_367_33_Wind_V2 : public Ais8_367_33_SensorReport {
 public:
  int wind_speed = 0;  // Knots
  int wind_gust = 0;  // Knots
  int wind_dir = 0;  // Degrees
  int averaging_time = 0;  // Minutes
  int sensor_type = 0;
  int wind_speed_forecast = 0;  // Knots
  int wind_gust_forecast = 0;  // Knots
  int wind_dir_forecast = 0;  // Degrees

  int utc_hour_forecast = 0;
  int utc_min_forecast = 0;

  int duration = 0;  // Minutes

  int spare2 = 0;

  Ais8_367_33_Wind_V2(const AisBitset &bs, const size_t offset);
  ~Ais8_367_33_Wind_V2() {}
  Ais8_367_33_SensorEnum getType() const override {return AIS8_367_33_SENSOR_WIND_REPORT_2;}
};

class Ais8_367_33 : public Ais8 {
 public:
  // 1 to 8 sensor reports
  vector<unique_ptr<Ais8_367_33_SensorReport>> reports;

  Ais8_367_33(const char *nmea_payload, const size_t pad);
};
ostream& operator<< (ostream &o, const Ais8_367_33 &msg);


} //mylibais

#endif  // LIBAIS_AIS_H_

