/**
 * Copyright (C) 2018-2021 Jessica James.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Written by Jessica James <jessica.aj@outlook.com>
 */

#include "unicode_base.hpp"

namespace jessilib {

/** Allocating */

std::u8string encode_codepoint_u8(char32_t in_codepoint) {
	std::u8string result;
	encode_codepoint(result, in_codepoint);
	return result;
}

std::u16string encode_codepoint_u16(char32_t in_codepoint) {
	std::u16string result;
	encode_codepoint(result, in_codepoint);
	return result;
}

std::u32string encode_codepoint_u32(char32_t in_codepoint) {
	std::u32string result;
	encode_codepoint(result, in_codepoint);
	return result;
}

std::wstring encode_codepoint_w(char32_t in_codepoint) {
	std::wstring result;
	encode_codepoint(result, in_codepoint);
	return result;
}

/**
 * Codepoint folding (case-insensitive character comparisons)
 */

// describes a range
struct folding_set {
	enum class mode_type {
		constant,
		alternating,
		single
	} mode = mode_type::single;

	uint32_t range_start; // inclusive
	uint32_t range_end; // inclusive
	int64_t diff; // difference between two codepoints

	uint32_t fold(uint32_t in_codepoint) const {
		if (mode != mode_type::alternating // Constant & single types immediately apply diff and return
			|| (in_codepoint % 2) == (range_start % 2)) { // Alternating type apply diff only if matches range type
			return static_cast<uint32_t>(in_codepoint + diff);
		}

		return in_codepoint;
	}
};

bool operator<(const folding_set& in_lhs, const uint32_t in_rhs) {
	return in_rhs > in_lhs.range_end;
}

bool operator<(const uint32_t in_lhs, const folding_set& in_rhs) {
	return in_lhs < in_rhs.range_start;
}

char32_t fold(char32_t in_codepoint) {
	// Break this up? The most common case is going to be points < 0x80, but even without breaking this up we'll only
	// perform at most like 8+1 integer comparisons, which is hardly a major constraint
	static constexpr folding_set s_folding_sets[]{
		{ folding_set::mode_type::constant, 0x0041, 0x005A, 32 },
		{ folding_set::mode_type::single, 0x00B5, 0x00B5, 775 },
		{ folding_set::mode_type::constant, 0x00C0, 0x00D6, 32 },
		{ folding_set::mode_type::constant, 0x00D8, 0x00DE, 32 },
		{ folding_set::mode_type::alternating, 0x0100, 0x012E, 1 },
		{ folding_set::mode_type::alternating, 0x0132, 0x0136, 1 },
		{ folding_set::mode_type::alternating, 0x0139, 0x0147, 1 },
		{ folding_set::mode_type::alternating, 0x014A, 0x0176, 1 },
		{ folding_set::mode_type::single, 0x0178, 0x0178, -121 },
		{ folding_set::mode_type::alternating, 0x0179, 0x017D, 1 },
		{ folding_set::mode_type::single, 0x017F, 0x017F, -268 },
		{ folding_set::mode_type::single, 0x0181, 0x0181, 210 },
		{ folding_set::mode_type::alternating, 0x0182, 0x0184, 1 },
		{ folding_set::mode_type::single, 0x0186, 0x0186, 206 },
		{ folding_set::mode_type::single, 0x0187, 0x0187, 1 },
		{ folding_set::mode_type::constant, 0x0189, 0x018A, 205 },
		{ folding_set::mode_type::single, 0x018B, 0x018B, 1 },
		{ folding_set::mode_type::single, 0x018E, 0x018E, 79 },
		{ folding_set::mode_type::single, 0x018F, 0x018F, 202 },
		{ folding_set::mode_type::single, 0x0190, 0x0190, 203 },
		{ folding_set::mode_type::single, 0x0191, 0x0191, 1 },
		{ folding_set::mode_type::single, 0x0193, 0x0193, 205 },
		{ folding_set::mode_type::single, 0x0194, 0x0194, 207 },
		{ folding_set::mode_type::single, 0x0196, 0x0196, 211 },
		{ folding_set::mode_type::single, 0x0197, 0x0197, 209 },
		{ folding_set::mode_type::single, 0x0198, 0x0198, 1 },
		{ folding_set::mode_type::single, 0x019C, 0x019C, 211 },
		{ folding_set::mode_type::single, 0x019D, 0x019D, 213 },
		{ folding_set::mode_type::single, 0x019F, 0x019F, 214 },
		{ folding_set::mode_type::alternating, 0x01A0, 0x01A4, 1 },
		{ folding_set::mode_type::single, 0x01A6, 0x01A6, 218 },
		{ folding_set::mode_type::single, 0x01A7, 0x01A7, 1 },
		{ folding_set::mode_type::single, 0x01A9, 0x01A9, 218 },
		{ folding_set::mode_type::single, 0x01AC, 0x01AC, 1 },
		{ folding_set::mode_type::single, 0x01AE, 0x01AE, 218 },
		{ folding_set::mode_type::single, 0x01AF, 0x01AF, 1 },
		{ folding_set::mode_type::constant, 0x01B1, 0x01B2, 217 },
		{ folding_set::mode_type::alternating, 0x01B3, 0x01B5, 1 },
		{ folding_set::mode_type::single, 0x01B7, 0x01B7, 219 },
		{ folding_set::mode_type::single, 0x01B8, 0x01B8, 1 },
		{ folding_set::mode_type::single, 0x01BC, 0x01BC, 1 },
		{ folding_set::mode_type::single, 0x01C4, 0x01C4, 2 },
		{ folding_set::mode_type::single, 0x01C5, 0x01C5, 1 },
		{ folding_set::mode_type::single, 0x01C7, 0x01C7, 2 },
		{ folding_set::mode_type::single, 0x01C8, 0x01C8, 1 },
		{ folding_set::mode_type::single, 0x01CA, 0x01CA, 2 },
		{ folding_set::mode_type::alternating, 0x01CB, 0x01DB, 1 },
		{ folding_set::mode_type::alternating, 0x01DE, 0x01EE, 1 },
		{ folding_set::mode_type::single, 0x01F1, 0x01F1, 2 },
		{ folding_set::mode_type::alternating, 0x01F2, 0x01F4, 1 },
		{ folding_set::mode_type::single, 0x01F6, 0x01F6, -97 },
		{ folding_set::mode_type::single, 0x01F7, 0x01F7, -56 },
		{ folding_set::mode_type::alternating, 0x01F8, 0x021E, 1 },
		{ folding_set::mode_type::single, 0x0220, 0x0220, -130 },
		{ folding_set::mode_type::alternating, 0x0222, 0x0232, 1 },
		{ folding_set::mode_type::single, 0x023A, 0x023A, 10795 },
		{ folding_set::mode_type::single, 0x023B, 0x023B, 1 },
		{ folding_set::mode_type::single, 0x023D, 0x023D, -163 },
		{ folding_set::mode_type::single, 0x023E, 0x023E, 10792 },
		{ folding_set::mode_type::single, 0x0241, 0x0241, 1 },
		{ folding_set::mode_type::single, 0x0243, 0x0243, -195 },
		{ folding_set::mode_type::single, 0x0244, 0x0244, 69 },
		{ folding_set::mode_type::single, 0x0245, 0x0245, 71 },
		{ folding_set::mode_type::alternating, 0x0246, 0x024E, 1 },
		{ folding_set::mode_type::single, 0x0345, 0x0345, 116 },
		{ folding_set::mode_type::alternating, 0x0370, 0x0372, 1 },
		{ folding_set::mode_type::single, 0x0376, 0x0376, 1 },
		{ folding_set::mode_type::single, 0x037F, 0x037F, 116 },
		{ folding_set::mode_type::single, 0x0386, 0x0386, 38 },
		{ folding_set::mode_type::constant, 0x0388, 0x038A, 37 },
		{ folding_set::mode_type::single, 0x038C, 0x038C, 64 },
		{ folding_set::mode_type::constant, 0x038E, 0x038F, 63 },
		{ folding_set::mode_type::constant, 0x0391, 0x03A1, 32 },
		{ folding_set::mode_type::constant, 0x03A3, 0x03AB, 32 },
		{ folding_set::mode_type::single, 0x03C2, 0x03C2, 1 },
		{ folding_set::mode_type::single, 0x03CF, 0x03CF, 8 },
		{ folding_set::mode_type::single, 0x03D0, 0x03D0, -30 },
		{ folding_set::mode_type::single, 0x03D1, 0x03D1, -25 },
		{ folding_set::mode_type::single, 0x03D5, 0x03D5, -15 },
		{ folding_set::mode_type::single, 0x03D6, 0x03D6, -22 },
		{ folding_set::mode_type::alternating, 0x03D8, 0x03EE, 1 },
		{ folding_set::mode_type::single, 0x03F0, 0x03F0, -54 },
		{ folding_set::mode_type::single, 0x03F1, 0x03F1, -48 },
		{ folding_set::mode_type::single, 0x03F4, 0x03F4, -60 },
		{ folding_set::mode_type::single, 0x03F5, 0x03F5, -64 },
		{ folding_set::mode_type::single, 0x03F7, 0x03F7, 1 },
		{ folding_set::mode_type::single, 0x03F9, 0x03F9, -7 },
		{ folding_set::mode_type::single, 0x03FA, 0x03FA, 1 },
		{ folding_set::mode_type::constant, 0x03FD, 0x03FF, -130 },
		{ folding_set::mode_type::constant, 0x0400, 0x040F, 80 },
		{ folding_set::mode_type::constant, 0x0410, 0x042F, 32 },
		{ folding_set::mode_type::alternating, 0x0460, 0x0480, 1 },
		{ folding_set::mode_type::alternating, 0x048A, 0x04BE, 1 },
		{ folding_set::mode_type::single, 0x04C0, 0x04C0, 15 },
		{ folding_set::mode_type::alternating, 0x04C1, 0x04CD, 1 },
		{ folding_set::mode_type::alternating, 0x04D0, 0x052E, 1 },
		{ folding_set::mode_type::constant, 0x0531, 0x0556, 48 },
		{ folding_set::mode_type::constant, 0x10A0, 0x10C5, 7264 },
		{ folding_set::mode_type::single, 0x10C7, 0x10C7, 7264 },
		{ folding_set::mode_type::single, 0x10CD, 0x10CD, 7264 },
		{ folding_set::mode_type::constant, 0x13F8, 0x13FD, -8 },
		{ folding_set::mode_type::single, 0x1C80, 0x1C80, -6222 },
		{ folding_set::mode_type::single, 0x1C81, 0x1C81, -6221 },
		{ folding_set::mode_type::single, 0x1C82, 0x1C82, -6212 },
		{ folding_set::mode_type::constant, 0x1C83, 0x1C84, -6210 },
		{ folding_set::mode_type::single, 0x1C85, 0x1C85, -6211 },
		{ folding_set::mode_type::single, 0x1C86, 0x1C86, -6204 },
		{ folding_set::mode_type::single, 0x1C87, 0x1C87, -6180 },
		{ folding_set::mode_type::single, 0x1C88, 0x1C88, 35267 },
		{ folding_set::mode_type::constant, 0x1C90, 0x1CBA, -3008 },
		{ folding_set::mode_type::constant, 0x1CBD, 0x1CBF, -3008 },
		{ folding_set::mode_type::alternating, 0x1E00, 0x1E94, 1 },
		{ folding_set::mode_type::single, 0x1E9B, 0x1E9B, -58 },
		{ folding_set::mode_type::single, 0x1E9E, 0x1E9E, -7615 },
		{ folding_set::mode_type::alternating, 0x1EA0, 0x1EFE, 1 },
		{ folding_set::mode_type::constant, 0x1F08, 0x1F0F, -8 },
		{ folding_set::mode_type::constant, 0x1F18, 0x1F1D, -8 },
		{ folding_set::mode_type::constant, 0x1F28, 0x1F2F, -8 },
		{ folding_set::mode_type::constant, 0x1F38, 0x1F3F, -8 },
		{ folding_set::mode_type::constant, 0x1F48, 0x1F4D, -8 },
		{ folding_set::mode_type::alternating, 0x1F59, 0x1F5F, -8 },
		{ folding_set::mode_type::constant, 0x1F68, 0x1F6F, -8 },
		{ folding_set::mode_type::constant, 0x1F88, 0x1F8F, -8 },
		{ folding_set::mode_type::constant, 0x1F98, 0x1F9F, -8 },
		{ folding_set::mode_type::constant, 0x1FA8, 0x1FAF, -8 },
		{ folding_set::mode_type::constant, 0x1FB8, 0x1FB9, -8 },
		{ folding_set::mode_type::constant, 0x1FBA, 0x1FBB, -74 },
		{ folding_set::mode_type::single, 0x1FBC, 0x1FBC, -9 },
		{ folding_set::mode_type::single, 0x1FBE, 0x1FBE, -7173 },
		{ folding_set::mode_type::constant, 0x1FC8, 0x1FCB, -86 },
		{ folding_set::mode_type::single, 0x1FCC, 0x1FCC, -9 },
		{ folding_set::mode_type::constant, 0x1FD8, 0x1FD9, -8 },
		{ folding_set::mode_type::constant, 0x1FDA, 0x1FDB, -100 },
		{ folding_set::mode_type::constant, 0x1FE8, 0x1FE9, -8 },
		{ folding_set::mode_type::constant, 0x1FEA, 0x1FEB, -112 },
		{ folding_set::mode_type::single, 0x1FEC, 0x1FEC, -7 },
		{ folding_set::mode_type::constant, 0x1FF8, 0x1FF9, -128 },
		{ folding_set::mode_type::constant, 0x1FFA, 0x1FFB, -126 },
		{ folding_set::mode_type::single, 0x1FFC, 0x1FFC, -9 },
		{ folding_set::mode_type::single, 0x2126, 0x2126, -7517 },
		{ folding_set::mode_type::single, 0x212A, 0x212A, -8383 },
		{ folding_set::mode_type::single, 0x212B, 0x212B, -8262 },
		{ folding_set::mode_type::single, 0x2132, 0x2132, 28 },
		{ folding_set::mode_type::constant, 0x2160, 0x216F, 16 },
		{ folding_set::mode_type::single, 0x2183, 0x2183, 1 },
		{ folding_set::mode_type::constant, 0x24B6, 0x24CF, 26 },
		{ folding_set::mode_type::constant, 0x2C00, 0x2C2F, 48 },
		{ folding_set::mode_type::single, 0x2C60, 0x2C60, 1 },
		{ folding_set::mode_type::single, 0x2C62, 0x2C62, -10743 },
		{ folding_set::mode_type::single, 0x2C63, 0x2C63, -3814 },
		{ folding_set::mode_type::single, 0x2C64, 0x2C64, -10727 },
		{ folding_set::mode_type::alternating, 0x2C67, 0x2C6B, 1 },
		{ folding_set::mode_type::single, 0x2C6D, 0x2C6D, -10780 },
		{ folding_set::mode_type::single, 0x2C6E, 0x2C6E, -10749 },
		{ folding_set::mode_type::single, 0x2C6F, 0x2C6F, -10783 },
		{ folding_set::mode_type::single, 0x2C70, 0x2C70, -10782 },
		{ folding_set::mode_type::single, 0x2C72, 0x2C72, 1 },
		{ folding_set::mode_type::single, 0x2C75, 0x2C75, 1 },
		{ folding_set::mode_type::constant, 0x2C7E, 0x2C7F, -10815 },
		{ folding_set::mode_type::alternating, 0x2C80, 0x2CE2, 1 },
		{ folding_set::mode_type::alternating, 0x2CEB, 0x2CED, 1 },
		{ folding_set::mode_type::single, 0x2CF2, 0x2CF2, 1 },
		{ folding_set::mode_type::alternating, 0xA640, 0xA66C, 1 },
		{ folding_set::mode_type::alternating, 0xA680, 0xA69A, 1 },
		{ folding_set::mode_type::alternating, 0xA722, 0xA72E, 1 },
		{ folding_set::mode_type::alternating, 0xA732, 0xA76E, 1 },
		{ folding_set::mode_type::alternating, 0xA779, 0xA77B, 1 },
		{ folding_set::mode_type::single, 0xA77D, 0xA77D, -35332 },
		{ folding_set::mode_type::alternating, 0xA77E, 0xA786, 1 },
		{ folding_set::mode_type::single, 0xA78B, 0xA78B, 1 },
		{ folding_set::mode_type::single, 0xA78D, 0xA78D, -42280 },
		{ folding_set::mode_type::alternating, 0xA790, 0xA792, 1 },
		{ folding_set::mode_type::alternating, 0xA796, 0xA7A8, 1 },
		{ folding_set::mode_type::single, 0xA7AA, 0xA7AA, -42308 },
		{ folding_set::mode_type::single, 0xA7AB, 0xA7AB, -42319 },
		{ folding_set::mode_type::single, 0xA7AC, 0xA7AC, -42315 },
		{ folding_set::mode_type::single, 0xA7AD, 0xA7AD, -42305 },
		{ folding_set::mode_type::single, 0xA7AE, 0xA7AE, -42308 },
		{ folding_set::mode_type::single, 0xA7B0, 0xA7B0, -42258 },
		{ folding_set::mode_type::single, 0xA7B1, 0xA7B1, -42282 },
		{ folding_set::mode_type::single, 0xA7B2, 0xA7B2, -42261 },
		{ folding_set::mode_type::single, 0xA7B3, 0xA7B3, 928 },
		{ folding_set::mode_type::alternating, 0xA7B4, 0xA7C2, 1 },
		{ folding_set::mode_type::single, 0xA7C4, 0xA7C4, -48 },
		{ folding_set::mode_type::single, 0xA7C5, 0xA7C5, -42307 },
		{ folding_set::mode_type::single, 0xA7C6, 0xA7C6, -35384 },
		{ folding_set::mode_type::alternating, 0xA7C7, 0xA7C9, 1 },
		{ folding_set::mode_type::single, 0xA7D0, 0xA7D0, 1 },
		{ folding_set::mode_type::alternating, 0xA7D6, 0xA7D8, 1 },
		{ folding_set::mode_type::single, 0xA7F5, 0xA7F5, 1 },
		{ folding_set::mode_type::constant, 0xAB70, 0xABBF, -38864 },
		{ folding_set::mode_type::constant, 0xFF21, 0xFF3A, 32 },
		{ folding_set::mode_type::constant, 0x10400, 0x10427, 40 },
		{ folding_set::mode_type::constant, 0x104B0, 0x104D3, 40 },
		{ folding_set::mode_type::constant, 0x10570, 0x1057A, 39 },
		{ folding_set::mode_type::constant, 0x1057C, 0x1058A, 39 },
		{ folding_set::mode_type::constant, 0x1058C, 0x10592, 39 },
		{ folding_set::mode_type::constant, 0x10594, 0x10595, 39 },
		{ folding_set::mode_type::constant, 0x10C80, 0x10CB2, 64 },
		{ folding_set::mode_type::constant, 0x118A0, 0x118BF, 32 },
		{ folding_set::mode_type::constant, 0x16E40, 0x16E5F, 32 },
		{ folding_set::mode_type::constant, 0x1E900, 0x1E921, 34 },
	};

	const folding_set* folding_sets_end = std::end(s_folding_sets);
	auto match = std::lower_bound(s_folding_sets, folding_sets_end, in_codepoint);
	if (match == folding_sets_end
		|| in_codepoint < *match) {
		// in_codepoint doesn't fit into any set;
		return in_codepoint;
	}

	return match->fold(in_codepoint);
}

} // namespace jessilib
