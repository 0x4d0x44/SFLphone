/*
 *  Copyright (C) 2005-2007 Savoir-Faire Linux inc.
 *  Author: Alexandre Bourget <alexandre.bourget@savoirfairelinux.com>
 *  Author: Yan Morin <yan.morin@savoirfairelinux.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "config.h"
#include "../global.h"
#include <fstream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

namespace Conf
{

// ctor
ConfigTree::ConfigTree() :_sections()
{
}

// dtor
ConfigTree::~ConfigTree()
{
    // erase every new ItemMap (by CreateSection)
    SectionMap::iterator iter = _sections.begin();

    while (iter != _sections.end()) {
        delete iter->second;
        iter->second = NULL;
        iter++;
    }
}

/**
 * Create the section only if it doesn't exists
 */
void
ConfigTree::createSection (const std::string& section)
{
    // if we doesn't find the item, create it
    if (_sections.find (section) == _sections.end()) {
        _sections[section] = new ItemMap;
    }
}

/**
 * Remove the section only if it exists
 */
void
ConfigTree::removeSection (const std::string& section)
{
    // if we doesn't find the item, create it
    SectionMap::iterator iter = _sections.find (section);

    if (iter != _sections.end()) {
        _sections.erase (iter);
    }
}

/** Retrieve the sections as an array */
TokenList
ConfigTree::getSections()
{
    TokenList sections;

    SectionMap::iterator iter = _sections.begin();

    while (iter != _sections.end()) {
        // add to token list the: iter->second;
        sections.push_back (iter->first);
        iter++;
    }

    return sections;
}


/**
 * Add the config item only if it exists..
 * If the section doesn't exists, create it
 */
void
ConfigTree::addConfigTreeItem (const std::string& section, const ConfigTreeItem item)
{
    // if we doesn't find the item, create it
    SectionMap::iterator iter = _sections.find (section);

    if (iter == _sections.end()) {
        _sections[section] = new ItemMap;
        iter = _sections.find (section);
    }

    // be prudent here
    if (iter != _sections.end()) {
        std::string name = item.getName();

        if (iter->second->find (name) == iter->second->end()) {
            (* (iter->second)) [name] = item;
        }
    }
}

// throw a ConfigTreeItemException if not found
std::string
ConfigTree::getConfigTreeItemValue (const std::string& section, const std::string& itemName)
{
    ConfigTreeItem* item = getConfigTreeItem (section, itemName);

    if (item != NULL) {
        return item->getValue();
    } else {
        _debug ("Option doesn't exist: [%s] %s\n", section.c_str(), itemName.c_str());
        /** @todo If item doesn't exist, we should check against the default values for those
         * types of information, and return the default value.
         * ...
         * Maybe this should be implemented when called ? When we need a bit of configuration,
         * we call the getConfig with a defaultValue as parameter, in that context we know best
         * what would be the default value, rather than inside this generic configuration
         * management class.
         */
    }

    return "";
}

// throw a ConfigTreeItemException if not found
int
ConfigTree::getConfigTreeItemIntValue (const std::string& section, const std::string& itemName)
{
    std::string configItem = getConfigTreeItemValue (section, itemName);
    int retval = atoi (configItem.data());

    return retval;
}

bool
ConfigTree::getConfigTreeItemToken (const std::string& section, const std::string& itemName, TokenList& arg)
{
    ConfigTreeItem *item = getConfigTreeItem (section, itemName);

    if (item) {
        arg.clear();
        arg.push_back (section);
        arg.push_back (itemName);
        arg.push_back (item->getType());
        arg.push_back (item->getValue());
        arg.push_back (item->getDefaultValue());
        return true;
    }

    return false;
}



/**
 * Return a ConfigTreeItem or NULL if not found
 */
ConfigTreeItem*
ConfigTree::getConfigTreeItem (const std::string& section, const std::string& itemName)
{
    SectionMap::iterator iter = _sections.find (section);

    if (iter == _sections.end()) {
        return NULL;
    }

    ItemMap::iterator iterItem = iter->second->find (itemName);

    if (iterItem == iter->second->end()) {
        return NULL;
    }

    return & (iterItem->second);
}

/**
 * Set the configItem if found, if not, *CREATE IT*
 *
 * @todo Élimier les 45,000 classes qui servent à rien pour Conf.
 */
bool
ConfigTree::setConfigTreeItem (const std::string& section,
                               const std::string& itemName,
                               const std::string& value)
{

    SectionMap::iterator iter = _sections.find (section);

    if (iter == _sections.end()) {
        // Not found, create section
        _sections[section] = new ItemMap;
        iter = _sections.find (section);
    }

    ItemMap::iterator iterItem = iter->second->find (itemName);

    if (iterItem == iter->second->end()) {
        // Item not found, create it, defaults to type "string"
        addConfigTreeItem (section, ConfigTreeItem (itemName, value, "string"));
        return true;
    }

    iterItem->second.setValue (value);

    return true;
}

