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

#include "juce_DirectoryContentsList.h"
#include "../../graphics/imaging/juce_ImageCache.h"
#include "../../../threads/juce_ScopedLock.h"


//==============================================================================
DirectoryContentsList::DirectoryContentsList (const FileFilter* const fileFilter_,
                                              TimeSliceThread& thread_)
   : fileFilter (fileFilter_),
     thread (thread_),
     fileTypeFlags (File::ignoreHiddenFiles | File::findFiles),
     fileFindHandle (0),
     shouldStop (true)
{
}

DirectoryContentsList::~DirectoryContentsList()
{
    clear();
}

void DirectoryContentsList::setIgnoresHiddenFiles (const bool shouldIgnoreHiddenFiles)
{
    setTypeFlags (shouldIgnoreHiddenFiles ? (fileTypeFlags | File::ignoreHiddenFiles)
                                          : (fileTypeFlags & ~File::ignoreHiddenFiles));
}

bool DirectoryContentsList::ignoresHiddenFiles() const
{
    return (fileTypeFlags & File::ignoreHiddenFiles) != 0;
}

//==============================================================================
const File& DirectoryContentsList::getDirectory() const
{
    return root;
}

void DirectoryContentsList::setDirectory (const File& directory,
                                          const bool includeDirectories,
                                          const bool includeFiles)
{
    jassert (includeDirectories || includeFiles); // you have to speciify at least one of these!

    if (directory != root)
    {
        clear();
        root = directory;

        // (this forces a refresh when setTypeFlags() is called, rather than triggering two refreshes)
        fileTypeFlags &= ~(File::findDirectories | File::findFiles);
    }

    int newFlags = fileTypeFlags;
    if (includeDirectories) newFlags |= File::findDirectories;  else newFlags &= ~File::findDirectories;
    if (includeFiles)       newFlags |= File::findFiles;        else newFlags &= ~File::findFiles;

    setTypeFlags (newFlags);
}

void DirectoryContentsList::setTypeFlags (const int newFlags)
{
    if (fileTypeFlags != newFlags)
    {
        fileTypeFlags = newFlags;
        refresh();
    }
}

void DirectoryContentsList::clear()
{
    shouldStop = true;
    thread.removeTimeSliceClient (this);

    fileFindHandle = 0;

    if (files.size() > 0)
    {
        files.clear();
        changed();
    }
}

void DirectoryContentsList::refresh()
{
    clear();

    if (root.isDirectory())
    {
        fileFindHandle = new DirectoryIterator (root, false, "*", fileTypeFlags);
        shouldStop = false;
        thread.addTimeSliceClient (this);
    }
}

//==============================================================================
int DirectoryContentsList::getNumFiles() const
{
    return files.size();
}

bool DirectoryContentsList::getFileInfo (const int index,
                                         FileInfo& result) const
{
    const ScopedLock sl (fileListLock);
    const FileInfo* const info = files [index];

    if (info != 0)
    {
        result = *info;
        return true;
    }

    return false;
}

const File DirectoryContentsList::getFile (const int index) const
{
    const ScopedLock sl (fileListLock);
    const FileInfo* const info = files [index];

    if (info != 0)
        return root.getChildFile (info->filename);

    return File::nonexistent;
}

bool DirectoryContentsList::isStillLoading() const
{
    return fileFindHandle != 0;
}

void DirectoryContentsList::changed()
{
    sendChangeMessage();
}

//==============================================================================
bool DirectoryContentsList::useTimeSlice()
{
    const uint32 startTime = Time::getApproximateMillisecondCounter();
    bool hasChanged = false;

    for (int i = 100; --i >= 0;)
    {
        if (! checkNextFile (hasChanged))
        {
            if (hasChanged)
                changed();

            return false;
        }

        if (shouldStop || (Time::getApproximateMillisecondCounter() > startTime + 150))
            break;
    }

    if (hasChanged)
        changed();

    return true;
}

bool DirectoryContentsList::checkNextFile (bool& hasChanged)
{
    if (fileFindHandle != 0)
    {
        bool fileFoundIsDir, isHidden, isReadOnly;
        int64 fileSize;
        Time modTime, creationTime;

        if (fileFindHandle->next (&fileFoundIsDir, &isHidden, &fileSize,
                                  &modTime, &creationTime, &isReadOnly))
        {
            if (addFile (fileFindHandle->getFile(), fileFoundIsDir,
                         fileSize, modTime, creationTime, isReadOnly))
            {
                hasChanged = true;
            }

            return true;
        }
        else
        {
            fileFindHandle = 0;
        }
    }

    return false;
}

int DirectoryContentsList::compareElements (const DirectoryContentsList::FileInfo* const first,
                                            const DirectoryContentsList::FileInfo* const second)
{
#if JUCE_WINDOWS
    if (first->isDirectory != second->isDirectory)
        return first->isDirectory ? -1 : 1;
#endif

    return first->filename.compareIgnoreCase (second->filename);
}

bool DirectoryContentsList::addFile (const File& file,
                                     const bool isDir,
                                     const int64 fileSize,
                                     const Time& modTime,
                                     const Time& creationTime,
                                     const bool isReadOnly)
{
    if (fileFilter == 0
         || ((! isDir) && fileFilter->isFileSuitable (file))
         || (isDir && fileFilter->isDirectorySuitable (file)))
    {
        ScopedPointer <FileInfo> info (new FileInfo());

        info->filename = file.getFileName();
        info->fileSize = fileSize;
        info->modificationTime = modTime;
        info->creationTime = creationTime;
        info->isDirectory = isDir;
        info->isReadOnly = isReadOnly;

        const ScopedLock sl (fileListLock);

        for (int i = files.size(); --i >= 0;)
            if (files.getUnchecked(i)->filename == info->filename)
                return false;

        files.addSorted (*this, info.release());
        return true;
    }

    return false;
}

END_JUCE_NAMESPACE
