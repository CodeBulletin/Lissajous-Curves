#include "curve.hpp"
#include "activeCurve.hpp"
#include "wave.h"

static const std::string name = "Lissajous Curve | Figures";
static int width;
static int height;


float phase1 = PI / 2.0f;
float phase2 = 0;
float freq1 = 1001;
float freq2 = 1000;
float amp1 = 100;
float amp2 = 100;
float traillength = 0.01;


float phasex = PI / 2.0f;
float phasey = 0;
float freqincx = 1;
float freqincy = 1;
float phaseincx = 0;
float phaseincy = 0;

int mode = 0;

void setup();
void draw(sf::RenderWindow&);
void draw2(sf::RenderWindow&);
void popUpWindow(sf::RenderWindow&);
void eventManager(sf::Event&, sf::RenderWindow&);
void keyboardeEvent(sf::Event&, sf::RenderWindow&);
void mouseEvent(sf::Event&);

float angle = 0;
float timeScale = 0.015625f;
int timestep = 4;
float timeunit;
int rows, cols;
int nx = 5, ny = 5, nr = -1;
int snx, sny, snr;
static bool need_cleaning = false;
std::vector<Circle> circles;
std::vector<std::vector<Curve>> curves;
bool samexy = false;
sf::VertexArray v;
bool simulationRunning = true;
int selectedaxis = 0;

bool popupmod = false;
bool pop2 = true;
bool highlighted = false;
sf::Vector2i selected;
sf::RectangleShape rs, rh;
PopUp popup;
Circle popup_circle1, popup_circle2;
Curve popup_curve;
float popupangle;

float constant1, constant2, constant3, constant4;

Wave w1, w2;
ActiveCurve c;
sf::VertexArray vOsc;
float angle2 = 0, angle2inc = 0;
int selectedwave = 0;
int fc = 1;
sf::Font font;

std::vector<sf::Text> texts;

void clear() {
	angle = 0;
	popupangle = 0;
	circles.clear();
	curves.clear();
}

void setpopup(int i, int j) {
	if (!pop2) {
		popup_circle1 = Circle(150, popup.getNGlobalPos({ 0.25f, 3.0f / 4.0f }), circles[i].getFreq(), circles[i].getPhase(),
			circles[i].getAxis(), popup.getNGlobalPos({1, 0}).x, 8, 3, circles[i].getCol());

		popup_circle2 = Circle(150, popup.getNGlobalPos({ 3.0f / 4.0f, 0.25f }), circles[j].getFreq(), circles[j].getPhase(),
			circles[j].getAxis(), popup.getNGlobalPos({ 0, 1 }).y, 8, 3, circles[j].getCol());
		popup_curve = Curve(8, 3, popup_circle1, popup_circle2, timeunit / 2.0f, { 0, 0 }, { 0, 0 });
	}
	else {
		popup_circle1 = Circle(300, popup.getNGlobalPos({ 0.5f, 0.5f }), circles[i].getFreq(), circles[i].getPhase(),
			circles[i].getAxis(), 0, 16, 6, circles[i].getCol());

		popup_circle2 = Circle(300, popup.getNGlobalPos({ 0.5f, 0.5f }), circles[j].getFreq(), circles[j].getPhase(),
			circles[j].getAxis(), 0, 16, 6, circles[j].getCol());
		popup_curve = Curve(16, 6, popup_circle1, popup_circle2, timeunit / 2.0f, { 0, 0 }, { 0, 0 });
	}
}

void setText(const sf::Color& col, int textSize, const std::string fontname, const std::vector<std::string> strings, const std::vector<sf::Vector2f> centers) {

	font.loadFromFile(fontname);

	for (int i = 0; i < strings.size(); i++) {
		sf::Text text;
		text.setFont(font);
		text.setCharacterSize(textSize);
		text.setFillColor(col);
		text.setString(strings[i]);
		sf::FloatRect textRect = text.getLocalBounds();
		text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);

		text.setPosition(centers[i]);
		texts.push_back(text);
	}
}

