#pragma once

#include "Common/def.h"
SWITCH_WARNINGS_OFF
#include <QPaintEvent>
#include <QPainter>

#include <QString>
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>
#include <QTabWidget>
#include <QStackedLayout>
#include <QScrollArea>
SWITCH_WARNINGS_ON

#include <zfs/zfs.h>
#include <zfs/zfi/zfi_zfs.h>
#include <zfs/zfi/zfi_mmap.h>

#include "QToolsApi.h"

static const int DEFAULT_IMAGE_WIDTH = 128;
static const int DEFAULT_IMAGE_HEIGHT = 128;
static const int MAX_ROW_COUNT = 4;

//-----------------------------------------------------------------------------

class QTOOLS_API ZFSImageFile {
public:
    QString fn;
    double hor = 0.0;
    double vert = 0.0;

    bool operator<(const ZFSImageFile& other) const { return fn.compare(other.fn) < 0; }
};

//-----------------------------------------------------------------------------
/**
* @brief Widget to paint selection handles over a ImageStack.
*/
class QTOOLS_API DotWidget : public QWidget {
    Q_OBJECT

    /**
    * @brief Currently selected image in the stack
    */
    int current;

    /**
    * @brief Number of images in the stack
    */
    int count;

    /**
    * @brief Size of the selection rects
    */
    int rectSize;

    QGridLayout* layout = nullptr;

public:
    DotWidget(QWidget* parent) : QWidget(parent), current(-1), count(-1), rectSize(20) {}

    void setCurrent(int i) {
        current = i;
    }

    void setCount(int i) {
        count = i;
    }

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent* event);

signals:
    /**
    * @brief Signals that the selection of the underlaying image stack has been changed
    */
    void selectionChanged(int selected);

    /**
    * @brief Signals that the user clicked a image stack (even without selecting)
    */
    void activeImage();
};

//-----------------------------------------------------------------------------

class ColorImageWidget;

/**
* @brief Layout to display a stack of images. If more than one image is present, small rects are displayed to
*        allow the user to choose between the images.
*        Currently it is not possible to use ImageStacks and selectable images at the same time!!
*
*/
class QTOOLS_API ImageStack : public QStackedLayout {
    Q_OBJECT

    /**
    * @brief List of ColorImageWidgets in the image stack
    */
#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle
    std::vector<ColorImageWidget*> imageWidgets;
#pragma warning(pop)

    int originalImage = -1;

    /**
    * @brief Index of the currently selected image
    */
    int currentImage = -1;

    /**
    * @brief Widget to display the selection handles
    */
    DotWidget* dotWidget = nullptr;

    /**
    * @brief Flag, if this image stack is selected
    */
    bool selected = false;

    /**
    * @brief Flag, if this image stack is marked
    */
    bool marked = false;

public:
    ImageStack(QWidget* parent);
    virtual ~ImageStack() {}

    /**
    * @brief Add the given ColorImageWidget to the stack
    */
    QString addImage(ColorImageWidget* image);

    /**
    * @brief Get the amount of stacked images
    */
    int getImageCount() const;

    /**
    * @brief Get the data pair for the selected image. It contains the index in the ZFI file and the image name
    */
    std::pair<int, QString> getCurrentImageName();

    /**
    * @brief Returns true if the selected image has changes for this stack.
    */
    bool hasChanged() {
        return currentImage != originalImage;
    }

    /**
    * @brief Get the ColorImageWidget for the active image
    */
    ColorImageWidget* getCurrentImage() {
        return imageWidgets.at(currentImage);
    }

signals:
    /**
    * @brief Signals that this ImageStack has been clicked
    */
    void activeImageStack(ImageStack*);

    // void clicked();

 public slots:
    /**
    * @brief Change selection state and change representation accordingly.
    */
    void setSelected(bool selected);

    /**
    * @brief Change marked state and change representation accordingly.
    */
    void setMarked(bool selected);

    /**
    * @brief Check, if the image stack is marked.
    */
    bool isMarked() const;

    /**
    * @brief Sets the image at the given index to be the current one
    *
    * @param selected the id of the current image
    */
    void changeImage(int selected);

    /**
    * @brief Notify listeners that the current image has been changed
    */
    void notifyActiveImage();
};

/**
 * @brief Class to display a selectable image.
 */
class QTOOLS_API ColorImageWidget : public QLabel
{
    Q_OBJECT

private:
    QHBoxLayout *layout = nullptr;
    QCheckBox* checkBox = nullptr;

public:
    ColorImageWidget(QWidget* parent, const QString& fileName, 
        int width = DEFAULT_IMAGE_WIDTH, 
        int height = DEFAULT_IMAGE_HEIGHT, bool load = true, bool _deleted = false);

