// Copyright (c) 2012-2014 Konstantin Isakov <ikm@zbackup.org> and ZBackup contributors, see CONTRIBUTORS
// Part of ZBackup. Licensed under GNU GPLv2 or later + OpenSSL, see LICENSE

#ifndef FILE_HH_INCLUDED
#define FILE_HH_INCLUDED

#include <stddef.h>
#include <cstdio>
#include <exception>
#include <string>

#include "ex.hh"

using std::string;

/// A simple wrapper over FILE * operations with added write-buffering
class File
{
  FILE * f;
  char * writeBuffer;
  size_t writeBufferLeft;

public:
  DEF_EX( Ex, "File exception", std::exception )
  DEF_EX_STR( exCantOpen, "Can't open", Ex )
  DEF_EX( exReadError, "Error reading from file", Ex )
  DEF_EX( exShortRead, "Short read from the file", exReadError )
  DEF_EX( exWriteError, "Error writing to the file", Ex )
  DEF_EX( exSeekError, "File seek error", Ex )
  DEF_EX_STR( exCantErase, "Can't erase file", Ex )
  DEF_EX_STR( exCantRename, "Can't rename file", Ex )

  enum OpenMode
  {
    ReadOnly,
    WriteOnly,
    Update
  };

  typedef long Offset;

  File( char const * filename, OpenMode )
    noexcept(false);

  File( std::string const & filename, OpenMode )
    noexcept(false);

  File( int fd, OpenMode )
    noexcept(false);

  /// Reads the number of bytes to the buffer, throws an error if it
  /// failed to fill the whole buffer (short read, i/o error etc)
  void read( void * buf, size_t size ) noexcept(false);

  template< typename T >
  void read( T & value ) noexcept(false)
  { read( &value, sizeof( value ) ); }

  template< typename T >
  T read() noexcept(false)
  { T value; read( value ); return value; }

  /// Attempts reading at most 'count' records sized 'size'. Returns
  /// the number of records it managed to read, up to 'count'
  size_t readRecords( void * buf, size_t size, size_t count ) noexcept(false);

  /// Writes the number of bytes from the buffer, throws an error if it
  /// failed to write the whole buffer (short write, i/o error etc).
  /// This function employs write buffering, and as such, writes may not
  /// end up on disk immediately, or a short write may occur later
  /// than it really did. If you don't want write buffering, use
  /// writeRecords() function instead
  void write( void const * buf, size_t size ) noexcept(false);

  template< typename T >
  void write( T const & value ) noexcept(false)
  { write( &value, sizeof( value ) ); }

  /// Attempts writing at most 'count' records sized 'size'. Returns
  /// the number of records it managed to write, up to 'count'.
  /// This function does not employ buffering, but flushes the buffer if it
  /// was used before
  size_t writeRecords( void const * buf, size_t size, size_t count )
    noexcept(false);

  /// Reads a string from the file. Unlike the normal fgets(), this one
  /// can strip the trailing newline character, if this was requested.
  /// Returns either s or 0 if no characters were read
  char * gets( char * s, int size, bool stripNl = false ) noexcept(false);

  /// Like the above, but uses its own local internal buffer (1024 bytes
  /// currently), and strips newlines by default
  std::string gets( bool stripNl = true ) noexcept(false);

  /// Seeks in the file, relative to its beginning
  void seek( long offset ) noexcept(false);
  /// Seeks in the file, relative to the current position
  void seekCur( long offset ) noexcept(false);
  /// Seeks in the file, relative to the end of file
  void seekEnd( long offset = 0 ) noexcept(false);

  /// Seeks to the beginning of file
  void rewind() noexcept(false);

  /// Tells the current position within the file, relative to its beginning
  size_t tell() noexcept(false);

  /// Returns file size
  size_t size() noexcept(false);

  /// Returns true if end-of-file condition is set
  bool eof() noexcept(false);

  /// Returns ferror
  int error() noexcept(false);

  /// Returns the underlying FILE * record, so other operations can be
  /// performed on it
  FILE * file() noexcept(false);

  /// Releases the file handle out of the control of the class. No further
  /// operations are valid. The file will not be closed on destruction
  FILE * release() noexcept(false);

  /// Closes the file. No further operations are valid
  void close() noexcept(false);

  /// Checks if the file exists or not
  static bool exists( char const * filename ) noexcept;

  static bool exists( std::string const & filename ) noexcept
  { return exists( filename.c_str() ); }

  /// Returns false when lstat returns stat::st_mode having 
  /// S_IFREG (filename is a file, or softlink to a file, and
  /// is not a socket, fifo, device node, etc.
  static bool special( std::string const & filename ) noexcept;

  ~File() noexcept;

  /// Erases the given file
  static void erase( std::string const & ) noexcept(false);

  /// Renames the given file
  static void rename( std::string const & from,
                      std::string const & to ) noexcept(false);

  /// Throwing this class instead of exReadError will make the description
  /// include the file name
  class exReadErrorDetailed: public exReadError
  {
    string description;

  public:
    exReadErrorDetailed( int fd );
    exReadErrorDetailed( FILE * f );
    virtual const char * what() const noexcept;
    virtual ~exReadErrorDetailed() throw ();

  private:
    void buildDescription( int fd );
  };

private:

  void open( char const * filename, OpenMode ) noexcept(false);
  void open( int fd, OpenMode ) noexcept(false);
  void flushWriteBuffer() noexcept(false);
  void releaseWriteBuffer() noexcept(false);
};

#endif
