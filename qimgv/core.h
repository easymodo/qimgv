#pragma once

#include <QObject>
#include <QDebug>
#include <QMutex>
#include <QClipboard>
#include <QDrag>
#include <QFileSystemModel>
#include <QDesktopServices>
#include <QTranslator>
#include "appversion.h"
#include "settings.h"
#include "components/directorymodel.h"
#include "components/directorypresenter.h"
#include "components/scriptmanager/scriptmanager.h"
#include "gui/mainwindow.h"
#include "utils/randomizer.h"
#include "utils/filegrouping.h"
#include "gui/dialogs/printdialog.h"

#ifdef __GLIBC__
#include <malloc.h>
#endif

// the scaler only remembers one pending request, so in split view we have to
// keep track of what each pane asked for and re-issue whatever got dropped
struct PendingScaleRequest {
    bool pending = false;
    QSize size;
    ScalingFilter filter = QI_FILTER_BILINEAR;
};

/* Everything that belongs to a single pane. In split view the two panes browse
 * independently, so each one carries its own copy and switching the focus is
 * just a matter of pointing at the other one.
 */
struct PaneState {
    bool hasImage = false;
    QString filePath = "";
    std::shared_ptr<Image> img;
    PendingScaleRequest scaleRequest;

    void clear() {
        hasImage = false;
        filePath = "";
        img = nullptr;
        scaleRequest.pending = false;
    }
};

// state shared by both panes
struct State {
    bool delayModel = false;
    QString directoryPath = "";
};

enum MimeDataTarget {
    TARGET_CLIPBOARD,
    TARGET_DROP
};

class Core : public QObject {
    Q_OBJECT
public:
    Core();
    void showGui();

public slots:
    void updateInfoString();
    bool loadPath(QString);

private:
    QElapsedTimer t;

    void initGui();
    void initComponents();
    void connectComponents();
    void initActions();
    void loadTranslation();
    void onUpdate();
    void onFirstRun();

    // ui stuff
    MW *mw;

    State state;
    PaneState panes[2];
    // always point at the pane holding the focus frame and at the other one
    PaneState *activePane, *inactivePane;
    void setActivePane(int index);

    bool loopSlideshow, slideshow, shuffle;
    FolderEndAction folderEndAction;

    // components
    std::shared_ptr<DirectoryModel> model;

    DirectoryPresenter thumbPanelPresenter, folderViewPresenter;

    void rotateByDegrees(int degrees);
    void reset();
    bool setDirectory(QString path);

    QDrag *mDrag;
    QMimeData *getMimeDataForImage(std::shared_ptr<Image> img, MimeDataTarget target);
    QTranslator *translator = nullptr;

    Randomizer randomizer;
    void syncRandomizer();

    void attachModel(DirectoryModel *_model);
    QString selectedPath();
    void guiSetImage(std::shared_ptr<Image> img);
    void guiSetImageInactive(std::shared_ptr<Image> img);

    SplitViewMode splitMode = SPLIT_NONE;
    void setSplitViewMode(SplitViewMode mode);
    void requestSplitViewMode(SplitViewMode mode);
    void setGrouping(bool enabled);
    void loadInactiveImage(const QString &path);
    std::shared_ptr<Image> imageForScaling(const PaneState *pane) const;
    QTimer slideshowTimer;

    void startSlideshowTimer();
    void startSlideshow();
    void stopSlideshow();

    bool saveFile(const QString &filePath, const QString &newPath);
    bool saveFile(const QString &filePath);

    std::shared_ptr<ImageStatic> getEditableImage(const QString &filePath);
    QList<QString> currentSelection();

    // `onEdited` gets the image and its size from before the edit was
    // applied, so a crop can be recorded in the coordinates it was
    // drawn in. Leave it empty for edits that can't be replayed as a
    // JPEG transform - lossless tracking is then dropped.
    template<typename... Args>
    void edit_template(bool save, QString actionName,
                        const std::function<QImage*(std::shared_ptr<const QImage>, Args...)>& func,
                        const std::function<void(std::shared_ptr<ImageStatic>, QSize)>& onEdited,
                        Args&&... as);

