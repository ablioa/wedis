# About Wedis
---
The wedis project is aim to build a light weight redis client.

# Build & Run Wedis
---
Wedis can be compiled and used on windows.

wedis is written in C and compiled with MinGW,it depends on nothing but windows system libirary.


## build

1. make sure that gcc compiler in MinGW is set in evironment variables.

2. enter directory **src**

3. %make

4. wedis.exe file can be found in **output** directory.

## run

wedis is configured with a json file named 'wedis.json',it will create an empty one when run at the first time.

```
{
 "pageSize": 20,
 "logTraffic": 1,
 "lwidth": 1400,
 "lheight": 858,
 "twidth": 408,
 "hosts": [{
   "name": "warmcode.net",
   "host": "192.168.1.5",
   "port": 6380,
   "password": "password",
   "requirepass": true,
   "description": "configuration cache"
  }, {
   "name": "secure-redis",
   "host": "192.168.1.6",
   "port": 5555,
   "password": "password",
   "requirepass": true,
   "description": "image cache"
  }]
}
```

**pageSize** the page size of items when scanning a database.

**logTraggic** whether to log the communication traffic of the RESP protocal.

**hosts** customized connection managed by wedis.

# Screen Shot
---
![wedis screen shot](screen/screenshot.png)
