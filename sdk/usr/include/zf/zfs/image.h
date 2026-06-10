#pragma once
#include "zfsapi.h"

#include <cinttypes>
#include <string>
#include <vector>
#include <memory>

#include "Math/rgb.h"

class ZFSHeader;
class CxImage;

namespace zfs
{

	/**
	 * class which can handle very large images (not used jet)
	 */
	class ZFS_API TiledImage
	{
	public:
		virtual ~TiledImage();

		bool empty() const;
		uint32_t width() const { return m_width; }
		uint32_t height() const { return m_height; }

		bool load(const std::wstring& fn, const ZFSHeader* hd = {});

		zfs::math::rgb rgb(uint32_t u, uint32_t v, bool* ok = {}) const;

		const std::wstring& filename() const { return m_fn; }

	protected:
		std::wstring m_fn;
		std::vector<CxImage*> m_images{};

		uint32_t m_width{};
		uint32_t m_height{};

		uint32_t m_startx{};
		uint32_t m_starty{};
		uint32_t m_deep{};
		uint32_t m_rot{};
	};
}
