/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-10 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#include "../../../core/juce_StandardHeader.h"

BEGIN_JUCE_NAMESPACE

#include "juce_ResizableWindow.h"
#include "juce_ComponentPeer.h"
#include "../juce_Desktop.h"
#include "../lookandfeel/juce_LookAndFeel.h"
#include "../../graphics/geometry/juce_RectangleList.h"


//==============================================================================
ResizableWindow::ResizableWindow (const String& name,
                                  const bool addToDesktop_)
    : TopLevelWindow (name, addToDesktop_),
      resizeToFitContent (false),
      fullscreen (false),
      lastNonFullScreenPos (50, 50, 256, 256),
      constrainer (0)
#if JUCE_DEBUG
      , hasBeenResized (false)
#endif
{
    defaultConstrainer.setMinimumOnscreenAmounts (0x10000, 16, 24, 16);

    lastNonFullScreenPos.setBounds (50, 50, 256, 256);

    if (addToDesktop_)
        Component::addToDesktop (getDesktopWindowStyleFlags());
}

ResizableWindow::ResizableWindow (const String& name,
                                  const Colour& backgroundColour_,
                                  const bool addToDesktop_)
    : TopLevelWindow (name, addToDesktop_),
      resizeToFitContent (false),
      fullscreen (false),
      lastNonFullScreenPos (50, 50, 256, 256),
      constrainer (0)
#if JUCE_DEBUG
      , hasBeenResized (false)
#endif
{
    setBackgroundColour (backgroundColour_);

    defaultConstrainer.setMinimumOnscreenAmounts (0x10000, 16, 24, 16);

    if (addToDesktop_)
        Component::addToDesktop (getDesktopWindowStyleFlags());
}

ResizableWindow::~ResizableWindow()
{
    // Don't delete or remove the resizer components yourself! They're managed by the
    // ResizableWindow, and you should leave them alone! You may have deleted them
    // accidentally by careless use of deleteAllChildren()..?
    jassert (resizableCorner == 0 || getIndexOfChildComponent (resizableCorner) >= 0);
    jassert (resizableBorder == 0 || getIndexOfChildComponent (resizableBorder) >= 0);

    resizableCorner = 0;
    resizableBorder = 0;
    contentComponent.deleteAndZero();

    // have you been adding your own components directly to this window..? tut tut tut.
    // Read the instructions for using a ResizableWindow!
    jassert (getNumChildComponents() == 0);
}

int ResizableWindow::getDesktopWindowStyleFlags() const
{
    int styleFlags = TopLevelWindow::getDesktopWindowStyleFlags();

    if (isResizable() && (styleFlags & ComponentPeer::windowHasTitleBar) != 0)
        styleFlags |= ComponentPeer::windowIsResizable;

    return styleFlags;
}

//==============================================================================
void ResizableWindow::setContentComponent (Component* const newContentComponent,
                                           const bool deleteOldOne,
                                           const bool resizeToFit)
{
    resizeToFitContent = resizeToFit;

    if (newContentComponent != static_cast <Component*> (contentComponent))
    {
        if (deleteOldOne)
            contentComponent.deleteAndZero(); // (avoid using a scoped pointer for this, so that it survives
                                              //  external deletion of the content comp)
        else
            removeChildComponent (contentComponent);

        contentComponent = newContentComponent;

        Component::addAndMakeVisible (contentComponent);
    }

    if (resizeToFit)
        childBoundsChanged (contentComponent);

    resized(); // must always be called to position the new content comp
}

void ResizableWindow::setContentComponentSize (int width, int height)
{
    jassert (width > 0 && height > 0); // not a great idea to give it a zero size..

    const BorderSize border (getContentComponentBorder());

    setSize (width + border.getLeftAndRight(),
             height + border.getTopAndBottom());
}

const BorderSize ResizableWindow::getBorderThickness()
{
    return BorderSize (isUsingNativeTitleBar() ? 0 : ((resizableBorder != 0 && ! isFullScreen()) ? 5 : 3));
}

