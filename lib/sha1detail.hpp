/*****
  Copyright (C) 2016 Marc Stevens, Centrum Wiskunde & Informatica (CWI), Amsterdam.

  This file is part of sha1freestart80 source-code and released under the MIT License
*****/

#ifndef HASHCLASH_SHA1DETAIL_HPP
#define HASHCLASH_SHA1DETAIL_HPP

#include "types.hpp"

#ifdef _DEBUG
#ifdef SHA1DETAIL_INLINE_IMPL
#undef SHA1DETAIL_INLINE_IMPL
#endif
#else
#ifndef SHA1DETAIL_INLINE_IMPL
#define SHA1DETAIL_INLINE_IMPL
#endif
#endif

namespace hc {

	void sha1compress(uint32_t ihv[5], const uint32_t block[16]);
	void sha1compress_me(uint32_t ihv[5], const uint32_t me[80]);

	const uint32_t Qoffsha1 = 4;

	FUNC_PREFIX inline uint32_t sha1_f1(uint32_t b, uint32_t c, uint32_t d)
	{ return d ^ (b & (c ^ d)); }
	FUNC_PREFIX inline uint32_t sha1_f2(uint32_t b, uint32_t c, uint32_t d)
	{ return b ^ c ^ d; }
	FUNC_PREFIX inline uint32_t sha1_f3(uint32_t b, uint32_t c, uint32_t d)
	{ return (b & (c | d)) | (c & d); }
	FUNC_PREFIX inline uint32_t sha1_f4(uint32_t b, uint32_t c, uint32_t d)
	{ return b ^ c ^ d; }

#ifndef SHA1DETAIL_INLINE_IMPL
	
	extern const uint32_t sha1_iv[];
	extern const uint32_t sha1_ac[];

#else

	const uint32_t sha1_iv[] = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };
	const uint32_t sha1_ac[] = { 0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6 };

