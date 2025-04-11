#include <atomic>
#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <windows.h>

#include "ConsoleManager.h"
#include "InputManager.h"
#include "RenderAPI/RenderBackend.h"

#include "AudioManager.h"
#include "miniaudio/miniaudio.h"

#define M_PI 3.141592653589793115998

int main(int argc, char* argv[])
{
	if (argc > 2)
	{
		Console::GetInstance().Log("Too many arguments", API::MAIN, LEVEL::ERRORS);
		return 1;
	}
	AudioManager audioManager;
	std::string soundFile = "Sounds/NIKKE.mp3";
	Sound sound(&audioManager.engine, soundFile);

	const enum RendererBackend Backend = ParseBackend(argv[1]);
	Renderer::Init(Backend);

	RendererAPI* renderer = Renderer::Get();

	renderer->Init();
	renderer->createWindow(1280, 720, "Dark Matter Engine");
	renderer->InitUI();
	renderer->LoadShader("shaders\\basic");
	renderer->SetFinnalPassShader("shaders\\FRAME");
	renderer->AddScene();

	Scene* current_scene = renderer->GetCurrentScene();

	current_scene->AddModel("assets\\car\\car.obj", "assets\\car\\tex.jpg");
	current_scene->AddPostProccessPass("shaders\\DEPTH", 1280, 720);
	current_scene->CreatePhysics();

	ModelInfo* modelInfo = current_scene->GetModelInfoAtIdx(0); // Copie de la liste

	current_scene->AddPhysicsGround();
	current_scene->AddPhysicsBody(modelInfo);

	//float timing = 10;
	float i = 0;
	std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now(); // Moment initial
	float deltaTime = 0.0f;

	while (renderer->isWindowOpened())
	{
		// Calcul du delta time (temps �coul� depuis la derniere frame)
		std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> delta = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - lastTime);
		deltaTime = delta.count();
		// Mise � jour de la derni�re frame
		lastTime = currentTime;

		current_scene->UpdatePhysics(deltaTime);
		btTransform transform;
		current_scene->GetPhysics().getRigidBodies().at(0)->GetBulletBody()->getMotionState()->getWorldTransform(transform);
		btVector3 position = transform.getOrigin();
		btVector3 euler;
		transform.getBasis().getEulerZYX(euler[0], euler[1], euler[2]); // Recuperer les angles en radians

		// Convertir en degres si besoin
		euler *= (180.0 / M_PI);

		for (int idx = 0; idx < current_scene->GetModelCount(); idx++)
		{
			ModelInfo* model_info = current_scene->GetModelInfoAtIdx(idx);

			model_info->position[0] = position.getX();
			model_info->position[1] = position.getY();
			model_info->position[2] = position.getZ();

			model_info->rotation[0] = euler.getX();
			model_info->rotation[1] = euler.getY();
			model_info->rotation[2] = euler.getZ();
		}
		renderer->Clear(0.0f, 0.0f, 0.0f, 0.0f);
		current_scene->Draw();
		renderer->DrawUI();
		renderer->Present();
		i += 0.001f;
	}
	current_scene->DestroyPhysics();
	renderer->ClearUI();
	Renderer::Shutdown();

	return 0;
}