void makeOscWinGraph(float w, float h, float bunit, float munit, float sunit, sf::VertexArray& v, sf::Vector2f&& center, float size,
	sf::Color&& bunit_col, sf::Color&& munit_col, sf::Color&& sunit_col, sf::Color&& col) {
	sf::Vector2f min = -center;
	sf::Vector2f max = sf::Vector2f({w, h}) - center;
	sf::Vector2i bmin = sf::Vector2i(std::ceil(min.x / bunit), std::ceil(min.y / bunit));
	sf::Vector2i bmax = sf::Vector2i(std::floor(max.x / bunit) + 2, std::floor(max.y / bunit) + 2);
	sf::Vector2i mmax = sf::Vector2i(std::floor(bunit / munit) + 1, std::floor(bunit / munit) + 1);
	sf::Vector2i smax = sf::Vector2i(std::floor(munit / sunit), std::floor(munit / sunit));
	sf::Vertex vert1, vert2;

	makeLine({ 0, center.y }, { w, center.y }, col, 3, v);
	makeLine({ center.x, 0 }, { center.x, h }, col, 3, v);

	for (int i = bmin.x; i < bmax.x; i++) {
		sf::Vector2f cord = sf::Vector2f(i * bunit + center.x, center.y);
		if (i != 0 && cord.x >= 0 && cord.x <= w) {
			makeLine({ cord.x, 0 }, { cord.x, h }, bunit_col, 2, v);
		}
		for (int j = 0; j < mmax.x; j++) {
			sf::Vector2f cord2 = sf::Vector2f(-j * munit + cord.x, center.y);
			if (j != 0 && cord2.x >= 0 && cord2.x <= w) {
				makeLine({ cord2.x, 0 }, { cord2.x, h }, munit_col, 2, v);
				makeLine({ cord2.x, cord2.y + size * 2.0f}, { cord2.x, cord2.y - size * 2.0f }, col, 3, v);
			}
			for (int k = 0; k < smax.x; k++) {
				sf::Vector2f cord3 = sf::Vector2f(-k * sunit + cord2.x, center.y);
				if (k != 0 && cord3.x >= 0 && cord3.x <= w) {
					makeLine({ cord3.x, cord3.y + size }, { cord3.x, cord3.y - size }, sunit_col, 2, v);
				}
			}
		}
	}

	for (int i = bmin.y; i < bmax.y; i++) {
		sf::Vector2f cord = sf::Vector2f(center.x, i * bunit + center.y);
		if (i != 0 && cord.y >= 0 && cord.y <= h) {
			makeLine({ 0, cord.y }, { w, cord.y }, bunit_col, 2, v);
		}
		for (int j = 0; j < mmax.x; j++) {
			sf::Vector2f cord2 = sf::Vector2f(center.x, -j * munit + cord.y);
			if (j != 0 && cord2.y >= 0 && cord2.y <= h) {
				makeLine({ 0, cord2.y }, { w, cord2.y }, munit_col, 2, v);
				makeLine({ cord2.x + size * 2.0f, cord2.y }, { cord2.x - size * 2.0f, cord2.y }, col, 3, v);
			}
			for (int k = 0; k < smax.x; k++) {
				sf::Vector2f cord3 = sf::Vector2f(center.x, -k * sunit + cord2.y);
				if (k != 0 && cord3.y >= 0 && cord3.y <= h) {
					makeLine({ cord3.x + size, cord3.y }, { cord3.x - size, cord3.y }, sunit_col, 2, v);
				}
			}
		}
	}
}

