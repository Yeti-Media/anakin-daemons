// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*-
/*
 * Author: Paul Fitzpatrick
 */

#ifndef WORDBOX_HPP_
#define WORDBOX_HPP_

#include <string>

namespace Anakin {
	class WordBox;
}

/**
 *
 * Helper class to order words found though text detection.
 * Currently assumes text is laid out close to the horizontal..
 *
 */
class Anakin::WordBox {
public:
	std::string text;  ///< the word in the box
	double x, y;       ///< coordinates of the center of the box
	double w, h;       ///< width and height of the box
	int id1, id2;      ///< external identifiers for the box

	/**
	 *
	 * Construct an empty box.
	 *
	 */
	WordBox() {
		id1 = id2 = -1;
		x = y = 0;
		w = h = 0;
	}

	/**
	 *
	 * Construct and initialize a box.
	 *
	 */
	WordBox(const std::string& text,
			int id1, int id2,
			double x, double y,
			double w, double h) {
		this->text = text;
		this->id1 = id1;
		this->id2 = id2;
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}

	/**
	 *
	 * Define an ordering for boxes representing word locations
	 * in more-or-less horizontally-aligned text.
	 *
	 */
	struct Order {
		bool operator() (const WordBox& a, const WordBox& b) {
			// If second box is far below first, it come later.
			if (b.y-b.h/2>a.y+a.h/2) return true;
			// If first box is far below second, it come later.
			if (a.y-a.h/2>b.y+b.h/2) return false;
			// Otherwise order along the horizontal coordinate.
			return (a.x<b.x);
		}
	};
};

#endif
