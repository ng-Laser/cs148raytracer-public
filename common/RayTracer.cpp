// #include "stdafx.h"
#include <thread>
#include <vector>
// #include <omp.h>


#include "common/RayTracer.h"
#include "common/Application.h"
#include "common/Scene/Scene.h"
#include "common/Scene/Camera/Camera.h"
#include "common/Scene/Geometry/Ray/Ray.h"
#include "common/Intersection/IntersectionState.h"
#include "common/Sampling/ColorSampler.h"
#include "common/Output/ImageWriter.h"
#include "common/Rendering/Renderer.h"

// #include "common/boost_1_67_0/boost_1_67_0/boost/asio/thread_pool.hpp"
// #include "common/ctpl_stl.h"

// using namespace std;
// #undef _STD
// #define _STD  std::
// #include "stdafx.h"
// #include <omp.h>

#include "common/Scene/Geometry/Primitives/Triangle/Triangle.h"
RayTracer::RayTracer(std::unique_ptr<class Application> app) :
	storedApplication(std::move(app))
{
}

void RayTracer::Run()
{
	// Scene Setup -- Generate the camera and scene.
	DIAGNOSTICS_TIMER(timer1, "Set up time");
	std::shared_ptr<Camera> currentCamera = storedApplication->CreateCamera();
	std::shared_ptr<Scene> currentScene = storedApplication->CreateScene();
	std::shared_ptr<ColorSampler> currentSampler = storedApplication->CreateSampler();
	std::shared_ptr<Renderer> currentRenderer = storedApplication->CreateRenderer(currentScene, currentSampler);
	assert(currentScene && currentCamera && currentSampler && currentRenderer);
	DIAGNOSTICS_END_TIMER(timer1);
	DIAGNOSTICS_PRINT();

	currentSampler->InitializeSampler(storedApplication.get(), currentScene.get());

	// Scene preprocessing -- generate acceleration structures, etc.
	// After this call, we are guaranteed that the "acceleration" member of the scene and all scene objects within the scene will be non-NULL.
	currentScene->GenerateDefaultAccelerationData();
	currentScene->Finalize();

	currentRenderer->InitializeRenderer();

	// Prepare for Output
	const glm::vec2 currentResolution = storedApplication->GetImageOutputResolution();
	ImageWriter imageWriter(storedApplication->GetOutputFilename(), static_cast<int>(currentResolution.x), static_cast<int>(currentResolution.y));

	// Perform forward ray tracing
	const int maxSamplesPerPixel = storedApplication->GetSamplesPerPixel();
	assert(maxSamplesPerPixel >= 1);

	// printf("max_threads %d", omp_get_max_threads());
	// #pragma omp parallel for num_threads(2)
	// ctpl::thread_pool p(2 /* two threads in the pool */);

	int w = static_cast<int>(currentResolution.x);

	auto get_pixel_values = [&](int start, int end)
	{
//		#pragma omp parallel for num_threads(2)
		for (int itr = start; itr < end; ++itr) {
			int r = itr / w;
			int c = itr % w;
			// for (int r = 0; r < static_cast<int>(currentResolution.y); ++r) {
			// 	for (int c = 0; c < static_cast<int>(currentResolution.x); ++c) {
			//		printf("Thread number %d", omp_get_thread_num());

			glm::vec3 color = currentSampler->ComputeSamplesAndColor(maxSamplesPerPixel, 2, [&](glm::vec3 inputSample) {
				const glm::vec3 minRange(-0.5f, -0.5f, 0.f);
				const glm::vec3 maxRange(0.5f, 0.5f, 0.f);
				const glm::vec3 sampleOffset = (maxSamplesPerPixel == 1) ? glm::vec3(0.f, 0.f, 0.f) : minRange + (maxRange - minRange) * inputSample;

				glm::vec2 normalizedCoordinates(static_cast<float>(c) + sampleOffset.x, static_cast<float>(r) + sampleOffset.y);
				normalizedCoordinates /= currentResolution;

				// Construct ray, send it out into the scene and see what we hit.
				std::shared_ptr<Ray> cameraRay = currentCamera->GenerateRayForNormalizedCoordinates(normalizedCoordinates);
				assert(cameraRay);

				IntersectionState rayIntersection(storedApplication->GetMaxReflectionBounces(), storedApplication->GetMaxRefractionBounces());
				bool didHitScene = currentScene->Trace(cameraRay.get(), &rayIntersection);

				// Use the intersection data to compute the BRDF response.
				glm::vec3 sampleColor;
				if (didHitScene) {
					sampleColor = currentRenderer->ComputeSampleColor(rayIntersection, *cameraRay.get());
				}
				return sampleColor;
			});
// #pragma omp critical
			imageWriter.SetPixelColor(color, c, r);
			//	}
		}
	};


	//	for (int itr = 0; itr < static_cast<int>(currentResolution.y) * static_cast<int>(currentResolution.x); ++itr) {
	int total = static_cast<int>(currentResolution.y) * static_cast<int>(currentResolution.x);
	std::vector<std::thread> v;
	int num_threads = 8;
	for (int i = 0; i < num_threads; ++i) {
		v.push_back(std::thread(get_pixel_values, total / num_threads * i, total / num_threads * (i + 1)));
	}
	for (int i = 0; i < num_threads; ++i) {
		v[i].join();
	}

	//std::thread t1(get_pixel_values, 0, total/8);
	//std::thread t2(get_pixel_values, total / 8, 2* total / 8);
	//std::thread t3(get_pixel_values, 2 * total / 8, 3 * total / 8);
	//std::thread t4(get_pixel_values, 3 * total / 8, 4 * total / 8);
	//std::thread t5(get_pixel_values, 4 * total / 8, 5 * total / 8);
	//std::thread t6(get_pixel_values, 5 * total / 8, 6 * total / 8);
	//std::thread t7(get_pixel_values, 6 * total / 8, 7 * total / 8);
	//std::thread t8(get_pixel_values, 7 * total / 8, total);

	//t1.join();
	//t2.join();
	//t3.join();
	//t4.join();
	//t5.join();
	//t6.join();
	//t7.join();
	//t8.join();
	

	// Apply post-processing steps (i.e. tone-mapper, etc.).
	storedApplication->PerformImagePostprocessing(imageWriter);

	// Now copy whatever is in the HDR data and store it in the bitmap that we will save (aka everything will get clamped to be [0.0, 1.0]).
	imageWriter.CopyHDRToBitmap();

	// Save image.
	imageWriter.SaveImage();
}