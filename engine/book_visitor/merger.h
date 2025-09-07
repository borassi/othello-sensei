/*
 * Copyright 2025 Michele Borassi
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

#include "visitor.h"
#include "../book/book.h"

template<int source_version = kBookVersion, int target_version = kBookVersion>
class BookVisitorMerge : public BookVisitorWithProgress<source_version> {
 public:
  using BookSource = ::Book<1>;
  using BookTarget = ::Book<1>;
  using Base = ::BookVisitorWithProgress<source_version>;

  BookVisitorMerge(
      const BookSource& source,
      BookTarget& destination,
      void (*leaf_func)(Node*) = nullptr,
      void (*internal_func)(Node*) = nullptr) :
      Base(source),
      destination_(destination),
      leaf_func_(leaf_func),
      internal_func_(internal_func) {
    // This avoids adding a lot of roots and removing them afterwards (to avoid
    // memory problems).
    for (const auto& root : ::BookVisitor<source_version>::book_.Roots()) {
      Board unique = root.Unique();
      if (!destination_.Get(unique)) {
        destination_.roots_.insert(unique);
      }
    }
  }

  virtual void VisitAll() override {
    ::BookVisitor<source_version>::VisitAll();
    destination_.Commit();
  }

 protected:
  BookTarget& destination_;
  void (*leaf_func_)(Node*);
  void (*internal_func_)(Node*);
  std::unordered_set<Board> visited_boards_;

  void FirstVisit(Node& source, void (*function)(Node*)) {
    Board board = source.ToBoard();
    auto* destination_node = destination_.Mutable(source.ToBoard());
    if (destination_node) {
      if (visited_boards_.find(board) == visited_boards_.end()) {
        destination_node->AddDescendants(source.GetNVisited());
        // The min / max is computed inside these functions.
        destination_node->SetLower(source.Lower());
        destination_node->SetUpper(source.Upper());
      }
    } else {
      if (function) {
        function(&source);
      }
      // NOTE: This makes the added node always a leaf.
      destination_.AddNoRootsUpdate(source);
    }
    visited_boards_.insert(board);
  }

  virtual void VisitLeaf(Node& node) override {
    FirstVisit(node, leaf_func_);
  }

  virtual bool PreVisitInternalNode(Node& node) override {
    FirstVisit(node, internal_func_);
    return true;
  }

  virtual void PostVisitInternalNode(Node& node) override {
    // Node is the source node
    auto* destination_node = destination_.Mutable(node.ToBoard());
    assert(destination_node);
    if (destination_node->IsLeaf()) {
      destination_.AddChildren(destination_node->ToBoard(), {});
    } else {
      destination_node->UpdateFather();
    }
  }
};

template<int version = kBookVersion>
Book<version> RemoveDescendants(const Book<version>& book, const std::string& filepath) {
  Book<version> book_no_descendants(filepath);
  assert(book_no_descendants.Size() == 0);

  BookVisitorMerge<version, version>(
      book,
      book_no_descendants,
      [](Node* node) { node->ResetDescendants(); },
      [](Node* node) { node->ResetDescendants(); }).VisitAll();
  return book_no_descendants;
}