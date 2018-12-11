#include <thread>

#include "assignment7/Assignment7.h"
#include "common/core.h"

std::shared_ptr<Camera> Assignment7::CreateCamera() const
{
	const glm::vec2 resolution = GetImageOutputResolution();
	//    std::shared_ptr<Camera> camera = std::make_shared<PerspectiveCamera>(resolution.x / resolution.y, 26.6f);
	std::shared_ptr<Camera> camera = std::make_shared<PerspectiveCamera>(resolution.x / resolution.y, 50, 50, 300);// 60.0f);

	// SceneObject::GetWorldUp() // 0.f, 1.f, 0.f

//	camera->SetPosition(glm::vec3(3.579f, -29.794f, 100.f));
//	camera->Rotate(glm::vec3(0.f, 1.f, 0.f) , -0.35f); // get world up

//	camera->SetPosition(glm::vec3(0.f, -29.794f, 80.f));

	camera->SetPosition(glm::vec3(0.f, 0.f, 0.f));
	//	camera->Rotate(glm::vec3(SceneObject::GetWorldRight()), 20/180.0*3.15149); // get world right
	//	camera->Rotate(glm::vec3(camera->GetForwardDirection()), -7 / 180.0*3.15149); // trying recovery??

	camera->SetPosition(glm::vec3(0.f, 16.958f, 77));
	camera->Rotate(glm::vec3(SceneObject::GetWorldUp()), -20 * 3.14159 / 180.0f);  // -0.42f); // get world up
	// camera->Rotate(glm::vec3(camera->GetWorldToObjectMatrix() * SceneObject::GetWorldUp()), 19.524*3.14159 / 180.0f);  // -0.42f); // get world up


//	camera->Rotate(glm::vec3(0.f, 1.f, 0.f), -0.42f); // get world up
//	camera->Rotate(glm::vec3(1.f, 0.f, 0.f), 1.7845801995f); // get world right

	return camera;
}


// Assignment 7 Part 1 TODO: Change the '1' here.
// 0 -- Naive.
// 1 -- BVH.
// 2 -- Grid.
#define ACCELERATION_TYPE 2

