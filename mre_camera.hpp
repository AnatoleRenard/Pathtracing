#pragma once

//std
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <thread>

//mre
#include "mre_object.hpp"
#include "mre_utility.hpp"
#include "mre_ray.hpp"

namespace mre
{
	struct MreCameraSetting
	{
		//can be edited by user
		//cam dimensions
		glm::dvec3 widthCam    = glm::dvec3(4.0, 0.0, 0.0);
		glm::dvec3 heightCam   = glm::dvec3(0.0, 3.0, 0.0);
		double focalLength     = 1.0;
		
		//cam translations
		glm::dvec3 translation = glm::dvec3(0.0);
		glm::dvec3 rotation    = glm::dvec3(0.0);
		
		//bg color
		glm::dvec3 col1        = glm::dvec3(0.5, 0.7, 1.0); // blue sky color
		glm::dvec3 col2        = glm::dvec3(1.0);           //while for gradient
		bool       skybox      = true;
		
		//img
		id_t widthTexture      = 400;
		id_t heightTexture     = 300;
		id_t samples           = 10;
		id_t depth             = 20;
		bool maxDepth          = true;
		bool addImgOverTime    = true; //add cur render to previous for cleaner results
		
		//threads
		int threads = 4;
		
		//defocus
		double defocusAngle = 0.0;
		double focusDist    = 1.0;
	};
	
	struct MreCameraUtility
	{
		//can not be edited by user
		//camera settings
		glm::dvec3 eye      = glm::dvec3(0.0); //eye of camera
		glm::dvec3 dx       = glm::dvec3(0.0); //change in width in pixel center
		glm::dvec3 dy       = glm::dvec3(0.0); //change in height in pixel center
		glm::dvec3 pixel00  = glm::dvec3(0.0); //pixel top-left
		
		
		glm::dvec3 defocusU = glm::dvec3(0.0); // Defocus disk horizontal radius
		glm::dvec3 defocusV = glm::dvec3(0.0); // Defocus disk vertical radius
		
		//final transformations
		glm::dmat4 finalMat   = glm::dmat4(0.0);
		
		//blending imgs
		size_t numImgs = 0;
		
		//stratisfy
		int ssp = 1;
		double recipSSP = 1;
		double pixelSamplesScale;
	};
	
	struct MreThreadSettings
	{
		int firstLine = 0;
		int endLine = 0;
		int id;
		MreCameraUtility utility;
		MreCameraSetting settings;
	};
	
	class MreCamera
	{
		public:
			MreCamera(const MreCameraSetting& setting);
			~MreCamera();
			
			static void render(const std::shared_ptr<MreObject> world, glm::dvec3* img);
			static void renderThreads(const std::shared_ptr<MreObject> world, id_t id);
		
		private:
			static MreCameraSetting settings;
			static MreCameraUtility utility;
			
			static void setUp();
			static void trace(const MreRay& ray, const std::shared_ptr<MreObject> world, glm::dvec3& col, id_t depth, bool maxDepth);
			
			static void getRay(MreRay& ray, int x, int y, id_t i, id_t j);
			static glm::dvec3 pixelSampleSquare();
			static glm::dvec3 pixelSampleSquareStratisfied(id_t i, id_t j);
			static glm::dvec3 defocusDiskSample();
			static void getBG(glm::dvec3& col, const MreRay& ray);
			
			static glm::dvec3* imageBlend;
	};
};
