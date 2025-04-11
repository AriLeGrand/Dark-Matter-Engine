#include "Sound.h"

#include <atomic>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <thread>

ma_sound Sound::sound;

Sound::Sound(ma_engine* engine, const std::string& filePath) : enable3DMovement(false), running3DMovement(false), soundX(0.0f), soundY(0.0f), soundZ(0.0f)
{
	if (ma_sound_init_from_file(engine, filePath.c_str(), 0, NULL, NULL, &sound) != MA_SUCCESS)
	{
		throw std::runtime_error("Erreur : Impossible de charger le son " + filePath);
	}
}

Sound::~Sound()
{
	//Stop3DMovement();
	ma_sound_uninit(&sound);
}

void Sound::Play()
{
	ma_sound_start(&sound);
}

void Sound::Pause()
{
	ma_sound_stop(&sound);
}

void Sound::Resume()
{
	ma_sound_start(&sound);
}

void Sound::Restart()
{
	// Stop le son, le repositionne au début, puis le relance
	ma_sound_stop(&sound);
	ma_sound_seek_to_pcm_frame(&sound, 0);
	ma_sound_start(&sound);
}

void Sound::SetVolume(float volume)
{
	ma_sound_set_volume(&sound, volume);
}

void Sound::SetLooping(bool loop)
{
	ma_sound_set_looping(&sound, loop);
}

void Sound::SetPosition(float x, float y, float z)
{
	ma_sound_set_position(&sound, x, y, z);
	//soundX = x;
	//soundY = y;
	//soundZ = z;
}

void Sound::SetPitch(float pitch)
{
	ma_sound_set_pitch(&sound, pitch);
}

//void Sound::Enable3DMovement(bool enable) {
//    if (enable && !running3DMovement) { // Démarrer le mouvement si ce n'est pas déjà en cours
//        Start3DMovement();
//    }
//    else if (!enable && running3DMovement) { // Arrêter le mouvement si activé
//        Stop3DMovement();
//    }
//    enable3DMovement = enable;
//}

//void Sound::Start3DMovement() {
//    running3DMovement = true;
//    movementThread = std::thread([this]() {
//        float angle = 0.0f;
//        while (running3DMovement) {
//            if (enable3DMovement) {  // Ne bouge que si l'effet 3D est activé
//                soundX = std::sin(angle) * 5.0f; // Déplacement sur un rayon de 5 unités
//                ma_sound_set_position(&sound, soundX, soundY, soundZ);
//                angle += 0.1f;
//            }
//
//            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Pause entre chaque mise à jour
//        }
//        });
//}

//void Sound::Stop3DMovement() {
//    if (running3DMovement) {
//        running3DMovement = false;
//        if (movementThread.joinable()) {
//            movementThread.join();
//        }
//    }
//}

//void Sound::Update3DPosition() {
//    float minX = -10.0f;
//    float maxX = 10.0f;
//    float speed = 0.5f;
//    int direction = 1; // 1 = droite, -1 = gauche
//
//    while (running3DMovement) {
//        if (enable3DMovement) {
//            soundX += speed * direction;
//
//            // Inversion de direction si on atteint une limite
//            if (soundX >= maxX || soundX <= minX) {
//                direction *= -1;
//            }
//
//            SetPosition(soundX, soundY, soundZ);
//            std::this_thread::sleep_for(std::chrono::milliseconds(100));
//        }
//    }
//}
