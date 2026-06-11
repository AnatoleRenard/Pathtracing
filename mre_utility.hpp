#pragma once
#include <limits>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include "mre_window.hpp"
#define crop(mn,x,mx) (x < mn ? mn : (x > mx ? mx : x))

namespace mre
{
	const double infinity = std::numeric_limits<double>::infinity();
	const double pi = 3.1415926535897932385;
	const double epsilonCam = 1e-9;
	const double epsilon = 0.0;
	
	inline void offsetRandomizer()
	{
		std::srand(clock());
	}
	
	void static dumpAsPPM(const glm::dvec3* img, const MreWindowDescriptor& wd, const std::string& name)
	{	
		//open file
		std::ofstream file("progress/" + name + ".ppm");
		file << "P3\n" << wd.width << " " << wd.height << "\n255\n";
		
		//transform img from dvec3 to ppm
		for(int y = 0; y < wd.height; y++)
		{
			for(int x = 0; x < wd.width; x++)
			{
				int index = y * wd.width + x;
				//dump to file
				file << crop(0, static_cast<int>(img[index].x*255), 255) << " " << crop(0, static_cast<int>(img[index].y*255), 255) << " " << crop(0, static_cast<int>(img[index].z*255), 255) << "\n";
			}
		}
		file.close();
	}
	
	glm::dvec3 inline applyMat(const glm::dmat4& mat, const glm::dvec3& vec, double w)
	{
		return glm::dvec3(mat * glm::dvec4(vec, w));
	}
	
	inline double randomDouble()
	{
		// Returns a random real in [0,1).
		return rand() / (RAND_MAX + 1.0);
	}
	
	inline double randomDouble(double min, double max)
	{
		// Returns a random real in [min,max).
		return min + (max-min)*randomDouble();
	}
	
	inline glm::dvec3 randomInUnitSphere() {
		double angle1 = randomDouble(0, 2*pi);
		double angle2 = randomDouble(0, 2*pi);
		
		return glm::dvec3(std::sin(angle1)*std::cos(angle2),
						  std::sin(angle1)*std::sin(angle2),
						  std::cos(angle1));
	}
	
	inline glm::dvec3 randomUnitVector() {
		return glm::normalize(randomInUnitSphere());
	}
	
	inline glm::dvec3 randomOnHemisphere(const glm::dvec3& normal) {
		glm::dvec3 on_unit_sphere = randomUnitVector();
		if (glm::dot(on_unit_sphere, normal) > epsilon) // In the same hemisphere as the normal
			return on_unit_sphere;
		else
			return -on_unit_sphere;
	}
	
	/*inline glm::dvec3 randomVec()
	{
		// Returns a random real in [0,1).
		return glm::dvec3(randomDouble(), randomDouble(), randomDouble());
	}
	
	inline glm::dvec3 randomVec(double min, double max)
	{
		// Returns a random real in [min,max).
		return glm::dvec3(randomDouble(min, max), randomDouble(min, max), randomDouble(min, max));
	}*/
	
	inline int randomInt(int min, int max) {
		// Returns a random integer in [min,max].
		return static_cast<int>(randomDouble(min, max+1));
	}
	
	inline glm::dvec3 randomInUnitDisk() {
		//use polar coordinates to find random angle and convert to x,y with sin and cos
		double y = randomDouble(0.0, 2*pi);
		double x = randomDouble(0.0, 2*pi);
		y = glm::sin(y);
		x = glm::sin(x);
		return glm::dvec3(x, y, 0.0);
	}
	
	inline void gammaCorrect(glm::dvec3& col, id_t samples)
	{
		col = col * (1.0/(samples));
		col.x = std::sqrt(col.x);
		col.y = std::sqrt(col.y);
		col.z = std::sqrt(col.z);
	}
	
	inline bool nearZero(const glm::dvec3& v)
	{
        // Return true if the vector is close to zero in all dimensions.
        return (fabs(v.x) < epsilonCam) && (fabs(v.y) < epsilonCam) && (fabs(v.z) < epsilonCam);
    }
    
    inline glm::dvec3 reflect(const glm::dvec3& v, const glm::dvec3& n) {
		return v - 2*glm::dot(v,n)*n;
	}
	
	inline glm::dvec3 refract(const glm::dvec3& uv, const glm::dvec3& n, double etai_over_etat) 
	{
		auto cos_theta = fmin(glm::dot(-uv, n), 1.0);
		glm::dvec3 r_out_perp     =  etai_over_etat * (uv + cos_theta*n);
		glm::dvec3 r_out_parallel = -sqrt(fabs(1.0 - glm::dot(r_out_perp, r_out_perp))) * n;
		return r_out_perp + r_out_parallel;
	}
}
