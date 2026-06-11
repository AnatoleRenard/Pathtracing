#pragma once

#include <glm/glm.hpp>
#include <iostream>

#include "mre_perlin.hpp"

namespace mre
{
	class MreTexture
	{
		public:
			virtual ~MreTexture() = default;
			
			virtual glm::dvec3 value(double u, double v, const glm::dvec3& p) = 0;
			
			virtual void newCol(const glm::dvec3& c) = 0;
	};
	
	class MreSolidCol : public MreTexture
	{
		public:
			MreSolidCol(const glm::dvec3& color) : col(color){}
			
			glm::dvec3 value(double u, double v, const glm::dvec3& p) override
			{
				return col;
			}
			
			void newCol(const glm::dvec3& c) override
			{
				col = c;
			}
		
		private:
			glm::dvec3 col;
	};
	
	class MreNoise : public MreTexture
	{
		public:
			MreNoise(const glm::dvec3& color, double scale, bool useSin = true, bool useTan = false) : col(color), sc(scale), useSin(useSin), useTan(useTan){}
			
			glm::dvec3 value(double u, double v, const glm::dvec3& p) override 
			{
				auto s = sc * p;
				
				if(useTan)
				{
					return col * 0.5 * (1 + std::tan(s.z + 10*perlin.turb(s)));
				}
				return useSin ? col * 0.5 * (1 + std::sin(s.z + 10*perlin.turb(s))) : col * 0.5 * (1 + std::cos(s.z + 10*perlin.turb(s)));
			}
			
			void newCol(const glm::dvec3& c) override
			{
				col = c;
			}
		
		private:
			double sc;
			bool useSin;
			bool useTan;
			glm::dvec3 col;
			MrePerlin perlin;
	};
	
	class MreImage : public MreTexture
	{
		public:
			MreImage(const char *filename);
			
			~MreImage();
			
			glm::dvec3 value(double u, double v, const glm::dvec3& p) override 
			{	
				u = 1 - glm::clamp(u, 0.0, 1.0);
				v = 1 - glm::clamp(v, 0.0, 1.0);
				
				int i = static_cast<int>(u * width);
				int j = static_cast<int>(v * height);
				
				i = clamp(i, 0, width);
				j = clamp(j, 0, height);

				auto pixel = data + j*bytes_per_scanline + i*bytes_per_pixel;
				
				return glm::dvec3(pixel[0]/255.0, pixel[1]/255.0, pixel[2]/255.0);
			}
			
			void newCol(const glm::dvec3& c) override {}
		
		private:
			unsigned char *data;
			int width, height, nrChannels, bytes_per_scanline;
			int bytes_per_pixel = 3;
			
			static int clamp(int x, int low, int high)
			{
				// Return the value clamped to the range [low, high).
				if (x < low) return low;
				if (x < high) return x;
				return high - 1;
			}
	};
}
