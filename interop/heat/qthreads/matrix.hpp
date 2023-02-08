#ifndef MATRIX_HPP
#define MATRIX_HPP

#ifndef BSX
#define BSX 512
#endif

#ifndef BSY
#define BSY BSX
#endif

#define LEFT bx*nby + (by-1)
#define RIGHT bx*nby + (by+1)
#define TOP (bx-1)*nby + by
#define BOTTOM (bx+1)*nby + by
#define CENTER bx*nby + by
#define CENTER_ARGS (bx-1)*(nby-2) + by - 1

// Definition of types
typedef double row_t[BSY];
typedef row_t block_t[BSX];

typedef struct {
	block_t * matrix;
	int bx;
	int by;
	int nbx;
	int nby;
	int rank;
	int rank_size;
} task_arg_t;

template <typename Func>
inline void traverseRow(block_t *matrix, int numRowBlocks, int numColBlocks, int row, int startCol, int endCol, Func func) {
	int realRow = row + BSX-1;
	int rowBlock = realRow / BSX;
	for (int col = startCol; col < endCol; ++col) {
		int realCol = col + BSY-1;
		int colBlock = realCol / BSY;
		block_t &block = matrix[rowBlock*numColBlocks + colBlock];
		
		func(row, col, block[realRow%BSX][realCol%BSY]);
	}
}

// Useful functions for matrices
template <typename Func>
inline void traverseByRows(block_t *matrix, int rowBlocks, int colBlocks, Func func) {
	int numRows = (rowBlocks - 2) * BSX + 2;
	int numCols = (colBlocks - 2) * BSY + 2;

	for (int x = 0; x < numRows; ++x) {
		traverseRow(matrix, rowBlocks, colBlocks, x, 0, numCols, func);
	}
}

template <typename Func>
inline void traverseCol(block_t *matrix, int numRowBlocks, int numColBlocks, int col, int startRow, int endRow, Func func) {
	int realCol = col + BSY-1;
	int colBlock = realCol / BSY;
	for (int row = startRow; row < endRow; ++row) {
		int realRow = row + BSX-1;
		int rowBlock = realRow / BSX;
		block_t &block = matrix[rowBlock*numColBlocks + colBlock];
		
		func(row, col, block[realRow%BSX][realCol%BSY]);
	}
}

#endif // MATRIX_HPP
