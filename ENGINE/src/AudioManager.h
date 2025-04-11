#pragma once

#include "Sound.h"
#include "miniaudio/miniaudio.h"
#include <string>
#include <vector>
#include <iostream>

class AudioManager {
public:
	AudioManager();
	~AudioManager();

	bool PlaySound(const std::string& filePath);
	void Shutdown();
	void SetListenerPosition(float x, float y, float z);
	void EnableEcho();
	void DisableEcho();


	ma_engine engine; // Rendu public pour permettre l'acc�s depuis Sound

private:
	bool Init();
	std::vector<Sound*> sounds;
	ma_node_graph nodeGraph;
	ma_delay_node delayNode;
	bool echoEnabled = false;
};

// Position du joueur
static float listenerX = 0.0f;
static float listenerY = 0.0f;
static float listenerZ = 0.0f;

// Position du son
static float soundX = 0.0f;
static float soundY = 0.0f;
static float soundZ = 0.0f;

// Variables de contr�le
static std::atomic<bool> running(true);
static std::atomic<bool> paused(false);

static void UpdateSoundPosition(Sound& sound, AudioManager& audioManager) {
	while (running) {
		if (!paused) {
			// D�placement � gauche
			if (GetAsyncKeyState(VK_LEFT)) {
				soundX -= 0.5f;
			}

			// D�placement � droite
			if (GetAsyncKeyState(VK_RIGHT)) {
				soundX += 0.5f;
			}

			// D�placement vers l'avant
			if (GetAsyncKeyState(VK_UP)) {
				soundZ += 0.5f;
			}

			// D�placement vers l'arri�re
			if (GetAsyncKeyState(VK_DOWN)) {
				soundZ -= 0.5f;
			}

			// Mise � jour de la position du son
			sound.SetPosition(soundX, soundY, soundZ);

			// Mettre � jour la position du joueur (�couteur)
			audioManager.SetListenerPosition(listenerX, listenerY, listenerZ);

			std::cout << "Position du son : (" << soundX << ", " << soundY << ", " << soundZ << ")\n";

			Sleep(100);
		}
	}
}