#pragma once
#include <vector>
#include <assert.h>

float frac(float v) {
	return v - (int)v;
}

struct AlphaPyramidLevel
{
	int width, height;
	std::vector<float> alpha;
	float total_alpha;

	float GetAlpha(int x, int y) const { return alpha[y*width + x]; }
	template <typename ARG_SCALE> void SetData(int w, int h, int prev_width, int prev_height, ARG_SCALE accessor)
	{
		total_alpha = 0;
		width = w;
		height = h;
		alpha.resize(width*height);
		for (int ih = 0; ih < height; ih++) {
			for (int iw = 0; iw < width; iw++) {
				float a0 = accessor((ih * 2)*prev_width + iw * 2);
				float a1 = accessor((ih * 2)*prev_width + iw * 2 + 1);
				float a2 = accessor((ih * 2 + 1)*prev_width + iw * 2);
				float a3 = accessor((ih * 2 + 1)*prev_width + iw * 2 + 1);
				alpha[ih*width + iw] = a0 + a1 + a2 + a3;
				total_alpha += a0 + a1 + a2 + a3;
			}
			if (width * 2 < prev_width) {
				float a0 = accessor((ih * 2)*prev_width + width * 2);
				float a1 = accessor((ih * 2 + 1)*prev_width + width * 2);
				alpha[(ih + 1)*width - 1] += a0 + a1;
				total_alpha += a0 + a1;
			}
		}
		if (height * 2 < prev_height) {
			int ii = (height - 1)*width;
			for (int iw = 0; iw < width; iw++) {
				float a0 = accessor((height * 2)*prev_width + iw * 2);
				float a1 = accessor((height * 2)*prev_width + iw * 2 + 1);
				alpha[ii + iw] += a0 + a1;
				total_alpha += a0 + a1;
			}
			if (width * 2 < prev_width) {
				float a0 = accessor((height * 2)*prev_width + width * 2);
				alpha[ii + width - 1] += a0;
				total_alpha += a0;
			}
		}
	}
	void Alpha2CountBlock(const int *ix, int n, uint32_t count, int spp)
	{
		uint32_t sum = 0, remSum = 0;
		for (int j = 0; j < n; j++) {
			float v = alpha[ix[j]];
			uint32_t c = floor(v);
			float r = v - c;
			if (spp > 1) {
				c = floor(v*spp);
				r = (v*spp) - c;
			}
			sum += c;
			remSum += r;
			//alpha[ix[j]] = (c << 8) | r;
		}
		//assert(sum <= count);
		//assert(sum + remSum / 255 + 1 >= count);

		for (uint32_t rem = count - sum; rem > 0; rem--) {
			int max_i = 0;
			uint32_t max_v;
			int eqCount = 1;
			max_v = frac(alpha[ix[0]]);
			for (int j = 1; j < n; j++) {
				uint32_t v = frac(alpha[ix[j]]);
				if (max_v < v) {
					max_v = v;
					max_i = j;
					eqCount = 1;
				}
			}
			assert(max_v > 0);
			alpha[ix[max_i]] += 1.0 - max_v;
		}
		for (int j = 0; j < n; j++) alpha[ix[j]] = floor(alpha[ix[j]]);
	}
	void Alpha2Count(const AlphaPyramidLevel *parent, int spp)
	{
		int hLim = (height & 1) ? height - 3 : height;
		int wLim = (width & 1) ? width - 3 : width;
		for (int ih = 0; ih < hLim; ih += 2) {
			for (int iw = 0; iw < wLim; iw += 2) {
				float count = parent->GetAlpha(iw / 2, ih / 2);
				int i = ih * width + iw;
				int ix[] = { i, i + width + 1, i + 1, i + width };
				Alpha2CountBlock(ix, 4, count, spp);
			}
			if (wLim < width) {
				float count = parent->GetAlpha(wLim / 2, ih / 2);
				int i = ih * width + wLim;
				int ix[] = { i, i + width + 1, i + 2, i + width, i + 1, i + width + 2 };
				Alpha2CountBlock(ix, 6, count, spp);
			}
		}
		if (hLim < height) {
			for (int iw = 0; iw < wLim; iw += 2) {
				float count = parent->GetAlpha(iw / 2, hLim / 2);
				int i = hLim * width + iw;
				int ix[] = { i, i + width + 1, i + width + width, i + 1, i + width, i + width + width + 1 };
				Alpha2CountBlock(ix, 6, count, spp);
			}
			if (wLim < width) {
				float count = parent->GetAlpha(wLim / 2, hLim / 2);
				int i = hLim * width + wLim;
				int ix[] = { i, i + width + width + 2, i + 2, i + width + width, i + width + 1, i + 1, i + width + width + 1, i + width, i + width + 2 };
				Alpha2CountBlock(ix, 9, count, spp);
			}
		}
	}
	void Alpha2CountSimple(uint32_t count, int spp)
	{
		int n = width * height;
		std::vector<int> ix(n);
		for (int i = 0; i < n; i++) ix[i] = i;
		Alpha2CountBlock(ix.data(), n, count, spp);
	}
};
