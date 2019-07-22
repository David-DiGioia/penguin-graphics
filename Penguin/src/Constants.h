#pragma once

namespace Constants {

	constexpr float  PI{ 3.14159265358979f };

}

// Store all the binding point indices in one place,
// to avoid having to memorize what each point is used for.

namespace UboBindings {

	constexpr unsigned int MATERIAL{ 0 };
	constexpr unsigned int LIGHT{ 1 };
	constexpr unsigned int CAMERA_TO_CLIP{ 2 };

}

namespace TextureBindings {

	constexpr unsigned int DIFFUSE{ 0 };
	constexpr unsigned int SPECULAR{ 1 };

}