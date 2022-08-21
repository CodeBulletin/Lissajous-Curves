#pragma once
#include "Utility.hpp"

class Circle {
	sf::CircleShape m_circle;
	sf::CircleShape m_point;
	float m_radius;
	sf::Vector2f m_center;
	float m_freq;
	float m_phase;
	bool m_is_x;
	float m_endp;
	float m_pointRadius;
	float m_strokeWeight;
	sf::Color m_col;
	sf::Vector2f m_pos, m_pos1, m_pos2;
	sf::Vertex m_v1;
	sf::Vertex m_v2;
	sf::Vertex m_v3;
	sf::Vertex m_v4;
public:
	Circle() {}

	Circle(float radius, sf::Vector2f center, float freq, float phase, bool is_x, float endp, float pointRadius, float strokeWeight = 2.0f,
		sf::Color col = sf::Color::White, int resolution = 100) : m_radius(radius), m_center(center), m_freq(freq), m_phase(phase), m_is_x(is_x), m_endp(endp),
		m_pointRadius(pointRadius), m_strokeWeight(strokeWeight), m_col(col) {
		m_circle.setFillColor(sf::Color::Black);
		m_circle.setOutlineColor(m_col);
		m_circle.setOutlineThickness(m_strokeWeight);
		m_circle.setPointCount(resolution);
		m_circle.setRadius(m_radius - m_strokeWeight / 2.0f);
		m_circle.setPosition(m_center);
		m_circle.setOrigin(m_radius - m_strokeWeight / 2.0f, m_radius - m_strokeWeight / 2.0f);
		m_point.setFillColor(m_col);
		m_point.setOutlineColor(m_col);
		m_point.setOutlineThickness(1);
		m_point.setPointCount(30);
		m_point.setRadius(m_pointRadius);
		m_point.setPosition(m_radius * sf::Vector2f(cosf(-PI / 2.0f), sinf(-PI / 2.0f)) + m_center);
		m_point.setOrigin(m_pointRadius, m_pointRadius);
		m_v1.color = m_col;
		m_v1.color.a *= 0.7f;
		m_v2.color = m_col;
		m_v2.color.a *= 0.7f;
		m_v3.color = m_col;
		m_v3.color.a *= 0.7f;
		m_v4.color = m_col;
		m_v4.color.a *= 0.7f;
	}

	void setTime(float t, float tu) {
		m_pos = m_radius * sf::Vector2f(cosf(m_freq * 2 * PI * t - PI / 2.0f + m_phase), sinf(m_freq * 2 * PI * t - PI / 2.0f + m_phase)) + m_center;
		m_pos1 = m_radius * sf::Vector2f(cosf(m_freq * 2 * PI * (t + tu) - PI / 2.0f + m_phase), sinf(m_freq * 2 * PI * (t + tu) - PI / 2.0f + m_phase)) + m_center;
		m_pos2 = m_radius * sf::Vector2f(cosf(m_freq * 2 * PI * (t - tu) - PI / 2.0f + m_phase), sinf(m_freq * 2 * PI * (t - tu) - PI / 2.0f + m_phase)) + m_center;

		m_point.setPosition(m_pos);

		m_v1.position = m_is_x ? m_pos - sf::Vector2f(0.0f, 0.8f * m_strokeWeight / 2.0f) : m_pos - sf::Vector2f(0.8f * m_strokeWeight / 2.0f, 0.0f);
		m_v2.position = m_is_x ? m_pos + sf::Vector2f(0.0f, 0.8f * m_strokeWeight / 2.0f) : m_pos + sf::Vector2f(0.8f * m_strokeWeight / 2.0f, 0.0f);
		m_v3.position = m_is_x ? sf::Vector2f({ m_endp, m_pos.y }) + sf::Vector2f(0.0f, 0.8f * m_strokeWeight / 2.0f) :
			sf::Vector2f({ m_pos.x, m_endp }) + sf::Vector2f(0.8f * m_strokeWeight / 2.0f, 0.0f);
		m_v4.position = m_is_x ? sf::Vector2f({ m_endp, m_pos.y }) - sf::Vector2f(0.0f, 0.8f * m_strokeWeight / 2.0f) :
			sf::Vector2f({ m_pos.x, m_endp }) - sf::Vector2f(0.8f * m_strokeWeight / 2.0f, 0.0f);
	}

	void draw(sf::RenderWindow& win) {
		win.draw(m_circle);
		win.draw(m_point);
	}

	void drawLine(sf::VertexArray& v) {
		v.append(m_v1);
		v.append(m_v2);
		v.append(m_v3);
		v.append(m_v4);
	}

	sf::Vector2f& getMainPos() {
		return m_pos;
	}

	std::tuple<sf::Vector2f&, sf::Vector2f&> getPos() {
		return { m_pos1, m_pos2 };
	}

	float getFreq() {
		return m_freq;
	}

	float getPhase() {
		return m_phase;
	}

	bool getAxis() {
		return m_is_x;
	}

	sf::Color getCol() {
		return m_col;
	}
};