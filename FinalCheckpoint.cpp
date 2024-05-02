// FinalCheckpoint.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>

using namespace sf;
using namespace std;

void blurImage(const Image& sourceImage, Image& destImage, int radius) {
    if (radius < 1) {
        destImage = sourceImage;
        return;
    }
    Vector2u size = sourceImage.getSize();
    vector<Uint8> tempBuffer(size.x * size.y * 4);
    const Uint8* originalPixels = sourceImage.getPixelsPtr();

    int diameter = radius * 2 + 1;
    int elements = diameter * diameter;

    for (unsigned int y = 0; y < size.y; ++y) {
        for (unsigned int x = 0; x < size.x; ++x) {
            int sumR = 0, sumG = 0, sumB = 0;
            for (int dy = -radius; dy <= radius; ++dy) {
                for (int dx = -radius; dx <= radius; ++dx) {
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < size.x && ny >= 0 && ny < size.y) {
                        const Uint8* p = originalPixels + ((ny * size.x + nx) * 4);
                        sumR += p[0];
                        sumG += p[1];
                        sumB += p[2];
                    }
                }
            }
            Uint8* p = &tempBuffer[(y * size.x + x) * 4];
            p[0] = sumR / elements;
            p[1] = sumG / elements;
            p[2] = sumB / elements;
            p[3] = 255; // Fully opaque
        }
    }
    destImage.create(size.x, size.y, &tempBuffer[0]);
}

void handleInput(atomic<int>& blurRadius, const string& correctAnswer, atomic<bool>& gameOver) {
    string guess;
    while (!gameOver) {
        cout << "Guess the Boilermaker: ";
        getline(cin, guess);  // Use getline to read the full line including spaces
        if (guess == correctAnswer) {
            cout << "Congratulations! You guessed right!" << endl;
            gameOver = true;
        }
        else {
            cout << "Wrong guess. Try again!" << endl;
            blurRadius = max(blurRadius.load() - 5, 1);
        }
    }
}

int main() {
    string imageFile = "D://spring 2024/programming/FinalCheckpoint/images/calebfurst.jpg";
    string correctAnswer = "caleb furst";
    atomic<int> blurRadius(15);
    atomic<bool> gameOver(false);

    Image originalImage;
    if (!originalImage.loadFromFile(imageFile)) {
        cout << "Couldn't load image" << endl;
        return -1;
    }

    Image blurredImage = originalImage;
    blurImage(originalImage, blurredImage, blurRadius);

    Texture texture;
    texture.loadFromImage(blurredImage);
    Sprite sprite;
    sprite.setTexture(texture);

    RenderWindow window(VideoMode(originalImage.getSize().x, originalImage.getSize().y), "Who's that Boilermaker?!");

    thread inputThread(handleInput, ref(blurRadius), cref(correctAnswer), ref(gameOver));

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();
        }

        blurImage(originalImage, blurredImage, blurRadius);
        texture.loadFromImage(blurredImage);
        sprite.setTexture(texture);

        window.clear();
        window.draw(sprite);
        window.display();
    }

    gameOver = true;
    inputThread.join(); // Make sure to join the thread on exit

    return 0;
}