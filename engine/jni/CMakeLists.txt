# Copyright 2023 Michele Borassi
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#add_library(
#        jni
#        SHARED
#        jni.cpp
#)
#
#target_link_libraries( # Specifies the target library.
#        jni
#        LINK_PUBLIC
#        book
#        evaluator_alpha_beta
#        evaluator_derivative
#        files
#        hash_map
#        pattern_evaluator
#        thor
#        tree_node
#        ${JNI_LIBRARIES}
#        -fPIC
#)