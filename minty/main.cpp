#include "includes.h"
//#include "lua/funcs.h"
#include "themes.h"
#include "res/fonts/font.h"
#include "gilua/luaHook.h"
//#include "lua/luaHook.cpp"
#include "lua/funcs.hpp"
#include "imgui/TextEditor.h"
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include "imgui/L2DFileDialog.h"
#define _CRT_SECURE_NO_WARNINGS
#include <chrono>
#include <thread>
#include "gilua/logtextbuf.h"
#include "json/json.hpp"

using namespace std;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);

}

bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Use the UNORM format to specify RGB88 color space
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;
			pDevice->CreateRenderTargetView(pBackBuffer, &rtvDesc, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			init = true;
			ImGuiIO& io = ImGui::GetIO();
			ImFontConfig fontmenu;
			fontmenu.FontDataOwnedByAtlas = false;
			/*ImFontConfig fontcoding;
			fontcoding.FontDataOwnedByAtlas = false;*/

			ImWchar ranges[] = { 0x0020, 0x00FF, 0x0100, 0x024F, 0x0370, 0x03FF, 0x0400, 0x04FF, 0x3040, 0x309F, 0x30A0, 0x30FF, 0x4E00, 0x9FBF, 0xAC00, 0xD7AF, 0xFF00, 0xFFEF, 0 };

			io.Fonts->AddFontFromMemoryTTF((void*)rawData, sizeof(rawData), 18.f, &fontmenu, ranges);
			io.Fonts->Build();

			//ImFont* jetbr = io.Fonts->AddFontFromMemoryTTF((void*)jetbrains, sizeof(jetbrains), 18.f, &fontcoding);
			//io.Fonts->Build();

			ImGui_ImplDX11_InvalidateDeviceObjects();

			ImFontConfig fontcoding;
			fontcoding.FontDataOwnedByAtlas = false;
			ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)jetbrains, sizeof(jetbrains), 18.f, &fontcoding, ranges);
			io.Fonts->Build();

			//ImGui::GetIO().Fonts->Build();

			ImGui_ImplDX11_InvalidateDeviceObjects();
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}
	
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		// imgui code between newframe and render

		ImGui::NewFrame();
		ImGui::GetStyle().IndentSpacing = 16.0f;
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[fontindex_menu]);
		setlocale(LC_ALL, "C");

		static bool showEditor = false;
		static bool isopened = true;
		static bool show_compile_log = false;

		static char* file_dialog_buffer = nullptr;
		static char path3[500] = "";

		static float TimeScale = 1.0f;


		if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F11)))
		{
			TimeScale = 1.0f;
			string result = "CS.UnityEngine.Time.timeScale = 1.0";
			luahookfunc(result.c_str());
		}

		if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F12)))
		{
			isopened = !isopened;
		}

		if (isopened) {
			ImGui::Begin("Minty");
			ImGui::BeginTabBar("Minty");

			if (ImGui::BeginTabItem("Main"))
			{
				if (!is_hook_success)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "WARNING! LUA NOT HOOKED");
					ImGui::Separator();
				}
				ImGui::Text("Player");

				static char inputTextBuffer[512] = "";
				ImGui::InputTextWithHint("##input", "Enter custom UID text here...", inputTextBuffer, sizeof(inputTextBuffer));
				ImGui::SameLine();
				if (ImGui::Button("Update custom UID")) {
					string result = R"MY_DELIMITER(CS.UnityEngine.GameObject.Find("/BetaWatermarkCanvas(Clone)/Panel/TxtUID"):GetComponent("Text").text = ")MY_DELIMITER" + string(inputTextBuffer) + "\"";
					luahookfunc(result.c_str());
				}

				static bool show_modelswap = false;
				if (ImGui::Checkbox("Model swapper", &show_modelswap)) {
					luahookfunc("CS.LAMLMFNDPHJ.HAFGEFPIKFK(\"snoo.\",\"snoo.\")");
				}
				if (show_modelswap)
				{
					ImGui::Indent();

					if (ImGui::Button("Clone model")) {
						luahookfunc(char_modelswap_clone);
					}
					if (ImGui::Button("Paste model")) {
						luahookfunc(char_modelswap_paste);
					}
					ImGui::Unindent();
				}

				static bool show_resizer = false;
				static float boob_size = 1.0f;
				ImGui::Checkbox("Booba resizer", &show_resizer);
				if (show_resizer)
				{
					ImGui::Indent();
					if (ImGui::Button("Initiate resize")) {
						boob_size = 1.0f;
						luahookfunc(char_bub_initiate);
					}

					ImGui::SameLine();

					if (ImGui::SliderFloat("Booba scale", &boob_size, 0.0f, 4.0f, "%.3f"))
					{
						string result = char_bub_resize + to_string(boob_size) + "," + to_string(boob_size) + "," + to_string(boob_size) + ")";
						luahookfunc(result.c_str());
					}
					ImGui::Unindent();
				}

				static bool show_avatarresizer = false;
				ImGui::Checkbox("Avatar resizer", &show_avatarresizer);

				if (show_avatarresizer) {
					static float avatarsize = 1.0f;
					ImGui::Indent();
					string result = char_avatarresize + to_string(avatarsize) + "," + to_string(avatarsize) + "," + to_string(avatarsize) + ")";

					if (ImGui::SliderFloat("Avatar scale", &avatarsize, 0.0f, 25.0f, "%.3f"))
					{
						luahookfunc(result.c_str());
					}

					ImGui::SameLine();

					if (ImGui::Button("reset")) 
					{
						string result = string(char_avatarresize) + "1 , 1, 1)";
						avatarsize = 1.0f;
						luahookfunc(result.c_str());
					}

					ImGui::Unindent();
				}

				static bool show_colorator3000 = false;
				static float cc_r = 1.0f;
				static float cc_g = 1.0f;
				static float cc_b = 1.0f;
				static float cc_a = 1.0f;

				ImGui::Checkbox("Infusion changer", &show_colorator3000);
				if (show_colorator3000)
				{
					ImGui::Indent();

					if (ImGui::Button("Change")) {
						string result = char_eleminf + to_string(cc_r) + "," + to_string(cc_g) + "," + to_string(cc_b) + "," + to_string(cc_a) + char_eleminf_end;
						luahookfunc(result.c_str());
					}
					ImGui::SameLine();
					ImGui::SliderFloat("Red color", &cc_r, 0.0f, 1.0f, "%.3f");
					ImGui::SliderFloat("Green color", &cc_g, 0.0f, 1.0f, "%.3f");
					ImGui::SliderFloat("Blue color", &cc_b, 0.0f, 1.0f, "%.3f");
					ImGui::SliderFloat("Alpha", &cc_a, 0.0f, 1.0f, "%.3f");

					ImGui::Unindent();
				}

				ImGui::Separator();
				ImGui::Text("World");
				static bool browser_is_enabled = false;
				if (ImGui::Checkbox("Browser", &browser_is_enabled)) {
					if (browser_is_enabled) {
						luahookfunc(char_browser_on);
					}
					else {
						luahookfunc(char_browser_off);
					}
				}

				static bool no_fog = false;
				if(ImGui::Checkbox("Disable fog", &no_fog))
				{
					if (no_fog) {
						luahookfunc(R"MY_DELIMITER(CS.UnityEngine.RenderSettings.fog = false)MY_DELIMITER");
					}
					else {
						luahookfunc(R"MY_DELIMITER(CS.UnityEngine.RenderSettings.fog = true)MY_DELIMITER");
					}
				}

				if (ImGui::SliderFloat("Timescale", &TimeScale, 0.0f, 2.0f, "%.3f"))
				{
					string result = "CS.UnityEngine.Time.timeScale = " + to_string(TimeScale);
					luahookfunc(result.c_str());
				}
				ImGui::SameLine();

				if (ImGui::Button("Reset (F11)")) {
					TimeScale = 1.0f;
					string result = "CS.UnityEngine.Time.timeScale = 1.0";
					luahookfunc(result.c_str());
				}

				ImGui::Separator();
				ImGui::Text("Misc");
				// static bool is_fps_shown = false;
				// ImGui::Checkbox("Show FPS", &is_fps_shown);
				// if (is_fps_shown) 
				// {
				// 	luahookfunc(char_showfps);
				// }

				static bool unlockfps = false;
				static float targetfps = 60;
				ImGui::Checkbox("Unlock FPS", &unlockfps);
					if (unlockfps) {
						ImGui::Indent();
						ImGui::SliderFloat("Target FPS", &targetfps, 10.0f, 360.0f, "%.3f");
						string result = "CS.UnityEngine.Application.targetFrameRate = " + to_string(targetfps);
						luahookfunc(result.c_str());
						luahookfunc("CS.UnityEngine.QualitySettings.vSyncCount = 0");
						ImGui::Unindent();
					}

				ImGui::Separator();
				ImGui::Text("Lua");
				ImGui::Checkbox("Lua editor", &showEditor);
				
				ImGui::EndTabItem();
			}

			if (showEditor)
			{
				if (FileDialog::file_dialog_open) {
					FileDialog::ShowFileDialog(&FileDialog::file_dialog_open, file_dialog_buffer, sizeof(file_dialog_buffer), FileDialog::file_dialog_open_type);
				}

				static TextEditor editor;
				static bool initialized = false;

				if (!initialized)
				{
					editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
					editor.SetPalette(TextEditor::GetDarkPalette());

					editor.SetTabSize(4);
					editor.SetShowWhitespaces(false);
					editor.SetColorizerEnable(true);

					initialized = true;
				}
				ImGui::Begin("Lua editor", &showEditor, ImGuiWindowFlags_MenuBar);

				if (ImGui::Button("Run"))
				{
					string code = editor.GetText();
					if (!code.empty() && code.find_first_not_of(" \t\n\v\f\r") != string::npos)
					{
						luahookfunc(code.c_str());
					}
				}

				ImGui::SameLine();
				//saver to button below.

				static vector<pair<string, function<void()>>> buttonFuncs;
				static char buttonLabel[256] = "";

				if (ImGui::Button("Create new button")) {
					ImGui::OpenPopup("New button");
				}
				ImGui::SameLine();
				ImGui::Checkbox("Show log", &show_compile_log);

				if (ImGui::BeginPopup("New button")) {
					ImGui::InputText("Label", buttonLabel, 256);
					if (ImGui::Button("Create")) {
						string functionText = editor.GetText();
						function<void()> buttonFunc = [functionText]() {
							luahookfunc(functionText.c_str());
						};
						buttonFuncs.emplace_back(string(buttonLabel), buttonFunc);
						memset(buttonLabel, 0, sizeof(buttonLabel));
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}

				ImGui::Begin("Minty");
				for (const auto& button : buttonFuncs) {
					if (ImGui::Button(button.first.c_str())) {
						button.second();
					}
				}
				ImGui::End();

				if (ImGui::BeginMenuBar())
				{
					//ImGui::InputText("##path3", path3, sizeof(path3));
					if (ImGui::BeginMenu("File"))
					{
						if (ImGui::MenuItem("Load .lua file"))
						{
							file_dialog_buffer = path3;
							FileDialog::file_dialog_open = true;
							FileDialog::file_dialog_open_type = FileDialog::FileDialogType::OpenFile;
							FileDialog::ShowFileDialog(&FileDialog::file_dialog_open, file_dialog_buffer, sizeof(file_dialog_buffer), FileDialog::file_dialog_open_type);
						}
						ImGui::EndMenu();
					}
					/*if (FileDialog::file_dialog_open) {
						FileDialog::ShowFileDialog(&FileDialog::file_dialog_open, file_dialog_buffer, sizeof(file_dialog_buffer), FileDialog::file_dialog_open_type);
					}*/
					if (ImGui::BeginMenu("Edit"))
					{
						bool ro = editor.IsReadOnly();
						if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
							editor.SetReadOnly(ro);
						ImGui::Separator();

						if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
							editor.Undo();
						if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
							editor.Redo();

						ImGui::Separator();

						if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
							editor.Copy();
						if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
							editor.Cut();
						if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
							editor.Delete();
						if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
							editor.Paste();

						ImGui::Separator();

						if (ImGui::MenuItem("Select all", nullptr, nullptr))
							editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("View"))
					{
						if (ImGui::MenuItem("Dark palette"))
							editor.SetPalette(TextEditor::GetDarkPalette());
						if (ImGui::MenuItem("Light palette"))
							editor.SetPalette(TextEditor::GetLightPalette());
						if (ImGui::MenuItem("Retro blue palette"))
							editor.SetPalette(TextEditor::GetRetroBluePalette());
						ImGui::EndMenu();
					}
					ImGui::EndMenuBar();
				}

				editor.Render("TextEditor");

				ImGui::End();
			}
			
			ImGuiTextFilter Filter;
			if (show_compile_log)
			{
				ImGui::Begin("Log");
				if (ImGui::SmallButton("[Debug] Add 5 entries"))
				{
					static int counter = 0;
					const char* categories[3] = { "info", "warn", "error" };
					const char* words[] = { "Bumfuzzled", "Cattywampus", "Snickersnee", "Abibliophobia", "Absquatulate", "Nincompoop", "Pauciloquent" };
					for (int n = 0; n < 5; n++)
					{
						const char* category = categories[counter % IM_ARRAYSIZE(categories)];
						const char* word = words[counter % IM_ARRAYSIZE(words)];
						log_textbuf.appendf("[%05d] [%s] Hello, current time is %.1f, here's a word: '%s'\n",
									ImGui::GetFrameCount(), category, ImGui::GetTime(), word);
						counter++;
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Clear")) {
					log_textbuf.clear();
				}
				Filter.Draw("Filter");
				ImGui::Separator();
				if (Filter.IsActive())
				{
					const char* buf_begin = log_textbuf.begin();
					const char* buf_end = log_textbuf.end();
					ImGuiTextBuffer log_filtered;
					while (buf_begin < buf_end)
					{
						const char* line_end = strchr(buf_begin, '\n');
						if (Filter.PassFilter(buf_begin, line_end))
						{
							log_filtered.append(buf_begin, line_end);
							log_filtered.append("\n");
						}
						buf_begin = line_end + 1;
					}
					ImGui::TextUnformatted(log_filtered.begin(), log_filtered.end());
				}
				else {
					ImGui::TextUnformatted(log_textbuf.begin(), log_textbuf.end());
				}
				
				ImGui::End();
			}

			if (ImGui::BeginTabItem("Themes"))
			{
				// Content for themes
				ImGui::Text("Theme colors");

				static int themeIndex = 0;

				if (ImGui::RadioButton("Default dark", &themeIndex, 0))
				{
					settheme(1);
				}

				if (ImGui::RadioButton("Default light", &themeIndex, 1))
				{
					settheme(2);
				}

				if (ImGui::RadioButton("Dark theme", &themeIndex, 2))
				{
					settheme(3);
				}

				if (ImGui::RadioButton("Minty Red", &themeIndex, 3))
				{
					settheme(4);
				}
				
				if (ImGui::RadioButton("Minty Mint", &themeIndex, 4))
				{
					settheme(5);
				}

				ImGui::Separator();
				ImGui::Text("Theme style");

				static int themestyleindex = 0;

				if (ImGui::RadioButton("Rounded compact style", &themestyleindex, 0))
				{
					setstyle(1);
				}
				
				if (ImGui::RadioButton("Big", &themestyleindex, 1))
				{
					setstyle(2);
				}

				ImGui::Separator();

				ImGui::Text("Menu font");

				static int fontSelectionIndex = 0;
				if (ImGui::RadioButton("Myriad pro", &fontSelectionIndex, 0))
				{
					setfont(1);
				}

				if (ImGui::RadioButton("Jetbrains Mono", &fontSelectionIndex, 1))
				{
					setfont(2);
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("About"))
			{
				// Content for About
				ImGui::Text("Minty BETA v0.7.1 WIP");
				ImGui::Text("ImGui version: %s", ImGui::GetVersion());

				ImVec4 linkColor = ImVec4(34.0f/255.0f, 132.0f/255.0f, 230.0f/255.0f, 1.0f);

				ImGui::Text("Contributors: мятный пряник#0086, Moistcrafter#9172, yarik#4571, azzu#2731");
				ImGui::Separator();

				ImGui::TextColored(linkColor, "KWT Team Discord (click)");
				if (ImGui::IsItemClicked()) {
					system("start https://discord.gg/kj7PQrr6CV");
				}
				ImGui::TextColored(linkColor, "KWT Team GitHub (click)");
				if (ImGui::IsItemClicked()) {
					system("start https://github.com/kindawindytoday");
				}
				ImGui::Separator();

				if (ImGui::CollapsingHeader("License"))
				{
					ImGui::Indent();

					ImGui::Text("ImGui\n");
					string licenseimgui = string(license_ImGui) + string(license_Generic);
					ImGui::Text(licenseimgui.c_str());
					ImGui::Separator();

					ImGui::Text("ImGuiColorTextEdit\n");
					string licensetextedit = string(license_ColorTextEdit) + string(license_Generic);
					ImGui::Text(licensetextedit.c_str());
					ImGui::Separator();
					
					ImGui::Text("ImGuiFileDialog\n");
					string licensefiledialog = string(license_FileDialog) + string(license_Generic);
					ImGui::Text(licensefiledialog.c_str());
					ImGui::Separator();

					ImGui::Text("Json\n");
					string licensejson = string(license_json) + string(license_Generic);
					ImGui::Text(licensejson.c_str());

					ImGui::Unindent();
				}
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Performance"))
			{

				//static float fps_buffer[1200] = {}; 
				//static int buffer_index = 0;
				//static float fps_max = 0;
				ImGuiIO& io = ImGui::GetIO();
				float frametime = io.DeltaTime;
				float fps = 1.0f / frametime;
				static float timer = 0.0f;
				timer += frametime;
				static float fps_slow = 0.0f;
				// fps_buffer[buffer_index] = fps;
				// buffer_index = (buffer_index + 1) % 1200;
				// if (fps > fps_max) {
				// 	fps_max = fps;
				// }
				// else if (buffer_index == 0) {
				// 	fps_max = 0;
				// 	for (int i = 0; i < 1200; i++) {
				// 		if (fps_buffer[i] > fps_max) {
				// 			fps_max = fps_buffer[i];
				// 		}
				// 	}
				// }
				//ImGui::PlotLines("FPS", fps_buffer, 1200, buffer_index, NULL, 0.0f, fps_max + 10.0f, ImVec2(0, 80));
				if (timer > 1) {
				 	fps_slow = 1.0f / frametime;
					timer = 0.0f;
				}
				ImGui::Text("Current FPS: %.2f", fps_slow);
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
			ImGui::End();
		}
		ImGui::PopFont();
		ImGui::EndFrame();
		ImGui::Render();

	//end of imgui code
	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
	}

//DWORD WINAPI MainThread(LPVOID lpReserved)
//{
//	bool init_hook = false;
//	do
//	{
//		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
//		{
//			kiero::bind(8, (void**)& oPresent, hkPresent);
//			init_hook = true;
//			//initLua();
//		}
//	} while (!init_hook);
//	return TRUE;
//}

//BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
//{
//	/*switch (dwReason)
//	{
//	case DLL_PROCESS_ATTACH:
//		DisableThreadLibraryCalls(hMod);
//		CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
//		initLua();
//		break;
//	case DLL_PROCESS_DETACH:
//		kiero::shutdown();
//		break;
//	}
//	return TRUE;*/
//	initLua();
//	return TRUE;
//}
//HMODULE hMod = nullptr;
//DWORD start(LPVOID)
//{
//	//create thread initLua
//	//CreateThread(nullptr, 0, initLua, hMod, 0, nullptr);
//	CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
//	return 0;
//}
//BOOL WINAPI DllMain(HMODULE hMod, DWORD fdwReason, LPVOID lpvReserved)
//{
//	hMod = hMod;
//	if (fdwReason == DLL_PROCESS_ATTACH)
//		CloseHandle(CreateThread(NULL, 0, &start, NULL, NULL, NULL));
//	return TRUE;
//}
DWORD WINAPI MainThread(LPVOID lpReserved)
{
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)&oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);
	return TRUE;
}
//BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
//{
//	//initLua();
//	//switch (dwReason)
//	//{
//	//case DLL_PROCESS_ATTACH:
//	//	//DisableThreadLibraryCalls(hMod);
//	//	CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
//	//	break;
//	//case DLL_PROCESS_DETACH:
//	//	kiero::shutdown();
//	//	break;
//	//}
//	//return TRUE;
//}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH) {
		CloseHandle(CreateThread(NULL, 0, &initLua, NULL, NULL, NULL));
		CreateThread(NULL, 0, &MainThread, NULL, NULL, NULL);
	}
	return TRUE;
}