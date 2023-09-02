#include "cheat.h"

#include "json.hpp"
#include "easywsclient.hpp"
#include "skCrypter.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include <stdio.h>

using json = nlohmann::json;
using easywsclient::WebSocket;

constexpr float M_PI = 3.1415927f;

void normalise(vector3& angle) 
{
	if (angle.x > 89.0f) angle.x -= 180.0f;
	if (angle.x < -89.0f) angle.x += 180.0f;
	angle.y = std::remainderf(angle.y, 360.0f);
}

vector3 calc_angle(const vector3& source, const vector3& destination) 
{
	vector3 retAngle;
	vector3 delta = source - destination;
	float hyp = sqrtf(delta.x * delta.x + delta.y * delta.y);
	retAngle.x = (float)(atan(delta.z / hyp) * (180.0f / M_PI));
	retAngle.y = (float)(atan(delta.y / delta.x) * (180.0f / M_PI));
	retAngle.z = 0.f;
	if (delta.x >= 0.f)
		retAngle.y += 180.f;

	return retAngle;
}

float get_distance(const vector3& point1, const vector3& point2) {
	vector3 delta = point1 - point2;
	normalise(delta);
	float temp = (delta.x * delta.x) + (delta.y * delta.y) + (delta.z * delta.z);
	return sqrtf(temp);
}

void cheat::run()
{
	INT rc;
	WSADATA wsaData;

	rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (rc)
	{
		printf("WSAStartup Failed.\n");
		return;
	}

	WebSocket::pointer ws = WebSocket::from_url("ws://localhost:6969");
	assert(ws);
	puts("Connected.\n");
	
	ma_result result;
	ma_engine engine;

	result = ma_engine_init(NULL, &engine);
	if (result != MA_SUCCESS)
	{
		printf("miniaudio.h Failed.\n");
		return;
	}

	ma_result sound_result;
	ma_sound sound;

	sound_result = ma_sound_init_from_file(&engine, "sounds/beep.wav", 0, NULL, NULL, &sound);
	if (sound_result != MA_SUCCESS) 
	{
		printf("miniaudio.h sound Failed.\n");
		return;
	}

	printf("[kernel-csgo]: Looking for csgo.exe process...\n");
	ULONG process_id = memory::get_process_id("csgo.exe");
	if (process_id)
	{
		printf("[kernel-csgo]: Process identifier found: %d\n", process_id);
		printf("[kernel-csgo]: Looking for client.dll...\n");
		ULONG client_base = driver_controller::get_client_base(process_id);

		printf("[kernel-csgo]: Looking for engine.dll...\n");
		ULONG engine_base = driver_controller::get_engine_base(process_id);

		if (client_base)
		{
			printf("[kernel-csgo]: client.dll found: 0x%x\n", client_base);
			printf("[kernel-csgo]: engine.dll found: 0x%x\n", engine_base);
			printf("[kernel-csgo]: Running...\n");

			for (;;)
			{
				ULONG client_state = driver_controller::read_memory<ULONG>(process_id, engine_base + dwClientState);

				int n = 0;
				char c = 'o';
				char map_name_buffer[32];

				while (c != '\0')
				{
					c = driver_controller::read_memory<char>(process_id, client_state + dwClientState_Map + n);
					map_name_buffer[n] = c;
					n++;
				}
				std::string map_name = std::string(map_name_buffer);

				json data;
				data["map_name"] = map_name;
				data["x_positions"] = json::array();
				data["y_positions"] = json::array();
				data["health"] = json::array();
				data["last_places"] = json::array();
				data["simulation_times"] = json::array();

				const auto local_player = driver_controller::read_memory<ULONG>(process_id, client_base + dwLocalPlayer);
				const auto local_team = driver_controller::read_memory<ULONG>(process_id, local_player + m_iTeamNum);
				
				for (int i = 1; i <= 64; ++i)
				{
					ULONG base_entity = driver_controller::read_memory<ULONG>(process_id, client_base + dwEntityList + i * 0x10);
					if (base_entity == (unsigned long)NULL || base_entity == local_player)
						continue;
					
					ULONG entity_health = driver_controller::read_memory<ULONG>(process_id, base_entity + m_iHealth);
					if (entity_health <= 0 || entity_health > 100) // check if entity is alive
						continue;
					
					ULONG entity_team = driver_controller::read_memory<ULONG>(process_id, base_entity + m_iTeamNum);
					if (local_team == entity_team)
						continue;
					
					if (driver_controller::read_memory<bool>(process_id, local_player + m_bDormant)) // check if dormant
						continue;

					float local_player_x = driver_controller::read_memory<float>(process_id, local_player + m_vecOrigin);
					float local_player_y = driver_controller::read_memory<float>(process_id, local_player + m_vecOrigin + 0x04);

					float entity_origin_x = driver_controller::read_memory<float>(process_id, base_entity + m_vecOrigin);
					float entity_origin_y = driver_controller::read_memory<float>(process_id, base_entity + m_vecOrigin + 0x04);

					// reading player last place names
					n = 0;
					c = '0';
					char last_place[32];
					while (c != '\0')
					{
						c = driver_controller::read_memory<char>(process_id, base_entity + m_szLastPlaceName + n);
						last_place[n] = c;
						n++;
					}
					
					int simulation_time = driver_controller::read_memory<int>(process_id, base_entity + m_flSimulationTime);

					// use this value in sound esp?
					float distance = calculate_distance(local_player_x, local_player_y, entity_origin_x, entity_origin_y);
					
					vector3 local_head_pos = driver_controller::read_memory<vector3>(process_id, local_player + m_vecOrigin) +
						driver_controller::read_memory<vector3>(process_id, local_player + m_vecViewOffset);
					
					DWORD bone_matrix = driver_controller::read_memory<uintptr_t>(process_id, base_entity + m_dwBoneMatrix);

					vector3 end_head_pos = vector3{
						driver_controller::read_memory<float>(process_id, bone_matrix + 0x30 * 8 + 0x0C),
						driver_controller::read_memory<float>(process_id, bone_matrix + 0x30 * 8 + 0x1C),
						driver_controller::read_memory<float>(process_id, bone_matrix + 0x30 * 8 + 0x2C)
					};
					
					vector3 aim_angle = calc_angle(local_head_pos, end_head_pos);
					vector3 view_angles = driver_controller::read_memory<vector3>(process_id, client_state + dwClientState_ViewAngles);
					
					float fov_dist = get_distance(view_angles, aim_angle);

					if (fov_dist < 1.0f)
					{
						ma_sound_set_volume(&sound, 1); // 1 is default
						ma_sound_set_pitch(&sound, 1);
						ma_sound_start(&sound);
					}
					else if (fov_dist < 7.5f)
					{
						ma_sound_set_volume(&sound, 1);
						ma_sound_set_pitch(&sound, 0.75);
						ma_sound_start(&sound);
					}
					else if (fov_dist < 20.5f)
					{
						ma_sound_set_volume(&sound, 1);
						ma_sound_set_pitch(&sound, 0.5);
						ma_sound_start(&sound);
					}

					data["x_positions"].push_back(entity_origin_x);
					data["y_positions"].push_back(entity_origin_y);
					data["health"].push_back(entity_health);
					data["last_places"].push_back(std::string(last_place));
					data["simulation_times"].push_back(simulation_time);
				}

				ws->send(data.dump());
				ws->poll();
				data.clear();

				Sleep(1);
			}
		}
		else
		{
			printf("[kernel-csgo]: Failed to obtain client.dll\n");
		}
	}
	else
	{
		printf("[kernel-csgo]: Failed to find csgo.exe process\n");
	}
}
