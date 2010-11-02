#include "EasyUnzip.h"
#include "../../lib/minizip/unzip.h"
#include <apr_file_io.h>
#include <apr_strings.h>
#include <string>
#include <cstring>
#include <fstream>
#include <cassert>

namespace {
struct UnzFileGuard {
	unzFile f;
	UnzFileGuard(unzFile f) : f(f) {}
	~UnzFileGuard() { if (f) unzClose(f); }
};

struct AprPoolGuard {
	apr_pool_t *pool;
	AprPoolGuard(apr_pool_t *pool) : pool(pool) {}
	AprPoolGuard() {
		apr_pool_create(&pool, 0);
	}

	~AprPoolGuard() {
		if ( pool ) apr_pool_destroy(pool);
	}

};

/**
  * Get the common directory of all the files in the archive. e.g. if the archive contains the files:
  * "dir0/a.txt" and "dir0/b.txt" getCommonDirectory will return "dir0/".
  *
  * @throw UnzipArchiveException
  */
std::string getCommonDirectory(unzFile f) {
	unz_global_info zInfo;

	if ( unzGetGlobalInfo(f, &zInfo) != UNZ_OK ) {
		throw UnzipArchiveException("");
	}

	std::string common_directory;
	char tmp_filename[1024];

	unzGoToFirstFile(f);

	for ( int i = 0; i < zInfo.number_entry; ++i ) {
		unz_file_info fInfo;
		if ( unzGetCurrentFileInfo(f, &fInfo, tmp_filename, sizeof(tmp_filename), 0, 0, 0, 0) != UNZ_OK )
			throw UnzipArchiveException("");

		if ( i == 0 ) {
			//initialize the first.
			common_directory = tmp_filename;
			std::string::size_type pos = common_directory.find_last_of("\\/:");
			if ( pos == std::string::npos ) {
				//the first file is not in any directory so we're sure that everything else isn't.
				break;
			} else {
				//get the common directory with the trailing separator, i.e. erase everything after the
				//trailing separator of the directory.
				common_directory.erase(pos + 1);
			}
		} else {
			if ( common_directory.empty() ) break;

			const size_t sz = std::min((size_t)fInfo.size_filename, (size_t)common_directory.length());
			size_t lastSeparator = -1;
			for ( size_t i = 0; i < sz; ++i ) {
				if ( tmp_filename[i] != common_directory[i] ) {
					//if there were no directories encountered before the mismatch, this means that
					//there are no common directories, get out immediately.
					if ( lastSeparator == (size_t)-1 ) {
						common_directory.clear();
						break;
					}

					//get the common directory with the trailing separator and avoid trimming
					//if we don't need to.
					if ( lastSeparator + 1 < common_directory.length() )
							common_directory.erase(lastSeparator + 1);
				} else if ( tmp_filename[i] == '/' || tmp_filename[i] == '\\' || tmp_filename[i] == ':' ) {
					//keep track of where the last directory separator is encountered
					lastSeparator = i;
				}
			}
		}

		if ( (i+1) < zInfo.number_entry ) {
			if ( unzGoToNextFile(f) != UNZ_OK ) {
				throw UnzipArchiveException("");
			}
		}
	}
	return common_directory;
}
}


UnzipException::UnzipException() {
	std::strcpy(info, "An error occurred while extracting archive.");
}


UnzipIoException::UnzipIoException(const char *fileName, int type) {
	size_t offset = 0;
	if ( type & OPEN )
		offset = apr_snprintf(info, sizeof(info), "Unable to open file ");
	else
		offset = apr_snprintf(info, sizeof(info), "Unable to access file ");

	if ( (type & READ) && (type & WRITE) )
		apr_snprintf(info + offset, sizeof(info) - offset, "%s for reading and writing.", fileName);
	else if ( type & READ )
		apr_snprintf(info + offset, sizeof(info) - offset, "%s for reading.", fileName);
	else
		apr_snprintf(info + offset, sizeof(info) - offset, "%s for writing.", fileName);
}

UnzipArchiveException::UnzipArchiveException(const char *archiveName) {
	apr_snprintf(info, sizeof(info), "Bad archive: %s", archiveName);
}

UnzipManifestNotFoundException::UnzipManifestNotFoundException(const char *archiveName) {
	apr_snprintf(info, sizeof(info), "Unable to find manifest file in %s.", archiveName);
}