const BorderSize ResizableWindow::getContentComponentBorder()
{
    return getBorderThickness();
}

void ResizableWindow::moved()
{
    updateLastPos();
}

void ResizableWindow::visibilityChanged()
{
    TopLevelWindow::visibilityChanged();

    updateLastPos();
}

void ResizableWindow::resized()
{
    if (resizableBorder != 0)
    {
      #if JUCE_WINDOWS || JUCE_LINUX
        // hide the resizable border if the OS already provides one..
        resizableBorder->setVisible (! (isFullScreen() || isUsingNativeTitleBar()));
      #else
        resizableBorder->setVisible (! isFullScreen());
      #endif

        resizableBorder->setBorderThickness (getBorderThickness());
        resizableBorder->setSize (getWidth(), getHeight());
        resizableBorder->toBack();
    }

    if (resizableCorner != 0)
    {
      #if JUCE_MAC
        // hide the resizable border if the OS already provides one..
        resizableCorner->setVisible (! (isFullScreen() || isUsingNativeTitleBar()));
      #else
        resizableCorner->setVisible (! isFullScreen());
      #endif

        const int resizerSize = 18;
        resizableCorner->setBounds (getWidth() - resizerSize,
                                    getHeight() - resizerSize,
                                    resizerSize, resizerSize);
    }

    if (contentComponent != 0)
        contentComponent->setBoundsInset (getContentComponentBorder());

    updateLastPos();

  #if JUCE_DEBUG
    hasBeenResized = true;
  #endif
}

void ResizableWindow::childBoundsChanged (Component* child)
{
    if ((child == contentComponent) && (child != 0) && resizeToFitContent)
    {
        // not going to look very good if this component has a zero size..
        jassert (child->getWidth() > 0);
        jassert (child->getHeight() > 0);

        const BorderSize borders (getContentComponentBorder());

        setSize (child->getWidth() + borders.getLeftAndRight(),
                 child->getHeight() + borders.getTopAndBottom());
    }
}


//==============================================================================
void ResizableWindow::activeWindowStatusChanged()
{
    const BorderSize border (getContentComponentBorder());

    Rectangle<int> area (getLocalBounds());
    repaint (area.removeFromTop (border.getTop()));
    repaint (area.removeFromLeft (border.getLeft()));
    repaint (area.removeFromRight (border.getRight()));
    repaint (area.removeFromBottom (border.getBottom()));
}

//==============================================================================
void ResizableWindow::setResizable (const bool shouldBeResizable,
                                    const bool useBottomRightCornerResizer)
{
    if (shouldBeResizable)
    {
        if (useBottomRightCornerResizer)
        {
            resizableBorder = 0;

            if (resizableCorner == 0)
            {
                Component::addChildComponent (resizableCorner = new ResizableCornerComponent (this, constrainer));
                resizableCorner->setAlwaysOnTop (true);
            }
        }
        else
        {
            resizableCorner = 0;

            if (resizableBorder == 0)
                Component::addChildComponent (resizableBorder = new ResizableBorderComponent (this, constrainer));
        }
    }
    else
    {
        resizableCorner = 0;
        resizableBorder = 0;
    }

    if (isUsingNativeTitleBar())
        recreateDesktopWindow();

    childBoundsChanged (contentComponent);
    resized();
}

bool ResizableWindow::isResizable() const throw()
{
    return resizableCorner != 0
        || resizableBorder != 0;
}

void ResizableWindow::setResizeLimits (const int newMinimumWidth,
                                       const int newMinimumHeight,
                                       const int newMaximumWidth,
                                       const int newMaximumHeight) throw()
{
    // if you've set up a custom constrainer then these settings won't have any effect..
    jassert (constrainer == &defaultConstrainer || constrainer == 0);

    if (constrainer == 0)
        setConstrainer (&defaultConstrainer);

    defaultConstrainer.setSizeLimits (newMinimumWidth, newMinimumHeight,
                                      newMaximumWidth, newMaximumHeight);

    setBoundsConstrained (getBounds());
}

