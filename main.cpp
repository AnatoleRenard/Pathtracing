#include "src/mre.hpp"
#include <iostream>
#include <chrono>

/*For Dielctrics -> index of refraction
 * glass: 1.5
 * water: 1.3
 * diamond: 1.8
*/
double water   = 1.3;
double glass   = 1.5;
double diamond = 1.8;

//colors
auto gold = glm::dvec3(0.95, 0.89, 0.76);

/* use only for when using final render with high settings, to test scenes use frame accumulation*/
bool onlyOneImg = false;

using namespace mre;

void cornell(std::shared_ptr<MreObject> world)
{
	/* 
	 * x:      from(-2.5, 2.5)
	 * y:      from(-2.5, 5.0)
	 * z:      from(0.0, 5.0)
	 * center: (0.0, 0.0, 2.5)
	 * camera: (2.5, 2.5, -4.5)
	 * */
	//set up scene
	auto red     = std::make_shared<MreLambertian>(std::make_shared<MreSolidCol>(glm::dvec3(0.65, 0.05, 0.05)));//, 0.9);
    auto blue    = std::make_shared<MreLambertian>(std::make_shared<MreSolidCol>(glm::dvec3(0.05, 0.05, 0.65)));//, 0.9);
    auto green   = std::make_shared<MreLambertian>(std::make_shared<MreSolidCol>(glm::dvec3(0.05, 0.65, 0.05)));//, 0.9);
    auto white   = std::make_shared<MreLambertian>(std::make_shared<MreSolidCol>(glm::dvec3(0.73, 0.73, 0.73)));//, 0.9);
    auto mirror  = std::make_shared<MreMetal>(std::make_shared<MreSolidCol>(glm::dvec3(0.73, 0.73, 0.73)), 0.0);
    auto light   = std::make_shared<MreLight>(std::make_shared<MreSolidCol>(glm::dvec3(3.0, 3.0, 3.0)));

    // Quads
    world->add(std::make_shared<MreQuad>(glm::dvec3(2.5, -2.5, 0.0), glm::dvec3(0.0, 5.0, 0.0), glm::dvec3(0.0, 0.0, 5.0), blue));             //right
    world->add(std::make_shared<MreQuad>(glm::dvec3(-2.5, -2.5, 0.0), glm::dvec3(0.0, 5.0, 0.0), glm::dvec3(0.0, 0.0, 5.0), red));             //left
    world->add(std::make_shared<MreQuad>(glm::dvec3(-1.0, 2.491, 1.5), glm::dvec3(2.0, 0.0, 0.0), glm::dvec3(0.0, 0.0, 2.0), light));          //light
    world->add(std::make_shared<MreQuad>(glm::dvec3(-2.5, -2.5, 0.0), glm::dvec3(5.0, 0.0, 0.0), glm::dvec3(0.0, 0.0, 5.0), mirror));          //bottom
    world->add(std::make_shared<MreQuad>(glm::dvec3(-2.5, 2.5, 0.0), glm::dvec3(5.0, 0.0, 0.0), glm::dvec3(0.0, 0.0, 5.0), white));            //top
    world->add(std::make_shared<MreQuad>(glm::dvec3(-2.5, -2.5, 5.0), glm::dvec3(5.0 , 0.0, 0.0), glm::dvec3(0, 5.0, 0.0), white));            //back
    world->add(std::make_shared<MreQuad>(glm::dvec3(-2.5, -2.5, 0.0), glm::dvec3(5.0 , 0.0, 0.0), glm::dvec3(0, 5.0, 0.0), white, false));     //front
}

int main()
{	
	//sdl set up
	int width  = 500;
	int height = 500;
	MreWindowDescriptor	wd{width, height};
	MreWindow window(wd);
	
	glm::dvec3* img = new glm::dvec3[wd.width*wd.height];
	
	//model
	auto gray = std::make_shared<MreMetal>(std::make_shared<MreSolidCol>(gold), 0.8);
	/*MreTransformModel transform{glm::dvec3(0.75, 0.75, 0.75),
								glm::dvec3(100.0, 30.0, 140.0),
								glm::dvec3(0.0, -1.875, 2.5)};//knight*/
	MreTransformModel transform{glm::dvec3(4.0, 4.0, 4.0),
								glm::dvec3(0.0, 50.0, 0.0),
								glm::dvec3(0.5, -1.4, 2.5)};//Dragon*/
	std::shared_ptr<MreObject> model = std::make_shared<MreModel>("models/Dragon_full.obj", gray, transform, BOTH, 5);
	std::shared_ptr<MreObject> world = std::make_shared<MreWorld>();
	
	//sphere
	std::shared_ptr<MreMaterial> diamondMat = std::make_shared<MreDielectric>(std::make_shared<MreSolidCol>(glm::dvec3(1.0, 0.714, 0.757)), diamond);
	std::shared_ptr<MreObject> sphere = std::make_shared<MreSphere>(glm::dvec3(-1.5, -1.5, 2.0), diamondMat, 1.0);
	
	world->add(model);
	world->add(sphere);
	cornell(world);
	
	//offset randomness
	offsetRandomizer();
	
	//Cam
	MreCameraSetting settings;
	settings.focalLength      = 5.0;
	settings.depth            = 20;
	settings.maxDepth         = true;
	settings.samples          = 10;
	settings.widthCam         = glm::dvec3(5.0, 0.0, 0.0);
	settings.heightCam        = glm::dvec3(0.0, 5.0, 0.0);
	settings.translation      = glm::dvec3(0.0, 0.0, -5.0);
	settings.heightTexture    = height;
	settings.widthTexture     = width;
	settings.defocusAngle     = 1.0;
	
	if(onlyOneImg)
		settings.addImgOverTime = false;
	MreCamera cam(settings);
	
	//sdl render loop to see image
	bool run = true;
	SDL_Event e;
	auto startImg = std::chrono::high_resolution_clock::now();
	while(run)
	{
		//set up timer
		auto start = std::chrono::high_resolution_clock::now();
		
		while(SDL_PollEvent(&e))
		{
			switch(e.type)
			{
				case SDL_QUIT:
					run = false;
					break;
				case SDL_MOUSEBUTTONDOWN:
					window.setFullScreen();
					break;
			}
		}
		
		cam.render(world, img);
		window.update(img);
		window.draw();

		//get end time
		auto stop = std::chrono::high_resolution_clock::now();
		
		//print time it took
		auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
		std::clog << "\rFinished in: " << duration.count() << " sec   " << std::flush;
		
		if(onlyOneImg)
			break;
	}
	std::clog << "\n";
	
	//get end time
	auto stop = std::chrono::high_resolution_clock::now();
		
	//add to time it took
	auto duration = std::chrono::duration_cast<std::chrono::minutes>(stop - startImg);
	std::clog << "\rTotal time: " << duration.count() << " min" << std::flush;
	std::clog << "\n";
	
	//dump img
	const std::string name = "test";
	dumpAsPPM(img, wd, name);
	
	delete[] img;
}
