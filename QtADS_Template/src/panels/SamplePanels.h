#pragma once

// Registers all built-in sample panels with the PanelRegistry.
// Call this once at startup before creating the MainWindow.
//
// Categories registered:
//   Explorer  - Project Explorer, File Browser, Class View
//   Editor    - Code Editor, Text Editor, Hex Editor
//   Output    - Console, Build Output, Debug Output, Log Viewer, Terminal
//   Properties - Properties, Inspector, Settings, Style Editor
//   Debug     - Watch, Call Stack, Breakpoints, Locals, Threads, Memory, Registers
//   Tools     - Search Results, Bookmarks, TODO List
void registerSamplePanels();
