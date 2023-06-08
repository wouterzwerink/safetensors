// Copied from https://learn.microsoft.com/en-us/windows/win32/memory/creating-a-view-within-a-file
#include "safetensors/safetensors.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

/* The test file. The code below creates the file and populates it,
   so there is no need to supply it in advance. */

TCHAR * filename = TEXT("model.safetensors"); // the file to be manipulated

#define BUF_SIZE 256

int main(void)
{
  HANDLE hMapFile;
  HANDLE hFile;
  LPCTSTR pBuf;
  DWORD length;
  LPVOID lpMapAddress;

  // Create the test file. Open it "Create Always" to overwrite any
  // existing file. The data is re-created below
  // mapping = CreateFileMappingW(handle, ptr::null_mut(), protect, 0, 0, ptr::null());
  hFile = CreateFileMapping(
    INVALID_HANDLE_VALUE,
    NULL,
    PAGE_READONLY,
    0,
    BUF_SIZE,
    filename);               // name of mapping object
                             //
  length = GetFileSize(hFile,  NULL);
  _tprintf(TEXT("hFile size: %10d\n"), length);

  if (hFile == NULL)
  {
    _tprintf(TEXT("hFile is NULL\n"));
    _tprintf(TEXT("Target file is %s\n"),
             filename);
    return 1;
  }

  hMapFile = CreateFileMapping( hFile,          // current file handle
                NULL,           // default security
                PAGE_READWRITE, // read/write permission
                0,              // size of mapping object, high
                0,  // size of mapping object, low
                NULL);          // name of mapping object

  if (hMapFile == NULL)
  {
    _tprintf(TEXT("hMapFile is NULL: last error: %d\n"), GetLastError() );
    return (2);
  }

  // Map the view and test the results.

  lpMapAddress = MapViewOfFile(hMapFile,            // handle to
                                                    // mapping object
                               FILE_MAP_READ, // read/write
                               0,                   // high-order 32
                                                    // bits of file
                                                    // offset
                               0,      // low-order 32
                                                    // bits of file
                                                    // offset
                               BUF_SIZE);      // number of bytes
                                                    // to map
  if (lpMapAddress == NULL)
  {
    _tprintf(TEXT("lpMapAddress is NULL: last error: %d\n"), GetLastError());
    return 1;
  }

  // Calculate the pointer to the data.
  Handle* handle = nullptr;
  Status result = safetensors_deserialize(&handle, (const uint8_t*) lpMapAddress, length);
  if (result != Status::Ok){
      printf("Could not open safetensors file");
  }
  View *tensor = nullptr;
  auto tensor_name =  "wpe.weight";
  result = safetensors_get_tensor(handle, &tensor, tensor_name);
  if (result != Status::Ok){
      printf("Could not open find tensor");
  }else{
      printf("Found tensor %s: rank %d\n", tensor_name, tensor->rank);
      printf("Shape:\n    ");
      for (int i=0; i < tensor->rank; i++){
          printf("%d ", tensor->shape[i]);
      }
      printf("\n");
  }
  return 0;
}