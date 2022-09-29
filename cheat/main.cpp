#include "memory.h"
#include<thread>

using namespace std;

namespace offset
{
	constexpr ::std::ptrdiff_t dwLocalPlayer = 0xDC14CC;
	constexpr ::std::ptrdiff_t dwClientState = 0x58CFDC;
	constexpr ::std::ptrdiff_t dwClientState_ViewAngles = 0x4D90;
	constexpr ::std::ptrdiff_t m_aimPunchAngle = 0x303C;
	constexpr ::std::ptrdiff_t m_iShotsFired = 0x103E0;
}

struct Vector2
{
	float x = { }, y = { };
};

int main()
{
	auto mem = Memory("csgo.exe");
	const auto cli = mem.GetModuleAddress("client.dll");
	const auto eng = mem.GetModuleAddress("engine.dll");

	auto oldPunch = Vector2{ };

	while (true)
	{
		this_thread::sleep_for(chrono::microseconds(1));

		const auto localPlayer = mem.Read<uintptr_t>(cli + offset::dwLocalPlayer);
		const auto shotsFired = mem.Read<int32_t>(localPlayer + offset::m_iShotsFired);

		if (shotsFired)
		{
			const auto clientState = mem.Read<uintptr_t>(eng + offset::dwClientState);
			const auto viewAngles = mem.Read<Vector2>(clientState + offset::dwClientState_ViewAngles);

			const auto aimPunch = mem.Read<Vector2>(localPlayer + offset::m_aimPunchAngle);

			auto newAngles = Vector2
			{
				viewAngles.x + oldPunch.x - aimPunch.x * 2.f,
				viewAngles.y + oldPunch.y - aimPunch.y * 2.f
			};
			// to fix crash
			if (newAngles.x > 89.f)
				newAngles.x = 89.f;

			if (newAngles.x < -89.f)
				newAngles.x = -89.f;

			while (newAngles.y>180.f)
			{
				newAngles.y -= 360.f;
			}
			while (newAngles.y < -180.f)
			{
				newAngles.y += 360.f;
			}

			mem.Write<Vector2>(clientState + offset::dwClientState_ViewAngles, newAngles);

			oldPunch.x = aimPunch.x * 2.f;
			oldPunch.y = aimPunch.y * 2.f;
		}
		else
		{
			oldPunch.x = oldPunch.y = 0.f;
		}
	}
	return 0;
}