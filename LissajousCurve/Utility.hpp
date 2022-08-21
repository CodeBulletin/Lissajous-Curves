#pragma once

#include <tuple>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <numeric>
#include <random>
#include <functional>
#include <algorithm>
#include <sstream>
#include <iomanip>

#include <vector>
#include <execution>

#include <iostream>

#define PI 3.1415926535f
class ColFunc {
public:
	static std::tuple<float, float, float> RGBtoHSL(const sf::Color& color)
	{
		float R = color.r / 255.f;
		float G = color.g / 255.f;
		float B = color.b / 255.f;

		float M = std::max({ R, G, B });
		float m = std::min({ R, G, B });
		float C = M - m;

		float H = 0.f;
		float S = 0.f;
		float L = 0.f;

		if (C != 0.f)
		{
			if (M == R)
				H = std::fmod(((G - B) / C), 6.f);
			else if (M == G)
				H = ((B - R) / C) + 2;
			else if (M == B)
				H = ((R - G) / C) + 4;

			H *= 60;
		}

		if (H < 0.f)
			H += 360;

		L += (M + m) / 2;

		if (L != 1.f && L != 0.f)
			S = C / (1 - std::fabs(2 * L - 1));

		return { H, S, L };
	}


	static sf::Color HSLtoRGB(float H, float S, float L, float A = 1.0f)
	{
		float C = (1 - std::fabs(2 * L - 1)) * S;
		float HPrime = H / 60;
		float X = C * (1 - std::fabs(std::fmod(HPrime, 2.f) - 1));
		float M = L - C / 2;

		float R = 0.f;
		float G = 0.f;
		float B = 0.f;

		switch (static_cast<int>(HPrime))
		{
		case 0: R = C; G = X;        break;
		case 1: R = X; G = C;        break;
		case 2:        G = C; B = X; break;
		case 3:        G = X; B = C; break;
		case 4: R = X;        B = C; break;
		case 5: R = C;        B = X; break;
		}

		R += M;
		G += M;
		B += M;

		sf::Color color;
		color.r = static_cast<sf::Uint8>(std::round(R * 255));
		color.g = static_cast<sf::Uint8>(std::round(G * 255));
		color.b = static_cast<sf::Uint8>(std::round(B * 255));
		color.a = static_cast<sf::Uint8>(std::round(A * 255));

		return color;
	}

	static sf::Color LerpRGB(const sf::Color& a, const sf::Color& b, float t) {
		return sf::Color(
			a.r + (b.r - a.r) * t,
			a.g + (b.g - a.g) * t,
			a.b + (b.b - a.b) * t,
			(a.a + b.a) / 2.0f
		);
	}
};

namespace Random {
	static auto Random = std::bind(std::uniform_real_distribution<float>(0.0f,
		std::nextafter(1.0f, FLT_MAX)), std::mt19937{ std::random_device{}() });
};

namespace MathUtils {
	inline sf::Vector2f Norm(sf::Vector2f v) {
		float mag = std::sqrt(v.x * v.x + v.y * v.y);
		return v / mag;
	}

	inline float Mag(const sf::Vector2f& v) {
		return std::sqrt(v.x * v.x + v.y * v.y);
	}

	inline float Dot(const sf::Vector2f& v1, const sf::Vector2f& v2) {
		return (v1.x * v2.x + v1.y * v2.y);
	}
};

static bool operator>(const sf::Vector2f& v1, const sf::Vector2f& v2) {
	return (v1.x > v2.x && v1.y > v2.y);
}

static sf::Vector2f rot30(std::cosf(PI / 6.0f), std::sinf(PI / 6.0f));

static void makeLine(const sf::Vector2f& start, const sf::Vector2f& end, const sf::Color& col, float size, sf::VertexArray& v) {
	sf::Vertex V1, V2, V3, V4;
	V1.color = col;
	V2.color = col;
	V3.color = col;
	V4.color = col;

	sf::Vector2f line = MathUtils::Norm(end - start);
	sf::Vector2f NNormal = sf::Vector2f({ -line.y, line.x }) * 0.5f * size;

	V1.position = start + NNormal;
	V2.position = start - NNormal;
	V3.position = end - NNormal;
	V4.position = end + NNormal;

	v.append(V1);
	v.append(V2);
	v.append(V3);
	v.append(V4);
}