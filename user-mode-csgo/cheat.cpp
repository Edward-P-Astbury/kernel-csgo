#include "cheat.h"

void cheat::run()
{
	printf("[kernel-csgo]: Looking for csgo.exe process...\n");
	ULONG process_id = memory::get_process_id("csgo.exe");
	if (process_id)
	{
		printf("[kernel-csgo]: Process identifier found: %d\n", process_id);
		printf("[kernel-csgo]: Looking for client.dll...\n");
		ULONG client_base = driver_controller::get_client_base(process_id);
		if (client_base)
		{
			printf("[kernel-csgo]: client.dll found: 0x%x\n", client_base);
			printf("[kernel-csgo]: Running...\n");

			for (;;)
			{
				const auto local_player = driver_controller::read_memory<ULONG>(process_id, client_base + m_dwLocalPlayer);
				const auto local_team = driver_controller::read_memory<ULONG>(process_id, local_player + m_iTeamNum);

				/*ULONG my_cross = driver_controller::read_memory<ULONG>(process_id, local_player + m_iCrosshairId);
				if (my_cross >= 1 && my_cross <= 64)
				{
					ULONG entity = driver_controller::read_memory<ULONG>(process_id, client_base + dwEntityList + ((my_cross - 1) * 0x10));
					if (entity <= 0) { continue; }

					ULONG entity_health = driver_controller::read_memory<ULONG>(process_id, entity + m_iHealth);
					ULONG entity_team = driver_controller::read_memory<ULONG>(process_id, entity + m_iTeamNum);

					if (entity_health > 0 && entity_team != local_team && entity_team > 1)
					{
						Sleep(3);
						mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
						Sleep(1);
						mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
					}
				}*/
				for (auto i = 1; i <= 64; ++i)
				{
					const auto entity = driver_controller::read_memory<ULONG>(process_id, client_base + dwEntityList + i * 0x10);

					if (driver_controller::read_memory<ULONG>(process_id, entity + m_iTeamNum) == local_team)
						continue;

					driver_controller::write_memory<bool>(process_id, entity + m_bSpotted, true);
				}

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
