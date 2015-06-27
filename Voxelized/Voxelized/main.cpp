// Voxelized.cpp : Defines the entry point for the console application.
//

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Audio/Music.hpp>
#include <iostream>
#include "Vox.h"
#include "Utility.h"
#include <math.h>
#include <random>

#define GRAVITY 2000 // Gravitational constant
#define DRAG 10 // The friction/traction/drag

#define RESTITUTION_COEFFICIENT 1 // The elasticity of the wall
#define INITIAL_DISTANCE 0.25 // The space between each 3D pixel

#define MOUSE_FORCE -2000000

#define CHUNK_NO 10 // Number of 3D Pixel chunks ( 10 is the MAX allowed, without causing a stack overflow )
#define CHUNK_SIZE 100000 // Number of 3D Pixels in a chunk

using namespace std;
using namespace sf;

int main()
{
    static float vertexCoords[CHUNK_NO][2 * CHUNK_SIZE]; // Static array of 3D pixel coordinates, OpenGL convention
    
    static unsigned char colors[CHUNK_NO][3 * CHUNK_SIZE]; // Static array of 3D pixel colors, OpenGL convention
    static std::mt19937 rng(100);
    sf::Music music;
    srand(time(NULL));
    
    int width = 800; // window definition
    int height = 600;
    int volume = 50;
    Window window(sf::VideoMode(width, height, 32), "Voxelized");
    window.setFramerateLimit(0);
    window.setVerticalSyncEnabled(false);
    int rndColor = 0;
    int redColor = 0;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    if (!music.openFromFile("atarians.ogg")) {
        // error...
    }
    
    music.setPosition(0, 1, 10);
    
    music.setVolume(volume);
    music.setLoop(true);
    vector<Vox3D> Pixel3DArray;
    int blobSize = CHUNK_NO*CHUNK_SIZE;
    
    for (int i(0); i < 1000; i++) {
        for (int j(0); j < 1000; j++) {
            Vox3D pixel;
            pixel.setPosition(Vector2f(j*INITIAL_DISTANCE, i*INITIAL_DISTANCE) + Vector2f(20, 20));
            pixel.setMass(random_integer(rng, 10) + 1);
            Pixel3DArray.push_back(pixel);
        }
    }
    music.play();
    Clock deltaTime;
    float dt = 0.00000001;
    Vector2f previousPosition;
    
    vector<Vector2f> wallPoints; /* Array for 4 walls  */
    
    wallPoints.push_back(Vector2f(10, 10));
    wallPoints.push_back(Vector2f(11, height - 10));
    
    wallPoints.push_back(Vector2f(11, height - 10));
    wallPoints.push_back(Vector2f(width - 10, height - 10));
    
    wallPoints.push_back(Vector2f(width - 10, height - 10));
    wallPoints.push_back(Vector2f(width - 11, 10));
    
    wallPoints.push_back(Vector2f(width - 11, 10));
    wallPoints.push_back(Vector2f(10, 10));
    
    /*float** vertexCoords = new float*[CHUNK_NB];
     for (int i = 0; i < CHUNK_NB; ++i)
     vertexCoords[i] = new float[2 * VERTEX_CHUNK];
     
     
     unsigned char** colors = new unsigned char*[CHUNK_NB];
     for (int i = 0; i < CHUNK_NB; ++i)
     colors[i] = new unsigned char[3 * VERTEX_CHUNK];*/
    
    glEnable(GL_POINT_SMOOTH); // allow to have rounded dots
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(1);
    bool Gravity = false;
    bool InverseGravity = false;
    bool LMB = false;
    bool RMB = false;
    bool mus = true;
    float zoom = 1;
    Vector2f camPos(-250, -250);
    
    while (window.isOpen()){
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        if (event.type == sf::Event::Resized || event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2u windowsize = window.getSize();
            width = windowsize.x;
            height = windowsize.y;
            glViewport(0, 0, width, height);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, width, height, 0, -1, 1);
            cout << "Size : " << width << "x" << height << endl;
        }
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        
        if (Mouse::isButtonPressed(Mouse::Left)) {
            cout << "Mouse -> X: " << camPos.x << " | Y: " << camPos.y << endl;
            LMB = true;
            //rndColor = random_integer(rng, 255);
        }
        else LMB = false;
        if (Mouse::isButtonPressed(Mouse::Right)) {
            cout << "Mouse -> X: " << camPos.x << " | Y: " << camPos.y << endl;
            RMB = true;
            //rndColor = random_integer(rng, 255);
        }
        else RMB = false;
        
        if (Keyboard::isKeyPressed(Keyboard::G)) {
            Gravity = !Gravity;
        }
        if (Keyboard::isKeyPressed(Keyboard::H)) {
            InverseGravity = !InverseGravity;
        }
        if (Keyboard::isKeyPressed(Keyboard::M)) {
            mus = !mus;
            if (!mus) music.stop();
            else music.play();
        }
        if (Keyboard::isKeyPressed(Keyboard::C)) {
            if (rndColor >= 255) rndColor = 0;
            else rndColor += 5;
        }
        if (Keyboard::isKeyPressed(Keyboard::V)) {
            if (redColor >= 255) redColor = 0;
            else redColor += 5;
        }
        if (Keyboard::isKeyPressed(Keyboard::Add)) {
            volume += 5;
        }
        if (Keyboard::isKeyPressed(Keyboard::Subtract)) {
            volume -= 5;
        }
        if (Keyboard::isKeyPressed(Keyboard::X) || Keyboard::isKeyPressed(Keyboard::W))
            zoom += 1 * dt*zoom;
        else if (Keyboard::isKeyPressed(Keyboard::Z) || Keyboard::isKeyPressed(Keyboard::Y) || Keyboard::isKeyPressed(Keyboard::S))
            zoom -= 1 * dt*zoom;
        if (Keyboard::isKeyPressed(Keyboard::Left)) {
            cout << "Camera -> X: " << camPos.x << " | Y: " << camPos.y << endl;
            camPos.x += 500 * dt / zoom;
        }
        
        else if (Keyboard::isKeyPressed(Keyboard::Right)) {
            camPos.x -= 500 * dt / zoom;
            cout << "Camera -> X: " << camPos.x << " | Y: " << camPos.y << endl;
        }
        
        if (Keyboard::isKeyPressed(Keyboard::Up)) {
            camPos.y += 500 * dt / zoom;
            cout << "Camera -> X: " << camPos.x << " | Y: " << camPos.y << endl;
        }
        
        else if (Keyboard::isKeyPressed(Keyboard::Down)) {
            camPos.y -= 500 * dt / zoom;
            cout << "Camera -> X: " << camPos.x << " | Y: " << camPos.y << endl;
        }
        if (Keyboard::isKeyPressed(Keyboard::R)) {
            Pixel3DArray.clear();
            for (int i(0); i < 1000; i++) {
                for (int j(0); j < 1000; j++) {
                    
                    Vox3D pixel;
                    pixel.setPosition(Vector2f(j*INITIAL_DISTANCE, i*INITIAL_DISTANCE) + Vector2f(20, 20));
                    pixel.setMass(10);
                    Pixel3DArray.push_back(pixel);
                }
            }
        }
        Vector2f mousePos = (Vector2f(Mouse::getPosition(window).x, Mouse::getPosition(window).y) / zoom - Vector2f(width / 2, height / 2) / zoom - camPos);
        music.setVolume(volume);
        for (int i(0); i < blobSize; i++) { /* For each frame, we update the pixel's position, color and collision -> For loop executed 1 bilion times each frame */
            
            if (Gravity) Pixel3DArray[i].addForce(Vector2f(0, GRAVITY));
            if (InverseGravity) Pixel3DArray[i].addForce(Vector2f(0, -GRAVITY));
            
            Pixel3DArray[i].addForce((Vector2f(mousePos - Pixel3DArray[i].getPosition()) * (float)(LMB * 500000 / pow(Distance(mousePos, Pixel3DArray[i].getPosition()) + 10, 2)))); // if the user clicks we add a force proportionnal to the inverse of the distance squared
            Pixel3DArray[i].addForce(-Pixel3DArray[i].getSpeed()*(float)DRAG);
            Pixel3DArray[i].addForce(-(Vector2f(mousePos - Pixel3DArray[i].getPosition()) * (float)(RMB * 50000 / pow(Distance(mousePos, Pixel3DArray[i].getPosition()) + 10, 2)))); // if the user clicks we add a force proportionnal to the inverse of the distance squared
            
            previousPosition = Pixel3DArray[i].getPosition(); /* For Collision detection */
            Pixel3DArray[i].updatePosition(dt);
            
            Pixel3DArray[i].clearForce();
            
            for (int j(0); j < int(wallPoints.size()); j += 2) { /* For Collision detection */
                if (determinant(wallPoints[j + 1] - wallPoints[j], wallPoints[j + 1] - Pixel3DArray[i].getPosition())*determinant(wallPoints[j + 1] - wallPoints[j], wallPoints[j + 1] - previousPosition)<0) {
                    Vector2f v = wallPoints[j+1] - wallPoints[j] ;
                    Vector2f n = Vector2f(-v.y,v.x) ;
                    n/=Norm(n) ;
                    Pixel3DArray[i].setPosition(previousPosition);
                    //float j = -(1+RESTITUTION_COEFFICIENT)*dotProduct(particles[i].getSpeed(), n); // we compute the speed after bouncing off
                    float j = -(1 + RESTITUTION_COEFFICIENT)*vectorValue(Pixel3DArray[i].getSpeed());
                    Pixel3DArray[i].setSpeed(Pixel3DArray[i].getSpeed() + j*n);
                }
            }
            
        }
        
        for (int j(0); j < CHUNK_NO; j++) { /* Convert the Vector2f positions to the OpenGL's static arrays of floats */
            
            for (int i(0); i < CHUNK_SIZE; i++) {
                
                colors[j][3 * i] = redColor;
                colors[j][3 * i + 1] = clamp(255 - Norm(Pixel3DArray[i + j*CHUNK_SIZE].getSpeed()), 0, 255); /* Change the pixel's color according to it's speed */
                colors[j][3 * i + 2] = rndColor;
                
                vertexCoords[j][2 * i] = Pixel3DArray[i + j*CHUNK_SIZE].getPosition().x;
                vertexCoords[j][2 * i + 1] = Pixel3DArray[i + j*CHUNK_SIZE].getPosition().y;
            }
        }
        
        glPushMatrix();
        
        glTranslatef(width / 2.f, height / 2.f, 0); 
        glScaled(zoom, zoom, zoom);
        
        glTranslated(camPos.x, camPos.y, 0);
        
        glEnableClientState(GL_VERTEX_ARRAY); 
        glEnableClientState(GL_COLOR_ARRAY);
        
        for (int j(0); j < CHUNK_NO; j++) {
            
            glVertexPointer(2, GL_FLOAT, 0, vertexCoords[j]);
            glColorPointer(3, GL_UNSIGNED_BYTE, 0, colors[j]);
            glDrawArrays(GL_POINTS, 0, CHUNK_SIZE);
        }
        
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        
        glPopMatrix();
        
        glFlush();
        window.display(); 
        dt = deltaTime.restart().asSeconds();
    }
    /*for (int i = 0; i < CHUNK_NB; ++i)
     delete[] vertexCoords[i];
     delete[] vertexCoords;
     for (int i = 0; i < CHUNK_NB; ++i)
     delete[] colors[i];
     delete[] colors;*/
    
    return 0;
}
