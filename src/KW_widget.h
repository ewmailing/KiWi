#ifndef KW_WIDGET_H
#define KW_WIDGET_H

#include "KW_macros.h"
#include "SDL.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief   The KW_Widget type represents a widget on the KW_GUI instance.
 * \details Regardless of the widget type, they are all represented by this opaque type.
 */
struct KW_Widget;
typedef struct KW_Widget KW_Widget;


struct KW_GUI;
typedef struct KW_GUI KW_GUI;

/* forward declarations of these */
extern DECLSPEC SDL_Renderer * KW_GetRenderer(KW_GUI * gui);
extern DECLSPEC KW_GUI * KW_GetGUI(KW_Widget * widget);



typedef void (*KW_WidgetPaintFunction)(KW_Widget * widget);
typedef void (*KW_WidgetDestroyFunction)(KW_Widget * widget);

/**
 * \brief   The KW_WidgetType enumeration represents available widget types.
 * \details Every widget created must set a widget type even if its a custom widget.
 */
typedef enum KW_WidgetType {
  KW_WIDGETTYPE_NONE,
  KW_WIDGETTYPE_FRAME,
  KW_WIDGETTYPE_LABEL,
  KW_WIDGETTYPE_BUTTON,
  KW_WIDGETTYPE_CUSTOM0 = 0x100000,
  KW_WIDGETTYPE_CUSTOM1,
  KW_WIDGETTYPE_CUSTOM2,
  KW_WIDGETTYPE_CUSTOM3,
  KW_WIDGETTYPE_CUSTOM4,
  KW_WIDGETTYPE_CUSTOM5,
  KW_WIDGETTYPE_CUSTOM6,
  KW_WIDGETTYPE_CUSTOM7,
  KW_WIDGETTYPE_CUSTOM8,
  KW_WIDGETTYPE_CUSTOM9,
  KW_WIDGETTYPE_CUSTOM10,
  KW_WIDGETTYPE_CUSTOM11,
  KW_WIDGETTYPE_CUSTOM12,
  KW_WIDGETTYPE_CUSTOM13,
  KW_WIDGETTYPE_CUSTOM14,
  KW_WIDGETTYPE_CUSTOM15,
  KW_WIDGETTYPE_CUSTOM16,
  KW_WIDGETTYPE_CUSTOM17,
  KW_WIDGETTYPE_CUSTOM18,
  KW_WIDGETTYPE_CUSTOM19,
  KW_WIDGETTYPE_CUSTOM20
} KW_WidgetType;


/**
 * \brief   Creates a new custom widget instance.
 * \details If you want to provide a new custom widget this is the function that you should call.
 *          All KW_Widget instances are built using this class, if you should need additional widget 
 *          information you can use the data parameter.
 *          
 *          The widgetpaint function is called to paint the widget after the parent widget has been painted, never
 *          before, so you are garanteed to be rendering on top of it.
 *         
 *          Widget geometry is relative to its parent. Consider the following figure:
 * 
 * \verbatim
 * 
 * 10x10.---------------------------.
 *    |         Root Widget         |
 *    |                             |
 *    |                             |
 *  10x30.------------.             |
 *    |  |Root's Child|             |
 *    |  |            |             |    
 *    |  |  40x20.---------------.  |
 *    |  |       | Child's Child |  |
 *    |  |       |               |  |
 *    |  |       |               |  |
 *    |  |       `---------------´80x40
 *    |  |            |             |
 *    |  `------------´70x90        |
 *    |                             |
 *    `-----------------------------´150x150
 * 
 * Geometry is represented as XxY+WxH.
 * 
 * \endverbatim
 *  
 *          The root widget starts at 10x10. The child widget starts at 10x30 *relative* to the root widget. That means that
 *          its absolute position is actually 20x40! The 'Child's Child' widget starts at 40x20 relative to its parent but its
 *          absolute position is (10 + 10 + 40, 10 + 30 + 20) = 60x60. If the parent widget moves, all its children move together.
 *  
 *          Widgets also have a composed geometry that is calculated by considering each child geometry. They represent the total
 *          area occupied by a widget and its children. In the above picture,
 *          the 'Root's Child' widget composed geometry is calculated by taking each of its children composed geometry and checking
 *          if they are out of the current widget composed geometry. Since 'Child's Child' width and height is out of 'Root's Child'
 *          geometry, 'Root's Child' composed geometry ends up not being 10x30+70x90, but rather 10x30+(40+80+70x90) = 10x30+120x90.
 *          Note the width was updated but not the height.
 * 
 *          The absolute geometry of a widget is calculating by iterating the parent tree and adding their position (x and y).
 * 
 * \param   gui The KW_GUI instace that will hold this widget in its widget tree.
 * \param   parent The parent widget.
 * \param   type The type of this widget.
 * \param   geometry The geometry of this widget *relative* to its parent.
 * \param   widgetpaing The paint function of this widget.
 * \param   widgetdestroy The destroy function of this widget.
 * \param   data Any private data associated with this widget.
 * \returns The new widget.
 */
extern DECLSPEC KW_Widget * KW_CreateWidget(KW_GUI * gui, 
                                              KW_Widget * parent, 
                                              KW_WidgetType type, 
                                              const SDL_Rect * geometry, 
                                              KW_WidgetPaintFunction widgetpaint,
                                              KW_WidgetDestroyFunction widgetdestroy,
                                              void * data);


