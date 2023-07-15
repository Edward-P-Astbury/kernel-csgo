#include "cheat.h"

#include "json.hpp"
#include "easywsclient.hpp"

using json = nlohmann::json;
using easywsclient::WebSocket;

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
				char c = 'o'; // TODO: was 'c'
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

					INT simulation_time = driver_controller::read_memory<INT>(process_id, base_entity + m_flSimulationTime);

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
