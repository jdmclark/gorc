#include "image.hpp"
#include <SFML/Graphics/Image.hpp>

void gorc::save_image_to_file(image const &img, path const &out)
{
    sf::Image compiled_img;
    compiled_img.create(static_cast<unsigned int>(get<0>(img.size)),
                        static_cast<unsigned int>(get<1>(img.size)));

    for(size_t y = 0; y < get<1>(img.size); ++y) {
        for(size_t x = 0; x < get<0>(img.size); ++x) {
            auto pixel = img.get(x, y);
            sf::Color new_pixel(get<0>(pixel), get<1>(pixel), get<2>(pixel), get<3>(pixel));
            compiled_img.setPixel(static_cast<unsigned int>(x),
                                  static_cast<unsigned int>(y),
                                  new_pixel);
        }
    }

    compiled_img.saveToFile(out.generic_string());
}
