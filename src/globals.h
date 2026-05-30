#ifndef GLOBALS_H
#define GLOBALS_H

#include "book.h"
#include <string>

// Global book manager instance
inline BookManager g_book_manager;

// Global book settings
inline bool g_use_book = true;
inline std::string g_book_path = "books/book.bin";
inline bool g_use_default_book = true;

// Book initialization - loads the default opening book
inline void init_book_manager() {
    g_book_manager.setDefaultBookPath("books/book.bin");
    if (g_use_default_book) {
        g_book_manager.loadBook("books/book.bin");
    }
}

// UCI option handlers
inline void set_use_book(bool use) {
    g_use_book = use;
}

inline void set_book_path(const std::string& path) {
    g_book_path = path;
    if (g_book_manager.isBookLoaded()) {
        g_book_manager.clearBook();
    }
    g_book_manager.setBookPath(path);
    g_book_manager.loadBook(path);
}

inline void set_use_default_book(bool use) {
    g_use_default_book = use;
    if (use) {
        g_book_manager.loadBook("books/book.bin");
    } else {
        g_book_manager.clearBook();
    }
}

inline bool get_use_book() {
    return g_use_book;
}

inline bool get_use_default_book() {
    return g_use_default_book;
}

inline std::string get_book_path() {
    return g_book_path;
}

// Get book move for current position
inline Move get_book_move(const Position& pos) {
    if (!g_use_book || !g_book_manager.isBookLoaded()) {
        return 0;
    }
    auto result = g_book_manager.getBookMove(pos);
    return result.value_or(Move(0));
}

#endif // GLOBALS_H