#include "../cryfs_lib/CryDevice.h"

#include <memory>

#include "CryDir.h"
#include "CryFile.h"
#include "CryOpenFile.h"
#include "CryErrnoException.h"
#include "utils/pointer.h"

using namespace cryfs;

using std::unique_ptr;
using std::make_unique;

CryDevice::CryDevice(const bf::path &rootdir)
  :_rootdir(rootdir), _open_files() {
}

CryDevice::~CryDevice() {
}

unique_ptr<CryNode> CryDevice::Load(const bf::path &path) {
  auto real_path = RootDir() / path;
  if(bf::is_directory(real_path)) {
    return make_unique<CryDir>(this, path);
  } else if(bf::is_regular_file(real_path)) {
    return make_unique<CryFile>(this, path);
  }

  throw CryErrnoException(ENOENT);
}

unique_ptr<CryFile> CryDevice::LoadFile(const bf::path &path) {
  auto node = Load(path);
  auto file = dynamic_pointer_move<CryFile>(node);
  if (!file) {
	  throw CryErrnoException(EISDIR);
  }
  return file;
}

unique_ptr<CryDir> CryDevice::LoadDir(const bf::path &path) {
  auto node = Load(path);
  auto dir = dynamic_pointer_move<CryDir>(node);
  if (!dir) {
    throw CryErrnoException(ENOTDIR);
  }
  return dir;
}

int CryDevice::openFile(const bf::path &path, int flags) {
  auto file = LoadFile(path);
  return _open_files.open(*file, flags);
}

void CryDevice::closeFile(int descriptor) {
  _open_files.close(descriptor);
}

void CryDevice::lstat(const bf::path &path, struct ::stat *stbuf) {
  Load(path)->stat(stbuf);
}

void CryDevice::fstat(int descriptor, struct ::stat *stbuf) {
  _open_files.get(descriptor)->stat(stbuf);
}

void CryDevice::truncate(const bf::path &path, off_t size) {
  LoadFile(path)->truncate(size);
}

void CryDevice::ftruncate(int descriptor, off_t size) {
  _open_files.get(descriptor)->truncate(size);
}

void CryDevice::read(int descriptor, void *buf, size_t count, off_t offset) {
  _open_files.get(descriptor)->read(buf, count, offset);
}

void CryDevice::write(int descriptor, const void *buf, size_t count, off_t offset) {
  _open_files.get(descriptor)->write(buf, count, offset);
}

void CryDevice::fsync(int descriptor) {
  _open_files.get(descriptor)->fsync();
}

void CryDevice::fdatasync(int descriptor) {
  _open_files.get(descriptor)->fdatasync();
}

void CryDevice::access(const bf::path &path, int mask) {
  Load(path)->access(mask);
}

int CryDevice::createFile(const bf::path &path, mode_t mode) {
  //TODO Creating the file opens and closes it. We then reopen it afterwards.
  //     This is slow. Improve!
  auto dir = LoadDir(path.parent_path());
  dir->createFile(path.filename().native(), mode);
  return openFile(path, mode);
}