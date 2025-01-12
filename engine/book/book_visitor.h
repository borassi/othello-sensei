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

#include<string.h>

#include "book.h"

enum NodeType {
  LEAF = 0,
  FIRST_VISIT = 1,
  LAST_VISIT = 2,
};

template<int version = kBookVersion>
class BookVisitor {
 public:
  typedef Book<version> Book;
  typedef typename Book::BookNode BookNode;

  BookVisitor(const Book& book) :
      book_(book),
      roots_(book_.Roots().begin(), book_.Roots().end()) {}

  virtual void Visit() {
    StartVisit();
    for (auto& root : roots_) {
      VisitInternal(root);
    }
    EndVisit();
  }

  virtual void Visit(const Board& b) {
    StartVisit();
    VisitInternal(b);
    EndVisit();
  }

 protected:
  const Book& book_;
  const std::vector<Board> roots_;

  virtual void StartVisit() {};
  virtual void VisitLeaf(Node& node) {};
  virtual void PreVisitInternalNode(Node& node) {};
  virtual void PostVisitInternalNode(Node& node) {};
  virtual bool ShouldVisit(Node& node) { return true; }
  virtual void EndVisit() {};

  virtual void VisitInternal(const Board& board) {
    std::unique_ptr<Node> node = book_.Get(board);
    assert(node);
    if (!ShouldVisit(*node)) {
      return;
    }
    if (node->IsLeaf()) {
      VisitLeaf(*node);
      return;
    }
    PreVisitInternalNode(*node);
    auto children = GetUniqueNextBoardsWithPass(board);
    for (const auto& child : children) {
      VisitInternal(child.first);
    }
    PostVisitInternalNode(*node);
  }
};

template<int version = kBookVersion>
class BookVisitorNoTranspositions : public BookVisitor<version> {
 public:
  typedef BookVisitor<version> BookVisitor;
  using typename BookVisitor::Book;
  using typename BookVisitor::BookNode;

  BookVisitorNoTranspositions(const Book& book) : BookVisitor(book) {}

  void VisitInternal(const Board& board) override {
    if (!visited_.insert(board).second) {
      return;
    }
    BookVisitor::VisitInternal(board);
  }

  std::unordered_set<Board> visited_;
};


template<int version = kBookVersion>
class BookVisitorToVectorNoTransposition : public BookVisitorNoTranspositions<version> {
 public:
  typedef BookVisitor<version> BookVisitor;
  using typename BookVisitor::Book;
  using typename BookVisitor::BookNode;

  BookVisitorToVectorNoTransposition(const Book& book) : BookVisitorNoTranspositions<version>(book) {}

  const std::vector<std::pair<Node, NodeType>>& Get() {
    return nodes_;
  }

 protected:
  void StartVisit() override {
    nodes_.clear();
    nodes_.reserve(BookVisitor::book_.Size());
  }
  void VisitLeaf(Node& node) override {
    nodes_.push_back({node, LEAF});
  }
  void PreVisitInternalNode(Node& node) override {
    nodes_.push_back({node, FIRST_VISIT});
  }
  void PostVisitInternalNode(Node& node) override {
    nodes_.push_back({node, LAST_VISIT});
  }

 private:
  std::vector<std::pair<Node, NodeType>> nodes_;
};

template<int source_version = kBookVersion, int target_version = kBookVersion>
class BookMerge : public BookVisitorNoTranspositions<source_version> {
 public:
  typedef BookVisitor<source_version> BookVisitor;
  typedef Book<source_version> BookSource;
  typedef Book<target_version> BookTarget;
  typedef typename BookSource::BookNode BookNodeSource;
  typedef typename BookTarget::BookNode BookNodeTarget;
  using BookVisitor::book_;

  BookMerge(
      const BookSource& source,
      BookTarget& destination,
      void (*leaf_func)(Node*) = nullptr,
      void (*internal_func)(Node*) = nullptr) :
      BookVisitorNoTranspositions<source_version>(source),
      destination_(destination),
      leaf_func_(leaf_func),
      internal_func_(internal_func) {}

 protected:
  BookTarget& destination_;
  void (*leaf_func_)(Node*);
  void (*internal_func_)(Node*);

  void StartVisit() override {
    // This avoids adding a lot of roots and removing them afterwards (to avoid
    // memory problems).
    for (const auto& root: book_.Roots()) {
      Board unique = root.Unique();
      if (!destination_.Get(unique)) {
        destination_.roots_.insert(unique);
      }
    }
  }

  void FirstVisit(Node& source, void (*function)(Node*)) {
    auto* destination_node = destination_.Mutable(source.ToBoard());
    if (destination_node) {
      destination_node->AddDescendants(source.GetNVisited());
      // The min / max is computed inside these functions.
      destination_node->SetLower(source.Lower());
      destination_node->SetUpper(source.Upper());
    } else {
      if (function) {
        function(&source);
      }
      destination_.AddNoRootsUpdate(source);
    }
  }

  void VisitLeaf(Node& node) override {
    FirstVisit(node, leaf_func_);
  }

  void PreVisitInternalNode(Node& node) override {
    FirstVisit(node, internal_func_);
  }

  void PostVisitInternalNode(Node& node) override {
    // Node is the source node
    auto* destination_node = destination_.Mutable(node.ToBoard());
    assert(destination_node);
    if (destination_node->IsLeaf()) {
      destination_.AddChildren(destination_node->ToBoard(), {});
    } else {
      destination_node->UpdateFather();
    }
  }

  void EndVisit() override {
    destination_.Commit();
  }
};

template<int version = kBookVersion>
Book<version> RemoveDescendants(const Book<version>& book, const std::string& filepath) {
  Book<version> book_no_descendants(filepath);
  assert(book_no_descendants.Size() == 0);

  BookMerge<version, version>(
      book,
      book_no_descendants,
      [](Node* node) { node->ResetDescendants(); },
      [](Node* node) { node->ResetDescendants(); }).Visit();
  return book_no_descendants;
}