/**
 * \brief   Change the parent of a widget to another parent.
 * \details Remove \p widget as a child from its current parent and adds it as a child to \p parent.
 *          Every child of \p widget will then belong to the new \p parent tree. Note that \p widget
 *          geometry is **NOT** updated to keep it in the previous absolute position. You should call
 *          KW_SetWidgetGeometry() to update it.
 * 
 *          Keep in mind that this is one of the costliest functions to call in the KiWi library as it
 *          might involve memory reallocation both in the previous and in the new parent widget. The composed
 *          geometry also needs to be updated in both parents in a chain that might go as high as the root widget.
 *          You should avoid widget reparenting if you can.
 * 
 *          To make the widget a top-level widget you should pass \c NULL as the @p parent parameter.
 * 
 * \param   widget The widget that will have its parent changed.
 * \param   parent The new \p widget parent.
 */
extern DECLSPEC void KW_ReparentWidget(KW_Widget * widget, KW_Widget * parent);

/**
 * \brief   Retrieves the parent widget of a widget.
 * \param   widget The KW_Widget instance to retrieve the parent from.
 * \returns The parent widget or NULL if its a top-level widget.
 */
extern DECLSPEC KW_Widget * KW_GetWidgetParent(KW_Widget * widget);

/**
 * \brief   Retrieves the current data associated with a widget.
 * \details If you are implementing a custom widget, you must call this function to access the data you
 *          set up previously when the widget was created.
 * \param   widget The widget to retrieve the data from.
 * \returns The data pointer.
 */
extern DECLSPEC void * KW_GetWidgetData(KW_Widget * widget);

/**
 * \brief   Destroys a widget and free its resources.
 * \details Any custom resource that you set (fonts, custom textures, etc) needs to be freed by you unless specified otherwise.
 * \param   widget The widget to destroy.
 * \param   destroychildren Set to 1 if you want also to destroy all childrens (and their childrens) of this widget. If not, they will be
 *                          reparented to @p widget parent.
 */
extern DECLSPEC void KW_DestroyWidget(KW_Widget * widget, int destroychildren);

/**
 * \brief   Returns the associated KW_GUI instance of a widget.
 * \details If you have a widget instance, this is the means of retrieving its KW_GUI instance.
 * \param   widget The widget to get the KW_GUI instance from.
 * \return  The KW_GUI instance associated with the widget.
 */
extern DECLSPEC KW_GUI * KW_GetWidgetGUI(KW_Widget * widget);

/**
 * \brief   Returns the SDL_Renderer associated with a KW_GUI instance that the @p widget is associated with.
 * \details This is a helper function and its effects are the same of `KW_GetRenderer(KW_GetWidgetGUI(widget))`
 * \param   widget The widget to get the SDL_Renderer instance from.
 * \returns The associated SDL_Renderer instance.
 */
extern DECLSPEC SDL_Renderer * KW_GetWidgetRenderer(KW_Widget * widget);

/**
 * \brief   Sets a new geometry for a widget.
 * \details Use this function if you need to change the widget relative position or its size.
 * \param   widget The widget that will have its size changed.
 * \param   geometry The new widget geometry.
 */
extern DECLSPEC void KW_SetWidgetGeometry(KW_Widget * widget, const SDL_Rect * geometry);

/**
 * \brief   Gets the widget geometry.
 * \details Use this function to retrieve the current *relative* geometry of the widget.
 * \param   widget The widget to get the geometry from.
 * \param   geometry A SDL_Rect pointer that will receive the @p widget geometry. 
 */
extern DECLSPEC void KW_GetWidgetGeometry(KW_Widget * widget, SDL_Rect * geometry);

/**
 * \brief   Gets the absolute widget geometry.
 * \details Use this if you need to know the absolute widget geometry calculated taking its parents in consideration.
 *          It is useful inside the paint function of a widget.
 * \param   widget The widget to get the absolute geometry from.
 * \param   geometry A SDL_Rect pointer that will receive the @p widget absolute geometry.
 */
extern DECLSPEC void KW_GetWidgetAbsoluteGeometry(KW_Widget * widget, SDL_Rect * geometry);

/**
 * \brief   Gets the composed geometry of a widget.
 * \details   Returns the total area occupied by a widget and its children. Note that the position is
 *          still relative to its parent widget.
 * \param   widget The widget to get the compsoed geometry from.
 * \param   geometry A SDL_Rect pointer that will receive the @p widget composed geometry.
 */
extern DECLSPEC void KW_GetWidgetComposedGeometry(KW_Widget * widget, SDL_Rect * composed);

/**
 * \brief   Paint a widget and all of its children unless they are hidden.
 * \details Iterates through all of the @p widget subtree calling their paint functions.
 *          If you wish to paint all widgets of a KW_GUI instance it is better to call
 *          `KW_Paint()` rather than this function.
 * \param   widget The widget to paint.
 */
extern DECLSPEC void KW_PaintWidget(KW_Widget * widget);

extern DECLSPEC void KW_BlockWidgetEvents(KW_Widget * widget, int block);

#ifdef __cplusplus
}
#endif

#endif