#include <SFML/Graphics.hpp>
#include <optional>
#include <cmath>
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>

constexpr float PI = 3.14159265f;
constexpr float g = 9.81f;          // м/с²

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 900, 700 }), sf::String(L"Математичний маятник"));
    window.setFramerateLimit(60);

    //  Параметри моделі
    float L = 2.0f;                    // довжина підвісу (м)
    float theta = 45.0f * PI / 180.f;  // початковий кут (рад)
    float omega = 0.0f;                // кутова швидкість
    bool isPaused = false;

    // Точка підвісу
    const sf::Vector2f pivot(450.f, 80.f);
    const float scale = 150.f;         // масштаб (пікселів на метр)

    // Графічні об'єкти
    sf::CircleShape bob(20.f);
    bob.setFillColor(sf::Color::Red);
    bob.setOrigin({ 20.f, 20.f });

    sf::VertexArray rod(sf::PrimitiveType::Lines, 2);

    // Завантаження шрифту 
    sf::Font font;
    if (!font.openFromFile("../../resources/fonts/arial.ttf") &&
        !font.openFromFile("../resources/fonts/arial.ttf") &&
        !font.openFromFile("resources/fonts/arial.ttf"))
    {
        return 1;
    }

    sf::Text info(font, "", 18);
    info.setFillColor(sf::Color::White);
    info.setPosition({ 20.f, 20.f });

    sf::Clock clock;
    float time = 0.f;
    int zeroCrossings = 0;
    float lastTheta = theta;
    float measuredPeriod = 0.f;
    float periodStartTime = 0.f;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;   // обмеження стрибка часу

        // 1. Обробка подій
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Up)    L = std::min(L + 0.1f, 4.0f);
                if (key->code == sf::Keyboard::Key::Down)  L = std::max(L - 0.1f, 0.5f);
                if (key->code == sf::Keyboard::Key::Left)  theta = std::max(theta - 5.f * PI / 180.f, -PI / 2);
                if (key->code == sf::Keyboard::Key::Right) theta = std::min(theta + 5.f * PI / 180.f, PI / 2);
                if (key->code == sf::Keyboard::Key::Space) isPaused = !isPaused;
                if (key->code == sf::Keyboard::Key::R) {   // Скидання
                    theta = 45.0f * PI / 180.f;
                    omega = 0.f;
                    time = 0.f;
                    zeroCrossings = 0;
                    measuredPeriod = 0.f;
                    periodStartTime = 0.f;
                    lastTheta = theta;
                }
            }
        }

        //  2. Фізика (метод Ейлера-Кромера)
        if (!isPaused)
        {
            omega -= (g / L) * std::sin(theta) * dt;
            theta += omega * dt;

            time += dt;

            // Вимірювання періоду коливань
            if (lastTheta * theta < 0.f) {
                zeroCrossings++;
                if (zeroCrossings % 2 == 0) {
                    measuredPeriod = time - periodStartTime;
                    periodStartTime = time;
                }
            }
            lastTheta = theta;
        }

        // 3. Відображення 
        float x = pivot.x + L * scale * std::sin(theta);
        float y = pivot.y + L * scale * std::cos(theta);

        bob.setPosition({ x, y });

        rod[0].position = pivot;
        rod[0].color = sf::Color::White;
        rod[1].position = { x, y };
        rod[1].color = sf::Color::White;

        // Теоретичний період коливань (для малих кутів)
        float T_theory = 2.f * PI * std::sqrt(L / g);

        std::wostringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << L"Довжина L = " << L << L" м\n"
            << L"Кут (theta) = " << (theta * 180.f / PI) << L" deg\n"
            << L"Швидкість (w) = " << omega << L" рад/с\n"
            << L"Час = " << time << L" с\n"
            << L"T теор. = " << T_theory << L" с\n"
            << L"T вимір. = " << measuredPeriod << L" с\n\n"
            << L"Керування:\n"
            << L"Up / Down - довжина L\n"
            << L"Left / Right - початковий кут\n"
            << L"Space - пауза\n"
            << L"R - перезапуск";

        info.setString(ss.str());

        window.clear(sf::Color(30, 30, 40));
        window.draw(rod);
        window.draw(bob);
        window.draw(info);
        window.display();
    }

    return 0;
}
