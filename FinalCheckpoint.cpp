// FinalCheckpoint.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;
using namespace std;

void blurImage(Image& image, int blurLevel) {
    if (blurLevel < 1) return;

    Image original = image;
    Vector2u size = image.getSize();
    for (unsigned int y = 0; y < size.y; ++y) {
        for (unsigned int x = 0; x < size.x; ++x) {
            int r = 0, g = 0, b = 0;
            int count = 0;
            for (int ky = -blurLevel; ky <= blurLevel; ++ky) {
                for (int kx = -blurLevel; kx <= blurLevel; ++kx) {
                    int nx = x + kx;
                    int ny = y + ky;
                    if (nx >= 0 && nx < size.x && ny >= 0 && ny < size.y) {
                        Color c = original.getPixel(nx, ny);
                        r += c.r;
                        g += c.g;
                        b += c.b;
                        ++count;
                    }
                }
            }
            image.setPixel(x, y, Color(r / count, g / count, b / count));
        }
    }
}

int main() {
    string imageFile = "D://spring 2024/programming/FinalCheckpoint/images/calebfurst.jpg";
    Texture texture;
    if (!texture.loadFromFile(imageFile)) {
        cout << "Couldn't load image" << endl;
        return -1;
    }

    Image image = texture.copyToImage();
    blurImage(image, 10);

    texture.loadFromImage(image);
    Sprite sprite;
    sprite.setTexture(texture);

    RenderWindow window(VideoMode(image.getSize().x, image.getSize().y), "Image Guessing Game");
    string guess;
    int blurLevel = 10;
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }

        if (blurLevel > 0) {
            cout << "Guess the Boilermaker (first and last name, please): ";
            cin >> guess;
            if (guess == "caleb furst") {
                cout << "Correct!" << endl;
                break;
            }
            else {
                blurLevel--;
                blurImage(image, blurLevel);
                texture.loadFromImage(image);
                sprite.setTexture(texture);
            }
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}