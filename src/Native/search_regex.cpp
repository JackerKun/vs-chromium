// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "stdafx.h"

#include <regex>

#include "search_regex.h"

RegexSearch::RegexSearch()
    : pattern_(NULL),
      patternLen_(0),
      regex_(nullptr) {
}

RegexSearch::~RegexSearch() {
  delete regex_;
}

bool RegexSearch::PreProcess(const char *pattern, int patternLen, SearchOptions options) {
  auto flags = std::regex::ECMAScript;
  if ((options & kMatchCase) == 0) {
    flags = flags | std::regex::icase;
  }
  regex_ = new std::regex(pattern, patternLen, flags);
  pattern_ = pattern;
  patternLen_ = patternLen;
  return true;
}

AsciiSearchBase::SearchResult RegexSearch::Search(const char *text, int textLen) {
  const char* first = text;
  const char* last = text + textLen;
  std::cmatch match;
  bool result = std::regex_search(first, last, match, *regex_, std::regex_constants::match_default);
  if (!result) {
    return SearchResult();
  }
  auto position = match.position();
  auto length = match.length();

  // Special case of regex that match empty sequence (e.g. [a-z]*).
  // We assume this is a "no result" to avoid infinite loops.
  if (position == 0 && length == 0) {
    return SearchResult();
  }
  return SearchResult(first + position, length);
}