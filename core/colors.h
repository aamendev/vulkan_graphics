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
#endif
