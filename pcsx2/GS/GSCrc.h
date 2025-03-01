/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2021 PCSX2 Dev Team
 *
 *  PCSX2 is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  PCSX2 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with PCSX2.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <map>

class CRC
{
public:
	enum Title
	{
		NoTitle,
		AceCombatZero,
		AceCombat4,
		AceCombat5,
		ApeEscape2,
		ArTonelico2,
		Barnyard,
		BeyondGoodAndEvil,
		BigMuthaTruckers,
		BrianLaraInternationalCricket,
		BurnoutDominator,
		BurnoutRevenge,
		BurnoutTakedown,
		Clannad,
		CrashBandicootWoC,
		DarkCloud,
		DBZBT2,
		DBZBT3,
		DeathByDegreesTekkenNinaWilliams,
		DestroyAllHumans,
		DestroyAllHumans2,
		DrivingEmotionTypeS,
		DuelSaviorDestiny,
		EvangelionJo,
		FFX,
		FFX2,
		FFXII,
		FIFA03,
		FIFA04,
		FIFA05,
		FightingBeautyWulong,
		GetaWay,
		GetaWayBlackMonday,
		GiTS,
		GodHand,
		GodOfWar,
		GodOfWar2,
		GT3,
		GT4,
		GTConcept,
		HarleyDavidson,
		HarryPotterATCOS,
		HarryPotterATGOF,
		HarryPotterATHBP,
		HarryPotterATPOA,
		HarryPotterOOTP,
		HauntingGround,
		ICO,
		IkkiTousen,
		IndianaJonesAndTheEmperorsTomb,
		JackieChanAdv,
		Jak1,
		Jak2,
		Jak3,
		JakX,
		JurassicPark,
		KazokuKeikakuKokoroNoKizuna,
		KnightsOfTheTemple2,
		Kunoichi,
		KyuuketsuKitanMoonties,
		Lamune,
		LegacyOfKainDefiance,
		LordOfTheRingsThirdAge,
		MajokkoALaMode2,
		Manhunt2,
		MetalSlug6,
		MidnightClub3,
		NicktoonsUnite,
		Okami,
		Oneechanbara2Special,
		OneTwentyYenStories,
		Persona3,
		PiaCarroteYoukosoGPGakuenPrincess,
		ProjectSnowblind,
		Quake3Revolution,
		RadiataStories,
		RatchetAndClank,
		RatchetAndClank2,
		RatchetAndClank3,
		RatchetAndClank4,
		RatchetAndClank5,
		RedDeadRevolver,
		RickyPontingInternationalCricket,
		RozenMaidenGebetGarden,
		SacredBlaze,
		SakuraTaisen,
		SakuraWarsSoLongMyLove,
		SFEX3,
		ShadowHearts,
		ShadowofRome,
		ShinOnimusha,
		Shox,
		Simple2000Vol114,
		SkyGunner,
		SlamTennis,
		SMTNocturne,
		SonicUnleashed,
		SoTC,
		SoulReaver2,
		Spartan,
		StarOcean3,
		SteambotChronicles,
		SSX3,
		SuperManReturns,
		SVCChaos,
		TalesOfAbyss,
		TalesOfLegendia,
		TalesofSymphonia,
		Tekken5,
		TheIncredibleHulkUD,
		TombRaiderAnniversary,
		TombRaiderLegend,
		TombRaiderUnderworld,
		TribesAerialAssault,
		TomoyoAfter,
		TouristTrophy,
		UltramanFightingEvolution,
		UrbanReign,
		ValkyrieProfile2,
		Whiplash,
		WildArms4,
		XenosagaE3,
		Yakuza,
		Yakuza2,
		ZettaiZetsumeiToshi2,
		TitleCount,
	};

	enum Region
	{
		NoRegion,
		US,
		EU,
		JP,
		RU,
		FR,
		DE,
		IT,
		ES,
		CH,
		ASIA,
		KO,
		RegionCount,
	};

	enum Flags
	{
		PointListPalette = 1,
		TextureInsideRt = 2,
	};

	struct Game
	{
		u32 crc;
		Title title;
		Region region;
		u32 flags;
	};

private:
	static const Game m_games[];
	static std::map<u32, const Game*> m_map;

public:
	static const Game& Lookup(u32 crc);
};
