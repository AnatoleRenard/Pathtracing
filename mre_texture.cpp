#include "mre_texture.hpp"

#include "stb_image.h"
#define STB_IMAGE_IMPLEMENTATION

namespace mre
{
	MreImage::MreImage(const char *filename)
	{
		//stbi_set_flip_vertically_on_load(true);
		//stbi_set_flip_horizontally_on_load(true);
		data = stbi_load(filename, &width, &height, &nrChannels, bytes_per_pixel);
		bytes_per_scanline = width * bytes_per_pixel;
	}
	
	MreImage::~MreImage()
	{
		stbi_image_free(data);
	}
}