void ResizableWindow::setConstrainer (ComponentBoundsConstrainer* newConstrainer)
{
    if (constrainer != newConstrainer)
    {
        constrainer = newConstrainer;

        const bool useBottomRightCornerResizer = resizableCorner != 0;
        const bool shouldBeResizable = useBottomRightCornerResizer || resizableBorder != 0;

        resizableCorner = 0;
        resizableBorder = 0;

        setResizable (shouldBeResizable, useBottomRightCornerResizer);

        ComponentPeer* const peer = getPeer();
        if (peer != 0)
            peer->setConstrainer (newConstrainer);
    }
}

void ResizableWindow::setBoundsConstrained (const Rectangle<int>& bounds)
{
    if (constrainer != 0)
        constrainer->setBoundsForComponent (this, bounds, false, false, false, false);
    else
        setBounds (bounds);
}

//==============================================================================
void ResizableWindow::paint (Graphics& g)
{
    getLookAndFeel().fillResizableWindowBackground (g, getWidth(), getHeight(),
                                                    getBorderThickness(), *this);

    if (! isFullScreen())
    {
        getLookAndFeel().drawResizableWindowBorder (g, getWidth(), getHeight(),
                                                    getBorderThickness(), *this);
    }

#if JUCE_DEBUG
    /* If this fails, then you've probably written a subclass with a resized()
       callback but forgotten to make it call its parent class's resized() method.

       It's important when you override methods like resized(), moved(),
       etc., that you make sure the base class methods also get called.

       Of course you shouldn't really be overriding ResizableWindow::resized() anyway,
       because your content should all be inside the content component - and it's the
       content component's resized() method that you should be using to do your
       layout.
    */
    jassert (hasBeenResized || (getWidth() == 0 && getHeight() == 0));
#endif
}

void ResizableWindow::lookAndFeelChanged()
{
    resized();

    if (isOnDesktop())
    {
        Component::addToDesktop (getDesktopWindowStyleFlags());

        ComponentPeer* const peer = getPeer();
        if (peer != 0)
            peer->setConstrainer (constrainer);
    }
}

const Colour ResizableWindow::getBackgroundColour() const throw()
{
    return findColour (backgroundColourId, false);
}

void ResizableWindow::setBackgroundColour (const Colour& newColour)
{
    Colour backgroundColour (newColour);

    if (! Desktop::canUseSemiTransparentWindows())
        backgroundColour = newColour.withAlpha (1.0f);

    setColour (backgroundColourId, backgroundColour);

    setOpaque (backgroundColour.isOpaque());
    repaint();
}

//==============================================================================
bool ResizableWindow::isFullScreen() const
{
    if (isOnDesktop())
    {
        ComponentPeer* const peer = getPeer();
        return peer != 0 && peer->isFullScreen();
    }

    return fullscreen;
}

void ResizableWindow::setFullScreen (const bool shouldBeFullScreen)
{
    if (shouldBeFullScreen != isFullScreen())
    {
        updateLastPos();
        fullscreen = shouldBeFullScreen;

        if (isOnDesktop())
        {
            ComponentPeer* const peer = getPeer();

            if (peer != 0)
            {
                // keep a copy of this intact in case the real one gets messed-up while we're un-maximising
                const Rectangle<int> lastPos (lastNonFullScreenPos);

                peer->setFullScreen (shouldBeFullScreen);

                if ((! shouldBeFullScreen) && ! lastPos.isEmpty())
                    setBounds (lastPos);
            }
            else
            {
                jassertfalse;
            }
        }
        else
        {
            if (shouldBeFullScreen)
                setBounds (0, 0, getParentWidth(), getParentHeight());
            else
                setBounds (lastNonFullScreenPos);
        }

        resized();
    }
}

bool ResizableWindow::isMinimised() const
{
    ComponentPeer* const peer = getPeer();

    return (peer != 0) && peer->isMinimised();
}

void ResizableWindow::setMinimised (const bool shouldMinimise)
{
    if (shouldMinimise != isMinimised())
    {
        ComponentPeer* const peer = getPeer();

        if (peer != 0)
        {
            updateLastPos();
            peer->setMinimised (shouldMinimise);
        }
        else
        {
            jassertfalse;
        }
    }
}

