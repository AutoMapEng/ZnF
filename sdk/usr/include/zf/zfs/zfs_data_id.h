#pragma once

#include <vector>
#include "config.h"

namespace zfs
{
	enum class dataid_t
	{
		DATAID_UNKNOWN = 0,

		DATAID_5003 = 14,

		DATAID_5006_ENC = 114,
		DATAID_5006_LARA = 215,

		DATAID_5010_ENC = 214,
		DATAID_5010_LARA = 215,

		DATAID_5016_ENC = 314,
		DATAID_5016_LARA = 315, // und enc_comp

		DATAID_5024_ENC = 514,
		DATAID_5024_LARA = 515,
		DATAID_5024 = DATAID_5024_ENC, // default

		_DATAID_MAX_ = 1000
	};

	inline int dataid2int(dataid_t i) { return static_cast<int>(i); }
	inline dataid_t int2dataid(int i) { return static_cast<dataid_t>(i); }

	// the list of all supported data id's
	inline std::vector<dataid_t> dataids() { return { 
		dataid_t::DATAID_5003, 
		dataid_t::DATAID_5006_ENC, dataid_t::DATAID_5006_LARA, 
		dataid_t::DATAID_5010_ENC, dataid_t::DATAID_5010_LARA, 
		dataid_t::DATAID_5016_ENC, dataid_t::DATAID_5016_LARA, 
#ifdef ZFS_5024_SUPPORT
		dataid_t::DATAID_5024_ENC, dataid_t::DATAID_5024_LARA,
#endif
	}; }

	// exact check for a id
	inline bool is5006(dataid_t id) { return id == dataid_t::DATAID_5006_ENC || id == dataid_t::DATAID_5006_LARA; }
	inline bool is5010(dataid_t id) { return id == dataid_t::DATAID_5010_ENC || id == dataid_t::DATAID_5010_LARA; }
	inline bool is5016(dataid_t id) { return id == dataid_t::DATAID_5016_ENC || id == dataid_t::DATAID_5016_LARA; }
	inline bool is5024(dataid_t id) { return id == dataid_t::DATAID_5024_ENC || id == dataid_t::DATAID_5024_LARA; }

	// check for sync mode coded in data id
	inline bool isEnc(dataid_t id) {
		return id == dataid_t::DATAID_5003 || id == dataid_t::DATAID_5006_ENC || id == dataid_t::DATAID_5010_ENC || id == dataid_t::DATAID_5016_ENC || id == dataid_t::DATAID_5024_ENC; }
	inline bool isLara(dataid_t id) {
		return id == dataid_t::DATAID_5006_LARA || id == dataid_t::DATAID_5010_LARA || id == dataid_t::DATAID_5016_LARA || id == dataid_t::DATAID_5024_LARA;
	}

	// check for device or newer device
	inline bool is5006_or_newer(dataid_t id) { return id >= dataid_t::DATAID_5006_ENC && id < dataid_t::_DATAID_MAX_; }
	inline bool is5010_or_newer(dataid_t id) { return id >= dataid_t::DATAID_5010_ENC && id < dataid_t::_DATAID_MAX_; }
	inline bool is5016_or_newer(dataid_t id) { return id >= dataid_t::DATAID_5016_ENC && id < dataid_t::_DATAID_MAX_; }
	inline bool is5024_or_newer(dataid_t id) { return id >= dataid_t::DATAID_5024_ENC && id < dataid_t::_DATAID_MAX_; }
}