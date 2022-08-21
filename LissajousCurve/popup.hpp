#pragma once
#include "Utility.hpp"

struct PopUp {
	float m_width, m_height, m_borderThickness, m_cursorSideSize, m_limit, m_wlimit;
	sf::Color m_backgroundCol, m_borderCol;
	sf::Vector2f m_pos;
	sf::Vector2f m_cursor_pos1, m_cursor_pos2, m_cursor_pos3, m_globalStartPos;

	sf::VertexArray m_windowVertices;
	float m_totWidth, m_totHeight, m_theight;

	bool m_direction;

	PopUp() {}

	PopUp(float Width, float Height, float thickenss, float cursorSideSize, float limit, float wlimit, const sf::Color& backgroundCol,
		const sf::Color& borderCol) : m_width(Width), m_height(Height), m_borderThickness(thickenss), m_cursorSideSize(cursorSideSize), m_limit(limit),
		m_wlimit(wlimit), m_backgroundCol(backgroundCol), m_borderCol(borderCol), m_pos({ 0, 0 }), m_cursor_pos1({0, 0}), m_cursor_pos2({ 0, 0 }),
		m_cursor_pos3({ 0, 0 }), m_globalStartPos({0, 0}) {
		
		m_totHeight = m_height + 2 * m_borderThickness;
		m_totWidth = m_width + 2 * m_borderThickness;

		m_theight = rot30.x * m_cursorSideSize;

		m_windowVertices = sf::VertexArray(sf::Quads, 12);
		for (int i = 0; i < 12; i++) {
			if (i < 8) {
				m_windowVertices[i].color = borderCol;
			}
			else {
				m_windowVertices[i].color = backgroundCol;
			}
		}
	}

	void setCursorPos(sf::Vector2f pos, bool direction) {
		m_direction = direction;
		m_cursor_pos1 = pos;
		m_windowVertices[0].position = m_cursor_pos1;
		m_windowVertices[1].position = m_cursor_pos1;
		if (m_direction) {
			m_cursor_pos2 = m_cursorSideSize * rot30;
			m_cursor_pos3 = m_cursorSideSize * sf::Vector2f(rot30.x, -rot30.y);
		}
		else {
			m_cursor_pos2 = -m_cursorSideSize * sf::Vector2f(rot30.x, -rot30.y);
			m_cursor_pos3 = -m_cursorSideSize * rot30;
		}
		m_windowVertices[2].position = m_cursor_pos1 + m_cursor_pos2;
		m_windowVertices[3].position = m_cursor_pos1 + m_cursor_pos3;
		setBoxPos();
	}

	void setBoxPos() {
		m_pos = m_cursor_pos1 + m_cursor_pos3;

		sf::Vector2f lengthvec = sf::Vector2f(m_totWidth, 0);
		sf::Vector2f heightvec = sf::Vector2f(0, m_totHeight);

		m_pos.y = std::clamp(m_pos.y, 0.0f, m_limit - m_totHeight);

		if (m_direction) {
			m_globalStartPos = m_pos + m_borderThickness * sf::Vector2f(1, 1);
			m_windowVertices[4].position = m_pos;
			m_windowVertices[5].position = m_pos + lengthvec;
			m_windowVertices[6].position = m_pos + lengthvec + heightvec;
			m_windowVertices[7].position = m_pos + heightvec;

			m_windowVertices[8].position = m_globalStartPos;
			m_windowVertices[9].position = m_pos + lengthvec + m_borderThickness * sf::Vector2f(-1, 1);
			m_windowVertices[10].position = m_pos + lengthvec + heightvec + m_borderThickness * sf::Vector2f(-1, -1);
			m_windowVertices[11].position = m_pos + heightvec + m_borderThickness * sf::Vector2f(1, -1);
		}
		else {
			m_globalStartPos = m_pos - lengthvec - m_borderThickness * sf::Vector2f(-1, -1);
			m_windowVertices[4].position = m_pos;
			m_windowVertices[5].position = m_pos - lengthvec;
			m_windowVertices[6].position = m_pos - lengthvec + heightvec;
			m_windowVertices[7].position = m_pos + heightvec;

			m_windowVertices[8].position = m_pos - m_borderThickness * sf::Vector2f(1, -1);
			m_windowVertices[9].position = m_globalStartPos;
			m_windowVertices[10].position = m_pos - lengthvec + heightvec - m_borderThickness * sf::Vector2f(-1, 1);
			m_windowVertices[11].position = m_pos + heightvec - m_borderThickness * sf::Vector2f(1, 1);
		}
	}
	sf::Vector2f getGlobalPos(const sf::Vector2f& relative_pos) {
		return relative_pos + m_globalStartPos;
	}

	sf::Vector2f getNGlobalPos(const sf::Vector2f& normal_pos) {
		return m_globalStartPos + sf::Vector2f(m_width * normal_pos.x, m_height * normal_pos.y);
	}

	sf::Vector2f getRealtivePos(const sf::Vector2f& global_pos) {
		return global_pos - m_globalStartPos;
	}

	sf::Vector2f getNRelativePos(const sf::Vector2f& normal_pos) {
		return sf::Vector2f(m_width * normal_pos.x, m_height * normal_pos.y);
	}

	bool isOut(float value) {
		return value + m_totWidth + m_theight > m_wlimit;
	}

	float height() {
		return m_totHeight;
	}

	float width() {
		return m_totWidth;
	}

	void draw(sf::RenderWindow& win) {
		win.draw(m_windowVertices);
	}
};