int main() {
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	auto vid = sf::VideoMode::getDesktopMode();
	sf::RenderWindow window;
	window.create(vid, name, sf::Style::Fullscreen, settings);

	width = vid.width;
	height = vid.height;

	sf::Event event;
	window.setFramerateLimit(90);
	window.setVerticalSyncEnabled(true);

	vOsc = sf::VertexArray(sf::Quads, 0);
	popup = PopUp(700, 700, 20, 80, height, width, sf::Color::Black, { 50, 50, 50, 255 });
	sf::Color COLOR = sf::Color(0, 0, 0, 255);
	vOsc.append({ {0, 0}, COLOR });
	vOsc.append({ {0, (float)height}, COLOR });
	vOsc.append({ {(float)width - 600.0f, (float)height}, COLOR });
	vOsc.append({ {(float)width - 600.0f, 0}, COLOR });
	makeOscWinGraph(width - 600, height, 200, 100, 20, vOsc, { ((float)width - 600.0f) / 2.0f, ((float)height) / 2.0f }, 6,
		{ 255, 255, 255, 255 }, { 255, 255, 255, 75 }, { 255, 255, 255, 255 }, { 255, 255, 255, 255 });

	makeLine({ (float)width - 300.0f, 240.0f }, { (float)width - 300.0f, height - 40.0f }, sf::Color::White, 5, vOsc);
	makeLine({ (float)width - 270.0f, 240.0f }, { (float)width - 330.0f, 240.0f }, sf::Color::White, 5, vOsc);
	makeLine({ (float)width - 270.0f, height - 40.0f }, { (float)width - 330.0f, height - 40.0f }, sf::Color::White, 5, vOsc);
	setup();

	while (window.isOpen()) {
		while (window.pollEvent(event)) {
			eventManager(event, window);
		}
		if (mode == 0) {
			window.clear(sf::Color::Black);
			draw(window);
			if (popupmod && highlighted) {
				popUpWindow(window);
			}
		}
		else {
			window.clear(sf::Color(50, 50, 50, 255));
			draw2(window);
		}
		window.display();
	}
	return 0;
}

void eventManager(sf::Event& event, sf::RenderWindow& win) {
	switch (event.type)
	{
	case sf::Event::KeyPressed:
		keyboardeEvent(event, win);
		break;
	case sf::Event::MouseButtonPressed:
		mouseEvent(event);
		break;
	default:
		break;
	}
}

