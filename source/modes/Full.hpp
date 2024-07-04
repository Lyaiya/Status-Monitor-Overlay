class FullOverlay : public tsl::Gui {
private:
	uint64_t mappedButtons = MapButtons(keyCombo); // map buttons
	char RealCPU_Hz_c[32] = "";
	char DeltaCPU_c[12] = "";
	char DeltaGPU_c[12] = "";
	char DeltaRAM_c[12] = "";
	char RealGPU_Hz_c[32] = "";
	char RealRAM_Hz_c[32] = "";
	char GPU_Load_c[32] = "";
	char Rotation_SpeedLevel_c[64] = "";
	char RAM_compressed_c[64] = "";
	char RAM_var_compressed_c[128] = "";
	char CPU_Hz_c[64] = "";
	char GPU_Hz_c[64] = "";
	char RAM_Hz_c[64] = "";
	char CPU_compressed_c[160] = "";
	char CPU_Usage0[32] = "";
	char CPU_Usage1[32] = "";
	char CPU_Usage2[32] = "";
	char CPU_Usage3[32] = "";
	char SoCPCB_temperature_c[64] = "";
	char skin_temperature_c[32] = "";
	char BatteryDraw_c[64] = "";
	char FPS_var_compressed_c[64] = "";
	char RAM_load_c[64] = "";

	uint8_t COMMON_MARGIN = 20;
	FullSettings settings;
	uint64_t systemtickfrequency_impl = systemtickfrequency;
public:
    FullOverlay() {
		tsl::hlp::doWithSDCardHandle([this] {
			GetConfigSettings(&settings);
		});
		mutexInit(&mutex_BatteryChecker);
		mutexInit(&mutex_Misc);
		tsl::hlp::requestForeground(false);
		TeslaFPS = settings.refreshRate;
		systemtickfrequency_impl /= settings.refreshRate;
		if (settings.setPosRight) {
			tsl::gfx::Renderer::getRenderer().setLayerPos(1248, 0);
		}
		deactivateOriginalFooter = true;
		StartThreads();
	}
	~FullOverlay() {
		CloseThreads();
		FullMode = true;
		tsl::hlp::requestForeground(true);
		IsFrameBackground = true;
		if (settings.setPosRight) {
			tsl::gfx::Renderer::getRenderer().setLayerPos(0, 0);
		}
		deactivateOriginalFooter = false;
	}

	virtual tsl::elm::Element* createUI() override {
		rootFrame = new tsl::elm::OverlayFrame("PluginName"_tr, APP_VERSION);

		auto Status = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
			//Print strings
			///CPU
			if (R_SUCCEEDED(clkrstCheck) || R_SUCCEEDED(pcvCheck)) {

				uint32_t height_offset = 155;
				if (realCPU_Hz && settings.showRealFreqs) {
					height_offset = 162;
				}
				renderer->drawString("CPUUsageFullOverlayCustomDrawerText"_tr.c_str(), false, COMMON_MARGIN, 120, 20, renderer->a(0xFFFF));
				auto dimensions = renderer->drawString("CPUTargetFrequencyFullOverlayCustomDrawerText"_tr.c_str(), false, COMMON_MARGIN, height_offset, 15, renderer->a(0x0000));
				uint32_t offset = COMMON_MARGIN + dimensions.first;
				if (settings.showTargetFreqs) {
					renderer->drawString("CPUTargetFrequencyFullOverlayCustomDrawerText"_tr.c_str(), false, COMMON_MARGIN, height_offset, 15, renderer->a(0xFFFF));
					renderer->drawString(CPU_Hz_c, false, offset, height_offset, 15, renderer->a(0xFFFF));
				}
				if (realCPU_Hz && settings.showRealFreqs) {
					renderer->drawString("CPURealFrequencyFullOverlayCustomDrawerText"_tr.c_str(), false, COMMON_MARGIN, height_offset - 15, 15, renderer->a(0xFFFF));
					renderer->drawString(RealCPU_Hz_c, false, offset, height_offset - 15, 15, renderer->a(0xFFFF));
					if (settings.showDeltas && settings.showTargetFreqs) {
						renderer->drawString(DeltaCPU_c, false, COMMON_MARGIN + 230, height_offset - 7, 15, renderer->a(0xFFFF));
					}
					else if (settings.showDeltas && !settings.showTargetFreqs) {
						renderer->drawString(DeltaCPU_c, false, COMMON_MARGIN + 230, height_offset - 15, 15, renderer->a(0xFFFF));
					}
				}
				else if (realCPU_Hz && settings.showDeltas && (settings.showRealFreqs || settings.showTargetFreqs)) {
					renderer->drawString(DeltaCPU_c, false, COMMON_MARGIN + 230, height_offset, 15, renderer->a(0xFFFF));
				}
				renderer->drawString(CPU_compressed_c, false, COMMON_MARGIN, height_offset + 30, 15, renderer->a(0xFFFF));
			}

			///GPU
			if (R_SUCCEEDED(clkrstCheck) || R_SUCCEEDED(pcvCheck) || R_SUCCEEDED(nvCheck)) {

				uint32_t height_offset = 320;
				if (realGPU_Hz && settings.showRealFreqs) {
					height_offset = 327;
				}

				renderer->drawString("GPUUsageFullOverlayCustomDrawerText"_tr.c_str(), false, COMMON_MARGIN, 285, 20, renderer->a(0xFFFF));
				if (R_SUCCEEDED(clkrstCheck) || R_SUCCEEDED(pcvCheck)) {
					auto dimensions = renderer->drawString("GPUTargetFrequencyFullOverlayCustomDrawerText"_tr.c_str(), false, COMMON_MARGIN, height_offset, 15, renderer->a(0x0000));
					uint32_t offset = COMMON_MARGIN + dimensions.first;
					if (settings.showTargetFreqs) {
						renderer->drawString("GPUTargetFrequencyFullOverlayCustomDrawerText"_tr.c_str(), false, COMMON_MARGIN, height_offset, 15, renderer->a(0xFFFF));
						renderer->drawString(GPU_Hz_c, false, offset, height_offset, 15, renderer->a(0xFFFF));

					}
					if (realCPU_Hz && settings.showRealFreqs) {
						renderer->drawString("GPURealFrequencyFullOverlayCustomDrawerText"_tr.c_str(), false, COMMON_MARGIN, height_offset - 15, 15, renderer->a(0xFFFF));
						renderer->drawString(RealGPU_Hz_c, false, offset, height_offset - 15, 15, renderer->a(0xFFFF));
						if (settings.showDeltas && settings.showTargetFreqs) {
							renderer->drawString(DeltaGPU_c, false, COMMON_MARGIN + 230, height_offset - 7, 15, renderer->a(0xFFFF));
						}
						else if (settings.showDeltas && !settings.showTargetFreqs) {
							renderer->drawString(DeltaGPU_c, false, COMMON_MARGIN + 230, height_offset - 15, 15, renderer->a(0xFFFF));
						}
					}
					else if (realGPU_Hz && settings.showDeltas && (settings.showRealFreqs || settings.showTargetFreqs)) {
						renderer->drawString(DeltaGPU_c, false, COMMON_MARGIN + 230, height_offset, 15, renderer->a(0xFFFF));
					}
				}
				if (R_SUCCEEDED(nvCheck)) {
					renderer->drawString(GPU_Load_c, false, COMMON_MARGIN, height_offset + 15, 15, renderer->a(0xFFFF));
				}
			}

			///RAM
			if (R_SUCCEEDED(clkrstCheck) || R_SUCCEEDED(pcvCheck) || R_SUCCEEDED(Hinted)) {

				uint32_t height_offset = 410;
				if (realRAM_Hz && settings.showRealFreqs) {
					height_offset += 7;
				}

				renderer->drawString("RAMUsageFullOverlayCustomDrawerText"_tr.c_str(), false, COMMON_MARGIN, 375, 20, renderer->a(0xFFFF));
				if (R_SUCCEEDED(clkrstCheck) || R_SUCCEEDED(pcvCheck)) {
					auto dimensions = renderer->drawString("RAMTargetFrequencyFullOverlayCustomDrawerText"_tr.c_str(), false, COMMON_MARGIN, height_offset, 15, renderer->a(0x0000));
					uint32_t offset = COMMON_MARGIN + dimensions.first;
					if (settings.showTargetFreqs) {
						renderer->drawString("RAMTargetFrequencyFullOverlayCustomDrawerText"_tr.c_str(), false, COMMON_MARGIN, height_offset, 15, renderer->a(0xFFFF));
						renderer->drawString(RAM_Hz_c, false, offset, height_offset, 15, renderer->a(0xFFFF));
					}
					if (realRAM_Hz && settings.showRealFreqs) {
						renderer->drawString("RAMRealFrequencyFullOverlayCustomDrawerText"_tr.c_str(), false, COMMON_MARGIN, height_offset - 15, 15, renderer->a(0xFFFF));
						renderer->drawString(RealRAM_Hz_c, false, offset, height_offset - 15, 15, renderer->a(0xFFFF));
						if (settings.showDeltas && settings.showTargetFreqs) {
							renderer->drawString(DeltaRAM_c, false, COMMON_MARGIN + 230, height_offset - 7, 15, renderer->a(0xFFFF));
						}
						else if (settings.showDeltas && !settings.showTargetFreqs) {
							renderer->drawString(DeltaRAM_c, false, COMMON_MARGIN + 230, height_offset - 15, 15, renderer->a(0xFFFF));
						}
					}
					else if (realRAM_Hz && settings.showDeltas && (settings.showRealFreqs || settings.showTargetFreqs)) {
						renderer->drawString(DeltaRAM_c, false, COMMON_MARGIN + 230, height_offset, 15, renderer->a(0xFFFF));
					}
					if (R_SUCCEEDED(sysclkCheck)) {
						renderer->drawString(RAM_load_c, false, COMMON_MARGIN, height_offset+15, 15, renderer->a(0xFFFF));
					}
				}
				if (R_SUCCEEDED(Hinted)) {
					auto dimensions = renderer->drawString("RAMTotalUsageFullOverlayCustomDrawerText"_tr.c_str(), false, 0, height_offset + 40, 15, renderer->a(0x0000));
					renderer->drawString("RAMTotalUsageFullOverlayCustomDrawerText"_tr.c_str(), false, COMMON_MARGIN, height_offset + 40, 15, renderer->a(0xFFFF));
					renderer->drawString(RAM_var_compressed_c, false, COMMON_MARGIN + dimensions.first, height_offset + 40, 15, renderer->a(0xFFFF));
				}
			}

			///Thermal
			if (R_SUCCEEDED(tsCheck) || R_SUCCEEDED(tcCheck) || R_SUCCEEDED(fanCheck)) {
				renderer->drawString("ThermalUsageFullOverlayCustomDrawerText"_tr.c_str(), false, 20, 550, 20, renderer->a(0xFFFF));
				if (R_SUCCEEDED(tsCheck)) renderer->drawString(BatteryDraw_c, false, COMMON_MARGIN, 575, 15, renderer->a(0xFFFF));
				if (R_SUCCEEDED(tsCheck)) {
					auto dimensions1 = renderer->drawString("ThermalTemperaturesDimensions1FullOverlayCustomDrawerText"_tr.c_str(), false, 0, 590, 15, renderer->a(0x0000));
					auto dimensions2 = renderer->drawString("ThermalTemperaturesDimensions2FullOverlayCustomDrawerText"_tr.c_str(), false, 0, 590, 15, renderer->a(0x0000));
					renderer->drawString("ThermalTemperaturesFullOverlayCustomDrawerText"_tr.c_str(), false, COMMON_MARGIN, 590, 15, renderer->a(0xFFFF));
					renderer->drawString("ThermalTemperaturesItemsFullOverlayCustomDrawerText"_tr.c_str(), false, COMMON_MARGIN + dimensions1.first, 590, 15, renderer->a(0xFFFF));
					renderer->drawString(SoCPCB_temperature_c, false, COMMON_MARGIN + dimensions1.first + dimensions2.first, 590, 15, renderer->a(0xFFFF));
				}
				if (R_SUCCEEDED(fanCheck)) renderer->drawString(Rotation_SpeedLevel_c, false, COMMON_MARGIN, 635, 15, renderer->a(0xFFFF));
			}

			///FPS
			if (GameRunning) {
				uint32_t width_offset = 230;
				auto dimensions = renderer->drawString("FPSTitleFullOverlayCustomDrawerText"_tr.c_str(), false, COMMON_MARGIN + width_offset, 120, 20, renderer->a(0xFFFF));
				uint32_t offset = COMMON_MARGIN + width_offset + dimensions.first;
				renderer->drawString(FPS_var_compressed_c, false, offset, 120, 20, renderer->a(0xFFFF));
			}

			std::string formattedKeyCombo = keyCombo;
			formatButtonCombination(formattedKeyCombo);

			std::string message = "HoldRefreshRateHitFullOverlayCustomDrawerText"_tr.c_str() + formattedKeyCombo + "ExitRefreshRateHitFullOverlayCustomDrawerText"_tr.c_str();

			renderer->drawString(message.c_str(), false, COMMON_MARGIN, 693, 23, renderer->a(0xFFFF));
		});

		rootFrame->setContent(Status);

		return rootFrame;
	}

	virtual void update() override {
		//Make stuff ready to print
		///CPU
		if (idletick0 > systemtickfrequency_impl)
			strcpy(CPU_Usage0, "UpdateDefaultCPUCore0FullOverlayCustomDrawerText"_tr.c_str());
		else snprintf(CPU_Usage0, sizeof CPU_Usage0, "UpdateCPUCore0FullOverlayCustomDrawerText"_tr.c_str(), (1.d - ((double)idletick0 / systemtickfrequency_impl)) * 100);
		if (idletick1 > systemtickfrequency_impl)
			strcpy(CPU_Usage1, "UpdateDefaultCPUCore1FullOverlayCustomDrawerText"_tr.c_str());
		else snprintf(CPU_Usage1, sizeof CPU_Usage1, "UpdateCPUCore1FullOverlayCustomDrawerText"_tr.c_str(), (1.d - ((double)idletick1 / systemtickfrequency_impl)) * 100);
		if (idletick2 > systemtickfrequency_impl)
			strcpy(CPU_Usage2, "UpdateDefaultCPUCore2FullOverlayCustomDrawerText"_tr.c_str());
		else snprintf(CPU_Usage2, sizeof CPU_Usage2, "UpdateCPUCore2FullOverlayCustomDrawerText"_tr.c_str(), (1.d - ((double)idletick2 / systemtickfrequency_impl)) * 100);
		if (idletick3 > systemtickfrequency_impl)
			strcpy(CPU_Usage3, "UpdateDefaultCPUCore3FullOverlayCustomDrawerText"_tr.c_str());
		else snprintf(CPU_Usage3, sizeof CPU_Usage3, "UpdateCPUCore3FullOverlayCustomDrawerText"_tr.c_str(), (1.d - ((double)idletick3 / systemtickfrequency_impl)) * 100);

		snprintf(CPU_compressed_c, sizeof CPU_compressed_c, "%s\n%s\n%s\n%s", CPU_Usage0, CPU_Usage1, CPU_Usage2, CPU_Usage3);

		mutexLock(&mutex_Misc);
		snprintf(CPU_Hz_c, sizeof(CPU_Hz_c), "%u.%u MHz", CPU_Hz / 1000000, (CPU_Hz / 100000) % 10);
		if (realCPU_Hz) {
			snprintf(RealCPU_Hz_c, sizeof(RealCPU_Hz_c), "%u.%u MHz", realCPU_Hz / 1000000, (realCPU_Hz / 100000) % 10);
			int32_t deltaCPU = (int32_t)(realCPU_Hz / 1000) - (CPU_Hz / 1000);
			snprintf(DeltaCPU_c, sizeof(DeltaCPU_c), "Δ %d.%u", deltaCPU / 1000, abs(deltaCPU / 100) % 10);
		}

		///GPU
		snprintf(GPU_Hz_c, sizeof GPU_Hz_c, "%u.%u MHz", GPU_Hz / 1000000, (GPU_Hz / 100000) % 10);
		if (realGPU_Hz) {
			snprintf(RealGPU_Hz_c, sizeof(RealGPU_Hz_c), "%u.%u MHz", realGPU_Hz / 1000000, (realGPU_Hz / 100000) % 10);
			int32_t deltaGPU = (int32_t)(realGPU_Hz / 1000) - (GPU_Hz / 1000);
			snprintf(DeltaGPU_c, sizeof(DeltaGPU_c), "Δ %d.%u", deltaGPU / 1000, abs(deltaGPU / 100) % 10);
		}
		snprintf(GPU_Load_c, sizeof GPU_Load_c, "UpdateGPULoadFullOverlayCustomDrawerText"_tr.c_str(), GPU_Load_u / 10, GPU_Load_u % 10);

		///RAM
		snprintf(RAM_Hz_c, sizeof RAM_Hz_c, "%u.%u MHz", RAM_Hz / 1000000, (RAM_Hz / 100000) % 10);
		if (realRAM_Hz) {
			snprintf(RealRAM_Hz_c, sizeof(RealRAM_Hz_c), "%u.%u MHz", realRAM_Hz / 1000000, (realRAM_Hz / 100000) % 10);
			int32_t deltaRAM = (int32_t)(realRAM_Hz / 1000) - (RAM_Hz / 1000);
			snprintf(DeltaRAM_c, sizeof(DeltaRAM_c), "Δ %d.%u", deltaRAM / 1000, abs(deltaRAM / 100) % 10);
		}
		float RAM_Total_application_f = (float)RAM_Total_application_u / 1024 / 1024;
		float RAM_Total_applet_f = (float)RAM_Total_applet_u / 1024 / 1024;
		float RAM_Total_system_f = (float)RAM_Total_system_u / 1024 / 1024;
		float RAM_Total_systemunsafe_f = (float)RAM_Total_systemunsafe_u / 1024 / 1024;
		float RAM_Total_all_f = RAM_Total_application_f + RAM_Total_applet_f + RAM_Total_system_f + RAM_Total_systemunsafe_f;
		float RAM_Used_application_f = (float)RAM_Used_application_u / 1024 / 1024;
		float RAM_Used_applet_f = (float)RAM_Used_applet_u / 1024 / 1024;
		float RAM_Used_system_f = (float)RAM_Used_system_u / 1024 / 1024;
		float RAM_Used_systemunsafe_f = (float)RAM_Used_systemunsafe_u / 1024 / 1024;
		float RAM_Used_all_f = RAM_Used_application_f + RAM_Used_applet_f + RAM_Used_system_f + RAM_Used_systemunsafe_f;
		char FULL_RAM_all_c[21] = "";
		char FULL_RAM_application_c[21] = "";
		char FULL_RAM_applet_c[21] = "";
		char FULL_RAM_system_c[21] = "";
		char FULL_RAM_systemunsafe_c[21] = "";
		snprintf(FULL_RAM_all_c, sizeof(FULL_RAM_all_c), "%4.2f / %4.2f MB", RAM_Used_all_f, RAM_Total_all_f);
		snprintf(FULL_RAM_application_c, sizeof(FULL_RAM_application_c), "%4.2f / %4.2f MB", RAM_Used_application_f, RAM_Total_application_f);
		snprintf(FULL_RAM_applet_c, sizeof(FULL_RAM_applet_c), "%4.2f / %4.2f MB", RAM_Used_applet_f, RAM_Total_applet_f);
		snprintf(FULL_RAM_system_c, sizeof(FULL_RAM_system_c), "%4.2f / %4.2f MB", RAM_Used_system_f, RAM_Total_system_f);
		snprintf(FULL_RAM_systemunsafe_c, sizeof(FULL_RAM_systemunsafe_c), "%4.2f / %4.2f MB", RAM_Used_systemunsafe_f, RAM_Total_systemunsafe_f);
		snprintf(RAM_var_compressed_c, sizeof(RAM_var_compressed_c), "%s\n%s\n%s\n%s\n%s", FULL_RAM_all_c, FULL_RAM_application_c, FULL_RAM_applet_c, FULL_RAM_system_c, FULL_RAM_systemunsafe_c);

		if (R_SUCCEEDED(sysclkCheck)) {
			int RAM_GPU_Load = ramLoad[SysClkRamLoad_All] - ramLoad[SysClkRamLoad_Cpu];
			snprintf(RAM_load_c, sizeof RAM_load_c,
				"UpdateRAMLoadFullOverlayCustomDrawerText"_tr.c_str(),
				ramLoad[SysClkRamLoad_All] / 10, ramLoad[SysClkRamLoad_All] % 10,
				ramLoad[SysClkRamLoad_Cpu] / 10, ramLoad[SysClkRamLoad_Cpu] % 10,
				RAM_GPU_Load / 10, RAM_GPU_Load % 10);
		}
		///Thermal
		if (hosversionAtLeast(10,0,0)) {
			snprintf(SoCPCB_temperature_c, sizeof SoCPCB_temperature_c,
				"%2.1f\u00B0C\n%2.1f\u00B0C\n%2d.%d\u00B0C",
				SOC_temperatureF, PCB_temperatureF, skin_temperaturemiliC / 1000, (skin_temperaturemiliC / 100) % 10);
		}
		else {
			snprintf(SoCPCB_temperature_c, sizeof SoCPCB_temperature_c,
				"%2d.%d\u00B0C\n%2d.%d\u00B0C\n%2d.%d\u00B0C",
				SOC_temperatureC / 1000, (SOC_temperatureC / 100) % 10,
				PCB_temperatureC / 1000, (PCB_temperatureC % 100) % 10,
				skin_temperaturemiliC / 1000, (skin_temperaturemiliC / 100) % 10);
		}
		snprintf(Rotation_SpeedLevel_c, sizeof Rotation_SpeedLevel_c, "UpdateFanRotationLevelFullOverlayCustomDrawerText"_tr.c_str(), Rotation_SpeedLevel_f * 100);

		///FPS
		snprintf(FPS_var_compressed_c, sizeof FPS_var_compressed_c, "%u\n%2.1f", FPS, FPSavg);

		mutexUnlock(&mutex_Misc);

		//Battery Power Flow
		char remainingBatteryLife[8];
		mutexLock(&mutex_BatteryChecker);
		if (batTimeEstimate >= 0) {
			snprintf(remainingBatteryLife, sizeof remainingBatteryLife, "%d:%02d", batTimeEstimate / 60, batTimeEstimate % 60);
		}
		else snprintf(remainingBatteryLife, sizeof remainingBatteryLife, "-:--");
		snprintf(BatteryDraw_c, sizeof BatteryDraw_c, "UpdateBatteryPowerFlowFullOverlayCustomDrawerText"_tr.c_str(), PowerConsumption, remainingBatteryLife);
		mutexUnlock(&mutex_BatteryChecker);
	}

	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, const HidTouchState &touchPos, HidAnalogStickState leftJoyStick, HidAnalogStickState rightJoyStick) override {
		if (isKeyComboPressed(keysHeld, keysDown, mappedButtons)) {
			TeslaFPS = 60;
			tsl::goBack();
			return true;
		}
		return false;
	}
};