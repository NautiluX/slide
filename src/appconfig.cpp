#include "appconfig.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDateTime>
#include <QFileInfo>
#include <QDir>

#include <iostream>

const std::string AppConfig::valid_aspects = "alpm"; // all, landscape, portait, monitor

ImageAspect parseAspectFromString(char aspect) {
  switch(aspect)
  {
    case 'l':
      return ImageAspect_Landscape;
      break;
    case 'p':
      return ImageAspect_Portrait;
      break;
    case 'm':
      return ImageAspect_Monitor;
      break;
    default:
    case 'a':
      return ImageAspect_Any;
      break;
  }
}

std::string ParseJSONString(QJsonObject jsonDoc, const char *key) {
  if(jsonDoc.contains(key) && jsonDoc[key].isString())
  {
   return jsonDoc[key].toString().toStdString();
  }
  return "";
}

void SetJSONBool(bool &value, QJsonObject jsonDoc, const char *key) {
  if(jsonDoc.contains(key) && jsonDoc[key].isBool())
  {
    value = jsonDoc[key].toBool();
  }
}

Config loadConfiguration(const std::string &configFilePath, const Config &currentConfig, bool debugMode) {
  QString jsonFile(configFilePath.c_str());
  QDir directory;
  if(!directory.exists(jsonFile))
  {
    return currentConfig; // nothing to load
  }

  Config userConfig = currentConfig;

  if(debugMode)
  {
    std::cout << "Found options file: " << jsonFile.toStdString() << std::endl;
  }

  QString val;
  QFile file;
  file.setFileName(jsonFile);
  file.open(QIODevice::ReadOnly | QIODevice::Text);
  val = file.readAll();
  file.close();
  QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
  QJsonObject jsonDoc = d.object();
  SetJSONBool(userConfig.baseDisplayOptions.fitAspectAxisToWindow, jsonDoc, "stretch");

  std::string aspectString = ParseJSONString(jsonDoc, "aspect");
  if(!aspectString.empty())
  {
    userConfig.baseDisplayOptions.onlyAspect = parseAspectFromString(aspectString[0]);
  }
  if(jsonDoc.contains("rotationSeconds") && jsonDoc["rotationSeconds"].isDouble())
  {
    userConfig.rotationSeconds = (int)jsonDoc["rotationSeconds"].toDouble();
  }

  if(jsonDoc.contains("opacity") && jsonDoc["opacity"].isDouble())
  {
    userConfig.backgroundOpacity = (int)jsonDoc["opacity"].toDouble();
  }

  if(jsonDoc.contains("blur") && jsonDoc["blur"].isDouble())
  {
    userConfig.blurRadius = (int)jsonDoc["blur"].toDouble();
  }

  userConfig.loadTime = QDateTime::currentDateTime();
  return userConfig;
}


QString getAppConfigFilePath(const std::string &configPath) {
  std::string userConfigFolder = "~/.config/slide/";
  std::string systemConfigFolder = "/etc/slide";
  QString baseConfigFilename("slide.options.json");
  
  QDir directory(userConfigFolder.c_str());
  QString jsonFile = "";
  if (!configPath.empty())
  { 
    directory.setPath(configPath.c_str());
    jsonFile = directory.filePath(baseConfigFilename);    
  }
   if(!directory.exists(jsonFile))
  { 
    directory.setPath(userConfigFolder.c_str());
    jsonFile = directory.filePath(baseConfigFilename);    
  }
  if(!directory.exists(jsonFile))
  {
    directory.setPath(systemConfigFolder.c_str());
    jsonFile = directory.filePath(baseConfigFilename);
  }

  if(directory.exists(jsonFile))
  {
    return jsonFile;
  }

  return "";
}

AppConfig loadAppConfiguration(const AppConfig &commandLineConfig) {
  QString jsonFile = getAppConfigFilePath(commandLineConfig.configPath);
  QDir directory;
  if(!directory.exists(jsonFile))
  {
    return commandLineConfig;
  }

  AppConfig loadedConfig = loadConfiguration(jsonFile.toStdString(), commandLineConfig, commandLineConfig.debugMode);
  
  QString val;
  QFile file;
  file.setFileName(jsonFile);
  file.open(QIODevice::ReadOnly | QIODevice::Text);
  val = file.readAll();
  file.close();
  QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
  QJsonObject jsonDoc = d.object();

  SetJSONBool(loadedConfig.recursive, jsonDoc, "recursive");
  SetJSONBool(loadedConfig.shuffle, jsonDoc, "shuffle");
  SetJSONBool(loadedConfig.sorted, jsonDoc, "sorted");
  SetJSONBool(loadedConfig.debugMode, jsonDoc, "debug");

  std::string overlayString = ParseJSONString(jsonDoc, "overlay");
  if(!overlayString.empty())
  {
    loadedConfig.overlay = overlayString;
  }
  std::string pathString = ParseJSONString(jsonDoc, "path");
  if(!pathString.empty())
  {
    loadedConfig.path = pathString;
  }
  std::string imageListString = ParseJSONString(jsonDoc, "imageList");
  if(!imageListString.empty())
  {
    loadedConfig.imageList = imageListString;
  }

  loadedConfig.configPath = commandLineConfig.configPath;
  return loadedConfig;
}

Config getConfigurationForFolder(const std::string &folderPath, const Config &currentConfig, bool debugMode) {
  QDir directory(folderPath.c_str());
  QString jsonFile = directory.filePath(QString("options.json"));
  if(directory.exists(jsonFile))
  {
      return loadConfiguration(jsonFile.toStdString(), currentConfig, debugMode );
  }
  return currentConfig;
}