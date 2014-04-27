#pragma once

struct noncopyable {
  noncopyable() = default;
  noncopyable(noncopyable&&) = default;
  noncopyable& operator=(noncopyable&&) = default;

  noncopyable(const noncopyable&) = delete;
  noncopyable& operator=(const noncopyable&) = delete;
};