void keyboardeEvent(sf::Event& event, sf::RenderWindow& win) {
	if (event.key.code == sf::Keyboard::Num1) {
		mode = 0;
		clear();
		setup();
		return;
	} 
	else if (event.key.code == sf::Keyboard::Num2) {
		mode = 1;
		clear();
		setup();
		return;
	} 
	else if (event.key.code == sf::Keyboard::Space) {
		simulationRunning = !simulationRunning;
		return;
	}
	else if (event.key.code == sf::Keyboard::Escape) {
		win.close();
		exit(0);
	}

	if (mode == 0) {
		if (event.key.code == sf::Keyboard::Left) {
			if (!samexy) {
				float oldnx = nx;
				nx -= 1;
				nx = std::clamp(nx, 1, 31);
				if (oldnx != nx) {
					clear();
					setup();
				}
			}
			else {
				float oldnr = nr;
				nr -= 1;
				nr = std::clamp(nr, 1, 17);
				if (oldnr != nr) {
					clear();
					setup();
				}
			}
		}
		else if (event.key.code == sf::Keyboard::Right) {
			if (!samexy) {
				float oldnx = nx;
				nx += 1;
				nx = std::clamp(nx, 1, 31);
				if (oldnx != nx) {
					clear();
					setup();
				}
			}
			else {
				float oldnr = nr;
				nr += 1;
				nr = std::clamp(nr, 1, 17);
				if (oldnr != nr) {
					clear();
					setup();
				}
			}
		}
		else if (event.key.code == sf::Keyboard::Up) {
			if (!samexy) {
				float oldny = ny;
				ny += 1;
				ny = std::clamp(ny, 1, 17);
				if (oldny != ny) {
					clear();
					setup();
				}
			}
		}
		else if (event.key.code == sf::Keyboard::Down) {
			if (!samexy) {
				float oldny = ny;
				ny -= 1;
				ny = std::clamp(ny, 1, 17);
				if (oldny != ny) {
					clear();
					setup();
				}
			}
		}
		else if (event.key.code == sf::Keyboard::Q) {
			timestep -= 1;
			timestep = std::clamp(timestep, 1, 8);
		}
		else if (event.key.code == sf::Keyboard::E) {
			timestep += 1;
			timestep = std::clamp(timestep, 1, 8);
		}
		else if (event.key.code == sf::Keyboard::Tab) {
			popupmod = !popupmod;
			if (!popupmod) {
				highlighted = false;
			}
		}
		else if (event.key.code == sf::Keyboard::Tilde) {
			pop2 = !pop2;
			if (popupmod && highlighted) {
				if (!samexy) {
					int x = std::min(selected.x, ny - 1);
					int y = std::min(selected.y, nx - 1);
					curves[x][y].setBox(rh, popup);
					setpopup(x, y + rows);
				}
				else {
					int x = std::min(selected.x, nr - 1);
					int y = std::min(selected.y, nr - 1);
					curves[x][y].setBox(rh, popup);
					setpopup(x, y + rows);
				}
				popupangle = 0;
			}
		}
		else if (event.key.code == sf::Keyboard::X) {
			samexy = !samexy;
			if (samexy) {
				snx = nx;
				sny = ny;
				nr = nr == -1 ? std::min(snx, sny) : snr;
			}
			else {
				nx = snx;
				ny = sny;
				snr = nr;
			}
			clear();
			setup();
		}
		else if (event.key.code == sf::Keyboard::LBracket) {
			selectedaxis = (selectedaxis + 1) % 2;
			clear();
			setup();
		}
		else if (event.key.code == sf::Keyboard::O) {
			if (!selectedaxis) {
				phaseincx = std::clamp(phaseincx - PI / 180.0f, - 2 * PI, 2 * PI);
			}
			else {
				phaseincy = std::clamp(phaseincy - PI / 180.0f, -2 * PI, 2 * PI);
			}
			clear();
			setup();
		}
		else if (event.key.code == sf::Keyboard::P) {
			if (!selectedaxis) {
				phaseincx = std::clamp(phaseincx + PI / 180.0f, - 2 * PI, 2 * PI);
			}
			else {
				phaseincy = std::clamp(phaseincy + PI / 180.0f, - 2 * PI, 2 * PI);
			}
			clear();
			setup();
		}
		else if (event.key.code == sf::Keyboard::K) {
			if (!selectedaxis) {
				phasex = std::clamp(phasex - PI / 180.0f, -2 * PI, 2 * PI);
			}
			else {
				phasey = std::clamp(phasey - PI / 180.0f, -2 * PI, 2 * PI);
			}
			clear();
			setup();
		}
		else if (event.key.code == sf::Keyboard::L) {
			if (!selectedaxis) {
				phasex = std::clamp(phasex + PI / 180.0f, -2 * PI, 2 * PI);
			}
			else {
				phasey = std::clamp(phasey + PI / 180.0f, -2 * PI, 2 * PI);
			}
			clear();
			setup();
		}
	}
	else if (mode == 1) {
		if (event.key.code == sf::Keyboard::Tab) {
			selectedwave = (selectedwave + 1) % 2;
			setup();
		}
		else if (event.key.code == sf::Keyboard::Tilde) {
			fc = (fc + 1) % 4;
			setup();
		}
		else if (event.key.code == sf::Keyboard::Q) {
			if (!selectedwave) {
				freq1 = std::clamp(freq1 - std::powf(10, fc), 0.0f, 5000.0f);
			}
			else {
				freq2 = std::clamp(freq2 - std::powf(10, fc), 0.0f, 5000.0f);
			}
			setup();
		}
		else if (event.key.code == sf::Keyboard::W) {
			if (!selectedwave) {
				freq1 = std::clamp(freq1 + std::powf(10, fc), 0.0f, 5000.0f);
			}
			else {
				freq2 = std::clamp(freq2 + std::powf(10, fc), 0.0f, 5000.0f);
			}
			setup();
		}
		else if (event.key.code == sf::Keyboard::A) {
			if (!selectedwave) {
				amp1 = std::clamp(amp1 - 5, 0.0f, 100.0f);
			}
			else {
				amp2 = std::clamp(amp2 - 5, 0.0f, 100.0f);
			}
			setup();
		}
		else if (event.key.code == sf::Keyboard::S) {
			if (!selectedwave) {
				amp1 = std::clamp(amp1 + 5, 0.0f, 100.0f);
			}
			else {
				amp2 = std::clamp(amp2 + 5, 0.0f, 100.0f);
			}
			setup();
		}
		else if (event.key.code == sf::Keyboard::Z) {
			if (!selectedwave) {
				phase1 = std::clamp(phase1 - PI / 180.0f, -2 * PI, 2 * PI);
			}
			else {
				phase2 = std::clamp(phase2 - PI / 180.0f, -2 * PI, 2 * PI);
			}
			setup();
		}
		else if (event.key.code == sf::Keyboard::X) {
			if (!selectedwave) {
				phase1 = std::clamp(phase1 + PI / 180.0f, -2 * PI, 2 * PI);
			}
			else {
				phase2 = std::clamp(phase2 + PI / 180.0f, -2 * PI, 2 * PI);
			}
			setup();
		}
	}
}

