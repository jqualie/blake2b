#define _CRT_SECURE_NO_WARNINGS /* Disable deprecation warning in VS2005+ */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "blake2b.h"
#include "util.h"

int main(int argc, void* argv)
{
	uint8_t* out = malloc(64);
	uint8_t* uin = malloc(1024);
	size_t len = 0;

	{
		uint8_t in[] = "";
		// 786a02f742015903c6c6fd852552d272912f4740e15847618a86e217f71f5419d25e1031afee585313896444934eb04b903a685b1448b755d56f701afe9be2ce
		blake2b(out, 64, NULL, 0, in, strlen(in));
		dump("in", in, strlen(in));
		dump("h", out, 64);
	}
	{
		uint8_t in[] = "abc";
		// ba80a53f981c4d0d6a2797b69f12f6e94c212f14685ac4b74b12bb6fdbffa2d17d87c5392aab792dc252d5de4533cc9518d38aa8dbf1925ab92386edd4009923
		blake2b(out, 64, NULL, 0, in, strlen(in));
		dump("in", in, strlen(in));
		dump("h", out, 64);
	}
	{
		uint8_t in[] = "abcdefghijklmnopqrstuvwxyz";
		// c68ede143e416eb7b4aaae0d8e48e55dd529eafed10b1df1a61416953a2b0a5666c761e7d412e6709e31ffe221b7a7a73908cb95a4d120b8b090a87d1fbedb4c
		blake2b(out, 64, NULL, 0, in, strlen(in));
		dump("in", in, strlen(in));
		dump("h", out, 64);
	}
	{
		uint8_t str[] = "ba80a53f981c4d0d6a2797b69f12f6e94c212f14685ac4b74b12bb6fdbffa2d17d87c5392aab792dc252d5de4533cc9518d38aa8dbf1925ab92386edd4009923";
		len = unhex(uin, 1024, str);
		// 66cb547665e462bbdd51d9b6ce1221116e9cfc6711c78d8798158349d12fa8ca513efb14bd84edf4e7cd3551355f14c1cf54dd203669b95675e52d72d3ec00d9
		blake2b(out, 64, NULL, 0, uin, len);
		dump("uin", uin, len);
		dump("h", out, 64);
	}
	{
		uint8_t str[] = "97fbbd8d5dbe1a3fadd0217b18541b04aaaa091c6df58a59c6916b5e40e4ddf750fe20261abc24f29454634f9978fafb787db67e983bd85011dc88eab7275f47ae62959241afd4e47bc2c8da3169d2aefc3bae563fd2ff712a77f73ff39c33bfb573efc967d5cd19154465de859cb9f91f0367b10357204698d048dbba85678175a6d91fdd66716e0743f0ea7f00d763a6aba029e7cd0303a25dd086ea1e884c279f3b431e567940dbed788c94017096d0f99bbdf3baa9a4d0eed44130d653bd8d834c480297e24254e8d18b27924198ea0d8724c00392e34a80dd47d1481642198565d563422c69d89834de76ea3d1a98d7a4fc31e06e00e918a03ebe61547b";
		len = unhex(uin, 1024, str);
		// 9798dfb7738a2430d92d8cded1b05e13a607f64c7252719cd8636c2e130e280af567d51d412fbeb1612c545bc06761c6b26457242148abf7210daba6b9150cbd
		blake2b(out, 64, NULL, 0, uin, len);
		dump("uin", uin, len);
		dump("h", out, 64);
	}
	{
		uint8_t str[] = "7b2df85dc1e04eee2c743173706ff05ad4e7f9a7f7e8006af97f8428c19b3a6a38a6328db9dca35375f5531238748709524d9c73862f0a7fef60d15cfce816accf3622b44f6adeb6984cd505c7ce7bb7566e36f11bb928e5fd608c61bad96db61e7b21774f7a2dee74ac7abbaf19d8974172601e21236c57e62676208b076277945c8ce227e8401553542828b355902f28d770bbfb2561881bb8dae6a31af5de6fdffa8d6634a8bf9dd2ebc31a7ed5b3674bc0d5e4ebf31da4951fe0673b001e6cf71640ce111b988f410168cb68d0b43739abb84109297e9fcab37899ead0b6eeb0a75ec20f77329542b20285fe990eb161d228b2f3313d6943ce8f32e4bc4c3ca8fa94b9e1578183380625c5b62e27e1e9b9e963e13b8622cc306b9f184d28e7e79f81ddaf83e22b42d066a2934e1e98abff979bf22cebf3af53f2c43e17fa3b06a7b2dbfd99bbd44e85f4a3b4e65b4e";
		len = unhex(uin, 1024, str);
		// 9be0a20a555cc0f0fb40c85be5d5abab974c3c2db5c1f925ee5ca513fd856ffcdbc353756a4e29253726284fc396d3d04fa9f90f96dd8e0a3095d90add93f7c9
		blake2b(out, 64, NULL, 0, uin, len);
		dump("uin", uin, len);
		dump("h", out, 64);
	}

	return 0;
}