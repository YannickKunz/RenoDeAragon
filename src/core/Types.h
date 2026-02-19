#pragma once

enum class DayNightState { DAY, NIGHT };

enum class Direction { LEFT, RIGHT };

enum class PlatformType {
  NORMAL,
  FLOWER,   // Solid during Day
  MUSHROOM, // Bouncy during Night
  INVISIBLE // Always solid but not drawn (unless debug)
};

// Aliases to match User's Logic if desired, or we just map them in Level
// loading The user used 'basic', 'flower', 'mushroom', 'invisible'. Let's make
// these global constants or constexpr to match user's syntax if we want to
// copy-paste their code OR, we just update the PlatformType to match
#define basic PlatformType::NORMAL
#define flower PlatformType::FLOWER
#define mushroom PlatformType::MUSHROOM
#define invisible PlatformType::INVISIBLE
