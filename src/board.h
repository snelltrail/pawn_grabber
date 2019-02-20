#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

// Each bit of a Bitboard represents a square. The a8 square is the most
// significant bit and the h1 square is the least significant bit. So the
// unsigned 64 bit integer 10000000 00000000 00000000 00000000 00000000 00000000
// 00000000 00000100 represents the board:

// 8 | 1000 0000
// 7 | 0000 0000
// 6 | 0000 0000
// 5 | 0000 0000
// 4 | 0000 0000
// 3 | 0000 0000
// 2 | 0000 0000
// 1 | 0000 0100
//    -----------
//     abcd efgh
typedef uint64_t Bitboard;

enum class Piece {
  white_pawn,
  white_rook,
  white_knight,
  white_bishop,
  white_queen,
  white_king,
  black_pawn,
  black_rook,
  black_knight,
  black_bishop,
  black_queen,
  black_king,
};

struct Move {
  Bitboard src_square;
  Bitboard dst_square;
  std::optional<Piece> promotion_piece;
  Move() : src_square(0), dst_square(0), promotion_piece(std::nullopt) {}
  Move(Bitboard p_src_square, Bitboard p_dst_square)
      : src_square(p_src_square),
        dst_square(p_dst_square),
        promotion_piece(std::nullopt) {}
  Move(Bitboard p_src_square, Bitboard p_dst_square,
       std::optional<Piece> p_promotion_piece)
      : src_square(p_src_square),
        dst_square(p_dst_square),
        promotion_piece(p_promotion_piece) {}
  std::string to_pretty_str() const;
  friend void PrintTo(const Move& move, std::ostream* os);
};

bool operator==(const Move& lhs, const Move& rhs) {
  return lhs.src_square == rhs.src_square && lhs.dst_square == rhs.dst_square &&
         lhs.promotion_piece == rhs.promotion_piece;
}

bool is_square(Bitboard);
int square_idx(Bitboard square);
int rank_idx(Bitboard square);
int file_idx(Bitboard square);
bool on_a_file(Bitboard square);
bool on_h_file(Bitboard square);
bool on_first_rank(Bitboard square);
bool on_eigth_rank(Bitboard square);

enum class Direction {
  north,
  south,
  east,
  west,
  northeast,
  northwest,
  southeast,
  southwest
};

Bitboard north_of(Bitboard square);
Bitboard south_of(Bitboard square);
Bitboard east_of(Bitboard square);
Bitboard west_of(Bitboard square);
Bitboard northeast_of(Bitboard square);
Bitboard northwest_of(Bitboard square);
Bitboard southeast_of(Bitboard square);
Bitboard southwest_of(Bitboard square);

enum class Color { white, black };

