#pragma once

#include "zfcore_api.h"

#include <string>
#include <vector>


namespace zf
{
	/**
	* \brief UUID generator and managing
	*/
	class ZFCORE_API UUID
	{
        static constexpr size_t sc_size = 16;

	public:
		typedef unsigned char bytes;

		UUID() = default;
		UUID(const UUID& aUUID) = default;
		UUID(const bytes*);
		UUID(const std::vector<bytes>& v);
		UUID(const std::string& aUUID);
		UUID(const std::wstring& aUUID);

		// generate a new uuid
		static UUID generate();

		// check if it is zero
		bool isZero() const;

		std::string toString() const;
		std::wstring toWString() const;
		std::vector<UUID::bytes> toBytes() const;

		bool operator == (const UUID& aUUID) const;
		bool operator != (const UUID& aUUID) const;
		const UUID& operator = (const UUID& aUUID);

		// uuid has a fixed size of 16
		constexpr size_t size() const noexcept { return sc_size; }
	protected:
		UUID::bytes	 Bytes[sc_size]{};
	};

} // end namespace
