#include "book.h"
#include "fen.h"
#include "display.h"
#include "tt.h"
#include "zobrist.h"
#include "bitscan.h"
#include "overloading.h"
#include "movegen.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <random>
#include <cstring>
#include <vector>

// Default opening book file path (Polyglot format)
constexpr const char* DEFAULT_BOOK_PATH = "books/book.bin";

BookManager::BookManager()
    : defaultBookPath_(DEFAULT_BOOK_PATH)
    , bookPath_()
    , useDefaultBook_(true)
    , isPolyglot_(false)
    , bookType_(BookType::None)
{
}

BookManager::~BookManager()
{
    clearBook();
}

void BookManager::clearBook()
{
    entries_.clear();
    bookPath_.clear();
    isPolyglot_ = false;
    bookType_ = BookType::None;
}

bool BookManager::loadBook(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[Book] Error: Cannot open file: " << filePath << std::endl;
        return false;
    }

    // Get file size
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    if (fileSize < 16) {
        std::cerr << "[Book] Error: File too small (" << fileSize << " bytes): " << filePath << std::endl;
        return false;
    }

    // Detect format: Polyglot (binary) vs EPD (text)
    // Read first 32 bytes to check for text content
    unsigned char header[32];
    file.read(reinterpret_cast<char*>(header), std::min(fileSize, static_cast<std::streamsize>(32)));

    bool isText = true;
    for (int i = 0; i < 32; i++) {
        if (header[i] == 0) {
            isText = false;
            break;
        }
        if (header[i] > 128) {
            isText = false;
            break;
        }
    }

    if (isText) {
        // It's a text file (EPD format)
        file.close();
        return loadEPDBook(filePath);
    }

    // Assume Polyglot binary format
    file.close();

    // Verify it looks like a Polyglot file by checking for "pglb" magic
    std::ifstream verifyFile(filePath, std::ios::binary);
    if (!verifyFile.is_open()) {
        std::cerr << "[Book] Error: Cannot verify Polyglot file: " << filePath << std::endl;
        return false;
    }

    char magicCheck[4];
    verifyFile.read(magicCheck, 4);
    verifyFile.close();

    if (magicCheck[0] == 'p' && magicCheck[1] == 'g' && magicCheck[2] == 'l' && magicCheck[3] == 'b') {
        // Valid Polyglot magic number
        bookPath_ = filePath;
        isPolyglot_ = true;
        bookType_ = BookType::Polyglot;
        std::cout << "info string Book: Loaded Polyglot book: " << filePath << std::endl;
        return true;
    }

    // No magic number but still binary - treat as raw Polyglot entries
    bookPath_ = filePath;
    isPolyglot_ = true;
    bookType_ = BookType::Polyglot;
    std::cout << "info string Book: Loaded binary book: " << filePath << std::endl;
    return true;
}

bool BookManager::loadEPDBook(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[Book] Error: Cannot open file: " << filePath << std::endl;
        return false;
    }

    std::string line;
    int lineNum = 0;

    while (std::getline(file, line)) {
        lineNum++;

        // Skip empty lines and comments
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }

        // Remove trailing semicolon if present (standard EPD ends with ;)
        if (!line.empty() && line.back() == ';') {
            line.pop_back();
            // Trim trailing whitespace
            while (!line.empty() && (line.back() == ' ' || line.back() == '\t')) {
                line.pop_back();
            }
        }

        EPDEntry entry;
        entry.polyglotKey = 0;  // EPD entries don't have Polyglot keys

        if (parseEPDLine(line, entry)) {
            entries_.push_back(entry);
        }
    }

    file.close();

    std::cout << "info string Book: Loaded " << entries_.size() << " positions from " << filePath << std::endl;

    bookPath_ = filePath;
    isPolyglot_ = false;
    bookType_ = BookType::EPD;
    return true;
}

