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

#include "assets.h"

#if ANDROID
#include <android/log.h>
#endif

#if ANDROID
AAssetManager* AndroidAsset::manager_ = nullptr;

// static
void AndroidAsset::Setup(JNIEnv *env, jobject assetManager) {
  manager_ = AAssetManager_fromJava(env, assetManager);
}

AndroidAsset::AndroidAsset(const std::string& filepath) {
  asset_ = AAssetManager_open(manager_, filepath.c_str(), AASSET_MODE_STREAMING);
  assert (asset_);
}
#endif

std::unique_ptr<Asset> GetAsset(const std::string& filepath) {
#if ANDROID
  return std::make_unique<AndroidAsset>(filepath);
#else
  return std::make_unique<PCAsset>(filepath);
#endif
}