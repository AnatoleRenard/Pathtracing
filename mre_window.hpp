#pragma once
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

/*
	Window is used to see what image looks like live as it is rendered
	To use call update and pass the img data as glm::dvec3, then draw to show img
 */

namespace mre
{
	struct MreWindowDescriptor
	{
		int width  = 400;
		int height = 400;
		bool fullscreen = false;
	};
	
	class MreWindow
	{
		public:
			MreWindow(const MreWindowDescriptor& windowDescriptor);
			~MreWindow();
			
			void draw();
			void update(const glm::dvec3* img);
			
			void setWinPos(int x, int y);
			
			void setFullScreen();
			
		private:
			bool full = false;
			SDL_Window* win;
			SDL_Renderer* rend;
			SDL_Texture* texture;
			MreWindowDescriptor wd;
			uint32_t* pixels;
	};
}
