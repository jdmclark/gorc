#pragma once

#include <boost/filesystem/path.hpp>
#include "base/gui/widgets/dialog_box.h"
#include "base/gui/widgets/grid_panel.h"
#include "base/gui/widgets/static_text.h"
#include "base/gui/widgets/edit_box.h"
#include "base/gui/widgets/button.h"

namespace gorc {
namespace gui {

class gui_view;

namespace widgets {

class file_open_box : public dialog_box {
private:
    grid_panel& content_panel;
    button &ok, &cancel;
    static_text& label;
    edit_box& path;
    std::function<void(const boost::filesystem::path&)> ok_p;

protected:
    void ok_clicked();
    void cancel_clicked();

public:
    void show(std::function<void(const boost::filesystem::path&)>&& ok_p);

    file_open_box(gui_view&, const content::assets::texture& skin, const content::assets::font& font);
};

}
}
}
