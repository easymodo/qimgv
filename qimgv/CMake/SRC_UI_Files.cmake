## UI Files

set(dir "gui")
file(GLOB SRC_UI_Files ${SRC_UI_Files} ${dir}/*.ui)

set(dir "gui/customwidgets")
file(GLOB SRC_UI_Files ${SRC_UI_Files} ${dir}/*.ui)

set(dir "gui/dialogs")
file(GLOB SRC_UI_Files ${SRC_UI_Files} ${dir}/*.ui)

set(dir "gui/overlays")
file(GLOB SRC_UI_Files ${SRC_UI_Files} ${dir}/*.ui)

set(dir "gui/panels/croppanel")
file(GLOB SRC_UI_Files ${SRC_UI_Files} ${dir}/*.ui)
set(dir "gui/panels/mainpanel")
file(GLOB SRC_UI_Files ${SRC_UI_Files} ${dir}/*.ui)
set(dir "gui/panels/sidepanel")
file(GLOB SRC_UI_Files ${SRC_UI_Files} ${dir}/*.ui)

set(dir "gui/panels/infobar")
file(GLOB SRC_UI_Files ${SRC_UI_Files} ${dir}/*.ui)

set(dir "gui/viewers")
file(GLOB SRC_UI_Files ${SRC_UI_Files} ${dir}/*.ui)

set(dir "gui/folderview")
file(GLOB SRC_UI_Files ${SRC_UI_Files} ${dir}/*.ui)
