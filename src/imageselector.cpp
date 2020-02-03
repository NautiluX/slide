#include "imageselector.h"
#include "mainwindow.h"
#include <QDirIterator>
#include <QTimer>
#include <QApplication>
#include <QDir>
#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <algorithm>    // std::shuffle
#include <random>       // std::default_random_engine

int current_image_shuffle = -1;
QStringList images;

ImageSelector::ImageSelector(std::string path, bool recursive, bool shuffle):
    path(path),
    recursive(recursive),
    shuffle(shuffle)
{
    srand (time(NULL));
}

std::string ImageSelector::getNextImage() const
{
    QDir directory(path.c_str());
    std:: string filename;
    try
    {
      if (recursive)
      {
        if (shuffle)
        {
            if (current_image_shuffle == -1 or current_image_shuffle == images.size())
            {
                images = listImagesRecursive();
		std::cout << "Shuffling " << images.size() << " images." << std::endl;
		std::srand(std::time(0));
		std::random_device rd;
		std::mt19937 randomizer(rd());
		std::shuffle(images.begin(), images.end(), randomizer);   
            }
            current_image_shuffle = current_image_shuffle + 1;
            filename = images.at(current_image_shuffle).toStdString();
        }
        else
        {
            unsigned int selectedImage = selectRandom(images);
            filename = images.at(selectedImage).toStdString();
        }
      }
      else
      {
        if (shuffle)
        {
	    if (current_image_shuffle == -1 or current_image_shuffle == images.size())
	    {
                images = directory.entryList(QStringList() << "*.jpg" << "*.JPG", QDir::Files);
		std::cout << "Shuffling " << images.size() << " images." << std::endl;
		std::srand(std::time(0));
		std::random_device rd;
		std::mt19937 randomizer(rd());
		std::shuffle(images.begin(), images.end(), randomizer);   
            }
            current_image_shuffle = current_image_shuffle + 1;
            filename = directory.filePath(images.at(current_image_shuffle)).toStdString();
        }
        else
        {
            unsigned int selectedImage = selectRandom(images);
            filename = directory.filePath(images.at(selectedImage)).toStdString();
        }
      }
    }
    catch(const std::string& err) 
    {
      std::cerr << "Error: " << err << std::endl;
    }
    std::cout << "updating image: " << filename << std::endl;
    return filename;
}

unsigned int ImageSelector::selectRandom(const QStringList& images) const
{
    std::cout << "images: " << images.size() << std::endl;
    if (images.size() == 0)
    {
      throw std::string("No jpg images found in folder " + path);
    }
    return rand() % images.size();
}


QStringList ImageSelector::listImagesRecursive() const
{
    QDirIterator it(QString(path.c_str()), QStringList() << "*.jpg" << "*.JPG", QDir::Files, QDirIterator::Subdirectories);
    QStringList files;
    while (it.hasNext())
    {
       files.append(it.next());
    }
    return files;
}
