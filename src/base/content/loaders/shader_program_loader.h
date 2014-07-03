#pragma once

#include "base/content/loader.h"
#include <GL/gl.h>

namespace gorc {
namespace content {
namespace loaders {

class shader_program_loader : public loader {
private:
    GLuint program_type;

public:
    static const std::vector<boost::filesystem::path> asset_root_path;

    virtual std::unique_ptr<asset> deserialize(const boost::filesystem::path& filename,
                                               io::read_only_file& file,
                                               content_manager& manager,
                                               diagnostics::report& report) override;

    shader_program_loader(GLuint program_type);
};

}
}
}
