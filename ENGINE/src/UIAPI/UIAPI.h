#pragma once

#include <commdlg.h>
#include <deque>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <windows.h>

#include "ConsoleManager.h"
#include "InputManager.h"
#include "Theme.h"

#include "imgui/imfilebrowser.h"
#include "imgui/backends/imgui_impl_dx11.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_win32.h"

#include "Sound.h"

/**
 * @brief Frame-per-second (FPS) value.
 */
static float fps = 0.0f;

/**
 * @brief Frame counter.
 */
static int frameCount = 0;

/**
 * @brief Time difference between frames.
 */
static float deltaTime = 0.0f;

/**
 * @brief Number of models.
 */
static int modelCount = 0;

/**
 * @brief String representing model details.
 */
static std::string modelString = "";

/**
 * @brief Number of triangles in the scene.
 */
static int triangleCount = 0;

/**
 * @brief Number of vertices in the scene.
 */
static int verticesCount = 0;

/**
 * @brief Mouse X position.
 */
static int mouseX, mouseY = 0;

/**
 * @brief Map storing key states for input management.
 */
static std::unordered_map<EKey, bool> keyState = {};

/**
 * @brief GPU usage percentage.
 */
static int GPUusage = 0;

/**
 * @brief Flag for performance stats display.
 */
static bool Performance_Stats = false;

/**
 * @brief Flag for FPS stats display.
 */
static bool Fps_Stats = false;

/**
 * @brief Flag for renderer stats display.
 */
static bool Renderer_Stats = true;

/**
 * @brief Flag for console logger display.
 */
static bool Console_Logger = true;

/**
 * @brief Flag for scene hierarchy display.
 */
static bool Scene_Hierarchy = true;

/**
 * @brief Flag for input debugger display.
 */
static bool Input_Debugger = true;

/**
 * @brief Flag for resource manager display.
 */
static bool Resource_Manager = false;

/**
 * @brief Flag for audio manager display.
 */
static bool Audio_Manager = true;

/**
 * @brief Opens a file explorer dialog for file selection using ImGui.
 *
 * Displays a button to open the file explorer, allowing the user to
 * select a file and prints the selected file path to the console.
 */
static void FileExplorer()
{
	// Création d'un objet FileBrowser
	static ImGui::FileBrowser fileDialog;

	// Configuration des propriétés de l'explorateur
	fileDialog.SetTitle("Select a file");

	// Affichage du bouton pour ouvrir l'explorateur de fichiers
	if (ImGui::Button("Open File Explorer"))
	{
		fileDialog.Open(); // Ouvrir la fenêtre de sélection de fichiers
	}

	// Affichage de l'explorateur de fichiers
	fileDialog.Display();

	// Si un fichier est sélectionné, l'afficher
	if (fileDialog.HasSelected())
	{
		std::cout << "Selected file : " << fileDialog.GetSelected().string() << std::endl;
		fileDialog.ClearSelected(); // Réinitialiser la sélection après l'utilisation
	}
}

/**
 * @brief Displays the input manager UI.
 *
 * This function creates an ImGui window that shows the current mouse
 * position and the state of all keys.
 */
static void InputManagerUI()
{
	ImGui::Begin("Input Manager");

	ImGui::Text("Mouse Position: (%d, %d)", mouseX, mouseY);

	// Display key states
	ImGui::Separator();
	ImGui::Text("Key States:");
	// Loop over all keys in the keyState map and display their status

	for (const auto& pair : keyState)
	{
		std::string keyName = inputManager.KeyToString(pair.first); // Get the key name
		ImGui::Text("Key: %s", keyName.c_str()); // Display the key name
		ImGui::SameLine();
		if (pair.second)
		{
			ImGui::Text("Pressed");
		}
		else
		{
			ImGui::Text("Not Pressed");
		}
	}

	ImGui::End();
}

/**
 * @brief Main UI function that handles the layout of the application's UI.
 *
 * This function contains various UI elements like performance stats, FPS
 * counter, renderer stats, and more. It also includes menu options for
 * setting configurations, displaying scenes, managing assets, and modifying
 * themes.
 *
 * @param p_open A pointer to a boolean value that indicates whether the
 *               window is open or not.
 * @param ScreenWidth The current width of the screen.
 * @param ScreenHeight The current height of the screen.
 */