#endif

	FUNC_PREFIX inline void sha1_me(uint32_t block[80], const uint32_t msg[16])
	{
		unsigned i;
		for (i = 0; i < 16; ++i)
			block[i]=(rotate_left(msg[i],24)&0xFF00FF00)|(rotate_left(msg[i],8)&0x00FF00FF);
		for (i = 16; i < 80; ++i)
			block[i]=rotate_left(block[i-3] ^ block[i-8] ^ block[i-14] ^ block[i-16], 1);
	}

	FUNC_PREFIX inline void sha1_me_simple(uint32_t block[80], const uint32_t msg[16])
	{
		unsigned i;
		for (i = 0; i < 16; ++i)
			block[i]=msg[i];
		for (i = 16; i < 80; ++i)
			block[i]=rotate_left(block[i-3] ^ block[i-8] ^ block[i-14] ^ block[i-16], 1);
	}

	FUNC_PREFIX inline void sha1_me_generalised(uint32_t block[80], unsigned offset)
	{
		int i;
		for (i = offset+16; i < 80; ++i)
			block[i]=rotate_left(block[i-3] ^ block[i-8] ^ block[i-14] ^ block[i-16], 1);
		for (i = int(offset)-1; i >= 0; --i)
			block[i]=rotate_right(block[i+16], 1) ^ block[i+13] ^ block[i+8] ^ block[i+2];
	}
	FUNC_PREFIX inline void sha1_me_generalised(uint32_t block[80], const uint32_t msg[16], unsigned offset)
	{
		int i;
		for (i = 0; i < 16; ++i)
			block[offset+i]=msg[i];
		for (i = offset+16; i < 80; ++i)
			block[i]=rotate_left(block[i-3] ^ block[i-8] ^ block[i-14] ^ block[i-16], 1);
		for (i = int(offset)-1; i >= 0; --i)
			block[i]=rotate_right(block[i+16], 1) ^ block[i+13] ^ block[i+8] ^ block[i+2];
	}

	FUNC_PREFIX inline void sha1_step_round1(unsigned t, uint32_t Q[], const uint32_t me[])
	{
		const int offset = 4;
		uint32_t Ft = sha1_f1(Q[offset+t-1], rotate_left(Q[offset+t-2],30), rotate_left(Q[offset+t-3],30));
		Q[offset+t+1] = Ft + sha1_ac[0] + me[t] + rotate_left(Q[offset+t],5) + rotate_left(Q[offset+t-4],30);
	}
	FUNC_PREFIX inline void sha1_step_round2(unsigned t, uint32_t Q[], const uint32_t me[])
	{
		const int offset = 4;
		uint32_t Ft = sha1_f2(Q[offset+t-1], rotate_left(Q[offset+t-2],30), rotate_left(Q[offset+t-3],30));
		Q[offset+t+1] = Ft + sha1_ac[1] + me[t] + rotate_left(Q[offset+t],5) + rotate_left(Q[offset+t-4],30);
	}
	FUNC_PREFIX inline void sha1_step_round3(unsigned t, uint32_t Q[], const uint32_t me[])
	{
		const int offset = 4;
		uint32_t Ft = sha1_f3(Q[offset+t-1], rotate_left(Q[offset+t-2],30), rotate_left(Q[offset+t-3],30));
		Q[offset+t+1] = Ft + sha1_ac[2] + me[t] + rotate_left(Q[offset+t],5) + rotate_left(Q[offset+t-4],30);
	}
	FUNC_PREFIX inline void sha1_step_round4(unsigned t, uint32_t Q[], const uint32_t me[])
	{
		const int offset = 4;
		uint32_t Ft = sha1_f4(Q[offset+t-1], rotate_left(Q[offset+t-2],30), rotate_left(Q[offset+t-3],30));
		Q[offset+t+1] = Ft + sha1_ac[3] + me[t] + rotate_left(Q[offset+t],5) + rotate_left(Q[offset+t-4],30);
	}

	FUNC_PREFIX inline void sha1_step(unsigned t, uint32_t Q[], const uint32_t me[])
	{
		if (t < 40) {
			if (t < 20)
				sha1_step_round1(t, Q, me);
			else
				sha1_step_round2(t, Q, me);
		} else {
			if (t < 60)
				sha1_step_round3(t, Q, me);
			else
				sha1_step_round4(t, Q, me);
		}
	}

	template<unsigned t>
	FUNC_PREFIX inline void sha1_step(uint32_t Q[], const uint32_t me[])
	{
		if (t < 40) {
			if (t < 20)
				sha1_step_round1(t, Q, me);
			else
				sha1_step_round2(t, Q, me);
		} else {
			if (t < 60)
				sha1_step_round3(t, Q, me);
			else
				sha1_step_round4(t, Q, me);
		}
	}

	FUNC_PREFIX inline void sha1_step_round1_bw(unsigned t, uint32_t Q[], const uint32_t me[])
	{
		const int offset = 4;
		uint32_t Ft = sha1_f1(Q[offset+t-1], rotate_left(Q[offset+t-2],30), rotate_left(Q[offset+t-3],30));
		Q[offset+t-4] = rotate_right( Q[offset+t+1] - (Ft + sha1_ac[0] + me[t] + rotate_left(Q[offset+t],5) ) , 30);
	}
	FUNC_PREFIX inline void sha1_step_round2_bw(unsigned t, uint32_t Q[], const uint32_t me[])
	{
		const int offset = 4;
		uint32_t Ft = sha1_f2(Q[offset+t-1], rotate_left(Q[offset+t-2],30), rotate_left(Q[offset+t-3],30));
		Q[offset+t-4] = rotate_right( Q[offset+t+1] - (Ft + sha1_ac[1] + me[t] + rotate_left(Q[offset+t],5) ) , 30);
	}
	FUNC_PREFIX inline void sha1_step_round3_bw(unsigned t, uint32_t Q[], const uint32_t me[])
	{
		const int offset = 4;
		uint32_t Ft = sha1_f3(Q[offset+t-1], rotate_left(Q[offset+t-2],30), rotate_left(Q[offset+t-3],30));
		Q[offset+t-4] = rotate_right( Q[offset+t+1] - (Ft + sha1_ac[2] + me[t] + rotate_left(Q[offset+t],5) ) , 30);
	}
	FUNC_PREFIX inline void sha1_step_round4_bw(unsigned t, uint32_t Q[], const uint32_t me[])
	{
		const int offset = 4;
		uint32_t Ft = sha1_f4(Q[offset+t-1], rotate_left(Q[offset+t-2],30), rotate_left(Q[offset+t-3],30));
		Q[offset+t-4] = rotate_right( Q[offset+t+1] - (Ft + sha1_ac[3] + me[t] + rotate_left(Q[offset+t],5) ) , 30);
	}

	FUNC_PREFIX inline void sha1_step_bw(unsigned t, uint32_t Q[], const uint32_t me[])
	{
		if (t < 40) {
			if (t < 20)
				sha1_step_round1_bw(t, Q, me);
			else
				sha1_step_round2_bw(t, Q, me);
		} else {
			if (t < 60)
				sha1_step_round3_bw(t, Q, me);
			else
				sha1_step_round4_bw(t, Q, me);
		}
	}

	template<unsigned t>
	FUNC_PREFIX inline void sha1_step_bw(uint32_t Q[], const uint32_t me[])
	{
		if (t < 40) {
			if (t < 20)
				sha1_step_round1_bw(t, Q, me);
			else
				sha1_step_round2_bw(t, Q, me);
		} else {
			if (t < 60)
				sha1_step_round3_bw(t, Q, me);
			else
				sha1_step_round4_bw(t, Q, me);
		}
	}



#if 0
	inline void test_compress(const uint32_t ihv[5], uint32_t me[80]) {
		uint32_t Q[85];
		uint32_t ihv1[5];
		uint32_t ihv2[5];
		for (unsigned i = 0; i < 5; ++i)
			ihv1[i] = ihv[i];
		sha1compress_me(ihv1, me);
		Q[0] = rotate_right(ihv[4], 30);
		Q[1] = rotate_right(ihv[3], 30);
		Q[2] = rotate_right(ihv[2], 30);
		Q[3] = ihv[1];
		Q[4] = ihv[0];
		for (unsigned t = 0; t < 80; ++t)
			sha1_step(t, Q, me);
		ihv2[0] = ihv[0] + Q[84];
		ihv2[1] = ihv[1] + Q[83];
		ihv2[2] = ihv[2] + rotate_left(Q[82],30);
		ihv2[3] = ihv[3] + rotate_left(Q[81],30);
		ihv2[4] = ihv[4] + rotate_left(Q[80],30);
		exit(0);
	}
#endif

} // namespace hash

#endif //HASHCLASH_SHA1DETAIL_HPP
