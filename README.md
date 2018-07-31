qimgv | Current version: 0.7.0
=====
A cross-platform image viewer with webm support. Written in qt5.

## [qimgv v0.6 video overview](https://www.youtube.com/watch?v=AODRGCRPCpw)

## qimgv v0.7 update overview [TODO]

![alt tag](https://i.imgur.com/prIficV.png)

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

You can also bring up a context menu by right-clicking on an image.

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

## Installation instructions

### Linux

  __Arch:__ Available in AUR - `qimgv-git`
  
  __Gentoo:__ `emerge qimgv`
    
  __Others distros (build & install via script):__
  
  1. Install dependencies ( git, cmake, qt >= 5.6, libmpv >= 0.22, mpv)
     - Ubuntu & derivatives
		```
		sudo apt install build-essential cmake qt5-default libmpv-dev 
		```
  2. Build
```
git clone https://github.com/easymodo/qimgv.git
cd qimgv/scripts
./build.sh
```
  3. Install  
```
./install.sh
```

### Windows

  [Grab the latest release here](https://github.com/easymodo/qimgv/releases)
  
