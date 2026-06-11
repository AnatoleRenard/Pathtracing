#include "mre_camera.hpp"

namespace mre
{
	MreCameraSetting MreCamera::settings;
	MreCameraUtility MreCamera::utility;
	glm::dvec3* MreCamera::imageBlend;
	
	MreCamera::MreCamera(const MreCameraSetting& setting)
	{
		settings = setting;
		imageBlend = new glm::dvec3[settings.heightTexture*settings.widthTexture];
	}
	
	MreCamera::~MreCamera()
	{
		delete[] imageBlend;
	}
	
	void MreCamera::render(const std::shared_ptr<MreObject> world, glm::dvec3* img)
	{
		//set up
		setUp();
		//add threads
		std::vector<std::thread*> threads;
		int dy = settings.heightTexture/settings.threads;
		for(id_t i = 0; i < settings.threads; i++)
		{
			threads.push_back(new std::thread(renderThreads, world, i));
		}
		
		//once threads end add color data to img
		int i = 0;
		id_t indexImg = 0;
		for(const auto& thr : threads)
		{
			thr->join();
			std::ifstream read("cache/cache" + std::to_string(i));
			std::string line;
			id_t x = 0;
			while(std::getline(read, line))
			{
				if(line != "")
				{	
					//extract numbers
					std::istringstream iss(line);
					std::string num;
					std::vector<std::string> nums;
					while (iss >> num)
					{
						nums.push_back(num);
					}
					glm::dvec3 col(std::stod(nums[0]), std::stod(nums[1]), std::stod(nums[2]));
					
					if(settings.addImgOverTime)
					{
						imageBlend[indexImg+x] += col;
						img[indexImg+x] = imageBlend[indexImg+x]*(1.0/double(utility.numImgs));
					} else
					{
						img[indexImg+x] = col;
					}
					x++;
				}
				if(x == settings.widthTexture)
				{
					indexImg += settings.threads * settings.widthTexture;
					x = 0;
				}
			}
			i++;
			indexImg = i*settings.widthTexture;
		}
		
		for(const auto& thr : threads)
		{
			delete thr;
		}
	}
	
	void MreCamera::renderThreads(const std::shared_ptr<MreObject> world, id_t id)
	{
		std::ofstream file("cache/cache" + std::to_string(id));
		
		//render
		for(int y = id; y < settings.heightTexture; y+=settings.threads)
		{
			for(int x = 0; x < settings.widthTexture; x++)
			{
				//set pixel as if camera was at 0,0,0
				glm::dvec3 pixel = utility.pixel00 + utility.dx*double(x) + utility.dy*double(y);
				
				//get dir
				glm::dvec3 dir = glm::normalize(pixel - utility.eye);
				
				glm::dvec3 allCol(0.0);
				for(id_t j = 0; j < utility.ssp; j++)
				{
					for(id_t i = 0; i < utility.ssp; i++)
					{
						//make ray and transform
						MreRay ray;
						getRay(ray, x, y, i, j);
					
						glm::dvec3 col(0.0);
						trace(ray, world, col, settings.depth, settings.maxDepth);
						allCol += col;
					}
				}
				
				//add to file
				gammaCorrect(allCol, settings.samples);
				file << allCol.x << " " << allCol.y << " " << allCol.z << std::endl;
			}
		}
		file.close();
	}
	
	void MreCamera::trace(const MreRay& ray, const std::shared_ptr<MreObject> world, glm::dvec3& col, id_t depth, bool maxDepth)
	{
		if(depth == 0 && maxDepth)
		{
			col = glm::dvec3(0.0);
			return;
		}
		
		MreHitRec rec;
		world->hit(ray, rec);
		
		//if no hit set color to bg or skybox
		if(!rec.hit)
		{
			getBG(col, ray);
			return;
		}
		
		//get new scattered ray and color
		glm::dvec3 attenuation(0.0);
		glm::dvec3 light(0.0);
		MreRay nray = MreRay();
		rec.mat->emitted(rec.u, rec.v, rec.point, light);
		if(!rec.mat->scatter(ray, rec, col, nray))
		{
			col = light;
			return;
		}
		
		//get next ray color from hit
		glm::dvec3 ncol(0.0);
		trace(nray, world, ncol, depth-1, maxDepth);
		
		//mix colors
		col = col * ncol + light;
	}
	
	void MreCamera::getBG(glm::dvec3& col, const MreRay& ray)
	{
		if(settings.skybox)
		{
			glm::dvec3 unitDir = glm::normalize(ray.direction);
			double a = 0.5*(unitDir.y + 1.0);
			col = (1.0-a)*settings.col2 + a*settings.col1;
		} else
		{
			col = settings.col1;
		}
	}
	
