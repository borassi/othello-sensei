/*
 * Copyright 2023 Michele Borassi
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OTHELLOSENSEI_APP_SRC_MAIN_CPP_UTILS_ASSETS_H_
#define OTHELLOSENSEI_APP_SRC_MAIN_CPP_UTILS_ASSETS_H_

#if ANDROID
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <jni.h>
#endif

#include <assert.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>
#include <vector>

constexpr char kAssetFilepath[] = "app/src/main/assets/";

typedef long FileOffset;

class Asset {
 public:
  virtual ~Asset() {};
  virtual void Seek(FileOffset position) = 0;
  virtual FileOffset Tellg() = 0;
  virtual FileOffset GetLength() = 0;
  virtual void Read(char* buffer, FileOffset length) = 0;
  virtual void Write(const char* buffer, FileOffset length) = 0;
  virtual bool Exists() = 0;
  virtual void Remove() = 0;
  virtual void CreateIfNotExists() {
    if (!Exists()) {
      CreateOrReset();
    }
  }
  virtual void CreateOrReset() = 0;

  template<typename T>
  std::vector<T> ReadAll() {
    std::vector<T> result;
    FileOffset length = GetLength();
    Seek(0);
    result.resize(length * sizeof(char) / sizeof(T));
    Read((char*) &result[0], length);
    return result;
  }
};

class PCAsset : public Asset {
 public:
  PCAsset(const std::string& filepath) : filepath_(filepath) {
    file_ = std::fstream(filepath_, std::ios::binary | std::ios::out | std::ios::in);
  }

  ~PCAsset() {
    if (file_.is_open()) {
      file_.close();
    }
  }

  FileOffset GetLength() override {
    assert(file_.is_open());
    auto position = file_.tellg();
    file_.seekg(0, std::ios_base::end);
    auto result = file_.tellg();
    file_.seekg(position, std::ios_base::beg);
    return result;
  }

  void Seek(FileOffset position) override {
    assert(file_.is_open());
    file_.seekg(position, std::ios_base::beg);
    file_.seekp(position, std::ios_base::beg);
  }

  FileOffset Tellg() override {
    assert(file_.is_open());
    return file_.tellg();
  }

  void Read(char* buffer, FileOffset length) override {
    assert(file_.is_open());
    file_.read(buffer, length);
  }

  void Write(const char* buffer, FileOffset length) override {
    assert(file_.is_open());
    file_.write(buffer, length);
  }

  void Remove() override {
    assert(file_.is_open());
    file_.close();
    remove(filepath_.c_str());
  }

  bool Exists() override { return file_.is_open(); }

  void CreateOrReset() override {
    auto path = std::filesystem::path(filepath_);
    std::filesystem::create_directories(path.remove_filename());
    std::ofstream(filepath_, std::ios::out).close();
    file_ = std::fstream(filepath_, std::ios::binary | std::ios::out | std::ios::in);
    file_.seekg(0, std::ios::end);
    assert(file_.is_open());
  }

 private:
  std::fstream file_;
  std::string filepath_;
};

#if ANDROID
class AndroidAsset : public Asset {
 public:
  static void Setup(JNIEnv *env, jobject assetManager);

  AndroidAsset(const std::string& filepath) {
    asset_ = AAssetManager_open(manager_, filepath.c_str(), AASSET_MODE_STREAMING);
  }

  ~AndroidAsset() {
    AAsset_close(asset_);
  }

  FileOffset GetLength() final {
    return AAsset_getLength(asset_);
  }

  void Seek(FileOffset position) final {
    AAsset_seek64(asset_, position, std::ios_base::beg);
  }

  FileOffset Tellg() override {
    return AAsset_seek(asset_, 0, SEEK_CUR);
  }

  void Read(char* buffer, FileOffset length) final {
    AAsset_read(asset_, buffer, length);
  }

  bool Exists() override { return asset_ != nullptr; }

  void Write(const char* buffer, FileOffset length) override {
    throw std::invalid_argument("Cannot write a mobile file.");
  }

  void CreateOrReset() override {
    throw std::invalid_argument("Cannot create a mobile file.");
  }

  void Remove() override {
    throw std::invalid_argument("Cannot remove a mobile file.");
  }

 private:
  AAsset* asset_;
  static AAssetManager* manager_;
};

#endif  // ANDROID

std::unique_ptr<Asset> GetAsset(const std::string& filepath);

#endif //OTHELLOSENSEI_APP_SRC_MAIN_CPP_UTILS_ASSETS_H_
