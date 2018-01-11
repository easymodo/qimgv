qimgv
=====
A cross-platform image viewer with webm support. Written in qt5.

![alt tag](https://i.imgur.com/prIficV.png)

## Key features:

- Simple UI

- Fast

- Easy to use

- Fully configurable, including shortcuts

- Basic image editing: Crop, Rotate and Resize

- Ability to quickly copy / move images to different folders

- Experimental webm playback support via libmpv

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
| Switch fit modes  | Space / MiddleClick |
| Toggle fullscreen mode  | DoubleClick / F / F11 |
| Exit fullscreen mode | Esc |
| Crop image  | X |
| Resize image  | R |
| Rotate left  | Ctrl+L |
| Rotate Right  | Ctrl+R |
| Quick copy  | C |
| Quick move  | M |
| Remove file (permanent)  | Shift+Delete |
| Save  | Ctrl+S |
| Save As  | Ctrl+Shift+S |
| Open | Ctrl+O |
| Settings  | Ctrl+P |
| Exit application | Esc / Ctrl+Q / Alt+X |

Note: you can configure every shortcut listed above by going to __Settings > Controls__

## User interface

The idea is to have a uncluttered, simple and easy to use UI. You can see ui elements only when you need them.

There is a window with image, and a panel with thumbnails at the top (only shows up on mouse hover).

### Using quick copy / quick move panels

Bring up the panel with C or M shortcut. You will see 9 destination directories, click them to set them up.

With panel visible, use 1 - 9 keys to copy/move current image to corresponding directory.

When you are done press C or M again to hide the panel.

## Installation instructions

### Linux

  __Arch:__ Available in AUR ( *qimgv-git* )
    
  __Others distros (build & install via script):__
  
  1. Install dependencies ( git, cmake, qt5, libmpv)
     - Ubuntu & derivatives
		```
		sudo apt install build-essential, cmake, qt5-default, libmpv-dev 
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
  
  
