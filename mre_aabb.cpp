#include "mre_aabb.hpp"

namespace mre
{
	MreAABB::MreAABB(const std::vector<std::shared_ptr<MreTriangle>>& tris, const glm::dvec3& nnmin, const glm::dvec3& nnmax, int minTri, bool first)
	{
		//check how many triangles are in box
		min = nnmin;
		max = nnmax;
		nmax = glm::dvec3(-infinity);
		nmin = glm::dvec3(infinity);
		for(const auto& tri : tris)
		{
			include(tri, first);
		}
		max = nmax;
		min = nmin;
		
		if(triangles.size() > minTri && (triangles.size() != tris.size() || first))
		{
			/*std::cout << "Node | Num triangles: " << triangles.size() << std::endl;
			std::cout << "Min | " << min.x << " " << min.y << " " << min.z << std::endl;
			std::cout << "Max | " << max.x << " " << max.y << " " << max.z << std::endl << std::endl;*/
			lastNode = false;
			makeKids(minTri);
			triangles.clear();
		} else
		{
			lastNode = true;
			/*std::cout << "Leaf Node | Num triangles: " << triangles.size() << std::endl;
			std::cout << "Min | " << min.x << " " << min.y << " " << min.z << std::endl;
			std::cout << "Max | " << max.x << " " << max.y << " " << max.z << std::endl << std::endl;*/
		}
	}
	
	MreAABB::~MreAABB(){}
	
	double MreAABB::hit(const MreRay& ray, const MreHitRec& rec)
	{
		glm::dvec3 tMin = (min - ray.origin) * ray.invDir;
		glm::dvec3 tMax = (max - ray.origin) * ray.invDir;
		glm::dvec3 t0 = glm::min(tMin, tMax);
		glm::dvec3 t1 = glm::max(tMin, tMax);
		
		double dstFar = glm::min(glm::min(t1.x, t1.y), t1.z);
		double dstNear = glm::max(glm::max(t0.x, t0.y), t0.z);
		
		bool hit = dstFar >= dstNear && dstFar > epsilon && dstNear < rec.at;
		return hit ? dstNear : infinity;
	}
	
	bool MreAABB::hitTri(const MreRay& ray, MreHitRec& rec)
	{
		for(const auto& tri : triangles)
		{
			tri->hit(ray, rec);
		}
		return rec.hit;
	}
	
	void MreAABB::hitKids(const MreRay& ray, MreHitRec& rec)
	{
		if(!lastNode)
		{
			double dst0 = childs[0]->hit(ray, rec);
			double dst1 = childs[1]->hit(ray, rec);
			if(dst0 < dst1 && dst0 < rec.at) 
			{
				childs[0]->hitKids(ray, rec);
				if(dst1 < rec.at)
					childs[1]->hitKids(ray, rec);
			}
			else if(dst1 < rec.at)
			{
				childs[1]->hitKids(ray, rec);
				if(dst0 < rec.at)
					childs[0]->hitKids(ray, rec);
			}
		} else
		{
			hitTri(ray, rec);
		}
	}
	
	void MreAABB::include(const std::shared_ptr<MreTriangle> tri, bool first)
	{
		if(tri->center.x <= max.x && tri->center.y <= max.y && tri->center.z <= max.z &&
		   tri->center.x > min.x && tri->center.y > min.y && tri->center.z > min.z)
		{
			nmax = glm::max(glm::max(tri->posA, nmax), glm::max(tri->posB, tri->posC));
			nmin = glm::min(glm::min(tri->posA, nmin), glm::min(tri->posB, tri->posC));
			triangles.push_back(tri);
		}
		else if(first)
		{
			nmax = glm::max(glm::max(tri->posA, nmax), glm::max(tri->posB, tri->posC));
			nmin = glm::min(glm::min(tri->posA, nmin), glm::min(tri->posB, tri->posC));
			triangles.push_back(tri);
		}
	}
	
	void MreAABB::makeKids(int minTri)
	{
		//max is going to be divided by cut and min, so 1 = no stay the same, 2 = divided by 2 
		glm::dvec3 sides = max-min;
		if(fabs(sides.x) >= fabs(sides.y) && fabs(sides.x) >= fabs(sides.z)) {cut = glm::dvec3(0.5, 1.0, 1.0);}
		else if(fabs(sides.y) >= fabs(sides.x) && fabs(sides.y) >= fabs(sides.z)) {cut = glm::dvec3(1.0, 0.5, 1.0);}
		else {cut = glm::dvec3(1.0, 1.0, 0.5);}
		
		glm::dvec3 newBoundmax = max-sides*cut;
		glm::dvec3 newBoundmin = min+sides*cut;
		
		//first always closest to center
		childs.push_back(std::make_shared<MreAABB>(triangles, min, newBoundmin, minTri));
		childs.push_back(std::make_shared<MreAABB>(triangles, newBoundmax, max, minTri));
	}
}
