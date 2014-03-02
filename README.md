IDFS
===========

IDFS is a distributed file system inspired by Hadoop (in a very small scale).

I haven't planned to make this code public, so I puked tons of code during the development.
The IDFS can still needs lots of improvement.


How it works?
--------------------------
IDFS works like your own file system, however the files are not stored locally in your
machine, but remotely in the cloud. The IDFS consists in 3 independent systems:
the master node, the slave nodes and a client.

### The master node

The master node is responsible to control the IDFS, it holds the file system image
(all files and dirs that are stored, and it's physical locations) and handles all
clients requests.

### The slaves node
There can be one or more (in different machines) connected to a master node,
it holds all files data.

### Client
The user interface with the master node.

When sending a file to the IDFS the master node will split the whole file in chunks and
distribute these chunks across the slave nodes.


Setup
----------

After compiling it, launch the master node.
```shell
./master_node -p 8554
```

Where -p is the port

Than connect how many slaves node you want, like this:
```shell
./slave_node -a 200.2.2.2 -p 8554
```

Where -a is the master_node IP and -p is the port.

Now start making requests via the idfs_client
```shell
./idfs_client -a 200.2.2.2 -p 8554 mkdir /myRemoteMusic
./idfs_client -a 200.2.2.2 -p 8554 push_file /myLocalMusicDir/music.mp3 /myMusic
```

Commands
----------
Listing files:
```shell
idfs_client ls /
```

or

```shell
idfs_client ls /a/path/here
```

Creating directories:
```shell
idfs_client mkdir /home
```

or

```shell
idfs_client mkdir /create/this/path/for/me
```

Sending a file:

```shell
idfs_client push_file /My/local/file /my/remote/file/dir
```

Fetching the file that was pushed:
```shell
idfs_client push_file /my/remote/file/dir /a/local/dir
```

What I need to compile it?
--------------------------
* Qt5

Compiling
----------
```shell
cd /path/go/idfs/src
qmake
make
```

Runs on
----------
* OS X
* Linux
* (Should work on Windows)


TODO
-----
* Duplicate file parts across the slave nodes.
* Add progress for the command get_file
* Add the commands: cat and appendToFile
* Clean fs_message.hh (Some fields may not be necessary)
* Improve the code