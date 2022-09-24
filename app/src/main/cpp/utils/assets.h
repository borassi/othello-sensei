/*
 * Copyright 2022 Google LLC
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
#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>
#include <unistd.h>

constexpr char kAssetFilepath[] = "app/src/main/assets/";

typedef long FileSize;

class Asset {
 public:
  virtual ~Asset() {};
  virtual void Seek(FileSize position) = 0;
  virtual FileSize GetLength() = 0;
  virtual void Read(char* buffer, FileSize length) = 0;
  std::vector<char> ReadAll() {
    std::vector<char> result;
    FileSize length = GetLength();
    Seek(0);
    result.resize(length);
    Read(&result[0], length);
    return result;
  }
};

class PCAsset : public Asset {
 public:
  PCAsset(const std::string& filepath) {
    file_.open(kAssetFilepath + filepath, std::ios_base::binary | std::ios_base::in);
    assert (file_.is_open());
  }

  ~PCAsset() {
    file_.close();
  }

  FileSize GetLength() override {
    file_.seekg(0, std::ios_base::end);
    return file_.tellg();
  }

  void Seek(FileSize position) override {
    file_.seekg(position, std::ios_base::beg);
  }

  void Read(char* buffer, FileSize length) override {
    file_.read(buffer, length);
  }

 private:
  std::ifstream file_;
};

#if ANDROID
class AndroidAsset : public Asset {
 public:
  static void Setup(JNIEnv *env, jobject assetManager);

  AndroidAsset(const std::string& filepath);

  ~AndroidAsset() {
    AAsset_close(asset_);
  }

  FileSize GetLength() final {
    return AAsset_getLength(asset_);
  }

  void Seek(FileSize position) final {
    AAsset_seek64(asset_, position, std::ios_base::beg);
  }

  void Read(char* buffer, FileSize length) final {
    AAsset_read(asset_, buffer, length);
  }

 private:
  AAsset* asset_;
  static AAssetManager* manager_;
};

#endif  // ANDROID

std::unique_ptr<Asset> GetAsset(const std::string& filepath);

#endif //OTHELLOSENSEI_APP_SRC_MAIN_CPP_UTILS_ASSETS_H_
