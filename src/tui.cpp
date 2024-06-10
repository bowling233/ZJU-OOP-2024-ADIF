
#include <stddef.h>   // for size_t
#include <array>      // for array
#include <atomic>     // for atomic
#include <chrono>     // for operator""s, chrono_literals
#include <cmath>      // for sin
#include <functional> // for ref, reference_wrapper, function
#include <memory>     // for allocator, shared_ptr, __shared_ptr_access
#include <string>     // for string, basic_string, char_traits, operator+, to_string
#include <thread>     // for sleep_for, thread
#include <utility>    // for move
#include <vector>     // for vector

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/table.hpp"
#include "adif.hpp"

using namespace ftxui;

int main()
{
    auto screen = ScreenInteractive::Fullscreen();

    // ---------------------------------------------------------------------------
    // FileList
    // ---------------------------------------------------------------------------

    bool FileList_dialog_shown = false;

    // Depth 0
    // Options
    std::vector<std::string> FileList_options = {
        "Read",
        "Merge",
        "Clean",
    };
    int FileList_options_selected = 0;
    MenuOption FileList_options_menu_option;
    FileList_options_menu_option.on_enter = [&]
    {
        switch (FileList_options_selected)
        {
        case 0:
        case 1:
            // Pop up a window to input file name
            break;
        case 2:
            // Clear the file list
            break;
        }
        FileList_dialog_shown = true;
    };
    auto FileList_options_menu = Menu(&FileList_options, &FileList_options_selected, FileList_options_menu_option);
    // Files
    std::vector<std::string> FileList_opened_files = {
        "file1.txt",
        "file2.txt",
        "file3.txt",
    };
    auto renderFileList = [&](const std::vector<std::string> &FileList_opened_files)
    {
        std::vector<Element> elements;
        for (const auto &file : FileList_opened_files)
        {
            elements.push_back(text(file));
        }
        return vbox(std::move(elements)) | frame | size(HEIGHT, LESS_THAN, 5) | size(WIDTH, EQUAL, 20) ;
    };

    // Depth 1
    // Input
    std::string FileList_filename = "";
    InputOption FileList_filename_option;
    FileList_filename_option.on_enter = [&]()
    {
        switch (FileList_options_selected)
        {
        case 0:
            // Read the file
            break;
        case 1:
            // Merge the file
            break;
        }
        FileList_dialog_shown = false;
    };
    Component FileList_input = Input(&FileList_filename, "Input File Name", FileList_filename_option);
    // Input dialog
    FileList_input |= Renderer([&](Element inner)
                               {
                                   if (FileList_dialog_shown)
                                   {
                                       return window(text("Input File Name"), inner);
                                   }
                                   return Element();
                               });
    auto FileList_dialog = Modal(FileList_input, &FileList_dialog_shown);

    // Compose and render
    auto FileList_component = Container::Vertical({
        FileList_options_menu,
    });

    FileList_component |= FileList_dialog;

    auto FileList_renderer = Renderer(FileList_component, [&]
                                      { return window(text("FileList"), hbox({
                                                                            FileList_options_menu->Render() | size(HEIGHT, EQUAL, 3) | frame | size(WIDTH, EQUAL, 10),
                                                                            separator(),
                                                                            renderFileList(FileList_opened_files) | vscroll_indicator | frame,
                                                                        })) | size(WIDTH, EQUAL, 30) | size(HEIGHT, EQUAL, 10); });

    FileList_renderer |= FileList_dialog;

    // ---------------------------------------------------------------------------
    // Table
    // ---------------------------------------------------------------------------

     adif::Document adifdoc;
     adifdoc.Open("../examples/test0.adif");

    auto table = Table(adifdoc.GetTable());
    //auto table = Table({{"a"}, {"b"}});

    table.SelectAll().Border(LIGHT);

    // Add border around the first column.
    table.SelectColumn(0).Border(LIGHT);

    // Make first row bold with a double border.
    table.SelectRow(0).Decorate(bold);
    table.SelectRow(0).SeparatorVertical(LIGHT);
    table.SelectRow(0).Border(DOUBLE);

    // Align right the "Release date" column.
    table.SelectColumn(2).DecorateCells(align_right);

    // Select row from the second to the last.
    auto content = table.SelectRows(1, -1);
    // Alternate in between 3 colors.
    content.DecorateCellsAlternateRow(color(Color::Blue), 3, 0);
    content.DecorateCellsAlternateRow(color(Color::Cyan), 3, 1);
    content.DecorateCellsAlternateRow(color(Color::White), 3, 2);

    auto document = table.Render();

    auto exit_button = Renderer([](bool focused)
                                {
    if (focused)
      return text("Exit") | center | bold | border;
    else
      return text(" Focusable renderer() ") | center | border; });

    // auto exit_button = Button("Exit", [&] { screen.Exit(); }, ButtonOption::Animated());

    // Component Architecture
    auto main_container = Container::Vertical({
        exit_button,
        FileList_component,
    });

    // How to render the component tree.
    auto main_renderer = Renderer(main_container, [&]
                                  { return vbox({
                                        text("ADIF Data Process Software by ZhuBaolin") | bold | hcenter,
                                        exit_button->Render(),
                                        FileList_renderer->Render() | flex,
                                        document | flex,
                                    }); }) ;

    screen.Loop(main_renderer);
    return 0;
}
