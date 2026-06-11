#include "mre_model.hpp"

namespace mre
{
	MreModel::MreModel(std::string file, std::shared_ptr<MreMaterial> mat, const MreTransformModel& transform, int sidesToSee, int minTriInBVH)
	{
		Assimp::Importer importer;
		
		//specify post processing steps
		const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals);

		//process each mesh in the scene
		for(id_t m = 0; m < scene->mNumMeshes; m++)
		{
			//get mesh
			aiMesh* mesh = scene->mMeshes[m];
			//access faces
			for(id_t f = 0; f < mesh->mNumFaces; f++)
			{
				aiFace face = mesh ->mFaces[f];
				
				//retrieve vertex indices
				auto index1 = face.mIndices[0];
				auto index2 = face.mIndices[1];
				auto index3 = face.mIndices[2];
				
				//retrieve vertex
				glm::dvec3 vert1(mesh->mVertices[index1].x, mesh->mVertices[index1].y, mesh->mVertices[index1].z);
				glm::dvec3 vert2(mesh->mVertices[index2].x, mesh->mVertices[index2].y, mesh->mVertices[index2].z);
				glm::dvec3 vert3(mesh->mVertices[index3].x, mesh->mVertices[index3].y, mesh->mVertices[index3].z);
				
				//retrieve normals
				glm::dvec3 normal1(mesh->mNormals[index1].x, mesh->mNormals[index1].y, mesh->mNormals[index1].z);
				glm::dvec3 normal2(mesh->mNormals[index2].x, mesh->mNormals[index2].y, mesh->mNormals[index2].z);
				glm::dvec3 normal3(mesh->mNormals[index3].x, mesh->mNormals[index3].y, mesh->mNormals[index3].z);
				
				
				//rotation
				glm::dmat4 rotMatX = glm::dmat4(1.0);
				rotMatX[1][1] = glm::cos(glm::radians(transform.rotation.x));
				rotMatX[1][2] = -glm::sin(glm::radians(transform.rotation.x));
				rotMatX[2][1] = glm::sin(glm::radians(transform.rotation.x));
				rotMatX[2][2] = glm::cos(glm::radians(transform.rotation.x));
				
				glm::dmat4 rotMatY = glm::dmat4(1.0);
				rotMatY[0][0] = glm::cos(glm::radians(transform.rotation.y));
				rotMatY[0][2] = glm::sin(glm::radians(transform.rotation.y));
				rotMatY[2][0] = -glm::sin(glm::radians(transform.rotation.y));
				rotMatY[2][2] = glm::cos(glm::radians(transform.rotation.y));
				
				glm::dmat4 rotMatZ = glm::dmat4(1.0);
				rotMatZ[0][0] = glm::cos(glm::radians(transform.rotation.z));
				rotMatZ[0][1] = -glm::sin(glm::radians(transform.rotation.z));
				rotMatZ[1][0] = glm::sin(glm::radians(transform.rotation.z));
				rotMatZ[1][1] = glm::cos(glm::radians(transform.rotation.z));
				
				glm::dmat4 finalRot = rotMatX * rotMatZ * rotMatY;
				
				//scale
				glm::dmat4 scaleMat = glm::dmat4(1.0);
				scaleMat[0][0] = transform.scale.x;
				scaleMat[1][1] = transform.scale.y;
				scaleMat[2][2] = transform.scale.z;
				
				//position
				glm::dmat4 transMat = glm::dmat4(1.0);
				transMat[3][0] = transform.position.x;
				transMat[3][1] = transform.position.y;
				transMat[3][2] = transform.position.z;
				
				//all transformations
				glm::dmat4 allMat = transMat * finalRot * scaleMat;
				
				vert1 = glm::dvec3(allMat * glm::dvec4(vert1, 1.0));
				vert2 = glm::dvec3(allMat * glm::dvec4(vert2, 1.0));
				vert3 = glm::dvec3(allMat * glm::dvec4(vert3, 1.0));
				
				normal1 = glm::normalize(glm::dvec3(finalRot * glm::dvec4(normal3, 1.0)));
				normal2 = glm::normalize(glm::dvec3(finalRot * glm::dvec4(normal2, 1.0)));
				normal3 = glm::normalize(glm::dvec3(finalRot * glm::dvec4(normal3, 1.0)));
				
				triangles.push_back(std::make_shared<MreTriangle>(vert1, vert2, vert3, normal1, normal2, normal3, mat, sidesToSee));
			}
		}
		
		bvh = MreAABB(triangles, glm::dvec3(0.0), glm::dvec3(0.0), minTriInBVH, true);
		triangles.clear();
	}
}
