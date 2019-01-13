qimgv | Current version: 0.7.2
=====
Qt5 image viewer. Fast, configurable, easy to use. Optional video support.

![alt tag](https://i.imgur.com/fdHKWtf.png)

## [qimgv v0.6 video overview](https://www.youtube.com/watch?v=AODRGCRPCpw)

## Key features:

- Simple UI

- Fast

- Easy to use

- Fully configurable, including shortcuts

- Basic image editing: Crop, Rotate and Resize

- Ability to quickly copy / move images to different folders

- Experimental video playback via libmpv

- Ability to run shell scripts

- A nice dark theme, should look identical on every OS / DE

## Default control scheme:

| Action  | Shortcut |
| ------------- | ------------- |
| Next image  | Right arrow / MouseWheel |
| Previous image  | Left arrow / MouseWheel |
| Goto first image  | Home |
| Goto last image  | End |
| Zoom in  | Ctrl+MouseWheel / Crtl+Up |
| Zoom out  | Ctrl+MouseWheel / Crtl+Down |
| Fit mode: window | 1 |
| Fit mode: width | 2 |
| Fit mode: 1:1 (no scaling) | 3 |
| Switch fit modes  | Space |
| Toggle fullscreen mode  | DoubleClick / F / F11 |
| Exit fullscreen mode | Esc |
| Crop image  | X |
| Resize image  | R |
| Rotate left  | Ctrl+L |
| Rotate Right  | Ctrl+R |
| Quick copy  | C |
| Quick move  | M |
| Move to trash | Delete |
| Delete file  | Shift+Delete |
| Save  | Ctrl+S |
| Save As  | Ctrl+Shift+S |
| Open | Ctrl+O |
| Settings  | Ctrl+P |
| Exit application | Esc / Ctrl+Q / Alt+X / MiddleClick |

Note: you can configure every shortcut listed above by going to __Settings > Controls__

## User interface

The idea is to have a uncluttered, simple and easy to use UI. You can see ui elements only when you need them.

There is a pull-down panel with thumbnails, as well as folder view (accessible by pressing Return).

You can also bring up a context menu by right-clicking an image.

### Using quick copy / quick move panels

Bring up the panel with C or M shortcut. You will see 9 destination directories, click them to set them up.

With panel visible, use 1 - 9 keys to copy/move current image to corresponding directory.

When you are done press C or M again to hide the panel.

### Running scripts

Starting with v0.7 you can run scripts on a current image.

Open __Settings > Scripts__. Press Add. Here you can choose between a shell command and a shell script. 

Example of a command: 

`convert %file% %file%_.pdf`

Example of a shell script file: 
```
#!/bin/bash
gimp "$1"
```
_Note: The $1 argument will be a full file path. Also, the script file must be an executable._

When you've created your script go to __Settings > Controls > Add__, then select it and assign a shortcut like for any regular action.

### HiDPI

If qimgv appears too small / too big on your display, you can override the scale factor. Example:
```
QT_SCALE_FACTOR="1.5" qimgv /path/to/image.png
```
You can put it in `qimgv.desktop` file to make it permanent. Using values less than `1.0` may break some things.

qimgv should also obey the global scale factor set in kde's systemsettings.

### APNG support

APNG is supported via third-party qt plugin. (Included in windows qimgv package)
 
If you are linux user, install the latest [QtApng by Skycoder42](https://github.com/Skycoder42/QtApng).

### RAW support

Viewing raw is supported via [qtraw plugin](https://github.com/mardy/qtraw). It is not included in windows release at the time.

## Installation instructions

### GNU+Linux

__Arch:__ Available in AUR - `qimgv-git`
  
__Gentoo:__ `emerge qimgv`
  
__Manual install:__
 
__1. Install dependencies ( git, cmake, qt >= 5.6, libmpv >= 0.22, mpv)__
  
_Ubuntu & derivatives:_
     
```
sudo apt install build-essential cmake qt5-default
```
     
_Fedora:_

```
sudo dnf install git cmake qt5 qt5-devel gcc-c++ qt5-devel
```
	
__2. _(Optional)_ Dependencies for video playback ( libmpv >= 0.22, mpv)__
  	
_Ubuntu & derivatives:_
     
```
sudo apt install libmpv-dev
```
     
_Fedora_:
     
Enable RPMFusion [https://rpmfusion.org/Configuration](https://rpmfusion.org/Configuration).
	
```
sudo dnf install mpv mpv-libs-devel
```
		
__2. Build__
```
git clone https://github.com/easymodo/qimgv.git
cd qimgv && mkdir -p build && cd build
```

Regular build

```
cmake -DCMAKE_INSTALL_PREFIX=/usr/ -DCMAKE_BINARY_DIR=${DIR}/ .. && make -j4
```

Build with __video support__ (note: negatively affects startup speed)

```
cmake -DVIDEO_SUPPORT=ON -DCMAKE_INSTALL_PREFIX=/usr/ -DCMAKE_BINARY_DIR=${DIR}/ .. && make -j4
```

Build with better __KDE support__

```
cmake -DKDE_SUPPORT=ON -DCMAKE_INSTALL_PREFIX=/usr/ -DCMAKE_BINARY_DIR=${DIR}/ .. && make -j4
```

__3. Install__

```
sudo make install
```

### Windows

  [Grab the latest release here.](https://github.com/easymodo/qimgv/releases)
  
  All windows builds are portable.
  
