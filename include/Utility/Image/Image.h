#pragma once

#include <cstdint>

namespace bc
{
	class Image
	{
	public:
		Image();
		~Image();

		bool            LoadImageFromFile(const char* path);
		bool            LoadImageFromData(uint8_t* pBuffer, uint32_t length);
		bool            LoadFromSize(int32_t w, int32_t h, int32_t n);

		void            GetColor(int32_t x, int32_t y, uint8_t* dstColor);
		void            GetColor(float u, float v, uint8_t* dstColor);
		void            GetLinearColor(int32_t x, int32_t y, uint8_t* dstColor);
		void            GetLinearColor(float u, float v, uint8_t* dstColor);
		void            SetColor(int32_t x, int32_t y, const uint8_t* srcColor);
		void            SetColor(float u, float v, const uint8_t* srcColor);
		void            CleanColor(uint8_t* srcColor);

		int32_t		GetWidth() const { return m_nWidth; }
		int32_t		GetHeight() const { return m_nHeight; }
		int32_t		GetChannel() const { return m_nChannel; }
		uint8_t* GetBuffer() const { return m_pBuffer; }

		void            SaveJPGAsPath(const char* path);
		void            SaveTGAAsPath(const char* path);
	protected:

	private:
		uint8_t* m_pBuffer;
		int32_t m_nWidth;
		int32_t m_nHeight;
		int32_t m_nChannel;
		int32_t m_nStride;
	};
}
