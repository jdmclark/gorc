#pragma once

#include "base/io/read_only_file.h"
#include "framework/diagnostics/report.h"
#include "virtual_file.h"
#include "container.h"
#include "content/vfs/episode/episode.h"
#include "base/io/file_system.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <iosfwd>

namespace gorc {
namespace content {
namespace vfs {

typedef std::unordered_map<std::string, const virtual_file*> vfsmap;

class virtual_file_system : public io::file_system {
private:
    class vfs_episode {
    private:
        std::unique_ptr<container> cntr;
        vfsmap FileMap;
        std::unique_ptr<episode::episode> Episode;

    public:
        vfs_episode(io::read_only_file& file, std::unique_ptr<class container>& container, diagnostics::report& report);

        const episode::episode& get_episode() const;
        vfsmap::const_iterator find(const std::string& path) const;
        vfsmap::const_iterator begin() const;
        vfsmap::const_iterator end() const;
    };

    const boost::filesystem::path RestrictedPath;
    const boost::filesystem::path ResourcePath;
    const boost::filesystem::path gamePath;
    bool hasgamePath;

    std::vector<std::unique_ptr<vfs_episode>> episodes;
    vfs_episode* currentEpisode = nullptr;

    vfsmap RestrictedFileMap;
    vfsmap ResourceFileMap;
    vfsmap gameFileMap;

    std::vector<std::unique_ptr<container>> containers;

    void loadGob(const boost::filesystem::path& gobFilePath, vfsmap& map, diagnostics::report& report);
    void loadGobs(const boost::filesystem::path& basepath, vfsmap& map, diagnostics::report& report);

    std::unique_ptr<io::read_only_file> findInGobs(const boost::filesystem::path& path,
            const boost::filesystem::path& basePath, const std::string& generic_string, const vfsmap& map) const;

public:
    virtual_file_system(const boost::filesystem::path& RestrictedPath, const boost::filesystem::path& ResourcePath,
            const boost::filesystem::path& EpisodePath, diagnostics::report& report);
    virtual_file_system(const boost::filesystem::path& RestrictedPath, const boost::filesystem::path& ResourcePath,
            const boost::filesystem::path& EpisodePath, const boost::filesystem::path& gamePath, diagnostics::report& report);

    virtual std::unique_ptr<io::read_only_file> open(const boost::filesystem::path& path) const override;

    inline size_t get_episode_count() const {
        return episodes.size();
    }

    inline const episode::episode& get_episode(size_t index) const {
        return episodes[index]->get_episode();
    }

    inline void set_episode(size_t index) {
        currentEpisode = episodes[index].get();
    }

    inline void clear_episode() {
        currentEpisode = nullptr;
    }

    void list(std::ostream& str) const;
};

}
}
}
