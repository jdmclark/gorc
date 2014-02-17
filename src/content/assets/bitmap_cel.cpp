#include "bitmap_cel.h"

gorc::content::assets::bitmap_cel::bitmap_cel(GLuint color, const box<2, int>& m_size, const box<2, int>& original_size)
    : color(color, m_size), original_size(original_size) {
    return;
}
