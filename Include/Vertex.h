#pragma once 

struct Vertex4{
	union{
		struct{
			float x, y, z, w;
		};
		struct{
			float p[4];
		};
	};
};