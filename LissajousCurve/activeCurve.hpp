#pragma once
#include "Utility.hpp"


class ActiveCurve {
	float m_freq1, m_freq2, m_amp1, m_amp2, m_phase1, m_phase2, m_tick, m_tr, m_strokeWeight;
	sf::Vector2f m_center;
	sf::Color m_col;

	sf::CircleShape m_cs;
	std::vector<sf::Vertex> m_vertexArray;
	float m_t;
	sf::Vector2f m_v, m_v1, m_v2;
	int m_totPoints;
public:
	ActiveCurve() {}
	ActiveCurve(float freq1, float freq2, float amp1, float amp2, float phase1, float phase2, float tick, float tr, float strokeWeight, const sf::Vector2f& center, const sf::Color& col) :
	m_freq1(freq1), m_freq2(freq2), m_amp1(amp1), m_amp2(amp2), m_phase1(phase1), m_phase2(phase2), m_tick(tick), m_tr(tr), m_strokeWeight(strokeWeight), m_center(center), m_col(col) {
		m_totPoints = std::max(int(ceilf(m_tr/m_tick)), 1);
		m_t = 0;
		m_vertexArray = std::vector<sf::Vertex>();
		m_cs.setRadius(strokeWeight);
		m_cs.setFillColor(m_col);
		m_cs.setOutlineThickness(0);
		m_cs.setOrigin(strokeWeight, strokeWeight);
		m_phase1 += PI / 2;
		m_phase2 -= PI / 2;
	}

	void addPoint() {
		sf::Vector2f line = m_v - m_v1;
		sf::Vector2f normal = MathUtils::Norm(sf::Vector2f(-line.y, line.x));
		sf::Vector2f tangent = MathUtils::Norm(MathUtils::Norm(m_v2 - m_v) + MathUtils::Norm(m_v - m_v1));
		sf::Vector2f miter = sf::Vector2f(-tangent.y, tangent.x);
		float length = std::abs(m_strokeWeight / (2.0f * MathUtils::Dot(miter, normal)));

		if (length <= m_strokeWeight) {
			m_vertexArray.push_back({ m_v - length * miter, m_col });
			m_vertexArray.push_back({ m_v + length * miter, m_col });
		}
	}

	void update() {
		m_vertexArray.clear();
		for (int _ = 0; _ < m_totPoints; _++) {
			m_v = { m_amp1 * std::cosf(m_freq1 * m_t + m_phase1) + m_center.x, m_amp2 * std::cosf(m_freq2 * m_t + m_phase2) + m_center.y };
			m_v1 = { m_amp1 * std::cosf(m_freq1 * (m_t - m_tick) + m_phase1) + m_center.x, m_amp2 * std::cosf(m_freq2 * (m_t - m_tick) + m_phase2) + m_center.y };
			m_v2 = { m_amp1 * std::cosf(m_freq1 * (m_t + m_tick) + m_phase1) + m_center.x, m_amp2 * std::cosf(m_freq2 * (m_t + m_tick) + m_phase2) + m_center.y };

			addPoint();

			m_t += m_tick;
		}
		m_cs.setPosition(m_v);
	}

	void draw(sf::RenderWindow& win) {
		win.draw(m_vertexArray.data(), m_vertexArray.size(), sf::TriangleStrip);
		win.draw(m_cs);
	}
};