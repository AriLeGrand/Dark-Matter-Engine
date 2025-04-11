#define MINIAUDIO_IMPLEMENTATION
#include "AudioManager.h"

#include <iostream>
#include <stdexcept>

AudioManager::AudioManager()
{
	if (!Init())
	{
		throw std::runtime_error("Échec de l'initialisation de Miniaudio");
	}
}

AudioManager::~AudioManager()
{
	Shutdown();
}

bool AudioManager::Init()
{
	ma_result result = ma_engine_init(NULL, &engine);
	if (result != MA_SUCCESS)
	{
		std::cerr << "Erreur : Impossible d'initialiser miniaudio (code " << result << ")\n";
		return false;
	}

	ma_delay_node_config delayConfig = ma_delay_node_config_init(2, 48000, 0.3f, 0.0f);
	result = ma_delay_node_init(ma_engine_get_node_graph(&engine), &delayConfig, nullptr, &delayNode);
	if (result != MA_SUCCESS)
	{
		std::cerr << "Erreur : Impossible d'initialiser le delay node (code " << result << ")\n";
		return false;
	}

	return true;
}

void AudioManager::Shutdown() {}

bool AudioManager::PlaySound(const std::string& filePath)
{
	ma_result result = ma_engine_play_sound(&engine, filePath.c_str(), NULL);
	if (result != MA_SUCCESS)
	{
		std::cerr << "Erreur : Impossible de jouer le son " << filePath << " (code " << result << ")\n";
		return false;
	}
	return true;
}

void AudioManager::SetListenerPosition(float x, float y, float z)
{
	ma_engine_listener_set_position(&engine, 0, x, y, z);
}

void AudioManager::EnableEcho()
{
	if (!echoEnabled)
	{
		ma_node_attach_output_bus(&delayNode, 0, ma_engine_get_endpoint(&engine), 0);
		echoEnabled = true;
		std::cout << "Effet d'écho activé.\n";
	}
}

void AudioManager::DisableEcho()
{
	if (echoEnabled)
	{
		ma_node_detach_output_bus(&delayNode, 0);
		echoEnabled = false;
		std::cout << "Effet d'écho désactivé.\n";
	}
}
