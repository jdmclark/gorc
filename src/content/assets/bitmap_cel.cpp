#include "bitmap_cel.h"

gorc::content::assets::bitmap_cel::bitmap_cel(GLuint color, const box<2, int>& m_size)
	: color(color, m_size) {
	return;
}
