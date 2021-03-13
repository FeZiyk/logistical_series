#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <random>
#include <iostream>
#include <functional>
#include <chrono>

unsigned int nb_threads = 4;
unsigned int nb_points = 1000000;
unsigned int width = 1920;
unsigned int height = 1080;
float start = 0.7;
float end = 4;
float w_ratio = 0;

sf::VertexArray vertices(sf::Points);
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);

void calc(double pitch, unsigned int offset, unsigned int s, unsigned int e) {
    for (unsigned int i = s; i < e; i++) {
        double m = i * pitch;
    
        double x = dis(gen);
        for (unsigned int i = 0; i < 1000; i++)
            x = (m * x) * (1 - x);

        vertices[i- offset].position = sf::Vector2f((i - offset)* w_ratio, height - (x * height));
        vertices[i- offset].color = sf::Color::Red;
    }
}

int main()
{
    std::cout << "---- LOGISTIC MAP ----" << std::endl;

    std::cout << "Threads (default 4) :" << std::endl;
    if (std::cin.peek() == '\n') {
        nb_threads = 4;
    } else if (!(std::cin >> nb_threads)) {
        std::cout << "Invalid input.\n";
    }
    std::cin.ignore();

    std::cout << "Points (default 1m) :" << std::endl;
    if (std::cin.peek() == '\n') {
        nb_points = 1000000;
    }
    else if (!(std::cin >> nb_points)) {
        std::cout << "Invalid input.\n";
    }
    std::cin.ignore();

    std::cout << "Image width (default 1920) :" << std::endl;
    if (std::cin.peek() == '\n') {
        width = 1920;
    }
    else if (!(std::cin >> width)) {
        std::cout << "Invalid input.\n";
    }
    std::cin.ignore();

    std::cout << "Image height (default 1080) :" << std::endl;
    if (std::cin.peek() == '\n') {
        height = 1080;
    }
    else if (!(std::cin >> height)) {
        std::cout << "Invalid input.\n";
    }
    std::cin.ignore();

    std::cout << "Start[0,4] (default 0.7) :" << std::endl;
    if (std::cin.peek() == '\n') {
        start = 0.7;
    }
    else if (!(std::cin >> start)) {
        std::cout << "Invalid input.\n";
    }
    std::cin.ignore();

    std::cout << "End[0,4] (default 4):" << std::endl;
    if (std::cin.peek() == '\n') {
        end = 4.0;
    }
    else if (!(std::cin >> end)) {
        std::cout << "Invalid input.\n";
    }
    std::cin.ignore();

    std::cout << "---- START COMPUTING ----" << std::endl;
    auto t1 = std::chrono::high_resolution_clock::now();

    sf::RenderTexture texture;
    if (!texture.create(width, height))
        return -1;

    vertices.resize(nb_points);
    w_ratio = static_cast<float>(width) / static_cast<float>(nb_points);
    double pitch = (end - start) / nb_points;
    unsigned int offset = start / pitch;
    unsigned int workload = nb_points / nb_threads;
    
    std::vector<sf::Thread*> threads(nb_threads);
    for (int i = 0; i < threads.size(); i++)
    {
        sf::Thread* thread = new sf::Thread(std::bind(&calc, pitch, offset, i * workload + offset, (i + 1) * workload + offset));
        thread->launch();

        threads[i] = thread;
    }

    for (int i = 0; i < threads.size(); i++)
    {
        threads[i]->wait();
    }
    
    texture.clear();
    texture.draw(vertices);
    texture.display();
    texture.getTexture().copyToImage().saveToFile("logistic.png");

    for (int i = 0; i < threads.size(); i++)
    {
        delete threads[i];
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    std::cout << "---- DONE! (Took " << duration << "ms) ----" << std::endl;
    std::cout << "Press any key to exit." << std::endl;
    std::cin.get();

    return 0;
}