void easy_unzip(const char *archive, const char *destination) {
  int casd = 0;
  
	UnzFileGuard f(unzOpen(archive));
  
	if (f.f == 0 )
		throw UnzipIoException(archive, UnzipIoException::READ | UnzipIoException::OPEN);
  
	unz_global_info zInfo;

	if ( unzGetGlobalInfo(f.f, &zInfo) != UNZ_OK ) {
		throw UnzipArchiveException(archive);
	}
  
	//detect common directory prefix and strip them out
	const std::string &common_directory = getCommonDirectory(f.f);
	unzGoToFirstFile(f.f);
  
	AprPoolGuard pool;
  
	//set the extraction buffer to 8KB
	std::vector<uint8_t> extract_buffer(1024 * 8);
	char tmp_filename[1024];
  
	for ( int i = 0; i < zInfo.number_entry; ++i ) {
		unz_file_info fInfo;
		unzGetCurrentFileInfo(f.f, &fInfo, tmp_filename, sizeof(tmp_filename), 0, 0, 0, 0);

		assert(common_directory.length() <= fInfo.size_filename);
    
    //if the file is a folder just skip it
    char lastChar = tmp_filename[strlen(tmp_filename)-1];
    if(lastChar != '/' && lastChar != '\\') {
      
      //skip to the "uncommon" segment. (this is one character after the directory separator)
      char *strippedPath = tmp_filename + common_directory.length();
      const char *dirname = strippedPath;
      //start scanning from the end of the string.
      char *filename = tmp_filename + fInfo.size_filename - 1;
      
      //find the last directory separator
      while(*filename != '\\' && *filename != '/' && *filename != ':' && filename > dirname)
        --filename;

      //if the file is not in root...
      if ( filename > dirname ) {
        //change the last directory separator into a null terminator so that dirname will end.
        *filename = 0;
        ++filename;
      } else {
        //the file is at the root so assign dirname to an empty string
        dirname = "";
      }
      
      char *absolutePath;
      apr_filepath_merge(&absolutePath, destination, dirname, APR_FILEPATH_NATIVE | APR_FILEPATH_TRUENAME, pool.pool);
      apr_dir_make_recursive(absolutePath, 0x0755, pool.pool);
      
      if ( unzOpenCurrentFile(f.f) != UNZ_OK )
        throw UnzipArchiveException(archive);
      
      char *absoluteFilePath;
      apr_filepath_merge(&absoluteFilePath, absolutePath, filename, APR_FILEPATH_NATIVE | APR_FILEPATH_TRUENAME, pool.pool);

      {
        
        std::ofstream out(absoluteFilePath, std::ios::binary | std::ios::out);
        if ( !out )
          throw UnzipIoException(absoluteFilePath, UnzipIoException::WRITE | UnzipIoException::OPEN);

        int extractedSize;
        do {
          extractedSize = unzReadCurrentFile(f.f, &extract_buffer[0], extract_buffer.size());
          if ( extractedSize < 0 ) {
            unzCloseCurrentFile(f.f);
            if ( extractedSize == UNZ_ERRNO )
              throw UnzipIoException(archive, UnzipIoException::READ);
            else
              throw UnzipArchiveException(archive);
          }
          out.write((const char *)&extract_buffer[0], extractedSize);

        } while (extractedSize > 0);
      }
    }
    
		unzCloseCurrentFile(f.f);

		if ( (i+1) < zInfo.number_entry ) {
			if ( unzGoToNextFile(f.f) != UNZ_OK ) {
				throw UnzipArchiveException(archive);
			}
		}
	}
}

ManifestData get_manifest(const char *archive) {
	UnzFileGuard f(unzOpen(archive));

	if (f.f == 0 )
		throw UnzipIoException(archive, UnzipIoException::READ | UnzipIoException::OPEN);

	unz_global_info zInfo;

	if ( unzGetGlobalInfo(f.f, &zInfo) != UNZ_OK ) {
		throw UnzipArchiveException(archive);
	}

	//detect common directory prefix and strip them out
	const std::string &common_directory = getCommonDirectory(f.f);
	bool found = false;
	unz_file_info fInfo;
	{
		char tmp_filename[1024];
		//find manifest.xml
		for ( int i = 0; i < zInfo.number_entry; ++i ) {
			unzGetCurrentFileInfo(f.f, &fInfo, tmp_filename, sizeof(tmp_filename), 0, 0, 0, 0);

			if ( strcmp(tmp_filename + common_directory.length(), MANIFEST_FILENAME_XML) == 0 ) {
				found = true;
				break;
			}
		}
	}

	if ( !found )
		throw UnzipManifestNotFoundException(archive);

	ManifestData ret(ManifestData::XML, fInfo.uncompressed_size);
	unzOpenCurrentFile(f.f);
	int szRead = unzReadCurrentFile(f.f, &ret.data[0], ret.data.size());
	unzCloseCurrentFile(f.f);
	if ( szRead < 0 ) {
		if ( szRead == UNZ_ERRNO )
			throw UnzipIoException(archive, UnzipIoException::READ);
		else
			throw UnzipArchiveException(archive);
	}
	assert(szRead == ret.data.size());

	//assure that there's a null terminator at the end.
	if ( ret.data.empty() || ret.data.back() != 0 )
		ret.data.push_back(0);
	return ret;
}
