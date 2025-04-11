#pragma once

#include "miniaudio/miniaudio.h"
#include <string>
#include <thread>
#include <atomic>

class Sound {
public:
    Sound(ma_engine* engine, const std::string& filePath);
    ~Sound();

    static void Play();
    static void Pause();
    static void Resume();
    static void Restart();  // Red�marrer depuis le d�but

    static void SetVolume(float volume);
    static void SetLooping(bool loop);

    // Effet 3D
    static void SetPosition(float x, float y, float z);
    static void SetPitch(float pitch);
    //static void Enable3DMovement(bool enable);  // Activer/d�sactiver le mouvement 3D
    
    static ma_sound sound;


private:
    

    // Gestion du mouvement 3D
    //static void Start3DMovement();            // D�marrer le mouvement 3D
    //static void Stop3DMovement();             // Arr�ter le mouvement 3D
    //static void Update3DPosition();           // Mise � jour continue de la position

    std::atomic<bool> enable3DMovement; // Indique si le mouvement 3D est activ�
    std::atomic<bool> running3DMovement; // Indique si le thread de mouvement est en cours
    std::thread movementThread;        // Thread pour g�rer le mouvement 3D

    // Variables de position
    float soundX;
    float soundY;
    float soundZ;
};