void mouseEvent(sf::Event& event) {
	if (mode == 0) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			if (popupmod) {
				int i = int(constant1 * ((float)event.mouseButton.y + 1.5f * constant3) - 2.5f);
				int j = int(constant2 * ((float)event.mouseButton.x + 1.5f * constant4) - 2.5f);

				if (i > -1 && j > -1) {
					if (curves[i][j].selected({ (float)event.mouseButton.x, (float)event.mouseButton.y })) {
						highlighted = true;
						curves[i][j].setBox(rh, popup);

						int y = rows + j;

						setpopup(i, y);

						selected = { i, j };
						popupangle = 0;
					}
					else {
						highlighted = false;
					}
				}
				else {
					highlighted = false;
				}
			}
		}
	}
}

void setup() {
	if (mode == 0) {
		timeunit = std::min(timeScale, 1.0f) / 60.0f;
		if (samexy) {
			nx = nr;
			ny = nr;
		}
		float d;
		float heightr = samexy ? std::min(width, height) : height;
		float widthr = samexy ? heightr : width;
		constant4 = widthr / (nx + 1);
		constant3 = heightr / (ny + 1);
		d = std::min(constant4, constant3);
		rows = ny;
		cols = nx;
		float cradius = d / 2.0f * 0.8f;
		int n = rows + cols;
		circles = std::vector<Circle>(n);

		constant1 = (ny + 1.0f) / heightr;
		constant2 = (nx + 1.0f) / widthr;

		int rx = Random::Random() * 360;
		int ry = Random::Random() * 360;
		for (int i = 0; i < rows + cols; i++) {
			if (i < rows) {
				circles[i] = Circle(cradius, { d / 2.0f ,(i + 1.5f) * 1.0f / constant1 },
					(i * freqincy + 1.0f), (i * phaseincy + phasey), true, samexy ? (ny + 1) * d + 10 : width, 0.06f * cradius,
					0.04f * cradius, ColFunc::HSLtoRGB(int(ry + i * 360.0f / rows) % 360, 100, 50, 1.0f));
			}
			else {
				circles[i] = Circle(cradius, { (i - rows + 1.5f) * 1.0f / constant2 , d / 2.0f },
					((i - rows) * freqincx + 1.0f), -((i - rows) * phaseincx + phasex - PI / 2.0f), false, height, 0.06f * cradius, 0.04f * cradius,
					ColFunc::HSLtoRGB(int(rx + (i - rows) * 360.0f / cols) % 360, 100, 50, 1.0f));
			}
			circles[i].setTime(angle, timeunit);
		}

		curves = std::vector<std::vector<Curve>>(rows, std::vector<Curve>(cols));

		float slength = d / 2.0f * 0.9f;

		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				int y = rows + j;
				sf::Vector2f minbound = { (j + 1.5f) * 1.0f / constant2 - slength, (i + 1.5f) * 1.0f / constant1 - slength };
				sf::Vector2f maxbound = { (j + 1.5f) * 1.0f / constant2 + slength, (i + 1.5f) * 1.0f / constant1 + slength };
				curves[i][j] = Curve(0.06f * cradius, 0.04f * cradius, circles[i], circles[y], timeunit, minbound, maxbound);
				curves[i][j].setPoint(angle);
			}
		}
		v = sf::VertexArray(sf::Quads, 0);
		rs.setFillColor(sf::Color::White);
		rs.setOutlineColor(sf::Color::White);
		rs.setSize(width > height ? sf::Vector2f(1, height) : sf::Vector2f(width, 1));
		rs.setPosition(width < height ? sf::Vector2f(0, width + 10) : sf::Vector2f(height + 10, 0));
		rs.setOutlineThickness(3.0f);

		rh.setFillColor({ 0, 0, 0, 0 });
		rh.setOutlineColor(sf::Color::Red);
		rh.setOutlineThickness(3.0f);

		if (highlighted) {
			if (!samexy) {
				int x = std::min(selected.x, ny - 1);
				int y = std::min(selected.y, nx - 1);
				curves[x][y].setBox(rh, popup);
				setpopup(x, y + rows);
			}
			else {
				int x = std::min(selected.x, nr - 1);
				int y = std::min(selected.y, nr - 1);
				curves[x][y].setBox(rh, popup);
				setpopup(x, y + rows);
			}
		}
		float x = (float)width - float(width - height) / 2.0f;
		std::string ipX = "Increment in Initial Phase X: " + std::to_string(int(phaseincx * 180.0f / PI)) + "°";
		std::string ipY = "Increment in Initial Phase Y: " + std::to_string(int(phaseincy * 180.0f / PI)) + "°";
		std::string pX = "Initial Phase X: " + std::to_string(int(phasex * 180.0f / PI)) + "°";
		std::string pY = "Initial Phase Y: " + std::to_string(int(phasey * 180.0f / PI)) + "°";
		std::string cSelected = selectedaxis ? "currently selected axis: Y" : "currently selected axis: X";
		texts.clear();
		setText(sf::Color::White, 40, "PTSans-Regular.ttf",
			{ cSelected, ipX, ipY, pX, pY },
			{ {x, 100}, {x, 200}, {x, 250}, {x, 350}, {x, 400} }
		);
	}
	else {
		float tmax = 2 / std::max((selectedwave ? freq2 : freq1), 1.0f);
		angle2 = 0.0f;
		angle2inc = 1.0f / std::max((selectedwave ? freq2 : freq1), 1.0f);

		std::string freqX = "Frequency X: " + std::to_string(int(freq1)) + "Hz";
		std::string freqY = "Frequency Y: " + std::to_string(int(freq2)) + "Hz";
		std::stringstream stream;
		stream << std::fixed << std::setprecision(2) << (4 * amp1 / 100.0f);
		std::string ampX = "Amplitude X: " + stream.str() + "m";
		stream.str(std::string());
		stream << std::fixed << std::setprecision(2) << (4 * amp2 / 100.0f);
		std::string ampY = "Amplitude Y: " + stream.str() + "m";
		std::string phaseX = "Phase X: " + std::to_string(int(phase1 * 180.0f / PI)) + "°";
		std::string phaseY = "Phase Y: " + std::to_string(int(phase2 * 180.0f / PI)) + "°";
		std::string currentlySelected;

		std::string ifb = "change freq by: " + std::to_string(int(std::powf(10.0f, fc))) + "Hz";

		std::string it = "increase in time: " + std::to_string(angle2inc) + "s";

		if (selectedwave) {
			currentlySelected = "selected: Y";
		}
		else {
			currentlySelected = "selected: X";
		}

		texts.clear();
		setText(sf::Color::White, 25, "PTSans-Regular.ttf",
			{ freqX, freqY, ampX, ampY, phaseX, phaseY, currentlySelected, ifb, "X", "Y", "0m", std::to_string(tmax) + "m", it },
			{{(float)width - 450, 20}, {(float)width - 150, 20}, {(float)width - 450, 60}, {(float)width - 150, 60},
			{(float)width - 450, 100}, {(float)width - 150, 100}, {(float)width - 450, 140}, {(float)width - 150, 140},
			{(float)width - 450, 220}, {(float)width - 165, 220}, {(float)width - 300, 220}, {(float)width - 300, (float)height - 20},
			{(float)width - 300, 180}}
		);
		w1 = Wave(freq1, amp1, phase1, tmax, 8, { 0, (float)height - 300 }, { (float)width - 450.0f, 250 }, sf::Color::Magenta, sf::Color::Black, 100);
		w2 = Wave(freq2, amp2, phase2, tmax, 8, { 0, (float)height - 300 }, { (float)width - 150.0f, 250 }, sf::Color::Cyan, sf::Color::Black, 100);
		c = ActiveCurve(freq1, freq2, 4 * amp1, 4 * amp2, phase1, phase2, 1 / (10.0f * std::max(freq1, freq2)), traillength, 10,
			{ (float)(width - 600) / 2.0f, (float)height / 2.0f }, sf::Color::Red);
	}
}