static void MainUI(bool* p_open, int ScreenWidth, int ScreenHeight)
{
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	// Get CPU Usage
	FILETIME idleTime, kernelTime, userTime;
	GetSystemTimes(&idleTime, &kernelTime, &userTime);

	ULARGE_INTEGER idle, kernel, user;
	idle.LowPart = idleTime.dwLowDateTime;
	idle.HighPart = idleTime.dwHighDateTime;
	kernel.LowPart = kernelTime.dwLowDateTime;
	kernel.HighPart = kernelTime.dwHighDateTime;
	user.LowPart = userTime.dwLowDateTime;
	user.HighPart = userTime.dwHighDateTime;

	// Calculate CPU usage (a simple example, can be more accurate)
	ULONGLONG sysTotal = (kernel.QuadPart + user.QuadPart);
	ULONGLONG sysIdle = idle.QuadPart;
	float cpuUsage = (sysTotal - sysIdle) * 100.f / sysTotal;

	// Get Memory Usage
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	DWORDLONG totalPhysMem = statex.ullTotalPhys;
	DWORDLONG physMemUsed = statex.ullTotalPhys - statex.ullAvailPhys;
	float memUsage = (float)physMemUsed / (float)totalPhysMem * 100.0f;

	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", p_open, window_flags);

	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Scene"))
		{
			if (ImGui::MenuItem("New Scene"))
			{
				// Handle New Scene
			}
			if (ImGui::MenuItem("Load Scene"))
			{
				// Handle Load Scene
			}
			if (ImGui::MenuItem("Save Scene"))
			{
				// Handle Save Scene
			}
			ImGui::EndMenu();
		}

		// Asset Menu
		if (ImGui::BeginMenu("Assets"))
		{
			if (ImGui::MenuItem("Import"))
			{
				// Handle Asset Import
			}
			if (ImGui::MenuItem("Export"))
			{
				// Handle Asset Export
			}
			if (ImGui::MenuItem("Asset Browser"))
			{
				// Open Asset Browser
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools"))
		{
			if (ImGui::MenuItem("PC Performance Stats", NULL, Performance_Stats))
			{
				Performance_Stats = !Performance_Stats;
			}
			if (ImGui::MenuItem("FPS Counter", NULL, Fps_Stats))
			{
				Fps_Stats = !Fps_Stats;
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Renderer Stats", NULL, Renderer_Stats))
			{
				Renderer_Stats = !Renderer_Stats;
			}
			if (ImGui::MenuItem("Console / Logger", NULL, Console_Logger))
			{
				Console_Logger = !Console_Logger;
			}
			if (ImGui::MenuItem("Scene Hierarchy", NULL, Scene_Hierarchy))
			{
				Scene_Hierarchy = !Scene_Hierarchy;
			}
			if (ImGui::MenuItem("Input Debugger", NULL, Input_Debugger))
			{
				Input_Debugger = !Input_Debugger;
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Resource Manager", NULL, Resource_Manager))
			{
				Resource_Manager = !Resource_Manager;
			}

			if (ImGui::MenuItem("Audio Manager", NULL, Audio_Manager))
			{
				Audio_Manager = !Audio_Manager;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Settings"))
		{
			// Graphics Settings
			if (ImGui::BeginMenu("Graphics"))
			{
				if (ImGui::BeginMenu("Resolution"))
				{
					ImGui::Text("Resolution Width : %d", ScreenWidth);
					ImGui::Text("Resolution Height : %d", ScreenHeight);
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Themes"))
			{
				if (ImGui::MenuItem("Moonlight Theme"))
				{
					SetMoonlightTheme();
				}
				if (ImGui::MenuItem("Moon Theme"))
				{
					SetMoonTheme();
				}
				if (ImGui::MenuItem("Default dark Theme"))
				{
					ImGui::StyleColorsDark();
				}
				if (ImGui::MenuItem("Default light Theme"))
				{
					ImGui::StyleColorsLight();
				}
				if (ImGui::MenuItem("Quentin Theme"))
				{
					SetQuentinTheme();
				}
				if (ImGui::MenuItem("Pinky Theme"))
				{
					SetPinkyTheme();
				}
				if (ImGui::MenuItem("Potato Theme"))
				{
					SetPotatoTheme();
				}
				if (ImGui::MenuItem("Brioche Theme"))
				{
					SetBriocheTheme();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Documentation"))
			{
				ImGui::Text("About: ");
				ImGui::TextLinkOpenURL("https://github.com/GamingCampus-MillieBourgois-24-25/Dark-Matter-Engine/pull/21/");
				// Open Documentation
			}
			if (ImGui::MenuItem("About"))
			{
				ImGui::OpenPopup("Hehe");
				ImGui::Text("About: ");
				ImGui::SetTooltip("Hello");
				ImGui::TextLinkOpenURL("https://github.com/GamingCampus-MillieBourgois-24-25/Dark-Matter-Engine/pull/21/");
				// Show About Dialog
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Options"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows,
			// which we can't undo at the moment without finer window depth/z control.
			ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
			ImGui::MenuItem("Padding", NULL, &opt_padding);
			ImGui::Separator();

			// Display a menu item for each Dockspace flag, clicking on one will toggle its assigned flag.
			if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))
			{
				dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
			}
			if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))
			{
				dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
			}
			if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))
			{
				dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
			}
			if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))
			{
				dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
			}
			if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen))
			{
				dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
			}
			ImGui::Separator();

			// Display a menu item to close this example.
			if (ImGui::MenuItem("Close", NULL, false, p_open != NULL))
				if (p_open != NULL)
					*p_open = false;
			ImGui::EndMenu();
		}

		// Show a help marker that gives an overview of what this example is and does.
		ImGui::EndMenuBar();
	}

	if (Performance_Stats)
	{
		ImGui::Begin("PC Performance Stats", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
		ImGui::Text("CPU Usage: %.2f%%", cpuUsage);
		ImGui::Text("Memory Usage: %.2f%%", memUsage);
		ImGui::End();
	}

	if (Fps_Stats)
	{
		// Show the FPS counter
		ImGui::Begin("FPS Counter", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
		ImGui::Text("FPS: %.2f", fps); // Show the FPS value
		ImGui::End();
	}

	if (Renderer_Stats)
	{
		ImGui::Begin("Renderer Stats", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImGui::Text("Draw Calls: %d", modelCount);
		ImGui::Text("Vertices Count: %d", verticesCount);
		ImGui::Text("Triangles Count: %d", triangleCount);
		/*ImGui::Text("GPU Memory Usage: %.2f MB",0);*/ //I (Ivania) decided it's optional
		ImGui::End();
	}

	if (Console_Logger)
	{
		ImGuiConsoleManager::GetInstance().Draw("Console");
	}

	if (Scene_Hierarchy)
	{
		ImGui::Begin("Scene Hierarchy", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImGui::Text("ModelCount: %d", modelCount);
		ImGui::Text(("Scene Hierachy: \n" + modelString + "\n").c_str());

		FileExplorer();
		ImGui::End();
	}

	if (Input_Debugger)
	{
		ImGui::Begin("Input Debugger", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

		auto& inputLog = inputManager.GetInputLog();
		for (const auto& entry : inputLog)
		{
			ImGui::Text("%s", entry.c_str());
		}

		auto [x, y] = inputManager.GetMousePosition();
		ImGui::Text("Mouse Position: (%d, %d)", mouseX, mouseY);

		static bool mouseButtonsPressed[3] = {false, false, false};
		static std::deque<std::string> mouseClickMessages; // Stores clicked buttons

		ImGui::Text("Mouse Input: ");

		for (int button = 0; button < 3; ++button)
		{
			if (ImGui::IsMouseClicked(button))
			{
				if (!mouseButtonsPressed[button])
				{
					// Store the button click message
					mouseClickMessages.push_back("Button " + std::to_string(button) + " clicked");
					mouseButtonsPressed[button] = true;
				}
			}
			else if (ImGui::IsMouseReleased(button))
			{
				mouseButtonsPressed[button] = false; // Reset on release
			}
		}

		// Display all stored messages
		for (const auto& msg : mouseClickMessages)
		{
			ImGui::Text("%s", msg.c_str());
		}

		// Optional: Clear messages when exceeding a limit (to prevent infinite growth)
		if (mouseClickMessages.size() > 1)
		{
			mouseClickMessages.pop_front();
		}

		static bool keyButtonsPressed[26] = {false};

		for (int key = 0; key < 26; ++key)
		{
			if (inputManager.CheckKeyPressed(static_cast<EKey>(key)))
			{
				if (!keyButtonsPressed[key])
				{
					std::string keyName = inputManager.KeyToString(static_cast<EKey>(key));
					keyButtonsPressed[key] = true;
				}
			}
			else
			{
				keyButtonsPressed[key] = false;
			}
		}

		ImGui::Separator();
		ImGui::Text("Keyboard Input");

		ImGui::Text("Key States:");

		for (const auto& key : {EKey::A, EKey::B, EKey::C, EKey::D, EKey::E, EKey::F, EKey::G, EKey::H, EKey::I, EKey::J, EKey::K, EKey::L, EKey::M, EKey::N, EKey::O, EKey::P, EKey::Q, EKey::R, EKey::S, EKey::T, EKey::U, EKey::V, EKey::W, EKey::X, EKey::Y, EKey::Z})
		{
			std::string keyName = inputManager.KeyToString(key);
			bool isPressed = inputManager.CheckKeyPressed(key);

			ImGui::Text("Key: %s", keyName.c_str());
			ImGui::SameLine();
			if (isPressed)
			{
				ImGui::Text("Pressed");
			}
			else
			{
				ImGui::Text("Not Pressed");
			}
		}

		ImGui::End();
	}

	if (Audio_Manager)
	{
		ImGui::Begin("Audio Manager");

		if (ImGui::Button("Play"))
		{
			Sound::Play();
		}
		ImGui::SameLine();
		if (ImGui::Button("Pause"))
		{
			Sound::Pause();
		}
		if (ImGui::Button("Restart"))
		{
			Sound::Restart();
		}

		static float volume = 1.0f;
		if (ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f))
		{
			Sound::SetVolume(volume);
		}

		static bool loop = false;
		if (ImGui::Checkbox("Looping", &loop))
		{
			Sound::SetLooping(loop);
		}

		static float posX = 0.0f, posY = 0.0f, posZ = 0.0f;
		if (ImGui::SliderFloat3("Position", &posX, -10.0f, 10.0f))
		{
			Sound::SetPosition(posX, posY, posZ);
		}

		static float pitch = 1.0f;
		if (ImGui::SliderFloat("Pitch", &pitch, 0.5f, 2.0f))
		{
			Sound::SetPitch(pitch);
		}

		//// Drag and Drop MP3 file
		//ImGui::Text("Drag and drop an MP3 file:");
		//if (ImGui::BeginDragDropTarget()) {
		//    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_DND")) {
		//        const char* filePath = (const char*)payload->Data;
		//        std::string soundFile = filePath;
		//    }
		//    ImGui::EndDragDropTarget();
		//}

		//if (ImGui::Button("Open MP3 File")) {
		//    wchar_t filePath[MAX_PATH] = { 0 };  // Utilisation de wchar_t pour Windows
		//    OPENFILENAMEW ofn = {};
		//    ofn.lStructSize = sizeof(ofn);
		//    ofn.hwndOwner = NULL;
		//    ofn.lpstrFilter = L"MP3 Files\0*.mp3\0All Files\0*.*\0";
		//    ofn.lpstrFile = filePath;
		//    ofn.nMaxFile = MAX_PATH;
		//    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
		//    ofn.lpstrDefExt = L"mp3";

		//    if (GetOpenFileNameW(&ofn)) {
		//        // Conversion de wchar_t* vers std::string
		//        std::wstring ws(filePath);
		//        std::string path(ws.begin(), ws.end());
		//        std::string soundFile = path.c_str();  // Charge le fichier
		//    }
		//}
		ImGui::End();
	}

	ImGui::End();
}
