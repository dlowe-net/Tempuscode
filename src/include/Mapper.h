#ifndef _MAPPER_H_
#define _MAPPER_H_

#include "screen.h"
#include "desc_data.h"
#include "comm.h"
#include "db.h"
#include "utils.h"
#include "interpreter.h"
#include <signal.h>

struct MapToken {
	MapToken(int d, int r, int c, struct room_data * s, struct room_data * t);
	void clear() {
		source = target = NULL;
		next = NULL;
		targetID = -1;
		row = -7777777;
		column = -7777777;
		direction = -7777777;
	} struct room_data *getSource() {
		return source;
	}
	struct room_data *getTarget() {
		return target;
	}
	MapToken *next;
	int direction;
	int row;
	int column;
	long targetID;
	struct room_data *target;
	struct room_data *source;
};
struct MapPixel {
	MapPixel() {
		terrain = -1;
		exits = 0;
		mapped = false;
	} char terrain;
	int exits;
	bool mapped;
};

struct Mapper {
	Mapper(struct creature * ch, int rows, int columns);
	~Mapper();
	bool build(bool stayzone);
	void display(int bRows, int bCols);
	void clear();
	int processed;
	int size;
	int maxSize;
	int last;
	bool full;
	 struct creature * ch;			// character doing the mapping
	int rows, columns;			// size of the desired map
	MapPixel *mapDisplay;
	MapToken *mapStack;
	struct zone_data *curZone;
	void drawLink(struct room_data * s, struct room_data * t, int row, int col,
		bool justLink = false);
	bool drawRoom(struct room_data * s, struct room_data * t, long row, long col);
	int getOppDir(int dir) {
		switch (dir) {
		case 0:
			return 2;
			case 1:return 3;
			case 2:return 0;
			case 3:return 1;
			case 4:return 5;
			case 5:return 4;
			default:return -1;
	}} inline bool validRow(int row) {
		return (row < rows && row >= 0);
	}
	inline bool validColumn(int col) {
		return (col < columns && col >= 0);
	}
	inline bool empty() {
		return mapStack == NULL;
	}
	inline MapToken *pop() {
		if (mapStack == NULL)
			return NULL;
		MapToken *t = mapStack;
		mapStack = t->next;
		t->next = NULL;
		++processed;
		--size;
		return t;
	}
	inline void push(MapToken * t) {
		++size;
		if (size > maxSize)
			maxSize = size;
		if (size > 30000) {
			full = true;
		}
		t->next = mapStack;
		mapStack = t;
	}
};
const int North = 0;
const int East = 1;
const int South = 2;
const int West = 3;
const int Up = 4;
const int Down = 5;
#endif