int64_t BookManager::binarySearchKey(std::ifstream& file, uint64_t targetKey) const
{
    // Get file size
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    if (fileSize < 16) {
        return -1;
    }

    int64_t left = 0;
    int64_t right = (fileSize / 16) - 1;
    int64_t result = -1;

    while (left <= right) {
        int64_t mid = left + (right - left) / 2;

        // Seek to the key position in the entry
        file.seekg(static_cast<std::streamsize>(mid * 16), std::ios::beg);
        if (!file.good()) {
            return -1;
        }

        uint64_t fileKey;
        file.read(reinterpret_cast<char*>(&fileKey), sizeof(uint64_t));

        if (!file.good()) {
            return -1;
        }

        // Convert from big-endian to native
        uint64_t nativeKey = be64ToNative(fileKey);

        if (nativeKey == targetKey) {
            result = static_cast<int64_t>(mid);
            // Continue searching to find the first occurrence
            right = mid - 1;
        } else if (nativeKey < targetKey) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return result;
}

std::optional<Move> BookManager::getBookMove(const Position& pos, int depth) const
{
    // If no book is loaded, return no move
    if (!isBookLoaded()) {
        return std::nullopt;
    }

    // Compute the Polyglot key for the current position using official Polyglot hashing
    uint64_t targetKey = generatePolyglotKey(pos);

    if (!isPolyglot_) {
        // EPD book - use existing vector-based lookup
        for (const auto& entry : entries_) {
            if (entry.polyglotKey == targetKey) {
                return static_cast<Move>(entry.move);
            }
        }
        return std::nullopt;
    }

    // Polyglot binary book - use O(log n) binary search
    std::ifstream file(bookPath_, std::ios::binary);
    if (!file.is_open()) {
        return std::nullopt;
    }

    // Find the first entry with matching key using binary search
    int64_t firstIdx = binarySearchKey(file, targetKey);
    if (firstIdx < 0) {
        return std::nullopt;
    }

    // Get file size to know bounds
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Scan backward to find the first entry with this key
    int64_t startIdx = firstIdx;
    while (startIdx > 0) {
        file.seekg(static_cast<std::streamsize>((startIdx - 1) * 16), std::ios::beg);
        uint64_t prevKey;
        file.read(reinterpret_cast<char*>(&prevKey), sizeof(uint64_t));
        if (be64ToNative(prevKey) != targetKey) {
            break;
        }
        startIdx--;
    }

    // Collect all entries with the matching key and their weights
    struct MoveWeight {
        uint16_t move;
        uint16_t weight;
    };
    std::vector<MoveWeight> moves;
    uint64_t totalWeight = 0;

    for (int64_t idx = startIdx; idx * 16 < fileSize; idx++) {
        file.seekg(static_cast<std::streamsize>(idx * 16), std::ios::beg);
        PolyglotEntry entry;
        file.read(reinterpret_cast<char*>(&entry), sizeof(PolyglotEntry));

        if (!file.good() || static_cast<std::streamsize>(idx * 16 + 16) > fileSize) {
            break;
        }

        if (be64ToNative(entry.key) != targetKey) {
            break;
        }

        moves.push_back({be16ToNative(entry.move), be16ToNative(entry.weight)});
        totalWeight += entry.weight;
    }

    file.close();

    if (moves.empty()) {
        return std::nullopt;
    }

    // Select a random move based on weights using std::mt19937 and std::random_device
    std::random_device rd;
    std::mt19937_64 gen(static_cast<uint64_t>(rd()));
    std::uniform_int_distribution<uint64_t> dist(0, totalWeight > 0 ? totalWeight - 1 : 0);

    uint64_t selected = dist(gen);
    uint64_t cumulative = 0;

    for (const auto& mw : moves) {
        cumulative += mw.weight;
        if (selected < cumulative) {
            // Decode the Polyglot move
            return decodePolyglotMove(mw.move, pos);
        }
    }

    // Fallback to the last move
    return decodePolyglotMove(moves.back().move, pos);
}

std::optional<Move> BookManager::getWeightedBookMove(const Position& pos) const
{
    // Delegate to getBookMove which now handles both EPD and Polyglot formats
    return getBookMove(pos, 0);
}

uint64_t BookManager::generatePolyglotKey(const Position& pos) const
{
    uint64_t key = 0;

    static const int pgBase[6] = { 5, 4, 3, 2, 1, 0 };

    std::cout << "FEN: " << positionToFEN(pos) << std::endl;

    // Pezzi
    for (int c = 0; c <= 1; c++) {
        Color color = static_cast<Color>(c);
        for (int p = 0; p < 6; p++) {
            Piece piece = static_cast<Piece>(p);
            int pgIdx = pgBase[p] * 2 + c;
            Bitboard bb = pos.getPieces(color, piece);
            while (bb) {
                Square sq = static_cast<Square>(bitscan_reset(bb));
                key ^= PG.Zobrist.psq[pgIdx][sq];
            }
        }
    }

    // Arrocco
    Castle whiteCastle = pos.getCastle(WHITE);
    Castle blackCastle = pos.getCastle(BLACK);
    if (whiteCastle == KINGSIDE || whiteCastle == ALL) key ^= PG.Zobrist.castle[0];
    if (whiteCastle == QUEENSIDE || whiteCastle == ALL) key ^= PG.Zobrist.castle[1];
    if (blackCastle == KINGSIDE || blackCastle == ALL) key ^= PG.Zobrist.castle[2];
    if (blackCastle == QUEENSIDE || blackCastle == ALL) key ^= PG.Zobrist.castle[3];

    // En passant
    Square ep = pos.getEnPassant();
    if (ep != SQ_EMPTY) {
        int epFile = ep % 8;
        Color turn = pos.getTurn();
        Bitboard enemyPawns = pos.getPieces(turn, PAWN);
        bool canCapture = false;
        if (epFile > 0 && (enemyPawns & (C64(1) << (ep - 1)))) canCapture = true;
        if (epFile < 7 && (enemyPawns & (C64(1) << (ep + 1)))) canCapture = true;
        if (canCapture)
            key ^= PG.Zobrist.enpassant[epFile];
    }

    // Turno
    if (pos.getTurn() == WHITE)
        key ^= PG.Zobrist.turn;

    return key;
}

bool BookManager::parseEPDLine(const std::string& line, EPDEntry& entry) const
{
    std::istringstream iss(line);
    std::string token;
    int field = 0;
    bool fenComplete = false;

    // Parse the EPD line field by field
    // Standard EPD: r1bqkb1r/pppp1ppp/2n2n2/4P3/4P3/2N2N2/PPPP1PPP/R1BQKB1R w KQkq - 0 1 move a3a4 100;
    while (iss >> token) {
        if (!fenComplete) {
            // Collect tokens until we have a complete FEN (6 parts)
            if (!entry.fen.empty()) {
                entry.fen += " ";
            }
            entry.fen += token;
            field++;
            if (field == 6) {
                fenComplete = true;
            }
        } else {
            // After FEN, we have alternating move weight pairs
            // We'll parse these later
            entry.move = parseAlgebraicMove(token);
            // Next token should be the weight
            if (iss >> token) {
                try {
                    entry.weight = static_cast<uint16_t>(std::stoi(token));
                } catch (...) {
                    entry.weight = 1000;
                }
            }
            // EPD entries don't have games, whiteWins, blackWins, draws, polyglotKey
            entry.games = 0;
            entry.whiteWins = 0;
            entry.blackWins = 0;
            entry.draws = 0;
            entry.polyglotKey = 0;
            break;
        }
    }

    return fenComplete;
}

uint32_t BookManager::parseAlgebraicMove(const std::string& moveStr) const
{
    uint32_t move = 0;
    std::string cleanMove = moveStr;

    // Remove capture symbol if present
    size_t xPos = cleanMove.find('x');
    if (xPos != std::string::npos) {
        cleanMove.erase(xPos, 1);
    }

    // Handle castling
    if (cleanMove == "O-O" || cleanMove == "0-0") {
        // Kingside castling - encode as special value
        move = 0x0001;  // Special castling marker
        return move;
    }
    if (cleanMove == "O-O-O" || cleanMove == "0-0-0") {
        // Queenside castling
        move = 0x0002;  // Special castling marker
        return move;
    }

    // Handle promotion (remove promotion indicator)
    size_t promoPos = cleanMove.find('=');
    if (promoPos != std::string::npos) {
        cleanMove.erase(promoPos);
    }

    // Parse standard move: fromFile fromRank toFile toRank
    // Example: e2e4, g1f3
    if (cleanMove.size() >= 4) {
        uint8_t fromFile = cleanMove[0] - 'a';
        uint8_t fromRank = cleanMove[1] - '1';
        uint8_t toFile = cleanMove[2] - 'a';
        uint8_t toRank = cleanMove[3] - '1';

        // Encode as: fromFile(5bits) | fromRank(5bits) | toFile(5bits) | toRank(5bits) | moveType(7bits)
        move = (fromFile & 0x1F) |
               ((fromRank & 0x1F) << 5) |
               ((toFile & 0x1F) << 10) |
               ((toRank & 0x1F) << 15);

        // Default move type is quiet move (type 0)
        move |= (0 << 20);
    } else if (cleanMove.size() == 2) {
        // Short notation: just fromFile toFile (rare, usually incomplete)
        move = (cleanMove[0] - 'a') |
               ((cleanMove[1] - 'a') << 5);
    }

    return move;
}

uint64_t BookManager::fenToZobrist(const std::string& fen) const
{
    // Parse FEN and compute Zobrist hash
    // This is a simplified implementation - for exact matching,
    // we should use the same hash computation as the Position class

    uint64_t hash = 0;

    // For simplicity, we use a basic hash of the FEN string
    // In production, this should match the Zobrist hashing in Position
    std::hash<std::string> hasher;
    hash = static_cast<uint64_t>(hasher(fen));

    return hash;
}

bool BookManager::parsePolyglotEntry(const unsigned char* data, PolyglotEntry& entry) const
{
    if (!data) return false;
    std::memcpy(&entry, data, sizeof(PolyglotEntry));
    return true;
}

uint16_t BookManager::encodePolyglotMove(Move move) const
{
    // Encode a move in Polyglot format
    // Polyglot: fromFile(5bits) | toFile(5bits) | fromRank(3bits) | toRank(3bits) | promotion(3bits)

    uint8_t fromFile = (move >> 0) & 0x1F;
    uint8_t fromRank = (move >> 5) & 0x1F;
    uint8_t toFile = (move >> 10) & 0x1F;
    uint8_t toRank = (move >> 15) & 0x1F;

    uint16_t pgMove = (fromFile & 0x1F) |
                      ((toFile & 0x1F) << 5) |
                      ((fromRank & 0x07) << 10) |
                      ((toRank & 0x07) << 13);

    return pgMove;
}

Move BookManager::decodePolyglotMove(uint16_t pgMove, const Position& pos) const
{
    // Decode a Polyglot move to our engine format
    Square toSquare   = Square((pgMove)       & 0x3F); // Prende i primi 6 bit
    Square fromSquare = Square((pgMove >> 6)  & 0x3F); // Prende i successivi 6 bit
    ushort promoType  = (pgMove >> 12) & 0x07; // Prende i 3 bit di promo
    Piece promotedTo{};

    switch (promoType) {
        case 1:
            promotedTo = KNIGHT;
        case 2:
            promotedTo = BISHOP;
        case 3:
            promotedTo = ROOK;
        case 4:
            promotedTo = QUEEN;
        default:
            promotedTo = NO_PIECE;
    }

    Move move{};

    move = (move << 6) | fromSquare;
    move = (move << 6) | toSquare;

    // Let's get the colour from position
    move = (move << 1) | pos.getTurn(); 
    
    // The piece
    Piece piece = pos.idPiece(fromSquare).piece;
    move = (move << 3) | piece;

    if (promoType)
        if (pos.occupiedSquare(toSquare)) 
            move = composeMove (fromSquare, toSquare, pos.getTurn(), PAWN, PROMOTION,
                    pos.idPiece(toSquare).piece, promotedTo);
        else
            move = composeMove (fromSquare, toSquare, pos.getTurn(), PAWN, PROMOTION,
                    NO_PIECE, promotedTo);
    else if (toSquare == pos.getEnPassant())
        {
            move = composeMove (fromSquare, toSquare, pos.getTurn(), PAWN, EN_PASSANT,
                    PAWN, 0);
        }
    else if (pos.occupiedSquare(toSquare)) 
    { 
        if (pos.idPiece(fromSquare).piece == KING && pos.idPiece(toSquare).piece == ROOK) // castle    
            if (toSquare == A1 || toSquare == A8) 
                move = composeMove (fromSquare, toSquare, pos.getTurn(), KING, CASTLE_Q,
                    NO_PIECE, 0);
            else
                move = composeMove (fromSquare, toSquare, pos.getTurn(), KING, CASTLE_K,
                    NO_PIECE, 0);
        else
            {
                move = composeMove (fromSquare, toSquare, pos.getTurn(), pos.idPiece(fromSquare).piece, CAPTURE,
                    pos.idPiece(toSquare).piece, 0);
            }
    }
    else
        move = composeMove (fromSquare, toSquare, pos.getTurn(), pos.idPiece(fromSquare).piece, QUIET,
                    NO_PIECE, 0);
    
    return move;
}

std::vector<BookManager::BookMove> BookManager::getAllBookMoves(const Position& pos) const
{
    std::vector<BookMove> result;

    if (!isBookLoaded()) {
        return result;
    }

    uint64_t targetKey = generatePolyglotKey(pos);

    if (!isPolyglot_) {
        // EPD book - return moves from vector
        for (const auto& entry : entries_) {
            if (entry.polyglotKey == targetKey) {
                result.push_back({static_cast<Move>(entry.move), entry.weight});
            }
        }
        return result;
    }

    // Polyglot binary book - use O(log n) binary search
    std::ifstream file(bookPath_, std::ios::binary);
    if (!file.is_open()) {
        return result;
    }

    // Find the first entry with matching key using binary search
    int64_t firstIdx = binarySearchKey(file, targetKey);
    if (firstIdx < 0) {
        return result;
    }

    // Get file size to know bounds
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Scan backward to find the first entry with this key
    int64_t startIdx = firstIdx;
    while (startIdx > 0) {
        file.seekg(static_cast<std::streamsize>((startIdx - 1) * 16), std::ios::beg);
        uint64_t prevKey;
        file.read(reinterpret_cast<char*>(&prevKey), sizeof(uint64_t));
        if (be64ToNative(prevKey) != targetKey) {
            break;
        }
        startIdx--;
    }

    // Collect all entries with the matching key
    for (int64_t idx = startIdx; idx * 16 < fileSize; idx++) {
        file.seekg(static_cast<std::streamsize>(idx * 16), std::ios::beg);
        PolyglotEntry entry;
        file.read(reinterpret_cast<char*>(&entry), sizeof(PolyglotEntry));

        if (!file.good() || static_cast<std::streamsize>(idx * 16 + 16) > fileSize) {
            break;
        }

        if (be64ToNative(entry.key) != targetKey) {
            break;
        }

        result.push_back({decodePolyglotMove(be16ToNative(entry.move), pos), be16ToNative(entry.weight)});
    }

    return result;
}

BookManager::UnpackedMove BookManager::unpackPolyglotMove(uint16_t pgMove) const
{
    UnpackedMove unpacked;
    unpacked.fromSquare = (pgMove >> 6) & 0x3F;
    unpacked.toSquare = pgMove & 0x3F;
    unpacked.promotion = (pgMove >> 12) & 7;
    return unpacked;
}