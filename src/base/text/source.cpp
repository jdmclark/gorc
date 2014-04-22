#include "source.h"

gorc::text::source::source(const boost::filesystem::path& filename, gorc::io::read_only_file& file)
    : index(0), filename(filename) {
    size_t fsize = file.size();
    if(fsize == 0) {
        return;
    }

    buffer.resize(fsize, 0);
    file.read(&buffer[0], fsize);
}

gorc::text::source::source(const std::string& str)
    : index(0), filename("internal buffer") {
    buffer.assign(str.begin(), str.end());
    return;
}
