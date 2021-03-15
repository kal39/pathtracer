#ifndef RENDERER_H
#define RENDERER_H

#include <CL/cl.h>

typedef struct CLProgram {
	cl_platform_id platform;
	cl_device_id device;
	cl_context context;
	cl_command_queue queue;
	cl_program program;
	cl_kernel kernel;
	cl_mem imageBuff;
	cl_mem sceneBuff;
} CLProgram;

typedef struct Scene {
	cl_int3 size;
	cl_int *data;
	cl_float3 bgColor;
} Scene;

typedef struct Camera {
	cl_float3 pos;
	cl_float3 rot;
	cl_float sensorWidth;
	cl_float focalLength;
	cl_float aperture;
	cl_float exposure;
} Camera;

typedef struct CLImage {
	cl_int2 size;
	cl_float3 *data;
} CLImage;

typedef struct Renderer {
	CLProgram program;
	CLImage image;
	Scene scene;
	Camera camera;
} Renderer;

typedef struct Image {
	int width;
	int height;
	unsigned char *data;
} Image;

typedef struct Material {
	cl_int type;
	cl_float3 color;
	cl_float tint;
	cl_float fuzzyness;
	cl_float refIdx;
} Material;

Renderer *create_renderer();

void set_image_properties(
	Renderer *renderer,
	int width, int height
);

void set_background_color(
	Renderer *renderer,
	float r, float g, float b
);

Material create_lambertian_material(
	float r, float g, float b
);

Material create_metal_material(
	float r, float g, float b,
	float tint,
	float fuzzyness
);

Material create_dielectric_material(
	float r, float g, float b,
	float tint,
	float fuzzyness,
	float refIdx
);

Material create_light_source_material(
	float r, float g, float b
);

void set_camera_properties(
	Renderer *renderer,
	float x, float y, float z,
	float rotX, float rotY, float rotZ,
	float sensorWidth,
	float focalLength,
	float aperture,
	float exposure
);

Image *render(
	Renderer *renderer,
	int samples,
	int verbose
);

void render_to_file(
	Renderer *renderer,
	int samples,
	char *fileName,
	int verbose
);

void destroy_renderer(
	Renderer *renderer
);

void write_image(
	Image *image,
	char *fileName
);

void destroy_image(
	Image *image
);

#endif