// Save config to a file (ini format)
// return false if empty, no config, or enable to open
// return true if everything is ok
bool
ConfigTree::saveConfigTree (const std::string& fileName)
{
    if (fileName.empty() && _sections.begin() == _sections.end()) {
        return false;
    }

    std::fstream file;

    file.open (fileName.data(), std::fstream::out);

    if (!file.is_open()) {
        return false;
    }

    // for each section, for each item...
    SectionMap::iterator iter = _sections.begin();

    while (iter != _sections.end()) {
        file << "[" << iter->first << "]" << std::endl;
        ItemMap::iterator iterItem = iter->second->begin();

        while (iterItem != iter->second->end()) {
            file << iterItem->first << "=" << iterItem->second.getValue() << std::endl;
            iterItem++;
        }

        file << std::endl;

        iter++;
    }

    file.close();

    if (chmod (fileName.c_str(), S_IRUSR | S_IWUSR)) {
        _debug ("Failed to set permission on configuration file because: %s\n",strerror (errno));
    }

    return true;
}

// Create the tree from an existing ini file
// 0 = error
// 1 = OK
// 2 = unable to open
int
ConfigTree::populateFromFile (const std::string& fileName)
{
    bool out = false;

    if (fileName.empty()) {
        return 0;
    }

    std::fstream file;

    file.open (fileName.data(), std::fstream::in);

    if (!file.is_open()) {
        file.open (fileName.data(), std::fstream::out);
        out = true;

        if (!file.is_open()) {
            return 0;
        }

        file.close();

        return 2;
    }

    // get length of file:
    file.seekg (0, std::ios::end);

    int length = file.tellg();

    file.seekg (0, std::ios::beg);

    if (length == 0) {
        file.close();
        return 2; // should load config
    }

    std::string line;

    std::string section ("");
    std::string key ("");
    std::string val ("");
    std::string::size_type pos;

    while (!file.eof()) {
        // Read the file line by line
        std::getline (file, line);

        if (!line.empty()) {
            if (line[0] == '[') {
                // If the line is a section
                pos = line.find (']');
                section = line.substr (1, pos - 1);
            } else if (line[0] != '#') {
                // If the line is "key=value" and doesn't begin with '#'(comments)

                pos = line.find ('=');
                key = line.substr (0, pos);
                val = line.substr (pos + 1, line.length() - pos);

                if (key.length() > 0 && val.length() > 0) {
                    setConfigTreeItem (section, key, val);
                }
            }
        }
    }

    file.close();

    if (chmod (fileName.c_str(), S_IRUSR | S_IWUSR)) {
        _debug ("Failed to set permission on configuration file because: %s\n",strerror (errno));
    }

    return 1;
}

TokenList
ConfigTreeIterator::begin()
{
    TokenList tk;
    _iter = _tree->_sections.begin();

    if (_iter!=_tree->_sections.end()) {
        _iterItem = _iter->second->begin();

        if (_iterItem!=_iter->second->end()) {
            tk.push_back (_iter->first);
            tk.push_back (_iterItem->first);
            tk.push_back (_iterItem->second.getType());
            tk.push_back (_iterItem->second.getValue());
            tk.push_back (_iterItem->second.getDefaultValue());
        }
    }

    return tk;
}

TokenList
ConfigTreeIterator::next()
{
    TokenList tk;
    // we return tk empty if we are at the end of the list...

    if (_iter==_tree->_sections.end()) {
        return tk;
    }

    if (_iterItem!=_iter->second->end()) {
        _iterItem++;
    }

    if (_iterItem==_iter->second->end()) {
        // if we increment, and we are at the end of a section
        _iter++;

        if (_iter!=_tree->_sections.end()) {
            _iterItem = _iter->second->begin();

            if (_iterItem!=_iter->second->end()) {
                tk.push_back (_iter->first);
                tk.push_back (_iterItem->first);
                tk.push_back (_iterItem->second.getType());
                tk.push_back (_iterItem->second.getValue());
                tk.push_back (_iterItem->second.getDefaultValue());
            }
        }
    } else {
        tk.push_back (_iter->first);
        tk.push_back (_iterItem->first);
        tk.push_back (_iterItem->second.getType());
        tk.push_back (_iterItem->second.getValue());
        tk.push_back (_iterItem->second.getDefaultValue());
    }

    return tk;
}

} // end namespace ConfigTree


