#include "appconfig.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDateTime>
#include <QTime>
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

  if(jsonDoc.contains("times") && jsonDoc["times"].isArray())
  {
      QJsonArray jsonArray = jsonDoc["times"].toArray();
      foreach (const QJsonValue & value, jsonArray) 
      {
        QJsonObject obj = value.toObject();
        if(obj.contains("start") || obj.contains("end"))
        {
            DisplayTimeWindow window;
            if(obj.contains("start"))
            {
                window.startDisplay = QTime::fromString(obj["start"].toString());
            }
            if(obj.contains("end"))
            {
                window.endDisplay = QTime::fromString(obj["end"].toString());
            }
            userConfig.baseDisplayOptions.timeWindows.append(window);
        }
     }
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

QVector<PathEntry> parsePathEntry(QJsonObject &jsonMainDoc, bool baseRecursive, bool baseShuffle, bool baseSorted)
{
  QVector<PathEntry> pathEntries;
  
  if(jsonMainDoc.contains("scheduler") && jsonMainDoc["scheduler"].isArray()) 
  {
    QJsonArray jsonArray = jsonMainDoc["scheduler"].toArray();
    foreach (const QJsonValue & value, jsonArray) 
    {
      PathEntry entry;
      entry.recursive = baseRecursive;
      entry.sorted = baseSorted;
      entry.shuffle = baseShuffle;

      QJsonObject schedulerJson = value.toObject();

      SetJSONBool(entry.recursive, schedulerJson, "recursive");
      SetJSONBool(entry.shuffle, schedulerJson, "shuffle");
      SetJSONBool(entry.sorted, schedulerJson, "sorted");

      std::string pathString = ParseJSONString(schedulerJson, "path");
      if(!pathString.empty()) {
        entry.path = pathString;
      }
      std::string imageListString = ParseJSONString(schedulerJson, "imageList");
      if(!imageListString.empty()) {
        entry.imageList = imageListString;
      }
      std::string typeString = ParseJSONString(schedulerJson, "type");
      if(!pathString.empty()) {
        entry.type = typeString;
      }
      SetJSONBool(entry.exclusive, schedulerJson, "exclusive");

      if(schedulerJson.contains("times") && schedulerJson["times"].isArray())
      {
          QJsonArray jsonTimesArray = schedulerJson["times"].toArray();
          foreach (const QJsonValue & timesValue, jsonTimesArray) 
          {
            QJsonObject timesJson = timesValue.toObject();
            if(timesJson.contains("start") || timesJson.contains("end"))
            {
                DisplayTimeWindow window;
                if(timesJson.contains("start"))
                {
                    window.startDisplay = QTime::fromString(timesJson["start"].toString());
                }
                if(timesJson.contains("end"))
                {
                    window.endDisplay = QTime::fromString(timesJson["end"].toString());
                }
                entry.timeWindows.append(window);
            }
        }
      }
      pathEntries.append(entry);
    }
  }
  return pathEntries;
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

  bool baseRecursive, baseShuffle, baseSorted;
  SetJSONBool(baseRecursive, jsonDoc, "recursive");
  SetJSONBool(baseShuffle, jsonDoc, "shuffle");
  SetJSONBool(baseSorted, jsonDoc, "sorted");
  SetJSONBool(loadedConfig.debugMode, jsonDoc, "debug");

  std::string overlayString = ParseJSONString(jsonDoc, "overlay");
  if(!overlayString.empty())
  {
    loadedConfig.overlay = overlayString;
  }

  loadedConfig.paths = parsePathEntry(jsonDoc, baseRecursive, baseShuffle, baseSorted);
  if(loadedConfig.paths.count() <= 0)
  {
    PathEntry entry;
    entry.recursive = baseRecursive;
    entry.sorted = baseSorted;
    entry.shuffle = baseShuffle;
    std::string pathString = ParseJSONString(jsonDoc, "path");
    if(!pathString.empty())
    {
      entry.path = pathString;
    }
    std::string imageListString = ParseJSONString(jsonDoc, "imageList");
    if(!imageListString.empty())
    {
      entry.imageList = imageListString;
    }
    loadedConfig.paths.append(entry);
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