// The Board class stores the current board state. Each bitboard tracks all
// places on the board there is a (piece, color) pair.
//
// For example, the Bitboards of the following position
//
//   ┌───┬───┬───┬───┬───┬───┬───┬───┐
// 8 │   │   │   │   │ ♚ │   │   │   │
//   ├───┼───┼───┼───┼───┼───┼───┼───┤
// 7 │   │   │   │   │   │   │   │   │
//   ├───┼───┼───┼───┼───┼───┼───┼───┤
// 6 │   │   │   │   │   │   │   │   │
//   ├───┼───┼───┼───┼───┼───┼───┼───┤
// 5 │   │   │   │   │   │   │   │   │
//   ├───┼───┼───┼───┼───┼───┼───┼───┤
// 4 │   │   │   │   │   │   │   │   │
//   ├───┼───┼───┼───┼───┼───┼───┼───┤
// 3 │   │   │   │   │   │   │   │   │
//   ├───┼───┼───┼───┼───┼───┼───┼───┤
// 2 │   │   │   │   │   │   │   │   │
//   ├───┼───┼───┼───┼───┼───┼───┼───┤
// 1 │   │   │   │   │ ♔ │   │   │   │
//   └───┴───┴───┴───┴───┴───┴───┴───┘
//     a   b   c   d   e   f   g   h
//
// are:
//
// white_pawns_ == 0x0
// white_rooks_ == 0x0
// white_knights_ == 0x0
// white_bishops_ == 0x0
// white_queens_ == 0x0
// white_king_ == 0x8
// black_pawns_ == 0x0
// black_rooks_ == 0x0
// black_knights_ == 0x0
// black_bishops_ == 0x0
// black_queens_ == 0x0
// black_king_ == 0x800000000000000.
//
// The Board class also tracks castling rights, the side to move, the move
// number and the number of moves until the fifty move rule applies.
class Board {
 public:
  Board();
  Board(std::string_view fen);
  Board(const Board& other);
  std::string to_pretty_str() const;
  friend void PrintTo(const Board& board, std::ostream* os);
  std::vector<Move> legal_moves();
  Bitboard white_pawns_;
  Bitboard white_rooks_;
  Bitboard white_knights_;
  Bitboard white_bishops_;
  Bitboard white_queens_;
  Bitboard white_king_;
  Bitboard black_pawns_;
  Bitboard black_rooks_;
  Bitboard black_knights_;
  Bitboard black_bishops_;
  Bitboard black_queens_;
  Bitboard black_king_;
  bool is_whites_move_;
  std::optional<Bitboard> en_passant_square_;
  bool white_has_right_to_castle_kingside_;
  bool white_has_right_to_castle_queenside_;
  bool black_has_right_to_castle_kingside_;
  bool black_has_right_to_castle_queenside_;
  int fifty_move_clock_;
  int num_moves_;
  Bitboard white_pieces() const;
  Bitboard black_pieces() const;
  Bitboard all_pieces() const;
  // The pseudo prefix refers to the fact that these functions generate
  // pesudolegal moves. We must also check if the king is in check before
  // generating all legal moves.
  void pseudolegal_sliding_moves(Direction direction, Color side,
                                 Bitboard src_square,
                                 std::vector<Move>* res_ptr) const;
  void pseudolegal_bishop_moves(Color side, std::vector<Move>* res_ptr) const;
  void pseudolegal_rook_moves(Color side, std::vector<Move>* res_ptr) const;
  void pseudolegal_queen_moves(Color side, std::vector<Move>* res_ptr) const;
  // "Simple" in this context means no two-step moves, no promotions, no en
  // passant.
  void pseudolegal_simple_pawn_moves(Color side,
                                     std::vector<Move>* res_ptr) const;
  void pseudolegal_two_step_pawn_moves(Color side,
                                       std::vector<Move>* res_ptr) const;
  void pseudolegal_en_passant_moves(Color side,
                                    std::vector<Move>* res_ptr) const;
  void pseudolegal_promotions(Color side, std::vector<Move>* res_ptr) const;
  void pseudolegal_pawn_captures(Color side, std::vector<Move>* res_ptr) const;
  void pseudolegal_pawn_moves(Color side, std::vector<Move>* res_ptr) const;

  void pseudolegal_king_moves(Color side, std::vector<Move>* res_ptr) const;
  void pseudolegal_knight_moves(Color side, std::vector<Move>* res_ptr) const;

  std::vector<Move> pseudolegal_moves(Color side) const;

  Bitboard all_dst_squares(Color color) const;

 private:
  void zero_all_bitboards();
  void init_bitboards(const std::string_view pieces_fen);
  void init_is_whites_move(const std::string_view pieces_fen);
  void init_color(const std::string_view side_to_move_fen);
  void init_castling_rights(const std::string_view castling_rights_fen);
  void init_en_passant(const std::string_view algebraic_square);
  void init_fifty_move_clock(const std::string_view num_half_moves_fen);
  void init_num_moves(const std::string_view num_moves_fen);
  std::string square_to_unicode(Bitboard square) const;
};

Bitboard str_to_square(const std::string_view alegbraic_square);
std::string square_to_str(Bitboard sq);
Bitboard coordinates_to_square(int file, int rank);
std::vector<Bitboard> bitboard_split(Bitboard bb);
std::function<Bitboard(Bitboard)> direction_to_function(Direction direction);

#endif
