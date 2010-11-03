/**
  * EasyUnzip.h: Because using miniunzip is hard.
  *
  * @author Wilhansen Li
  */

#ifndef COMMON_EASYUNZIP_H__
#define COMMON_EASYUNZIP_H__

#include <exception>
#include <vector>
#include <stdint.h>

#define MANIFEST_FILENAME_XML "manifest.xml"

struct UnzipException : public std::exception {
	char info[1024];

	UnzipException();

	const char * what() const throw() {
		return info;
	}
};

struct UnzipIoException : public UnzipException {
	enum AccessType { READ = 1, WRITE = 2, OPEN = 4 };
	UnzipIoException(const char *fileName, int accessTypeFlags);
};

struct UnzipArchiveException : public UnzipException {
	UnzipArchiveException(const char *archiveName);
};

struct UnzipManifestNotFoundException : public UnzipException {
	UnzipManifestNotFoundException(const char *archiveName);
};

/**
  * Extract the specified archive to the destination, filtering out the common directory. For example,
  * if all of the files in the archive are in problem0/files/*, then problem0/files/ will be filtered out
  * so there will be at least one file directly under the destination.
  *
  * @reentrant
  * @throw UnzipIoException
  * @throw UnzipArchiveException
  */
void easy_unzip(const char *archive, const char *destination);

struct ManifestData {
	enum Type { XML };
	ManifestData() {}
	ManifestData(Type type, size_t size) : data(size), type(type) {}
	ManifestData(ManifestData &&rval) : data(std::move(rval.data)), type(type) {}
	std::vector<uint8_t> data;
	Type type;
};

/**
  * Gets the extracted manifest data from the archive. It is assured that the data is always
  * null-terminated (even if it's empty).
  *
  * @reentrant
  * @throw UnzipIoException
  * @throw UnzipArchiveException
  * @throw UnzipManifestNotFoundException
  */
ManifestData get_manifest(const char *archive);

#endif
