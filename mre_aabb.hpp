#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include "mre_triangle.hpp"

namespace mre
{
	class MreAABB
	{
		public:
			//only used if not first node
			MreAABB(const std::vector<std::shared_ptr<MreTriangle>>& tris, const glm::dvec3& nnmin, const glm::dvec3& nnmax, int minTri = 5, bool first = false);
			MreAABB(){}
			
			~MreAABB();
			
			double hit(const MreRay& ray, const MreHitRec& rec); //returns dist to ray if hit, if not returns infinity
			void hitKids(const MreRay& ray, MreHitRec& rec); //returns dist to ray if hit, if not returns infinity
			bool hitTri(const MreRay& ray, MreHitRec& rec);
			
			//return true if is included and grows to include triangle, returns false and stays the same
			//tri to include
			void include(const std::shared_ptr<MreTriangle> tri, bool first);
			
			void makeKids(int minTri);
			
			//if kids
			bool lastNode;
			
			std::vector<std::shared_ptr<MreAABB>> childs; //pointer of childs
			std::vector<std::shared_ptr<MreTriangle>> triangles; //triangle indices
		private:
			glm::dvec3 min; //min on each axis
			glm::dvec3 max; //max on each axis
			glm::dvec3 nmax; //change max to fit each tri
			glm::dvec3 nmin; //change min to fit each tri
			glm::dvec3 cut; //axis to cut on, always cut in half
	};
}
