#pragma once
#include "circle.hpp"
#include "popup.hpp"

class Curve {
	float m_pointSize;
	float m_strokeWeight;
	Circle* m_p1, * m_p2;

	sf::Color m_col;
	sf::CircleShape m_pointShape;
	sf::VertexArray m_vertexArray;
	

	sf::Vector2f m_point;

	float m_doneIn;

	bool m_isComplete;
	sf::Vector2f m_bboxMin, m_bboxMax;

public:
	Curve() {}

	Curve(float pointSize, float strokeWeight, Circle& p1, Circle& p2, float timeunit, sf::Vector2f bboxMin, sf::Vector2f bboxMax) :
		m_pointSize(pointSize), m_strokeWeight(strokeWeight) {
		m_p1 = &p1;
		m_p2 = &p2;

		m_col = ColFunc::LerpRGB(m_p1->getCol(), m_p2->getCol(), 0.5);

		m_pointShape.setRadius(m_pointSize);
		m_pointShape.setFillColor(m_col);
		m_pointShape.setOutlineColor(m_col);
		m_pointShape.setOutlineThickness(1);
		m_pointShape.setPointCount(30);
		m_pointShape.setOrigin(m_pointSize, m_pointSize);

		float F1 = m_p1->getFreq();
		float F2 = m_p2->getFreq();

		m_doneIn = 1 / (float)std::gcd((int)F1, (int)F2) + timeunit;

		m_vertexArray = sf::VertexArray(sf::TrianglesStrip, 0);
		m_isComplete = false;

		m_bboxMin = bboxMin;
		m_bboxMax = bboxMax;
	}

	void setPoint(float time) {
		m_point = { m_p2->getMainPos().x, m_p1->getMainPos().y };
		m_pointShape.setPosition(m_point);
		if (!m_isComplete) {
			if (!complete(time)) {
				auto [v11, v12] = m_p1->getPos();
				auto [v21, v22] = m_p2->getPos();
				sf::Vector2f point1 = { v21.x, v11.y };
				sf::Vector2f point2 = { v22.x, v12.y };
				sf::Vector2f line = m_point - point2;
				sf::Vector2f normal = MathUtils::Norm(sf::Vector2f(-line.y, line.x));
				sf::Vector2f tangent = MathUtils::Norm(MathUtils::Norm(point1 - m_point) + MathUtils::Norm(m_point - point2));
				sf::Vector2f miter = sf::Vector2f(-tangent.y, tangent.x);
				float length = std::abs(m_strokeWeight / (2.0f * MathUtils::Dot(miter, normal)));

				if (length <= m_strokeWeight) {

					point1 = m_point - length * miter;
					point2 = m_point + length * miter;

					sf::Vertex v1, v2;
					v1.color = m_col;
					v1.position = point1;
					v2.color = m_col;
					v2.position = point2;
					m_vertexArray.append(v2);
					m_vertexArray.append(v1);
				}
			}
		}
	}

	bool complete(float time) {
		if (std::abs(time) > m_doneIn) {
			m_isComplete = true;
			return true;
		}
		else {
			return false;
		}
	}

	void draw(sf::RenderWindow& win) {
		win.draw(m_vertexArray);
		win.draw(m_pointShape);
	}

	bool selected(const sf::Vector2f& v) {
		return (v > m_bboxMin && m_bboxMax > v);
	}

	void setBox(sf::RectangleShape& r, PopUp& popup) {
		r.setPosition(m_bboxMin);
		r.setSize(m_bboxMax - m_bboxMin);
		if (!popup.isOut(m_bboxMax.x)) {
			popup.setCursorPos({ m_bboxMax.x, (m_bboxMax.y + m_bboxMin.y) / 2.0f }, true);
		}
		else {
			popup.setCursorPos({ m_bboxMin.x, (m_bboxMax.y + m_bboxMin.y) / 2.0f }, false);
		}
	}
};