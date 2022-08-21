#pragma once
#include "Utility.hpp"

struct Vector2inc
{
	sf::Vector2f v;
	float i;
	float j;
};

class Wave {
	float m_freq, m_amp, m_phase, m_tmax, m_strokeWeight;
	sf::Vector2f m_range, m_startPos;
	sf::Color m_col, m_boxCol;

	std::vector<Vector2inc> m_vert;
	sf::VertexArray m_vertexArray;
	sf::RectangleShape rs;
	int m_points;
	float m_lambda, m_length;
	bool over20000;
public:
	Wave() {}
	Wave(float freq, float amp, float phase, float tmax, float strokeWeight, const sf::Vector2f& range, const sf::Vector2f& startPos, const sf::Color& col, const sf::Color& boxCol, float maxAmp):
		m_freq(freq), m_amp(amp), m_phase(phase), m_tmax(tmax), m_strokeWeight(strokeWeight), m_range(range), m_startPos(startPos), m_col(col), m_boxCol(boxCol){
		
		m_length = (range.y - range.x);
		m_lambda = m_length / m_freq;

		m_points = std::max(int(m_freq * m_tmax * 100 + 2), 502);
		if (m_points > 20000) {
			m_points = 4;
			over20000 = true;
			m_vertexArray = sf::VertexArray(sf::TriangleStrip, 4);
			m_vertexArray[0] = { startPos - sf::Vector2f(amp, 0), m_col };
			m_vertexArray[1] = { startPos + sf::Vector2f(amp, 0), m_col };
			m_vertexArray[2] = { startPos + sf::Vector2f(-amp, range.y), m_col };
			m_vertexArray[3] = { startPos + sf::Vector2f(amp, range.y), m_col };
		}
		else {
			over20000 = false;
		}
		if (!over20000) {
			m_vert = std::vector<Vector2inc>(m_points, { {0, 0}, 0, 0 });
			m_vertexArray = sf::VertexArray(sf::TriangleStrip, 0);
			float hpoints = m_tmax / float(m_points - 2);
			float vpoints = m_length / float(m_points - 2);
			float err = (range.y - float(m_points) * vpoints) / float(m_points);
			vpoints -= err;
			int i = 0;
			for (auto& vert : m_vert) {
				vert.i = float(i - 1) * hpoints;
				vert.j = float(i - 1) * vpoints;
				i++;
			}

			updateVert(0);
		}
		rs.setFillColor(boxCol);
		rs.setOutlineThickness(0);
		rs.setPosition(startPos - sf::Vector2f(maxAmp * 1.1f, 10));
		rs.setSize(sf::Vector2f(2.2f * maxAmp, range.y + 20));
	}

	void updateVert(float t) {
		if (!over20000) {
			m_vertexArray.clear();

			std::for_each(std::execution::par_unseq, m_vert.begin(), m_vert.end(), [&t, this](auto& vert) -> void {
				vert.v.x = m_startPos.x + m_amp * std::cosf(2 * PI * m_freq * (t + vert.i) + m_phase);
				vert.v.y = m_startPos.y + vert.j;
				});

			for (int i = 0; i < m_points - 2; i += 2) {
				sf::Vector2f line = m_vert[i + 1].v - m_vert[i].v;
				sf::Vector2f normal = MathUtils::Norm(sf::Vector2f(-line.y, line.x));
				sf::Vector2f tangent = MathUtils::Norm(MathUtils::Norm(m_vert[i + 2].v - m_vert[i + 1].v) + MathUtils::Norm(m_vert[i + 1].v - m_vert[i].v));
				sf::Vector2f miter = sf::Vector2f(-tangent.y, tangent.x);
				float length = std::abs(m_strokeWeight / (2.0f * MathUtils::Dot(miter, normal)));

				if (length <= m_strokeWeight) {
					m_vertexArray.append({ m_vert[i + 1].v - length * miter, m_col });
					m_vertexArray.append({ m_vert[i + 1].v + length * miter, m_col });
				}
			}
		}
	}

	void draw(sf::RenderWindow& win) {
		win.draw(rs);
		win.draw(m_vertexArray);
	}
};