std::shared_ptr<Scene> Assignment7::CreateScene() const
{
	std::shared_ptr<Scene> newScene = std::make_shared<Scene>();
	std::shared_ptr<SceneObject> iguanabranchSceneObject = std::make_shared<SceneObject>();
	std::shared_ptr<SceneObject> TreeSceneObject = std::make_shared<SceneObject>();
	std::shared_ptr<SceneObject> DragonFlyObject = std::make_shared<SceneObject>();
	std::shared_ptr<SceneObject> BackgroundObject = std::make_shared<SceneObject>();

	// background
	std::vector<std::shared_ptr<aiMaterial>> backgroundMaterials;
	std::vector<std::shared_ptr<MeshObject>> backgroundObjs = MeshLoader::LoadMesh("background_plane.obj", &backgroundMaterials);
	std::shared_ptr<BlinnPhongMaterial> backgroundMat = std::make_shared<BlinnPhongMaterial>();
	backgroundMat->SetDiffuse(glm::vec3(1.0f, 1.0f, 1.0f));
	backgroundMat->SetSpecular(glm::vec3(0.0f, 0.0f, 0.0f), 40.f);
	backgroundMat->SetReflectivity(0.f);
	backgroundMat->SetTexture("diffuseTexture", TextureLoader::LoadTexture("background/forest_bokeh.jpg"));
	backgroundObjs[0]->SetMaterial(backgroundMat);
	BackgroundObject->AddMeshObject(backgroundObjs[0]);

	// Branch
	std::vector<std::shared_ptr<aiMaterial>> loadedBranchMaterials;
	std::vector<std::shared_ptr<MeshObject>> branchMeshObjs = MeshLoader::LoadMesh("branch_positioned.obj", &loadedBranchMaterials);
	std::shared_ptr<BlinnPhongMaterial> branchMat = std::make_shared<BlinnPhongMaterial>();
	branchMat->SetDiffuse(glm::vec3(1.0f, 1.0f, 1.0f));
	branchMat->SetSpecular(glm::vec3(0.0f, 0.0f, 0.0f), 40.f);
	branchMat->SetReflectivity(0.f);
	branchMat->SetTexture("diffuseTexture", TextureLoader::LoadTexture("L2_gray.jpg"));
	branchMat->SetTexture("normalTexture", TextureLoader::LoadTexture("branch/L2_object_normals.jpg"));
	branchMeshObjs[0]->SetMaterial(branchMat->Clone());

	// Chameleon
	std::vector<std::shared_ptr<aiMaterial>> loadedIguanaMaterials;
	std::vector<std::shared_ptr<MeshObject>> iguanaMeshObjs = MeshLoader::LoadMesh("chameleon_on_branch_2.obj", &loadedIguanaMaterials);
	std::shared_ptr<BlinnPhongMaterial> iguanaMat = std::make_shared<BlinnPhongMaterial>();
	iguanaMat->SetReflectivity(0.f);
	iguanaMat->SetTexture("diffuseTexture", TextureLoader::LoadTexture("diffuse map.jpg"));
	iguanaMat->SetTexture("normalTexture", TextureLoader::LoadTexture("chameleon/textures/NormalMap.png"));
	iguanaMat->SetDiffuse(glm::vec3(1.0f, 1.0f, 1.0f));
	iguanaMat->SetSpecular(glm::vec3(1.0f, 1.0f, 1.0f), 40.f);
	iguanaMeshObjs[0]->SetMaterial(iguanaMat);

	// eye
	std::vector<std::shared_ptr<aiMaterial>> eyeMaterials;
	std::vector<std::shared_ptr<MeshObject>> eyeMeshObjs = MeshLoader::LoadMesh("right_eye_2.obj", &eyeMaterials);
	eyeMeshObjs[0]->SetMaterial(iguanaMat);

	// tongue
	std::vector<std::shared_ptr<MeshObject>> tongueMeshObjs = MeshLoader::LoadMesh("tongue.obj");
	std::shared_ptr<BlinnPhongMaterial> tongueMat = std::make_shared<BlinnPhongMaterial>();
	tongueMat->SetDiffuse(glm::vec3(0.969f, .6f, .6f));
	tongueMat->SetSpecular(glm::vec3(0.969f, .6f, .6f), 40.f);
	tongueMeshObjs[0]->SetMaterial(tongueMat);

	// iguanaSceneObject
	iguanabranchSceneObject->AddMeshObject(branchMeshObjs[0]);
	iguanabranchSceneObject->AddMeshObject(iguanaMeshObjs[0]);
	iguanabranchSceneObject->AddMeshObject(eyeMeshObjs[0]);
	iguanabranchSceneObject->AddMeshObject(tongueMeshObjs[0]);

	// dragonfly 
	std::vector<std::shared_ptr<aiMaterial>> loadedDragonflyMaterials;
	std::vector<std::shared_ptr<MeshObject>> dragonflyMeshObjs = MeshLoader::LoadMesh("dragonfly.obj", &loadedDragonflyMaterials);
	std::shared_ptr<BlinnPhongMaterial> dragonflyMat = std::make_shared<BlinnPhongMaterial>();
	dragonflyMat->SetReflectivity(0.f);
	dragonflyMat->SetTexture("diffuseTexture", TextureLoader::LoadTexture("dragonfly/combined.jpg"));
	dragonflyMat->SetDiffuse(glm::vec3(1.0f, 1.0f, 1.0f));
	dragonflyMat->SetSpecular(glm::vec3(1.0f, 1.0f, 1.0f), 40.f);
	dragonflyMeshObjs[0]->SetMaterial(dragonflyMat);
	DragonFlyObject->AddMeshObject(dragonflyMeshObjs[0]);

	// Tree (trunk)
	std::vector<std::shared_ptr<aiMaterial>> TreeMaterials;
	std::vector<std::shared_ptr<MeshObject>> TreeMeshObjs = MeshLoader::LoadMesh("trunk.obj", &TreeMaterials);
	std::shared_ptr<BlinnPhongMaterial> TreeMat = std::make_shared<BlinnPhongMaterial>();
	TreeMat->SetReflectivity(0.f);
	TreeMat->SetTexture("diffuseTexture", TextureLoader::LoadTexture("tree/Textures/bark_tree_brown.jpg"));
	TreeMat->SetTexture("normalTexture", TextureLoader::LoadTexture("tree/Textures/bark_tree_nor.jpg"));
	TreeMat->SetDiffuse(glm::vec3(1.0f, 1.0f, 1.0f));
	TreeMat->SetSpecular(glm::vec3(0.0f, 0.0f, 0.0f), 10.f);
	TreeMeshObjs[0]->SetMaterial(TreeMat);
	TreeSceneObject->AddMeshObject(TreeMeshObjs[0]);

	std::vector<std::shared_ptr<MeshObject>> BranchMeshObjs = MeshLoader::LoadMesh("branches_cleaned_up.obj");
	BranchMeshObjs[0]->SetMaterial(TreeMat);
	TreeSceneObject->AddMeshObject(BranchMeshObjs[0]);

	// leaves 1
	std::vector<std::shared_ptr<aiMaterial>> leaveas1Materials;
	std::vector<std::shared_ptr<MeshObject>> leaves1MeshObjs = MeshLoader::LoadMesh("leaves_1.obj", &leaveas1Materials);
	std::shared_ptr<BlinnPhongMaterial> leaves1Mat = std::make_shared<BlinnPhongMaterial>();
	leaves1Mat->SetReflectivity(0.f);
	leaves1Mat->SetTexture("diffuseTexture", TextureLoader::LoadTexture("tree/Textures/leaves_01.jpg"));
	leaves1Mat->SetTexture("alphaTexure", TextureLoader::LoadTexture("tree/Textures/leaves_alpha.jpg"));
	leaves1Mat->SetDiffuse(glm::vec3(1.0f, 1.0f, 1.0f));
	leaves1Mat->SetSpecular(glm::vec3(0.0f, 0.0f, 0.0f), 40.f);
	leaves1MeshObjs[0]->SetMaterial(leaves1Mat);
	TreeSceneObject->AddMeshObject(leaves1MeshObjs[0]);

	std::vector<std::shared_ptr<MeshObject>> leaves1MeshObjs_rev = MeshLoader::LoadMesh("leaves_1_rev.obj", &leaveas1Materials);
	leaves1MeshObjs_rev[0]->SetMaterial(leaves1Mat);
	TreeSceneObject->AddMeshObject(leaves1MeshObjs_rev[0]);

	// leaves 2
	std::vector<std::shared_ptr<aiMaterial>> leaveas2Materials;
	std::vector<std::shared_ptr<MeshObject>> leaves2MeshObjs = MeshLoader::LoadMesh("leaves_2_alpha.obj", &leaveas2Materials);
	std::shared_ptr<BlinnPhongMaterial> leaves2Mat = std::make_shared<BlinnPhongMaterial>();
	leaves2Mat->SetReflectivity(0.f);
	leaves2Mat->SetTexture("diffuseTexture", TextureLoader::LoadTexture("tree/Textures/leaves_02.jpg"));
	leaves2Mat->SetTexture("alphaTexure", TextureLoader::LoadTexture("tree/Textures/leaves_alpha.jpg"));
	leaves2Mat->SetDiffuse(glm::vec3(1.0f, 1.0f, 1.0f));
	leaves2Mat->SetSpecular(glm::vec3(0.0f, 0.0f, 0.0f), 40.f);
	leaves2MeshObjs[0]->SetMaterial(leaves1Mat);
	TreeSceneObject->AddMeshObject(leaves2MeshObjs[0]);

	std::vector<std::shared_ptr<MeshObject>> leaves2MeshObjs_rev = MeshLoader::LoadMesh("leaves_2_alpha_rev.obj", &leaveas2Materials);
	leaves2MeshObjs_rev[0]->SetMaterial(leaves2Mat);
	TreeSceneObject->AddMeshObject(leaves2MeshObjs_rev[0]);

	// leaves 3
	std::vector<std::shared_ptr<aiMaterial>> leaveas3Materials;
	std::vector<std::shared_ptr<MeshObject>> leaves3MeshObjs = MeshLoader::LoadMesh("leaves_3.obj", &leaveas3Materials);
	std::shared_ptr<BlinnPhongMaterial> leaves3Mat = std::make_shared<BlinnPhongMaterial>();
	leaves3Mat->SetReflectivity(0.f);
	leaves3Mat->SetTexture("diffuseTexture", TextureLoader::LoadTexture("tree/Textures/leaves_03.jpg"));
	leaves3Mat->SetTexture("alphaTexure", TextureLoader::LoadTexture("tree/Textures/leaves_alpha.jpg"));
	leaves3Mat->SetDiffuse(glm::vec3(1.0f, 1.0f, 1.0f));
	leaves3Mat->SetSpecular(glm::vec3(0.0f, 0.0f, 0.0f), 40.f);
	leaves3MeshObjs[0]->SetMaterial(leaves3Mat);
	TreeSceneObject->AddMeshObject(leaves3MeshObjs[0]);

	std::vector<std::shared_ptr<MeshObject>> leaves3MeshObjs_rev = MeshLoader::LoadMesh("leaves_3_rev.obj", &leaveas3Materials);
	leaves3MeshObjs_rev[0]->SetMaterial(leaves3Mat);
	TreeSceneObject->AddMeshObject(leaves3MeshObjs_rev[0]);

// Get Maya coordinates to match up: -forward to x, up to y and  right to z
// currently  world up is y, world right is x, forward is -z 
// z needs to go to x, and x needs to go to z

	iguanabranchSceneObject->Rotate(glm::vec3(SceneObject::GetWorldUp()), 3.141592f*.5f); // rotating to face right
	TreeSceneObject->Rotate(glm::vec3(SceneObject::GetWorldUp()), 3.141592f*.5f);
	DragonFlyObject->Rotate(glm::vec3(SceneObject::GetWorldUp()), 3.141592f*.5f);
	BackgroundObject->Rotate(glm::vec3(SceneObject::GetWorldUp()), 3.141592f*.5f);

	iguanabranchSceneObject->CreateAccelerationData(AccelerationTypes::BVH);
	iguanabranchSceneObject->ConfigureAccelerationStructure([](AccelerationStructure* genericAccelerator) {
		BVHAcceleration* accelerator = dynamic_cast<BVHAcceleration*>(genericAccelerator);
		accelerator->SetMaximumChildren(2);
		accelerator->SetNodesOnLeaves(2);
	});
	iguanabranchSceneObject->ConfigureChildMeshAccelerationStructure([](AccelerationStructure* genericAccelerator) {
		BVHAcceleration* accelerator = dynamic_cast<BVHAcceleration*>(genericAccelerator);
		accelerator->SetMaximumChildren(2);
		accelerator->SetNodesOnLeaves(2);
	});

	BackgroundObject->CreateAccelerationData(AccelerationTypes::BVH);
	BackgroundObject->ConfigureAccelerationStructure([](AccelerationStructure* genericAccelerator) {
		BVHAcceleration* accelerator = dynamic_cast<BVHAcceleration*>(genericAccelerator);
		accelerator->SetMaximumChildren(2);
		accelerator->SetNodesOnLeaves(2);
	});
	BackgroundObject->ConfigureChildMeshAccelerationStructure([](AccelerationStructure* genericAccelerator) {
		BVHAcceleration* accelerator = dynamic_cast<BVHAcceleration*>(genericAccelerator);
		accelerator->SetMaximumChildren(2);
		accelerator->SetNodesOnLeaves(2);
	});

	DragonFlyObject->CreateAccelerationData(AccelerationTypes::BVH);
	DragonFlyObject->ConfigureAccelerationStructure([](AccelerationStructure* genericAccelerator) {
		BVHAcceleration* accelerator = dynamic_cast<BVHAcceleration*>(genericAccelerator);
		accelerator->SetMaximumChildren(2);
		accelerator->SetNodesOnLeaves(2);
	});
	DragonFlyObject->ConfigureChildMeshAccelerationStructure([](AccelerationStructure* genericAccelerator) {
		BVHAcceleration* accelerator = dynamic_cast<BVHAcceleration*>(genericAccelerator);
		accelerator->SetMaximumChildren(2);
		accelerator->SetNodesOnLeaves(2);
	});

	TreeSceneObject->CreateAccelerationData(AccelerationTypes::BVH);
	TreeSceneObject->ConfigureAccelerationStructure([](AccelerationStructure* genericAccelerator) {
		BVHAcceleration* accelerator = dynamic_cast<BVHAcceleration*>(genericAccelerator);
		accelerator->SetMaximumChildren(2);
		accelerator->SetNodesOnLeaves(2);
	});

	TreeSceneObject->ConfigureChildMeshAccelerationStructure([](AccelerationStructure* genericAccelerator) {
		BVHAcceleration* accelerator = dynamic_cast<BVHAcceleration*>(genericAccelerator);
		accelerator->SetMaximumChildren(2);
		accelerator->SetNodesOnLeaves(2);
	});


	newScene->AddSceneObject(iguanabranchSceneObject);
	newScene->AddSceneObject(TreeSceneObject);
	newScene->AddSceneObject(DragonFlyObject);
	newScene->AddSceneObject(BackgroundObject);

	// Lights
	std::shared_ptr<Light> areaLight = std::make_shared<AreaLight>(glm::vec2(30, 30));
	// areaLight->SetPosition(glm::vec3(-114.940f, -29.794f, 3.579f));
	areaLight->SetPosition(glm::vec3(0.f, 0.f, 0.f));
	// SceneObject::GetWorldForward() // 0.f, 0.f, -1.f
	// SceneObject::GetWorldRight() // 1.f, 0.f, 0.f
	// SceneObject::GetWorldUp() // 0.f, 1.f, 0.f
	areaLight->Translate(glm::vec3(3.579f, -29.794f, 114.940));

	areaLight->SetLightColor(glm::vec3(2.f, 2.f, 2.f));  // 2.f, 2.f, 2.f)); //1.f, 1.f, 1.f));

#if ACCELERATION_TYPE == 0
	newScene->GenerateAccelerationData(AccelerationTypes::NONE);
#elif ACCELERATION_TYPE == 1
	newScene->GenerateAccelerationData(AccelerationTypes::BVH);
#else
	UniformGridAcceleration* accelerator = dynamic_cast<UniformGridAcceleration*>(newScene->GenerateAccelerationData(AccelerationTypes::UNIFORM_GRID));
	assert(accelerator);
	// Assignment 7 Part 2 TODO: Change the glm::ivec3(10, 10, 10) here.

	glm::vec2 imgSize = GetImageOutputResolution();
	accelerator->SetSuggestedGridSize(glm::ivec3(imgSize[0] / 100, imgSize[1] / 100, 10));
#endif    
	newScene->AddLight(areaLight);

	return newScene;

}

