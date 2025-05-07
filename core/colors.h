#ifndef COLOURS_H
#define COLOURS_H
#include "../ecs/components/material.h"
/*
    Grass and Dirt:
Source: https://www.color-hex.com/color-palette/46612
    #14423f 	(20,66,63)
	#306b40 	(48,107,64)
	#66a558 	(102,165,88)
	#5c463e 	(92,70,62)
	#3a272b 	(58,39,43)

ICE:
    #1b232b 	(27,35,43)
	#668eab 	(102,142,171)
	#b1c4d8 	(177,196,216)
	#dde5f4 	(221,229,244)
	#ffffff 	(255,255,255)
Sun-type star:
    #ff0000 	(255,0,0)
	#ff4d00 	(255,77,0)
	#ff6700 	(255,103,0)
	#ff8100 	(255,129,0)
	#ffa700 	(255,167,0)
 */
using namespace Lina::ECS::Components;
#define WHITE ((RGBA){1.0f, 1.0f, 1.0f, 1.0f})
#define RED ((RGBA){1.0f, 0.0f, 0.0f, 1.0f})
#define GREEN ((RGBA){0.0f, 1.0f, 0.0f, 1.0f})
#define BLUE ((RGBA){0.0f, 0.0f, 1.0f, 1.0f})
#define BLACK ((RGBA){0.0f, 0.0f, 0.0f, 1.0f})
#define OCEAN ((RGBA){6.0f / 0xff, 66.0f / 0xff, 115.0f / 0xff, 1.0f})
#define GRASS ((RGBA){48.0f / 0xff, 107.0f / 0xff, 64.0f / 0xff, 1.0f})
#define DIRT ((RGBA){92.0f / 0xff, 70.0f / 0xff, 62.0f / 0xff, 1.0f})
#define LIGHT_ICE ((RGBA){221. / 0xff, 229. / 0xff, 244. / 0xff, 1.0f})
#define ICE_TOP ((RGBA){27. / 0xff, 35. / 43., 244. / 0xff, 1.0f})
#define SUN_ORANGE ((RGBA){255. / 0xff, 167. / 43., 0. / 0xff, 1.0f})
#endif
