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

struct Vertex4UV{
	union{
		struct{
			float x, y, z, w;
		};
		struct{
			float p[4];
		};
	};
	union{
		struct{
			float u, v;
		};
		struct {
			float uv[2];
		};
	};
};