    static bool isJpegPath(const QString &path);
    bool losslessTrackingApplies(std::shared_ptr<ImageStatic> img);
    void trackLosslessRotate(std::shared_ptr<ImageStatic> img, int degrees);
    void trackLosslessFlip(std::shared_ptr<ImageStatic> img, bool horizontal);
    void trackLosslessCrop(std::shared_ptr<ImageStatic> img, QRect rect, QSize sizeBeforeCrop);

    void doInteractiveCopy(QString path, QString destDirectory, DialogResult &overwriteAllFiles);
    void doInteractiveMove(QString path, QString destDirectory, DialogResult &overwriteAllFiles);

private slots:
    void readSettings();
    void nextImage();
    void prevImage();
    void nextImageSlideshow();
    void jumpToFirst();
    void jumpToLast();
    void onModelItemReady(std::shared_ptr<Image>, const QString&);
    void onModelItemUpdated(QString fileName);
    void onModelSortingChanged(SortingMode mode);
    void onLoadFailed(const QString &path);
    void rotateLeft();
    void rotateRight();
    void close();
    void scalingRequest(QSize, ScalingFilter);
    void scalingRequestInactive(QSize, ScalingFilter);
    void toggleSplitView();
    void splitViewNone();
    void splitViewHorizontal();
    void splitViewVertical();
    void onSplitFocusToggled();
    void onScalingFinished(QPixmap* scaled, ScalerRequest req);
    void copyCurrentFile(QString destDirectory);
    void moveCurrentFile(QString destDirectory);
    void copyPathsTo(QList<QString> paths, QString destDirectory);
    void interactiveCopy(QList<QString> paths, QString destDirectory);
    void interactiveMove(QList<QString> paths, QString destDirectory);
    void movePathsTo(QList<QString> paths, QString destDirectory);
    FileOpResult removeFile(QString fileName, bool trash);
    void onFileRemoved(QString filePath, int index);
    void onFileRenamed(QString fromPath, int indexFrom, QString toPath, int indexTo);
    void onFileAdded(QString filePath);
    void onFileModified(QString filePath);
    void showResizeDialog();
    void resize(QSize size);
    void flipH();
    void flipV();
    void crop(QRect rect);
    void cropAndSave(QRect rect);
    void discardEdits();
    void toggleCropPanel();
    void toggleFullscreenInfoBar();
    void requestSavePath();
    void saveCurrentFile();
    void saveCurrentFileAs(QString);
    void runScript(const QString&);
    void setWallpaper();
    void removePermanent();
    void moveToTrash();
    void reloadImage();
    void reloadImage(QString fileName);
    void copyFileClipboard();
    void copyPathClipboard();
    void openFromClipboard();
    void renameCurrentSelection(QString newName);
    void sortBy(SortingMode mode);
    void sortByName();
    void sortByTime();
    void sortBySize();
    void showRenameDialog();
    void onDraggedOut();
    void onDraggedOut(QList<QString> paths);
    void onDropIn(const QMimeData *mimeData, QObject* source);
    void toggleShuffle();
    void toggleGrouping();
    void groupingOn();
    void groupingOff();
    void onModelLoaded();
    void outputError(const FileOpResult &error) const;
    void showOpenDialog();
    void showInDirectory();
    void onDirectoryViewFileActivated(QString filePath);
    bool loadFileIndex(int index, bool async, bool preload);
    void enableDocumentView();
    void enableFolderView();
    void toggleFolderView();
    void toggleSlideshow();
    void onPlaybackFinished();
    void setFoldersDisplay(bool mode);
    void loadParentDir();
    void nextDirectory();
    void prevDirectory(bool selectLast);
    void prevDirectory();
    void print();
    void modelDelayLoad();
};
