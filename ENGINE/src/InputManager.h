#pragma once

#include <windows.h>
#include <iostream>
#include <dinput.h>
#include <unordered_map>
#include <functional>
#include <utility>
#include <deque>
#include <vector>
#include "ConsoleManager.h"

enum class EKey
{
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z
};

struct KeyInfo
{
	int keyCode;
	char character;
};

class InputManager
{
public:
	static InputManager& GetInstance();

	bool Init();

	bool CheckKeyPressed(EKey key);

	void LogKeyInput(EKey key);
	// Binds a key to a specific action (a function) with optional arguments to be executed when the key is pressed
	template<typename Func, typename... Args>
	void BindKeyToAction(EKey key, Func&& func, Args&&... args);

	std::pair<int, int> GetMousePosition();

	int GetMouseDelta();

	std::unordered_map<EKey, KeyInfo> GetKeyMap()
	{
		return keyMap;
	}

	std::unordered_map<EKey, bool> GetKeyState() &
	{
		return keyState;
	}

	bool SetKeyState(EKey key, bool state);

	void LogInput(EKey key);

	void LogMouseInput(int button);

	const std::vector<std::string>& GetInputLog() const
	{
		return inputLog;
	}

	const std::deque<std::string>& GetLastPressedKeys() const
	{
		return lastPressedKeys;
	}

	void ShutDown();

	InputManager(const InputManager&) = delete;

	std::string KeyToString(EKey key)
	{
		switch (key)
		{
		case EKey::A: return "A";
		case EKey::B: return "B";
		case EKey::C: return "C";
		case EKey::D: return "D";
		case EKey::E: return "E";
		case EKey::F: return "F";
		case EKey::G: return "G";
		case EKey::H: return "H";
		case EKey::I: return "I";
		case EKey::J: return "J";
		case EKey::K: return "K";
		case EKey::L: return "L";
		case EKey::M: return "M";
		case EKey::N: return "N";
		case EKey::O: return "O";
		case EKey::P: return "P";
		case EKey::Q: return "Q";
		case EKey::R: return "R";
		case EKey::S: return "S";
		case EKey::T: return "T";
		case EKey::U: return "U";
		case EKey::V: return "V";
		case EKey::W: return "W";
		case EKey::X: return "X";
		case EKey::Y: return "Y";
		case EKey::Z: return "Z";
		default: return "Unknown Key";
		}
	}

	InputManager& operator=(const InputManager&) = delete;

	~InputManager() {};

private:
	//Inits the input device if an error is raised return true
	bool initInputDevice();
	void UpdateKeyState(EKey key, bool isPressed);

private:
	InputManager() {};
	std::vector<std::string> inputLog;
	std::unordered_map<EKey, bool> keyState;
	std::deque<std::string> lastPressedKeys;
	const size_t maxLogSize = 10;

	const std::unordered_map<EKey, KeyInfo> keyMap = {{EKey::A, {0x41, 'A'}}, {EKey::B, {0x42, 'B'}}, {EKey::C, {0x43, 'C'}}, {EKey::D, {0x44, 'D'}}, {EKey::E, {0x45, 'E'}}, {EKey::F, {0x46, 'F'}}, {EKey::G, {0x47, 'G'}}, {EKey::H, {0x48, 'H'}}, {EKey::I, {0x49, 'I'}}, {EKey::J, {0x4A, 'J'}}, {EKey::K, {0x4B, 'K'}}, {EKey::L, {0x4C, 'L'}}, {EKey::M, {0x4D, 'M'}}, {EKey::N, {0x4E, 'N'}}, {EKey::O, {0x4F, 'O'}}, {EKey::P, {0x50, 'P'}}, {EKey::Q, {0x51, 'Q'}}, {EKey::R, {0x52, 'R'}}, {EKey::S, {0x53, 'S'}}, {EKey::T, {0x54, 'T'}}, {EKey::U, {0x55, 'U'}}, {EKey::V, {0x56, 'V'}}, {EKey::W, {0x57, 'W'}}, {EKey::X, {0x58, 'X'}}, {EKey::Y, {0x59, 'Y'}}, {EKey::Z, {0x5A, 'Z'}}};

	POINT point;
};

template<typename Func, typename... Args>
inline void InputManager::BindKeyToAction(EKey key, Func&& func, Args&&... args)
{
	if (CheckKeyPressed(key))
	{
		std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
	}
}

static InputManager& inputManager = InputManager::GetInstance();