	void MreCamera::setUp()
	{	
		//everything that wasn't set
		glm::dvec3 width  = settings.widthCam;
		glm::dvec3 height = settings.heightCam;
		utility.dx        = width/double(settings.widthTexture);
		utility.dy        = height/double(settings.heightTexture)*-1.0;
		glm::dvec3 topL   = utility.eye + (height / 2.0) + (width / -2.0) + glm::dvec3(0.0, 0.0, settings.focalLength);
		utility.pixel00   = topL + 0.5*(utility.dx+utility.dy);
				
		//transformations
		glm::dvec3 rotated = settings.rotation;
		rotated.x = glm::radians(rotated.x);
		rotated.z = glm::radians(rotated.y);
		rotated.y = glm::radians(rotated.z);
		
		glm::dmat4 rotMatX = glm::dmat4(1.0);
		rotMatX[1][1] = glm::cos(rotated.x);
		rotMatX[1][2] = -glm::sin(rotated.x);
		rotMatX[2][1] = glm::sin(rotated.x);
		rotMatX[2][2] = glm::cos(rotated.x);
		
		glm::dmat4 rotMatY = glm::dmat4(1.0);
		rotMatY[0][0] = glm::cos(rotated.y);
		rotMatY[0][2] = glm::sin(rotated.y);
		rotMatY[2][0] = -glm::sin(rotated.y);
		rotMatY[2][2] = glm::cos(rotated.y);
		
		glm::dmat4 rotMatZ = glm::dmat4(1.0);
		rotMatZ[0][0] = glm::cos(rotated.z);
		rotMatZ[0][1] = -glm::sin(rotated.z);
		rotMatZ[1][0] = glm::sin(rotated.z);
		rotMatZ[1][1] = glm::cos(rotated.z);
		
		glm::dmat4 rotMat = rotMatX * rotMatZ * rotMatY;
		
		glm::dmat4 transMat = glm::dmat4(1.0);
		transMat[3][0] = settings.translation.x;
		transMat[3][1] = settings.translation.y;
		transMat[3][2] = settings.translation.z;
		
		glm::dmat4 scaleMat = glm::dmat4(1.0);
		scaleMat[0][0] = 1.0;
		scaleMat[1][1] = 1.0;
		scaleMat[2][2] = 1.0;
		
		utility.finalMat = transMat * rotMat * scaleMat;
		
		//defocus blur
		double defocusRadius = settings.focusDist * glm::tan(glm::radians(settings.defocusAngle / 2));
		utility.defocusU = defocusRadius * glm::dvec3(1.0, 0.0, 0.0); //right
		utility.defocusV = defocusRadius * glm::dvec3(0.0, 1.0, 0.0); //up
		
		
		//image blend
		if(settings.addImgOverTime)
		{
			utility.numImgs += 1;
		}
		
		//stratisfy
		utility.ssp = int(sqrt(settings.samples));
		utility.pixelSamplesScale = 1.0 / (utility.ssp * utility.ssp);
		utility.recipSSP = 1.0/utility.ssp;
	}
	
	void MreCamera::getRay(MreRay& ray, int x, int y, id_t i, id_t j)
	{
		// Construct a camera ray originating from the defocus disk and directed at a randomly
        // sampled point around the pixel location i, j for stratified sample square s_i, s_j.

        glm::dvec3 offset = pixelSampleSquareStratisfied(i, j);
        glm::dvec3 pixel_sample = utility.pixel00 + ((double(x) + offset.x) * utility.dx) + ((double(y) + offset.y) * utility.dy);
		
		glm::dvec3 origin = (settings.defocusAngle <= 0) ? utility.eye : defocusDiskSample();
        glm::dvec3 dir = glm::normalize(pixel_sample - origin);

        ray = MreRay(applyMat(utility.finalMat, origin, 1.0), applyMat(utility.finalMat, dir, 0.0));
	}
	
	glm::dvec3 MreCamera::defocusDiskSample()
	{
        // Returns a random point in the camera defocus disk.
        auto p = randomInUnitDisk();
        return utility.eye + (p.y * utility.defocusV) + (p.x * utility.defocusU);
    }
	
	glm::dvec3 MreCamera::pixelSampleSquareStratisfied(id_t i, id_t j)
	{
        // Returns a random point in the square surrounding a pixel at the origin, given
        // the two subpixel indices.
        // Returns the vector to a random point in the square sub-pixel specified by grid
        // indices s_i and s_j, for an idealized unit square pixel [-.5,-.5] to [+.5,+.5].

        auto px = ((i + randomDouble()) * utility.recipSSP) - 0.5;
        auto py = ((j + randomDouble()) * utility.recipSSP) - 0.5;

        return glm::dvec3(px, py, 0.0);
    }
    
    glm::dvec3 MreCamera::pixelSampleSquare()
    {
        // Returns a random point in the square surrounding a pixel at the origin.
        auto px = -0.5 + randomDouble();
        auto py = -0.5 + randomDouble();
        return (px * utility.dx) + (py * utility.dy);
    }
}