std::shared_ptr<ColorSampler> Assignment7::CreateSampler() const
{
	std::shared_ptr<JitterColorSampler> jitter = std::make_shared<JitterColorSampler>();
	jitter->SetGridSize(glm::ivec3(4, 4, 1));

	std::shared_ptr<SimpleAdaptiveSampler> sampler = std::make_shared<SimpleAdaptiveSampler>();
	sampler->SetInternalSampler(jitter);

	// ASSIGNMENT 6 TODO: Change the '1.f' in '1.f * SMALL_EPSILON' here to be higher and see what your results are. (Part 3)
	sampler->SetEarlyExitParameters(1.f * SMALL_EPSILON, 4);

	return sampler;
}

std::shared_ptr<class Renderer> Assignment7::CreateRenderer(std::shared_ptr<Scene> scene, std::shared_ptr<ColorSampler> sampler) const
{
	return std::make_shared<BackwardRenderer>(scene, sampler);
}

int Assignment7::GetSamplesPerPixel() const
{
	return 32;
}

bool Assignment7::NotifyNewPixelSample(glm::vec3 inputSampleColor, int sampleIndex)
{
	return true;
}

int Assignment7::GetMaxReflectionBounces() const
{
	return 2;
}

int Assignment7::GetMaxRefractionBounces() const
{
	return 4;
}

glm::vec2 Assignment7::GetImageOutputResolution() const
{
	//	return glm::vec2(185*2, 138.75*2);// 4*640.f, 4*480.f);
	return glm::vec2(185 * 2, 136 * 2);// 4*640.f, 4*480.f);
}
