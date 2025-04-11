#include "InputManager.h"

InputManager& InputManager::GetInstance()
{
	static InputManager instance;
	return instance;
	// TODO: insert return statement here
}

bool InputManager::Init()
{
	RAWINPUTDEVICE devices[2];

	// Register keyboard
	devices[0].usUsagePage = 0x01; // Generic Desktop Controls
	devices[0].usUsage = 0x06; // Keyboard
	devices[0].dwFlags = 0; // Default processing
	devices[0].hwndTarget = NULL;

	// Register mouse
	devices[1].usUsagePage = 0x01;
	devices[1].usUsage = 0x02; // Mouse
	devices[1].dwFlags = 0;
	devices[1].hwndTarget = NULL;

	if (!RegisterRawInputDevices(devices, 2, sizeof(RAWINPUTDEVICE)))
	{
		console.Log("Failed to register raw input devices!", API::INPUT, LEVEL::ERRORS);
		return true;
	}
	return false;
}

bool InputManager::CheckKeyPressed(EKey key)
{
	// Find the key in the keyMap
	std::unordered_map<EKey, KeyInfo>::const_iterator got = keyMap.find(key);

	// If the key is found in the map
	if (got != keyMap.end())
	{
		// Use GetAsyncKeyState with the key code (from KeyInfo)
		if (GetAsyncKeyState(got->second.keyCode) & 0x8000)
		{
			return true; // The key is pressed
		}
	}

	return false; // The key is not pressed
}

void InputManager::LogKeyInput(EKey key)
{
	std::string keyString = KeyToString(key); // Convert EKey to string

	// Add the key press to the log
	lastPressedKeys.push_front(keyString); // Insert at the front
	if (lastPressedKeys.size() > maxLogSize)
	{
		lastPressedKeys.pop_back(); // Keep the list within the max size
	}
}

std::pair<int, int> InputManager::GetMousePosition()
{
	if (GetCursorPos(&point))
	{
		// If successful, return the mouse position as a pair
		return std::pair<int, int>{point.x, point.y};
	}
	else
	{
		// If failed, return a default value (e.g., (0, 0))
		return std::pair<int, int>{0, 0};
	}
}

bool InputManager::SetKeyState(EKey key, bool state)
{
	auto it = keyState.find(key);

	if (it != keyState.end())
	{
		it->second = state; // Update the existing value
		return true;
	}
	return false;
}
