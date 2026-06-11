#include "mre_window.hpp"

#define crop(mn,x,mx) (x < mn ? mn : (x > mx ? mx : x))

namespace mre
{
	MreWindow::MreWindow(const MreWindowDescriptor& windowDescriptor) : wd(windowDescriptor)
	{
		SDL_Init(SDL_INIT_VIDEO);
		win  = SDL_CreateWindow("Pathtracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, wd.width, wd.height, wd.fullscreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_SHOWN);
		rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
		texture = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, wd.width, wd.height);
		pixels = new uint32_t[wd.height*wd.width];
	}
	
	MreWindow::~MreWindow()
	{
		delete[] pixels;
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(rend);
		SDL_DestroyWindow(win);
		SDL_Quit();
	}
	
	void MreWindow::draw()
	{
		SDL_RenderCopy(rend, texture, NULL, NULL);
		SDL_RenderPresent(rend);
	}
	
	void MreWindow::update(const glm::dvec3* img)
	{
		for(int y = 0; y < wd.height; y++)
		{
			for(int x = 0; x < wd.width; x++)
			{
				int index = y * wd.width + x;
				pixels[index] = (crop(0, static_cast<int>(img[index].x*255), 255) << 24) + (crop(0, static_cast<int>(img[index].y*255), 255) << 16) + (crop(0, static_cast<int>(img[index].z*255), 255) << 8) + 255;
			}
		}
		SDL_UpdateTexture(texture, NULL, pixels, wd.width*sizeof(uint32_t));
	}
	
	void MreWindow::setWinPos(int x, int y)
	{
		SDL_SetWindowPosition(win, x, y);
	}
	
	void MreWindow::setFullScreen()
	{
		full = !full;
		SDL_SetWindowFullscreen(win, full);
	}
}
