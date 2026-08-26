// Dummy sh1106 drawing primitives
enum pattern { rect_clear };
static inline void sh1106_rectangle(int posX, int posY, int width, int height, enum pattern fill) { }
static inline void sh1106_puts_6x8(int x, int y, const char *s) { puts(s); }
static inline void sh1106_puts_8x16(int x, int y, const char *s) { puts(s); }
static inline void sh1106_graph(int start, int end, int min, int max, int (*fn)(int)) { }