    bool deleted = false;

    /**
     * @brief Width of the image widget.
     */
    int imageWidth = -1;

    /**
     * @brief Height of the image widget.
     */
    int imageHeight = -1;

    /**
     * @brief The filename of the image associated with this widget.
     */
    QString fileName = "";

    /**
     * @brief Flag, if this image is selected
     */
    bool selected = false;

    /**
     * @brief Flag, if this image is selectable
     */
    bool selectable = false;

    /**
     * @brief Flag, if this image is marked
     */
    bool marked = false;

    /**
     * @brief Flag, if this image is markable
     */
    bool markable = false;

    /**
     * @brief column of the image coded in the filename
     */
    int col = -1;

    /**
     * @brief row of the image coded in the filename
     */
    int row = -1;

    /**
     * @brief Vertical angle where the image was taken. If this is set to -1, no vertical angle was given
     */
    double verticalAngle = 0;

    /**
     * @brief Horizontal angle where the image was taken
     */
    double horizontalAngle = 0;

    /**
     * @brief index of the image coded in the filename
     */
    int position = -1;

    /**
    * @brief index of the image in the underlying ZFI container
    */
    int zfiIndex = -1;

    /**
     * @brief Loads the image with the filename given in the CTOR.
     */
    void loadImage();

    /**
     * @brief Parse the filename and extract the included position information.
     *
     * The filename has to contain the index and the col and row of the image.
     * Example:
     * <pre>
     * Scan{XX;vert-261.0000;vv--9.0;horz-89.9916;id-COL_ROW;hdr}.jpg
     * </pre>
     * where XX is the index of the image, COL and ROW are the column and the row of the image.
     */
    void parseFilename();

    /**
     * @brief Create the widget from a given QPixmap.
     *
     * Ownership of the given object is not taken!
     *
     * @param pixmap    the pixmap to use to create the widget.
     */
    void createFromPixmap(const QPixmap* pixmap);

protected:
    virtual void mouseReleaseEvent(QMouseEvent* event) {
        QLabel::mouseReleaseEvent(event);
        emit clicked();
    }

signals:
    void clicked();

    public slots:
    /**
     * @brief Toggle selection state and change representation accordingly.
     */
    void setSelected(bool selected);

    /**
     * @brief Enable selection for the widget. It adds a checkbox handling the selection state.
     */
    void setSelectable(bool isSelectable = true);

    /**
     * @brief Set marked state and change representation accordingly.
     */
    void setMarked(bool marked);

signals:
    /**
     * @brief Signals that the selection has been changed
     */
    void selectionChanged();
};

//-----------------------------------------------------------------------------

/**
 * @brief Base class for tabs in the ColorImageSelectionWnd.
 */
#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle
class QTOOLS_API BaseImageDisplayTab : public QWidget
{
    Q_OBJECT

protected:
    int imageWidth;
    int imageHeight;

    /**
    * @brief memory mapped ZFI container
    */
    std::shared_ptr<zfs::Zfi> zfi;

public:
    void resetZFI() {
        zfi = nullptr;
    }

    /**
     * @brief Maximal zoom level
     */
    int MAX_ZOOM_LEVEL;

    /**
     * @brief Info text displayed in the first info cell.
     */
    QString info1;

    /**
     * @brief Info text displayed in the second info cell.
     */
    QString info2;

    /**
     * @brief Info text displayed in the third info cell.
     */
    QString info3;

    /**
     * @brief Current zoom level of the fullscreen image.
     */
    int zoomLevel;

    /**
     * @brief List of ColorImageWidget object pointers containing all available footprints.
     */
    std::vector<ColorImageWidget*> imageList;

    /**
    * @brief List of ImageStack object pointers containing all available image stacks.
    */
    std::vector<ImageStack*> imageStacks;

    /**
     * @brief ColorImageWidget object to store the current full size image.
     *
     * Can be NULL!
     */
    ColorImageWidget* imgFull;

    /**
     * @brief The last clicked or selected ColorImageWidget.
     *
     * Can be NULL!
     */
    ColorImageWidget* currentColorImageWidged;

    /**
     * @brief Flag, if the display is in fullscreen mode or in grid view mode.
     */
    bool fullImageDisplay = false;

    /**
     * @brief The layout of the images. Use loadImages() to fill the layout with ColorImageWidget objects.
     */
    QGridLayout* layout;

    /**
     * @brief The scroll area containing eighter the footprints or the fullscreen image.
     */
    QScrollArea* area;

    /**
     * @brief Basic QWidget to store the images. Images are added to the layout of the widget.
     */
    QWidget* imageWidget;

