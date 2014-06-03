UnitSyncTool
============

Spring Engine Data Provider

Compile:

Run in Source dir qmake && make ( or nmake in windows ).
Tool wil be compiled into './Bin' directory.

Using:

To run tool you need qt runtime.

Run with command line arguments:

1. --unitsyncDir="/engine/dir" path to unit sync library.
Spring paths get by unitsync default paths.

2. --springDataDir="/data/dir" path to spring data directory. It's supposed next directory structure
	SpringData
	|-engines
	|	|-91.0
	|	|-94.1
	|	|-...
	|-games
	|-maps
	|-rapid
	|-springsettings.cfg
	
	when tool load unitsync from one of engines path, it's setup using springsettings.cfg from
	spring data directory, and also setup data directory in springsettings.cfg
	( by set SpringData option )
	
3. --port=8668 port listened by server.

After run tool you can request in standard http way any info.
( you can explore by browser available commands, type in address 127.0.0.1:port ).