void ResizableWindow::updateLastPos()
{
    if (isShowing() && ! (isFullScreen() || isMinimised()))
    {
        lastNonFullScreenPos = getBounds();
    }
}

void ResizableWindow::parentSizeChanged()
{
    if (isFullScreen() && getParentComponent() != 0)
    {
        setBounds (0, 0, getParentWidth(), getParentHeight());
    }
}

//==============================================================================
const String ResizableWindow::getWindowStateAsString()
{
    updateLastPos();
    return (isFullScreen() ? "fs " : "") + lastNonFullScreenPos.toString();
}

bool ResizableWindow::restoreWindowStateFromString (const String& s)
{
    StringArray tokens;
    tokens.addTokens (s, false);
    tokens.removeEmptyStrings();
    tokens.trim();

    const bool fs = tokens[0].startsWithIgnoreCase ("fs");
    const int firstCoord = fs ? 1 : 0;

    if (tokens.size() != firstCoord + 4)
        return false;

    Rectangle<int> newPos (tokens[firstCoord].getIntValue(),
                           tokens[firstCoord + 1].getIntValue(),
                           tokens[firstCoord + 2].getIntValue(),
                           tokens[firstCoord + 3].getIntValue());

    if (newPos.isEmpty())
        return false;

    const Rectangle<int> screen (Desktop::getInstance().getMonitorAreaContaining (newPos.getCentre()));

    ComponentPeer* const peer = isOnDesktop() ? getPeer() : 0;
    if (peer != 0)
        peer->getFrameSize().addTo (newPos);

    if (! screen.contains (newPos))
    {
        newPos.setSize (jmin (newPos.getWidth(), screen.getWidth()),
                        jmin (newPos.getHeight(), screen.getHeight()));

        newPos.setPosition (jlimit (screen.getX(), screen.getRight() - newPos.getWidth(), newPos.getX()),
                            jlimit (screen.getY(), screen.getBottom() - newPos.getHeight(), newPos.getY()));
    }

    if (peer != 0)
    {
        peer->getFrameSize().subtractFrom (newPos);
        peer->setNonFullScreenBounds (newPos);
    }

    lastNonFullScreenPos = newPos;
    setFullScreen (fs);

    if (! fs)
        setBoundsConstrained (newPos);

    return true;
}

//==============================================================================
void ResizableWindow::mouseDown (const MouseEvent& e)
{
    if (! isFullScreen())
        dragger.startDraggingComponent (this, e);
}

void ResizableWindow::mouseDrag (const MouseEvent& e)
{
    if (! isFullScreen())
        dragger.dragComponent (this, e, constrainer);
}

//==============================================================================
#if JUCE_DEBUG
void ResizableWindow::addChildComponent (Component* const child, int zOrder)
{
    /* Agh! You shouldn't add components directly to a ResizableWindow - this class
       manages its child components automatically, and if you add your own it'll cause
       trouble. Instead, use setContentComponent() to give it a component which
       will be automatically resized and kept in the right place - then you can add
       subcomponents to the content comp. See the notes for the ResizableWindow class
       for more info.

       If you really know what you're doing and want to avoid this assertion, just call
       Component::addChildComponent directly.
   */
    jassertfalse;

    Component::addChildComponent (child, zOrder);
}

void ResizableWindow::addAndMakeVisible (Component* const child, int zOrder)
{
    /* Agh! You shouldn't add components directly to a ResizableWindow - this class
       manages its child components automatically, and if you add your own it'll cause
       trouble. Instead, use setContentComponent() to give it a component which
       will be automatically resized and kept in the right place - then you can add
       subcomponents to the content comp. See the notes for the ResizableWindow class
       for more info.

       If you really know what you're doing and want to avoid this assertion, just call
       Component::addAndMakeVisible directly.
   */
    jassertfalse;

    Component::addAndMakeVisible (child, zOrder);
}
#endif

END_JUCE_NAMESPACE