    /**
     * @brief The associated scan
     */
    QString scanName;

public:
    BaseImageDisplayTab(QWidget* parent, int width = DEFAULT_IMAGE_WIDTH,
        int height = DEFAULT_IMAGE_HEIGHT, std::shared_ptr<zfs::Zfi> zfi = nullptr);
    virtual ~BaseImageDisplayTab();

    /**
     * @brief Load the images stored in the given filelist.
     *
     * New tabs have to implement this to load their images.
     *
     * @param fileNames a list of image filenames.
     */
    virtual QString loadImages(std::map<int, ZFSImageFile> fileNames, int zfi_id = ZFI_ID) = 0;

    /**
     * @brief Create an instance of ColorImageWidget for the given filename and size.
     *
     * The caller owns the new object and has to delete it!
     *
     * @param fileName  the filename to look up
     * @param width     the width of the result image
     * @param height    the height of the result image
     * @param zfi_id    the zfi id (ZFI_ID or ZFI_ID_REMOVED)
     * @param index     the index of the file
     *
     * @return the created instance or NULL
     */
    virtual ColorImageWidget* createColorImageWidget(const QString& fileName,
        int width, int height, int zfi_id = ZFI_ID, int index = 0);

    /**
     * @brief Create an image from raw data.
     *
     * The caller owns the new object and has to delete it!
     *
     * @param rawData    the raw data
     *
     * @return a new QPixmap object
     */
    QPixmap* createImageFromData(const unsigned char* rawData, int length) const;

    /**
     * @brief Displays the given image in fullscreen mode.
     *
     * @param fileName the filename of the image
     */
    virtual void showFullImage(const QString& fileName);

    /**
     * @brief Swaps the display mode to fullscreen of grid view.
     *
     * @param fullScreen if set to true display is changed to fullscreen
     */
    virtual void swapDisplay(bool fullScreen);

    /**
     * @brief Shows the next image based on the last displayed fullscreen image.
     */
    virtual void showNextImage();

    /**
     * @brief Shows the previous image based on the last displayed fullscreen image.
     */
    virtual void showPreviousImage();

    /**
     * @brief Shows the first image stored in the imageList.
     *
     * Works in fullscreen and in grid mode.
     */
    virtual void showFirstImage();

    /**
     * @brief Shows the last image stored in the imageList.
     *
     * Works in fullscreen and in list mode.
     */
    virtual void showLastImage();

    /**
    * @brief Deselect all image.
    */
    virtual void deselectAllImages();

    /**
    * @brief Unmark all image.
    */
    virtual void unmarkAllImages();

    /**
    * @brief Select all image.
    */
    virtual void selectAllImages();

    /**
    * @brief Enable the option to select an image, usually by clicking the checkbox displayed in the
    *        image.
    */
    virtual void setSelectable(bool isSelectable = true);

    /**
     * @brief Zoom the current image in.
     */
    virtual void zoomIn();

    /**
     * @brief Zoom the current image out. If the image is not zoomed, display is swaped to grid view.
     */
    virtual void zoomOut();

    /**
     * @brief Returns the name of this view.
     *
     * @return the name of the view
     */
    virtual QString description() = 0;

    /**
     * @brief Destroys all image widgets.
     */
    virtual void destroyImages();

    /**
     * @brief Get the last ColorImageWidget that was clicked or selected by the user.
     *
     * The view owns the object, do not delete it!
     *
     * @return a ColorImageWidget or NULL
     */
    virtual const ColorImageWidget* getCurrentColorImageWidget() const { return currentColorImageWidged; }

    /**
    * @brief Get a map containing the image names with their index in the ZFI file
    */
    virtual std::map<int, QString> getSelectedImageNames();

    virtual int getSelectedImagesCount();

    /**
    * @brief Get a list of ColorImageWidgets which has been changed
    */
    virtual std::vector<ColorImageWidget*> getChangedImages();

public slots:
    /**
     * @brief Updates status texts with widget information.
     *
     * @param widget the widget to use to update the status information
     */
    virtual void updateStatus(const ColorImageWidget* widget);

    /**
    * @brief Notify listeners that the selected image has been changed.
    */
    void handleUpdateSelectedImages() { emit updateSelectedImages(); }

    /**
    * @brief Mark the next image based on the first marked image.
    */
    void markNextImage(bool row = false);

    /**
    * @brief Mark the previous image based on the first marked image.
    */
    void markPreviousImage(bool row = false);

signals:
    void updateSelectedImages();

    void updateMarkedImage(ImageStack*);

};
#pragma warning(pop)

//-----------------------------------------------------------------------------