void draw(sf::RenderWindow& win) {
	if (simulationRunning) {
		for (int i = 0; i < timestep; i++) {
			angle -= timeunit;
			for (auto& c : circles) {
				c.setTime(angle, timeunit);
			}
			std::for_each(std::execution::par_unseq, curves.begin(), curves.end(), [](auto& l) -> void {
				for (auto& c : l) {
					c.setPoint(angle);
				}
			});
		}
	}

	if (samexy) {
		win.draw(rs);
		for (auto& text : texts) {
			win.draw(text);
		}
	}

	if (highlighted) {
		win.draw(rh);
	}

	for (auto& c : circles) {
		c.draw(win);
		c.drawLine(v);
	}
	win.draw(v);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			curves[i][j].draw(win);
		}
	}
	v.clear();
}

void popUpWindow(sf::RenderWindow& win) {
	popup.draw(win);
	if (simulationRunning) {
		for (int i = 0; i < timestep + 1; i++) {
			popup_circle1.setTime(popupangle, timeunit / 2.0f);
			popup_circle2.setTime(popupangle, timeunit / 2.0f);
			popup_curve.setPoint(popupangle);
			popupangle += timeunit / 2.0f;
		}
	}

	if (!pop2) {
		popup_circle1.draw(win);
		popup_circle2.draw(win);
		popup_circle1.drawLine(v);
		popup_circle2.drawLine(v);
	}
	win.draw(v);
	popup_curve.draw(win);
	v.clear();
}

void draw2(sf::RenderWindow& win) {
	if (simulationRunning) {
		c.update();
		w1.updateVert(angle2);
		w2.updateVert(angle2);
		angle2 -= angle2inc / 1.0f;
	}

	win.draw(vOsc);
	w1.draw(win);
	w2.draw(win);
	c.draw(win);

	for (auto& t : texts) {
		win.draw(t);
	}
}
