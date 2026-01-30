#pragma once
#include <cstdint>
#include <string>
#include <vector>

struct PublishCall {
  enum Type { kDouble, kFloat, kBool, kInt32, kUint32 } type;
  std::string path;
  double d = 0.0;
  float  f = 0.0f;
  bool   b = false;
  std::int32_t  i32 = 0;
  std::uint32_t u32 = 0;
};

struct IKuksaClient {
  virtual ~IKuksaClient() {}
  virtual void publishDouble(const std::string& path, double v) = 0;
  virtual void publishFloat (const std::string& path, float v) = 0;
  virtual void publishBool  (const std::string& path, bool v) = 0;
  virtual void publishInt32 (const std::string& path, std::int32_t v) = 0;
  virtual void publishUint32(const std::string& path, std::uint32_t v) = 0;
};

//For testeing purposes
struct FakeKuksaClient : public IKuksaClient {
  std::vector<PublishCall> calls;

  void publishDouble(const std::string& path, double v) override {
    PublishCall c; c.type = PublishCall::kDouble; c.path = path; c.d = v;
    calls.push_back(c);
  }
  void publishFloat(const std::string& path, float v) override {
    PublishCall c; c.type = PublishCall::kFloat; c.path = path; c.f = v;
    calls.push_back(c);
  }
  void publishBool(const std::string& path, bool v) override {
    PublishCall c; c.type = PublishCall::kBool; c.path = path; c.b = v;
    calls.push_back(c);
  }
  void publishInt32(const std::string& path, std::int32_t v) override {
    PublishCall c; c.type = PublishCall::kInt32; c.path = path; c.i32 = v;
    calls.push_back(c);
  }
  void publishUint32(const std::string& path, std::uint32_t v) override {
    PublishCall c; c.type = PublishCall::kUint32; c.path = path; c.u32 = v;
    calls.push_back(c);
  }

  size_t countPath(const std::string& p) const {
    size_t n = 0;
    for (size_t i = 0; i < calls.size(); ++i) if (calls[i].path == p) ++n;
    return n;
  }

  size_t countPathType(const std::string& p, PublishCall::Type t) const {
    size_t n = 0;
    for (size_t i = 0; i < calls.size(); ++i)
      if (calls[i].path == p && calls[i].type == t) ++n;
    return n;
  }

  bool lastDouble(const std::string& p, double& out) const {
    for (size_t i = calls.size(); i-- > 0; ) {
      if (calls[i].path == p && calls[i].type == PublishCall::kDouble) {
        out = calls[i].d;
        return true;
      }
    }
    return false;
  }

  bool lastFloat(const std::string& p, float& out) const {
    for (size_t i = calls.size(); i-- > 0; ) {
      if (calls[i].path == p && calls[i].type == PublishCall::kFloat) {
        out = calls[i].f;
        return true;
      }
    }
    return false;
  }

  bool lastBool(const std::string& p, bool& out) const {
    for (size_t i = calls.size(); i-- > 0; ) {
      if (calls[i].path == p && calls[i].type == PublishCall::kBool) {
        out = calls[i].b;
        return true;
      }
    }
    return false;
  }

  bool lastInt32(const std::string& p, std::int32_t& out) const {
    for (size_t i = calls.size(); i-- > 0; ) {
      if (calls[i].path == p && calls[i].type == PublishCall::kInt32) {
        out = calls[i].i32;
        return true;
      }
    }
    return false;
  }

  bool lastUint32(const std::string& p, std::uint32_t& out) const {
    for (size_t i = calls.size(); i-- > 0; ) {
      if (calls[i].path == p && calls[i].type == PublishCall::kUint32) {
        out = calls[i].u32;
        return true;
      }
    }
    return false;
  }
};
